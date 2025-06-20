#include "my_vector.h"
#include "img_processing/camera.h"


#define TAG "my_vector"

#define INIT_CAPACITY   24
#define REALLOC_COEF    2 // 1.5f

#define LOCAL_LOG_FREE  0


vector_t* vector_create(
    size_t sizeof_element
) {
    vector_t* vec = (vector_t *)heap_caps_malloc(sizeof(vector_t), MALLOC_CAP_SPIRAM);
    if (!vec) {
        ESP_LOGE(TAG, "vector_create \t\t couldn't malloc ");
        return NULL;
    }
    
    vec->data = calloc(INIT_CAPACITY, sizeof_element);
    vec->size = 0;
    vec->capacity = INIT_CAPACITY;
    vec->sizeof_element = sizeof_element;

    return vec;
}


esp_err_t vector_reserve(
    vector_t *vec,
    const size_t new_capacity
) {
    if (!vec) {
        ESP_LOGE(TAG, "vector_reserve !vec\n");
        return ESP_FAIL;
    }

    if (new_capacity <= vec->capacity) {
        ESP_LOGE(TAG, "vector_reserve \t\t expected bigger capacity, not <= ");
        return ESP_FAIL;
    }
    
    void *new_data = heap_caps_realloc(vec->data, new_capacity * vec->sizeof_element, MALLOC_CAP_SPIRAM);
    if (!new_data) {
        ESP_LOGE(TAG, "vector_reserve \t\t realloc lost data ");
        return ESP_FAIL;
    }
    
    vec->data = new_data;           // looks like memory safe
    vec->capacity = new_capacity;

    return ESP_OK;
}


esp_err_t vector_push_back(
    vector_t *vec,
    const void *element
) {
    if (!vec || !element) {
        ESP_LOGE(TAG, "vector_push_back \t\t (!vec || !element) ");
        return ESP_FAIL;
    }
    
    if (vec->size >= vec->capacity) {
        size_t new_capacity = (vec->capacity == 0) ? 8 : vec->capacity * REALLOC_COEF;
        vector_reserve(vec, new_capacity);
    }
    
    void *dest = (char*)vec->data + vec->size * vec->sizeof_element;
    memcpy(dest, element, vec->sizeof_element);
    vec->size ++;

    return ESP_OK;
}


void* vector_get(
    const vector_t *vec,
    const size_t index
) {
    if (!vec || index >= vec->size) return NULL;
        return vec->data + index * vec->sizeof_element;
}


esp_err_t vector_set(vector_t *vec, const size_t index, void *val)
{
    if (index >= vec->capacity)
        vector_reserve(vec, index + 2);

    void *dest = vec->data + index * vec->sizeof_element;
    if (!memcpy(dest, val, vec->sizeof_element)) {
        ESP_LOGE(TAG, "vector_set \t\t failed to set element in vec ");
        return ESP_FAIL;
    }
    vec->size ++;
    
    return ESP_OK;
}


esp_err_t vector_print(
    const vector_t *vec
) {
    const uint16_t msg_size = 512;
    char msg[msg_size];
    char elements_buf[msg_size / 2];
    // bzero(msg, msg_size);

    switch (vec->sizeof_element) {
        case sizeof(uint8_t):           // size 1
        case sizeof(uint16_t):          // size 2
            snprintf(msg, msg_size,
                "uintX_t vec size: %d cap: %d { ",
                vec->size, vec->capacity
            );
            for (uint16_t i = 0; i < vec->size; ++i) {
                snprintf(elements_buf, sizeof(elements_buf),
                        "%d%s ",
                        *(uint16_t*)vector_get(vec, i),
                        i < vec->size - 1 ? "," : ""
                );
                strcat(msg, elements_buf);
            }
            strcat(msg, "}\n\0");

            break;

        case sizeof(pixel_coord_t):     // size 4   like int, int32 so it will be printing like (uint16_t uint16_t)
            snprintf(msg, msg_size,
                "pixel_coord_t vec size: %d cap: %d { ",
                vec->size, vec->capacity
            );
            for (uint16_t i = 0; i < vec->size; ++i) {
                snprintf(elements_buf, sizeof(elements_buf),
                        "(%d %d)%s ",
                        ((pixel_coord_t *)vector_get(vec, i))->x,
                        ((pixel_coord_t *)vector_get(vec, i))->y,
                        i < vec->size - 1 ? "," : ""
                );
                strcat(msg, elements_buf);
            }
            strcat(msg, "}\0");

            break;
        
        default:
            ESP_LOGI(TAG, "vector_print \t\t not implemented for this vector size %d\n", vec->sizeof_element);
            return ESP_FAIL;
    }

    ESP_LOGI(TAG, "%s ", msg);

    return ESP_OK;
}


esp_err_t vector_clear(
    vector_t *vec
) {
    if (vec) {
        #if LOG_FREE && LOCAL_LOG_FREE
            ESP_LOGI(TAG, "vector_clear \t free ");
        #endif
        heap_caps_free(vec->data);
        vec->data = NULL;
        #if LOG_FREE && LOCAL_LOG_FREE
            ESP_LOGI(TAG, "vector_clear \t fried ");
        #endif
        vec->size = 0;
        vec->capacity = 0;
        return ESP_OK;
    } else {
        ESP_LOGE(TAG, "no vec\n");
        return ESP_FAIL;
    }
}


esp_err_t vector_destroy(
    vector_t *vec
) {
    if (vec) {
        #if LOG_FREE && LOCAL_LOG_FREE
            ESP_LOGI(TAG, "vector_destroy \t fre vec & data ");
        #endif
        heap_caps_free(vec->data);
        heap_caps_free(vec);
        #if LOG_FREE && LOCAL_LOG_FREE
            ESP_LOGI(TAG, "vector_destroy \t fried vec & data ");
        #endif
        return ESP_OK;
    } else {
        ESP_LOGE(TAG, "vector_destroy \t\t no vec\n");
        return ESP_FAIL;
    }
}











// // looks like not needed
// static esp_err_t vector_resize(
//      vector_t *vec, size_t new_size
// ) {
//     if (!vec) {
//         ESP_LOGI(TAG, "vector_resize no vec!\n");
//         return -1;
//     }
//     
//     if (new_size > vec->capacity) {
//         vector_reserve(vec, new_size);
//     }
//     
//     if (new_size > vec->size) {
//         void *start = (char*)vec->data + vec->size * vec->sizeof_element;
//         memset(start, 0, (new_size - vec->size) * vec->sizeof_element);
//     }
//     
//     vec->size = new_size;
// 
//     return ESP_OK;
// }