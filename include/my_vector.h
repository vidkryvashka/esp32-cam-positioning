#ifndef MY_VECTOR_H
#define MY_VECTOR_H

#include "defs.h"


/**
 * @brief base vector struct inspired by std::vector
 */
typedef struct {
    void*   data;
    size_t  size;
    size_t  capacity;
    size_t  sizeof_element;
} vector_t;


/**
 * @brief Initialize vector with specified element size
 * 
 * @param sizeof_element was convinced to determine by sizeof()
 * @return vector_t* pointer, should be freed
 */
vector_t* vector_create(const size_t sizeof_element);


/**
 * @brief Reserve memory for specified number of elements
 * 
 * @param vec 
 * @param new_capacity
 * @return esp_err_t -1: realloc failed
 */
esp_err_t vector_reserve(vector_t *vec, const size_t new_capacity);


/**
 * @brief Push element to the back
 * 
 * @param vec 
 * @param element 
 * @return esp_err_t -1: !vec || !element
 */
esp_err_t vector_push_back(vector_t *vec, const void *element);


/**
 * @brief Get pointer to element at index
 * 
 * @param vec 
 * @param index 
 * @return void* pointer to wanted data
 */
void* vector_get(const vector_t *vec, const size_t index);


/**
 * @brief Set value at index
 * 
 * @param vec 
 * @param index 
 * @param val
 * @return -1: memcpy returned NULL
 */
esp_err_t vector_set(vector_t *vec, size_t index, void *val);


/**
 * @brief Print uint8_t vector
 * 
 * @param vec 
 * @return esp_err_t 
 */
esp_err_t vector_print(const vector_t *vec);


/**
 * @brief Clear vector (set size to 0)
 * 
 * @param vec 
 * @return esp_err_t -1: no vec
 */
esp_err_t vector_clear(vector_t *vec);


/**
 * @brief Free vector memory
 * 
 * @param vec 
 * @return esp_err_t -1: no vec
 */
esp_err_t vector_destroy(vector_t *vec);







// // Resize vector to new size     // looks like not needed
// esp_err_t vector_resize(vector_t *vec, size_t new_size);

#endif