#ifndef DBSCAN_H
#define DBSCAN_H

#include "my_vector.h"
#include "img_processing/camera.h"

#define TAG_DBSCAN "dbscan"

#define UNCLASSIFIED -1
#define NOISE -2

typedef struct {
    vector_t *coords;       // Вектор координат точок (pixel_coord_t)
    vector_t *cluster_centers; // Вектор центрів кластерів (pixel_coord_t)
    int *cluster_ids;       // Масив ідентифікаторів кластерів для кожної точки
} dbscan_result_t;

/**
 * @brief Ініціалізує структуру dbscan_result_t
 * @param result Вказівник на структуру для ініціалізації
 * @param num_points Кількість точок
 * @return ESP_OK у разі успіху, інакше ESP_FAIL
 */
esp_err_t dbscan_result_init(dbscan_result_t *result, size_t num_points);

/**
 * @brief Звільняє пам’ять структури dbscan_result_t
 * @param result Вказівник на структуру
 */
void dbscan_result_destroy(dbscan_result_t *result);

/**
 * @brief Виконує кластеризацію DBSCAN і повертає центри кластерів
 * @param pixels_cloud Вхідні дані (координати точок)
 * @param epsilon Максимальна відстань між точками в одному кластері
 * @param min_points Мінімальна кількість точок для формування кластера
 * @param result Результат кластеризації (включає центри кластерів)
 * @return ESP_OK у разі успіху, інакше ESP_FAIL
 */
esp_err_t dbscan_cluster(pixels_cloud_t *pixels_cloud, double epsilon, unsigned int min_points, dbscan_result_t *result);

/**
 * @brief Вибирає випадковий центр кластера
 * @param result Результат кластеризації
 * @param selected_center Вказівник на pixel_coord_t для збереження вибраного центра
 * @return ESP_OK якщо центр вибрано, ESP_FAIL якщо немає кластерів
 */
esp_err_t dbscan_select_random_center(dbscan_result_t *result, pixel_coord_t *selected_center);

#endif // DBSCAN_H