#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "img_processing/camera.h"


#define TAG "my_recognizer"



typedef struct {
    uint8_t *buf;
    uint8_t width;
    uint8_t height;
} gray_img_t;


// Structure to hold keypoint and descriptor
#define DESC_SIZE 32 // 256-bit descriptor (32 bytes)
typedef struct {
    pixel_coordinate_t pt; // Keypoint coordinates
    uint8_t descriptor[DESC_SIZE]; // Binary descriptor
} keypoint_t;


// Convert RGB565 to grayscale
static void rgb565_to_gray(
    const camera_fb_t *src,
    // uint8_t *dst,
    gray_img_t *dst
) {

    // uint8_t *frame_gray = (uint8_t *)malloc(frame->width * frame->height);
    // uint8_t *frag_gray = (uint8_t *)malloc(fragment->width * fragment->height);
    // if (!frame_gray || !frag_gray) {
    //     free(frame_gray);
    //     free(frag_gray);
    //     ESP_LOGE(TAG, "couldn't malloc gray copies - abort find_fragment ");
    //     return ESP_FAIL;
    // }

    dst->buf = (uint8_t *)malloc(src->width * src->height);
    dst->width = src->width;
    dst->height = src->height;

    if (!dst->buf)
        ESP_LOGE(TAG, "couldn't malloc gray copies - abort find_fragment ");

    uint16_t *src_ptr = (uint16_t *)src->buf;
    for (size_t i = 0; i < src->width * src->height; ++i) {
        uint16_t pixel = src_ptr[i];
        // Extract R, G, B (5-6-5 bits)
        uint8_t r = (pixel >> 11) & 0x1F;
        uint8_t g = (pixel >> 5) & 0x3F;
        uint8_t b = pixel & 0x1F;
        // Scale to 8-bit
        r = (r << 3) | (r >> 2);
        g = (g << 2) | (g >> 4);
        b = (b << 3) | (b >> 2);
        // Grayscale: 0.299R + 0.587G + 0.114B
        dst->buf[i] = (uint8_t)(0.299 * r + 0.587 * g + 0.114 * b);
    }
}


#define FAST_CORNERS_RADIUS 3
#define FAST_CORNERS_THRESHOLD 20


// Simplified FAST corner detector with configurable radius
static void detect_fast_corners(
    const gray_img_t *img,
    keypoint_t *keypoints,
    size_t *num_keypoints,
    const size_t max_keypoints
) {
    uint8_t radius = FAST_CORNERS_RADIUS;
    int threshold = FAST_CORNERS_THRESHOLD;
    *num_keypoints = 0;

    // Validate inputs
    if (radius < 1 || radius >= img->width / 2 || radius >= img->height / 2) {
        ESP_LOGE(TAG, "Invalid radius: %d for image %zux%zu", radius, img->width, img->height);
        return;
    }
    if (!img || !keypoints || max_keypoints == 0) {
        ESP_LOGE(TAG, "Invalid parameters");
        return;
    }

    // // Generate offsets for a circle of given radius (16 pixels) idk if works
    // int offsets[16][2];
    // for (int i = 0; i < 16; ++i) {
    //     float angle = (float)i * (2.0f * M_PI / 16.0f);
    //     offsets[i][0] = (int)roundf(radius * cosf(angle));
    //     offsets[i][1] = (int)roundf(radius * sinf(angle));
    // }
    // Circle of 16 pixels around center (FAST-9)
    int offsets[16][2] = {
        {0, 3}, {1, 3}, {2, 2}, {3, 1},
        {3, 0}, {3, -1}, {2, -2}, {1, -3},
        {0, -3}, {-1, -3}, {-2, -2}, {-3, -1},
        {-3, 0}, {-3, 1}, {-2, 2}, {-1, 3}
    };
    ESP_LOGI(TAG, "detect_fast_corners generated offsets");

    // Adjust loop bounds to avoid accessing pixels outside the image
    for (uint16_t y = radius; y < img->height - radius; ++y) {
        for (uint16_t x = radius; x < img->width - radius; ++x) {
            uint16_t center = img->buf[y * img->width + x];
            uint16_t count = 0;
            // Check 9 contiguous pixels brighter or darker
            for (int i = 0; i < 16; ++i) {
                uint16_t px = x + offsets[i][0];
                uint16_t py = y + offsets[i][1];
                // Explicit bounds checking
                if (px < 0 || px >= (uint16_t)img->width || py < 0 || py >= (uint16_t)img->height) {
                    count = 0;
                    break;
                }
                uint16_t val = img->buf[py * img->width + px];
                if (val > center + threshold || val < center - threshold) {
                    ++ count;
                    if (count >= 9) {
                        keypoints[*num_keypoints].pt.x = x;
                        keypoints[*num_keypoints].pt.y = y;
                        ++(*num_keypoints);
                        if (*num_keypoints >= max_keypoints) {
                            ESP_LOGI(TAG, "detect_fast_corners reached max keypoints: %zu in %dx%d",
                                    *num_keypoints, img->width, img->height);
                            return;
                        }
                        break;
                    }
                } else {
                    count = 0;
                }
            }
        }
    }
    ESP_LOGI(TAG, "detect_fast_corners done, found %zu keypoints in %dx%d",
            *num_keypoints, img->width, img->height);
}


// Simplified BRIEF descriptor
static void compute_brief_descriptors(
    uint8_t *img,
    size_t width,
    size_t height,
    keypoint_t *keypoints,
    size_t num_keypoints
) {
    // Predefined pixel pairs for BRIEF (256 comparisons)
    static int pairs[256][4] = {
        // Example pairs (x1, y1, x2, y2), normally randomized
        {0, -2, 0, 2}, {1, -1, 1, 1}, {-1, -1, -1, 1}, // Simplified for demo
        // ... (fill with 256 pairs, can use ORB's bit_pattern_31_ for inspiration)
    };
    for (size_t i = 0; i < num_keypoints; ++i) {
        uint8_t *desc = keypoints[i].descriptor;
        int x = keypoints[i].pt.x;
        int y = keypoints[i].pt.y;
        for (int j = 0; j < 256; ++j) {
            int x1 = x + pairs[j][0];
            int y1 = y + pairs[j][1];
            int x2 = x + pairs[j][2];
            int y2 = y + pairs[j][3];
            if (x1 >= 0 && x1 < (int)width && y1 >= 0 && y1 < (int)height &&
                x2 >= 0 && x2 < (int)width && y2 >= 0 && y2 < (int)height) {
                uint8_t bit = (img[y1 * width + x1] < img[y2 * width + x2]) ? 1 : 0;
                desc[j / 8] |= bit << (j % 8);
            }
        }
    }
}


// Hamming distance between two descriptors
static uint32_t hamming_distance(
    const uint8_t *desc1, 
    const uint8_t *desc2
) {
    uint32_t distance = 0;
    for (int i = 0; i < DESC_SIZE; ++i) {
        uint8_t xor_val = desc1[i] ^ desc2[i];
        while (xor_val) {
            distance += xor_val & 1;
            xor_val >>= 1;
        }
    }
    return distance;
}


// Match keypoints between fragment and frame
typedef struct {
    size_t frame_idx;
    size_t frag_idx;
    uint32_t distance;
} match_t;


static void match_descriptors(
    keypoint_t *frame_kp,
    size_t frame_n,
    keypoint_t *frag_kp,
    size_t frag_n,
    match_t *matches,
    size_t *num_matches
) {
    *num_matches = 0;
    for (size_t i = 0; i < frag_n; ++i) {
        uint32_t best_dist = UINT32_MAX;
        size_t best_idx = 0;
        uint32_t second_best_dist = UINT32_MAX;
        for (size_t j = 0; j < frame_n; ++j) {
            uint32_t dist = hamming_distance(frag_kp[i].descriptor, frame_kp[j].descriptor);
            if (dist < best_dist) {
                second_best_dist = best_dist;
                best_dist = dist;
                best_idx = j;
            } else if (dist < second_best_dist) {
                second_best_dist = dist;
            }
        }
        // Ratio test: best distance < 0.7 * second best
        if (best_dist < 0.7 * second_best_dist && best_dist < 100) {
            matches[*num_matches].frame_idx = best_idx;
            matches[*num_matches].frag_idx = i;
            matches[*num_matches].distance = best_dist;
            (*num_matches)++;
            if (*num_matches >= 50) break; // Limit matches
        }
    }
}


// Find fragment in frame
esp_err_t find_fragment(
    const camera_fb_t *frame,
    const camera_fb_t *fragment,
    float *similarity,
    pixel_coordinate_t *top_left
) {
    esp_err_t err = ESP_OK;
    ESP_LOGI(TAG, "entered find_fragment ");
    // Convert to grayscale
    gray_img_t frame_gray, frag_gray;
    rgb565_to_gray(frame, &frame_gray);
    rgb565_to_gray(fragment, &frag_gray);
    if (!frame_gray.buf || !frag_gray.buf) {
        ESP_LOGE(TAG, "couldn't malloc gray copies - abort find_fragment ");
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "made gray copies ");

    // Detect keypoints
    uint8_t max_key_points = 255;
    keypoint_t frame_kp[max_key_points], frag_kp[max_key_points];
    size_t frame_n = 0, frag_n = 0;
    detect_fast_corners(&frame_gray, frame_kp, &frame_n, max_key_points);
    detect_fast_corners(&frag_gray, frag_kp, &frag_n, max_key_points);

    //if (frag_n < 4 || frame_n < 4) { // Need enough keypoints
    //    free(frame_gray->buf);
    //    free(frag_gray->buf);
    //    *similarity = 0;
    //    ESP_LOGE(TAG, "frag_n: %d < 4 || frame_n: %d < 4 : Need enough keypoints - abort find_fragment ", frag_n, frame_n);
    //    return ESP_FAIL;
    //}
    //ESP_LOGI(TAG, "detected keypoints ");

    // // Compute descriptors
    // compute_brief_descriptors(frame_gray, frame->width, frame->height, frame_kp, frame_n);
    // compute_brief_descriptors(frag_gray, fragment->width, fragment->height, frag_kp, frag_n);
    // ESP_LOGI(TAG, "computed descriptors ");

    // // Match descriptors
    // match_t matches[50];
    // size_t num_matches = 0;
    // match_descriptors(frame_kp, frame_n, frag_kp, frag_n, matches, &num_matches);
    
    ESP_LOGI(TAG, "before free ");
    free(frame_gray.buf);
    free(frag_gray.buf);
    ESP_LOGI(TAG, "after free ");

    // if (num_matches < 4) {
    //     *similarity = 0.0f;
    //     ESP_LOGE(TAG, "num_matches < 4 - abort find_fragment ");
    //     return ESP_FAIL;
    // }
    // ESP_LOGI(TAG, "matched descriptors ");

    // // Estimate fragment location
    // int min_x = frame->width, min_y = frame->height;
    // int max_x = 0, max_y = 0;
    // for (size_t i = 0; i < num_matches; i++) {
    //     pixel_coordinate_t pt = frame_kp[matches[i].frame_idx].pt;
    //     if (pt.x < min_x) min_x = pt.x;
    //     if (pt.y < min_y) min_y = pt.y;
    //     if (pt.x > max_x) max_x = pt.x;
    //     if (pt.y > max_y) max_y = pt.y;
    // }
    // ESP_LOGI(TAG, "fragment location estimated ");

    // // Compute similarity
    // float max_possible_matches = (float)frag_n;
    // *similarity = (num_matches / max_possible_matches) * 100.0f;
    // if (*similarity > 100.0f) *similarity = 100.0f;
    
    // // Set output coordinates
    // top_left->x = (uint8_t)min_x;
    // top_left->y = (uint8_t)min_y;
    
    // ESP_LOGI(TAG, "find_fragment done ");
    return err;
}







static void example_to_use()
{
    camera_fb_t *current_frame; // Initialize with 96x96 RGB565 data
    camera_fb_t *fragment;      // Initialize with smaller RGB565 data
    float similarity;
    pixel_coordinate_t top_left;    

    int result = find_fragment(current_frame, fragment, &similarity, &top_left);
    if (result == 0) {
        printf("Similarity: %.2f%%\n", similarity);
        printf("Top-left: (%d, %d)\n", top_left.x, top_left.y);
    } else {
        printf("Fragment not found\n");
    }
}