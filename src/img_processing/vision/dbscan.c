#include <math.h>
#include <stdlib.h>


#define TAG     "my_dbscan"


#define UNCLASSIFIED    -1
#define NOISE           -2

#include "img_processing/dbscan.h"


static vector_t *local_coords_ref = NULL;
static int *cluster_ids = NULL;


static esp_err_t dbscan_reboot(
    const pixels_cloud_t *pixels_cloud
) {
    if (!local_coords_ref)
        local_coords_ref = pixels_cloud->coords;

    if (!cluster_ids)
        cluster_ids = (int *)heap_caps_malloc(local_coords_ref->size * sizeof(int), MALLOC_CAP_SPIRAM);

    for (size_t i = 0; i < local_coords_ref->size; i++) {
        cluster_ids[i] = UNCLASSIFIED;
    }

    return ESP_OK;
}


static esp_err_t get_neighbors(
    size_t index,
    double epsilon,
    vector_t *neighbors
) {
    pixel_coord_t *point = (pixel_coord_t *)vector_get(local_coords_ref, index);
    if (!point)
        return ESP_FAIL;

    vector_clear(neighbors);

    for (size_t i = 0; i < local_coords_ref->size; i++) {
        if (i == index)
            continue;
        pixel_coord_t *other = (pixel_coord_t *)vector_get(local_coords_ref, i);
        if (sqrt(pow(point->x - other->x, 2) + pow(point->y - other->y, 2)) <= epsilon)
            if (vector_push_back(neighbors, &i) != ESP_OK) {
                ESP_LOGE(TAG, "Failed to add neighbor ");
                return ESP_FAIL;
            }
    }
    return ESP_OK;
}


static esp_err_t expand_cluster(
    size_t index,
    int cluster_id,
    double epsilon,
    uint min_points
) {
    vector_t *seeds = vector_create(sizeof(size_t));
    if (!seeds)
        return ESP_FAIL;

    if (get_neighbors(index, epsilon, seeds) != ESP_OK) {
        vector_destroy(seeds);
        return ESP_FAIL;
    }

    if (seeds->size < min_points) {
        cluster_ids[index] = NOISE;
        vector_destroy(seeds);
        return ESP_OK;
    }

    cluster_ids[index] = cluster_id;
    for (size_t i = 0; i < seeds->size; i++) {
        size_t *neighbor_idx = (size_t *)vector_get(seeds, i);
        cluster_ids[*neighbor_idx] = cluster_id;
    }

    size_t seed_idx = 0;
    while (seed_idx < seeds->size) {
        size_t *current_idx = (size_t *)vector_get(seeds, seed_idx);
        vector_t *neighbors = vector_create(sizeof(size_t));
        if (!neighbors) {
            vector_destroy(seeds);
            return ESP_FAIL;
        }

        if (get_neighbors(*current_idx, epsilon, neighbors) != ESP_OK) {
            vector_destroy(neighbors);
            vector_destroy(seeds);
            return ESP_FAIL;
        }

        if (neighbors->size >= min_points) {
            for (size_t j = 0; j < neighbors->size; j++) {
                size_t *n_idx = (size_t *)vector_get(neighbors, j);
                if (cluster_ids[*n_idx] == UNCLASSIFIED || cluster_ids[*n_idx] == NOISE) {
                    if (cluster_ids[*n_idx] == UNCLASSIFIED)
                        vector_push_back(seeds, n_idx);
                    cluster_ids[*n_idx] = cluster_id;
                }
            }
        }
        vector_destroy(neighbors);
        seed_idx++;
    }

    vector_destroy(seeds);
    return ESP_OK;
}


static esp_err_t calculate_cluster_centers(
    vector_t *cluster_centers
) {
    int max_cluster_id = -1;
    for (size_t i = 0; i < local_coords_ref->size; i++) {
        if (cluster_ids[i] > max_cluster_id) {
            max_cluster_id = cluster_ids[i];
        }
    }

    for (int cluster_id = 0; cluster_id <= max_cluster_id; cluster_id++) {
        float sum_x = 0, sum_y = 0;
        size_t count = 0;

        for (size_t i = 0; i < local_coords_ref->size; i++) {
            if (cluster_ids[i] == cluster_id) {
                pixel_coord_t *coord = (pixel_coord_t *)vector_get(local_coords_ref, i);
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
            vector_push_back(cluster_centers, &center);
        }
    }

    return ESP_OK;
}


static esp_err_t select_highest_center(
    pixels_cloud_t *pixels_cloud,
    vector_t *cluster_centers
) {
    if (!pixels_cloud || !cluster_centers || !cluster_centers->size) {
        ESP_LOGE(TAG, "select_highest_center no cloud|clusters no centers ");
        pixels_cloud->center_coord.x = 0;
        pixels_cloud->center_coord.y = 0;
        return ESP_FAIL;
    }

    pixel_coord_t *highest_center = NULL;
    uint16_t min_y = UINT16_MAX;

    for (size_t i = 0; i < cluster_centers->size; i++) {
        pixel_coord_t *center = (pixel_coord_t *)vector_get(cluster_centers, i);
        if (center->y < min_y) {
            min_y = center->y;
            highest_center = center;
        }
    }

    if (highest_center) {
        pixels_cloud->center_coord.x = highest_center->x;
        pixels_cloud->center_coord.y = highest_center->y;
        ESP_LOGI(TAG, "Selected highest center: (%d, %d)", pixels_cloud->center_coord.x, pixels_cloud->center_coord.y);
        return ESP_OK;
    }

    ESP_LOGE(TAG, "select_highest_center failed to find highest center ");
    return ESP_FAIL;
}



esp_err_t dbscan_cluster(
    pixels_cloud_t *pixels_cloud,
    const double epsilon,
    const uint min_points,
    vector_t *cluster_centers
) {
    if (!pixels_cloud || !pixels_cloud->coords || !cluster_centers) {
        ESP_LOGE(TAG, "dbscan_cluster invalid input ");
        return ESP_FAIL;
    }

    dbscan_reboot(pixels_cloud);

    int cluster_id = 0;
    for (size_t i = 0; i < local_coords_ref->size; i++)
        if (cluster_ids[i] == UNCLASSIFIED)
            if (expand_cluster(i, cluster_id, epsilon, min_points) == ESP_OK)
                cluster_id++;

    esp_err_t calc_centers_err = calculate_cluster_centers(cluster_centers);
    if (calc_centers_err != ESP_OK) {
        ESP_LOGE(TAG, "dbscan_cluster calc centers err ");
        return ESP_FAIL;
    }

    select_highest_center(pixels_cloud, cluster_centers);

    ESP_LOGI(TAG, "Found %d clusters ", cluster_centers->size);
    return ESP_OK;
}