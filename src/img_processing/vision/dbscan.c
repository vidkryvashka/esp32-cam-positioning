#include <math.h>
#include <stdlib.h>

#include "img_processing/dbscan.h"


static double euclidean_distance(pixel_coord_t *a, pixel_coord_t *b) {
    return sqrt(pow(a->x - b->x, 2) + pow(a->y - b->y, 2));
}

esp_err_t dbscan_result_init(dbscan_result_t *result, size_t num_points) {
    if (!result) return ESP_FAIL;

    result->coords = vector_create(sizeof(pixel_coord_t));
    result->cluster_centers = vector_create(sizeof(pixel_coord_t));
    result->cluster_ids = (int *)heap_caps_malloc(num_points * sizeof(int), MALLOC_CAP_SPIRAM);

    if (!result->coords || !result->cluster_centers || !result->cluster_ids) {
        ESP_LOGE(TAG_DBSCAN, "Failed to allocate memory for dbscan_result");
        dbscan_result_destroy(result);
        return ESP_FAIL;
    }

    for (size_t i = 0; i < num_points; i++) {
        result->cluster_ids[i] = UNCLASSIFIED;
    }

    return ESP_OK;
}

void dbscan_result_destroy(dbscan_result_t *result) {
    if (result) {
        vector_destroy(result->coords);
        vector_destroy(result->cluster_centers);
        heap_caps_free(result->cluster_ids);
        result->coords = NULL;
        result->cluster_centers = NULL;
        result->cluster_ids = NULL;
    }
}

static esp_err_t get_neighbors(vector_t *coords, size_t index, double epsilon, vector_t *neighbors) {
    pixel_coord_t *point = (pixel_coord_t *)vector_get(coords, index);
    if (!point) return ESP_FAIL;

    vector_clear(neighbors);

    for (size_t i = 0; i < coords->size; i++) {
        if (i == index) continue;
        pixel_coord_t *other = (pixel_coord_t *)vector_get(coords, i);
        if (euclidean_distance(point, other) <= epsilon) {
            if (vector_push_back(neighbors, &i) != ESP_OK) {
                ESP_LOGE(TAG_DBSCAN, "Failed to add neighbor");
                return ESP_FAIL;
            }
        }
    }
    return ESP_OK;
}

static esp_err_t expand_cluster(dbscan_result_t *result, size_t index, int cluster_id, double epsilon, unsigned int min_points) {
    vector_t *seeds = vector_create(sizeof(size_t));
    if (!seeds) return ESP_FAIL;

    if (get_neighbors(result->coords, index, epsilon, seeds) != ESP_OK) {
        vector_destroy(seeds);
        return ESP_FAIL;
    }

    if (seeds->size < min_points) {
        result->cluster_ids[index] = NOISE;
        vector_destroy(seeds);
        return ESP_OK;
    }

    result->cluster_ids[index] = cluster_id;
    for (size_t i = 0; i < seeds->size; i++) {
        size_t *neighbor_idx = (size_t *)vector_get(seeds, i);
        result->cluster_ids[*neighbor_idx] = cluster_id;
    }

    size_t seed_idx = 0;
    while (seed_idx < seeds->size) {
        size_t *current_idx = (size_t *)vector_get(seeds, seed_idx);
        vector_t *neighbors = vector_create(sizeof(size_t));
        if (!neighbors) {
            vector_destroy(seeds);
            return ESP_FAIL;
        }

        if (get_neighbors(result->coords, *current_idx, epsilon, neighbors) != ESP_OK) {
            vector_destroy(neighbors);
            vector_destroy(seeds);
            return ESP_FAIL;
        }

        if (neighbors->size >= min_points) {
            for (size_t j = 0; j < neighbors->size; j++) {
                size_t *n_idx = (size_t *)vector_get(neighbors, j);
                if (result->cluster_ids[*n_idx] == UNCLASSIFIED || result->cluster_ids[*n_idx] == NOISE) {
                    if (result->cluster_ids[*n_idx] == UNCLASSIFIED) {
                        vector_push_back(seeds, n_idx);
                    }
                    result->cluster_ids[*n_idx] = cluster_id;
                }
            }
        }
        vector_destroy(neighbors);
        seed_idx++;
    }

    vector_destroy(seeds);
    return ESP_OK;
}

static esp_err_t calculate_cluster_centers(dbscan_result_t *result) {
    int max_cluster_id = -1;
    for (size_t i = 0; i < result->coords->size; i++) {
        if (result->cluster_ids[i] > max_cluster_id) {
            max_cluster_id = result->cluster_ids[i];
        }
    }

    for (int cluster_id = 0; cluster_id <= max_cluster_id; cluster_id++) {
        float sum_x = 0, sum_y = 0;
        size_t count = 0;

        for (size_t i = 0; i < result->coords->size; i++) {
            if (result->cluster_ids[i] == cluster_id) {
                pixel_coord_t *coord = (pixel_coord_t *)vector_get(result->coords, i);
                sum_x += coord->x;
                sum_y += coord->y;
                count++;
            }
        }

        if (count > 0) {
            pixel_coord_t center = {
                .x = (uint16_t)(sum_x / count + 0.5f),
                .y = (uint16_t)(sum_y / count + 0.5f)
            };
            vector_push_back(result->cluster_centers, &center);
        }
    }

    return ESP_OK;
}

esp_err_t dbscan_cluster(pixels_cloud_t *pixels_cloud, double epsilon, unsigned int min_points, dbscan_result_t *result) {
    if (!pixels_cloud || !pixels_cloud->coords || !result) {
        ESP_LOGE(TAG_DBSCAN, "Invalid input");
        return ESP_FAIL;
    }

    if (dbscan_result_init(result, pixels_cloud->coords->size) != ESP_OK) {
        return ESP_FAIL;
    }

    for (size_t i = 0; i < pixels_cloud->coords->size; i++) {
        pixel_coord_t *coord = (pixel_coord_t *)vector_get(pixels_cloud->coords, i);
        vector_push_back(result->coords, coord);
    }

    int cluster_id = 0;
    for (size_t i = 0; i < result->coords->size; i++) {
        if (result->cluster_ids[i] == UNCLASSIFIED) {
            if (expand_cluster(result, i, cluster_id, epsilon, min_points) == ESP_OK) {
                cluster_id++;
            }
        }
    }

    if (calculate_cluster_centers(result) != ESP_OK) {
        dbscan_result_destroy(result);
        return ESP_FAIL;
    }

    ESP_LOGI(TAG_DBSCAN, "Found %d clusters", result->cluster_centers->size);
    return ESP_OK;
}

esp_err_t dbscan_select_random_center(dbscan_result_t *result, pixel_coord_t *selected_center) {
    if (!result || !result->cluster_centers || result->cluster_centers->size == 0 || !selected_center) {
        ESP_LOGE(TAG_DBSCAN, "No clusters available");
        return ESP_FAIL;
    }

    size_t random_idx = rand() % result->cluster_centers->size;
    pixel_coord_t *center = (pixel_coord_t *)vector_get(result->cluster_centers, random_idx);
    *selected_center = *center;

    ESP_LOGI(TAG_DBSCAN, "Selected center: (%d, %d)", selected_center->x, selected_center->y);
    return ESP_OK;
}