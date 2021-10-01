/*
 * Collections-C
 * Copyright (C) 2013-2015 Srđan Panić <srdja.panic@gmail.com>
 *
 * This file is part of Collections-C.
 *
 * Collections-C is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Collections-C is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Collections-C.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "coll/cc_deque.h"

#define DEFAULT_CAPACITY 8
#define DEFAULT_EXPANSION_FACTOR 2

struct cc_deque_s {
    size_t   size;
    size_t   capacity;
    size_t   first;
    size_t   last;
    void   **buffer;

    void *(*mem_alloc)  (size_t size);
    void *(*mem_calloc) (size_t blocks, size_t size);
    void  (*mem_free)   (void *block);
};

static size_t upper_pow_two (size_t);
static void   copy_buffer   (CC_Deque const * const deque, void **buff, void *(*cp) (void*));

static enum cc_stat expand_capacity (CC_Deque *deque);

/**
 * Creates a new empty deque and returns a status code.
 *
 * @param[out] out Pointer to where the newly created CC_Deque is to be stored
 *
 * @return CC_OK if the creation was successful, or CC_ERR_ALLOC if the
 * memory allocation for the new CC_Deque structure failed.
 */
enum cc_stat cc_deque_new(CC_Deque **deque)
{
    CC_DequeConf conf;
    cc_deque_conf_init(&conf);
    return cc_deque_new_conf(&conf, deque);
}

/**
 * Creates a new empty CC_Deque based on the specified CC_DequeConf object and
 * returns a status code.
 *
 * The CC_Deque is allocated using the allocators specified in the CC_DequeConf struct.
 * The allocation may fail if the underlying allocator fails.
 *
 * @param[in] conf CC_Deque configuration structure. All fields must be initialized
 *                 with appropriate values.
 * @param[out] out Pointer to where the newly created CC_Deque is to be stored
 *
 * @return CC_OK if the creation was successful, CC_ERR_INVALID_CAPACITY if
 * the above mentioned condition is not met, or CC_ERR_ALLOC if the memory
 * allocation for the new CC_Deque structure failed.
 */
enum cc_stat cc_deque_new_conf(CC_DequeConf const * const conf, CC_Deque **d)
{
    CC_Deque *deque = conf->mem_calloc(1, sizeof(CC_Deque));

    if (!deque)
        return CC_ERR_ALLOC;

    if (!(deque->buffer = conf->mem_alloc(conf->capacity * sizeof(void*)))) {
        conf->mem_free(deque);
        return CC_ERR_ALLOC;
    }

    deque->mem_alloc  = conf->mem_alloc;
    deque->mem_calloc = conf->mem_calloc;
    deque->mem_free   = conf->mem_free;
    deque->capacity   = upper_pow_two(conf->capacity);
    deque->first      = 0;

    deque->size       = 0;

    *d = deque;
    return CC_OK;
}

/**
 * Initializes the fields of the CC_DequeConf struct to default values.
 *
 * @param[in, out] conf CC_DequeConf structure that is being initialized
 */
void cc_deque_conf_init(CC_DequeConf *conf)
{
    conf->capacity   = DEFAULT_CAPACITY;
    conf->mem_alloc  = malloc;
    conf->mem_calloc = calloc;
    conf->mem_free   = free;
}

/**
 * Destroys the CC_Deque structure, but leaves the data it used to hold, intact.
 *
 * @param[in] deque CC_Deque that is to be destroyed
 */
void cc_deque_destroy(CC_Deque *deque)
{
    deque->mem_free(deque->buffer);
    deque->mem_free(deque);
}

/**
 * Destroys the CC_Deque structure along with all the data it holds.
 *
 * @note
 * This function should not be called on a CC_Deque that has some of its elements
 * allocated on the stack.
 *
 * @param[in] deque CC_Deque that is to be destroyed
 */
void cc_deque_destroy_cb(CC_Deque *deque, void (*cb) (void*))
{
    cc_deque_remove_all_cb(deque, cb);
    cc_deque_destroy(deque);
}

/**
 * Adds a new element to the deque. The element is appended to the deque making
 * it the last element (the one with the highest index) of the CC_Deque.
 *
 * @param[in] deque CC_Deque to which the element is being added
 * @param[in] element element that is being added
 *
 * @return CC_OK if the element was successfully added, or CC_ERR_ALLOC if the
 * memory allocation for the new element has failed.
 */
enum cc_stat cc_deque_add(CC_Deque *deque, void *element)
{
    return cc_deque_add_last(deque, element);
}

/**
 * Adds a new element to the front of the CC_Deque.
 *
 * @param[in] deque CC_Deque to which the element is being added
 * @param[in] element element that is being added
 *
 * @return CC_OK if the element was successfully added, or CC_ERR_ALLOC if the
 * memory allocation for the new element has failed.
 */
enum cc_stat cc_deque_add_first(CC_Deque *deque, void *element)
{
    if (deque->size >= deque->capacity && expand_capacity(deque) != CC_OK)
        return CC_ERR_ALLOC;

    deque->first = (deque->first - 1) & (deque->capacity - 1);
    deque->buffer[deque->first] = element;
    deque->size++;

    return CC_OK;
}

/**
 * Adds a new element to the back of the CC_Deque.
 *
 * @param[in] deque the CC_Deque to which the element is being added
 * @param[in] element the element that is being added
 *
 * @return CC_OK if the element was successfully added, or CC_ERR_ALLOC if the
 * memory allocation for the new element has failed.
 */
enum cc_stat cc_deque_add_last(CC_Deque *deque, void *element)
{
    if (deque->capacity == deque->size && expand_capacity(deque) != CC_OK)
        return CC_ERR_ALLOC;

    deque->buffer[deque->last] = element;
    deque->last = (deque->last + 1) & (deque->capacity - 1);
    deque->size++;

    return CC_OK;
}

/**
 * Inserts a new element at the specified index within the deque. The index
 * must be within the range of the CC_Deque.
 *
 * @param[in] deque CC_Deque to which this new element is being added
 * @param[in] element element that is being added
 * @param[in] index position within the CC_Deque at which this new element is
 *                  is being added
 *
 * @return CC_OK if the element was successfully added, CC_ERR_OUT_OF_RANGE if
 * the specified index was not in range, or CC_ERR_ALLOC if the memory
 * allocation for the new element failed.
 */
enum cc_stat cc_deque_add_at(CC_Deque *deque, void *element, size_t index)
{
    if (index >= deque->size)
        return CC_ERR_OUT_OF_RANGE;

    if (deque->capacity == deque->size && expand_capacity(deque) != CC_OK)
        return CC_ERR_ALLOC;

    const size_t c = deque->capacity - 1;
    const size_t l = deque->last & c;
    const size_t f = deque->first & c;
    const size_t p = (deque->first + index) & c;

    if (index == 0)
        return cc_deque_add_first(deque, element);

    if (index == c)
        return cc_deque_add_last(deque, element);

    if (index <= (deque->size / 2) - 1) {
        if (p < f || f == 0) {
            /* _________________________________
             * | 1 | 2 | 3 | 4 | 5 | . | . | 6 |
             * ---------------------------------
             *  (p) <--          L           F
             *
             * Left circular shift from (p)
             */
            const size_t r_move = (f != 0) ? c - f + 1 : 0;
            const size_t l_move = p;

            void *e_first = deque->buffer[0];

            if (f != 0) {
                memmove(&(deque->buffer[f - 1]),
                        &(deque->buffer[f]),
                        r_move * sizeof(void*));
            }
            if (p != 0) {
                memmove(&(deque->buffer[0]),
                        &(deque->buffer[1]),
                        l_move * sizeof(void*));
            }
            deque->buffer[c] = e_first;
        } else {
            memmove(&(deque->buffer[f - 1]),
                    &(deque->buffer[f]),
                    index * sizeof(void*));
        }
        deque->first = (deque->first - 1) & c;
    } else {
        if (p > l || l == c) {
            /* _________________________________
             * | 1 | . | . | 6 | 5 | 4 | 3 | 2 |
             * ---------------------------------
             *   L           F          (p) -->
             *
             * Circular right shift from (p)
             */
            void* e_last = deque->buffer[c];

            if (p != c) {
                memmove(&(deque->buffer[p + 1]),
                        &(deque->buffer[p]),
                        (c - p) * sizeof(void*));
            }
            if (l != c) {
                memmove(&(deque->buffer[1]),
                        &(deque->buffer[0]),
                        (l + 1) * sizeof(void*));
            }
            deque->buffer[0] = e_last;
        } else {
            memmove(&(deque->buffer[p + 1]),
                    &(deque->buffer[p]),
                    (deque->size - index) * sizeof(void*));
        }
        deque->last = (deque->last + 1) & c;
    }
    deque->buffer[p] = element;
    deque->size++;

    return CC_OK;
}

/**
 * Replaces a deque element at the specified index and optionally sets the out
 * parameter to the value of the replaced element. The specified index must be
 * within the bounds of the CC_Deque.
 *
 * @param[in] deque the deque whose element is being replaced
 * @param[in] element the replacement element
 * @param[in] index the index at which the replacement element should be inserted
 * @param[out] out     Pointer to where the replaced element is stored, or NULL if
 *                     it is to be ignored
 *
 * @return CC_OK if the element was successfully replaced, or CC_ERR_OUT_OF_RANGE
 *         if the index was out of range.
 */
enum cc_stat cc_deque_replace_at(CC_Deque *deque, void *element, size_t index, void **out)
{
    if (index >= deque->size)
        return CC_ERR_OUT_OF_RANGE;

    size_t i = (deque->first + index) & (deque->capacity - 1);

    if (out)
        *out = deque->buffer[i];

    deque->buffer[i] = element;

    return CC_OK;
}

/**
 * Removes the specified element from the deque if such element exists and
 * optionally sets the out parameter to the value of the removed element.
 *
 * @param[in] deque the deque from which the element is being removed
 * @param[in] element the element being removed
 * @param[out] out Pointer to where the removed value is stored, or NULL
 *                 if it is to be ignored
 *
 * @return CC_OK if the element was successfully removed, or
 * CC_ERR_VALUE_NOT_FOUND if the element was not found.
 */
enum cc_stat cc_deque_remove(CC_Deque *deque, void *element, void **out)
{
    size_t index;
    enum cc_stat status = cc_deque_index_of(deque, element, &index);

    if (status != CC_OK)
        return status;

    return cc_deque_remove_at(deque, index, out);
}

/**
 * Removes a CC_Deque element from the specified index and optionally sets the
 * out parameter to the value of the removed element. The index must  be within
 * the bounds of the deque.
 *
 * @param[in] deque the deque from which the element is being removed
 * @param[in] index the index of the element being removed
 * @param[out] out  Pointer to where the removed value is stored,
 *                  or NULL if it is to be ignored
 *
 * @return CC_OK if the element was successfully removed, or CC_ERR_OUT_OF_RANGE
 * if the index was out of range.
 */
enum cc_stat cc_deque_remove_at(CC_Deque *deque, size_t index, void **out)
{
    if (index >= deque->size)
        return CC_ERR_OUT_OF_RANGE;

    const size_t c = deque->capacity - 1;
    const size_t l = deque->last & c;
    const size_t f = deque->first & c;
    const size_t p = (deque->first + index) & c;

    void *removed  = deque->buffer[index];

    if (index == 0)
        return cc_deque_remove_first(deque, out);

    if (index == c)
        return cc_deque_remove_last(deque, out);

    if (index <= (deque->size / 2) - 1) {
        if (p < f) {
            void *e = deque->buffer[c];

            if (f != c) {
                memmove(&(deque->buffer[f + 1]),
                        &(deque->buffer[f]),
                        (c - f) * sizeof(void*));
            }
            if (p != 0) {
                memmove(&(deque->buffer[1]),
                        &(deque->buffer[0]),
                        p * sizeof(void*));
            }
            deque->buffer[0] = e;
        } else {
            memmove(&(deque->buffer[f + 1]),
                    &(deque->buffer[f]),
                    index * sizeof(void*));
        }
        deque->first = (deque->first + 1) & c;
    } else {
        if (p > l) {
            void *e = deque->buffer[0];

            if (p != c) {
                memmove(&(deque->buffer[p]),
                        &(deque->buffer[p + 1]),
                        (c - p) * sizeof(void*));
            }
            if (p != 0) {
                memmove(&(deque->buffer[1]),
                        &(deque->buffer[0]),
                        l * sizeof(void*));
            }
            deque->buffer[c] = e;
        } else {
            memmove(&(deque->buffer[p]),
                    &(deque->buffer[p + 1]),
                    (l - p) * sizeof(void*));
        }
        deque->last = (deque->last- 1) & c;
    }
    deque->size--;

    if (out)
        *out = removed;
    return CC_OK;
}

/**
 * Removes the first element of the deque and optionally sets the out parameter
 * to the value of the removed element.
 *
 * @param[in] deque the deque whose first element (or head) is being removed
 * @param[out] out Pointer to where the removed value is stored, or NULL if it is
 *                 to be ignored
 *
 * @return CC_OK if the element was successfully removed, or CC_ERR_OUT_OF_RANGE
 * if the CC_Deque is already empty.
 */
enum cc_stat cc_deque_remove_first(CC_Deque *deque, void **out)
{
    if (deque->size == 0)
        return CC_ERR_OUT_OF_RANGE;

    void *element = deque->buffer[deque->first];
    deque->first = (deque->first + 1) & (deque->capacity - 1);
    deque->size--;

    if (out)
        *out = element;

    return CC_OK;
}

/**
 * Removes the last element of the deque and optionally sets the out parameter
 * to the value of the removed element.
 *
 * @param[in] deque the deque whose last element (or tail) is being removed
 *
 * @return CC_OK if the element was successfully removed, or CC_ERR_OUT_OF_RANGE
 * if the CC_Deque is already empty.
 */
enum cc_stat cc_deque_remove_last(CC_Deque *deque, void **out)
{
    if (deque->size == 0)
        return CC_ERR_OUT_OF_RANGE;

    size_t  last    = (deque->last - 1) & (deque->capacity - 1);
    void   *element = deque->buffer[last];
    deque->last = last;
    deque->size--;

    if (out)
        *out = element;

    return CC_OK;
}

/**
 * Removes all elements from the CC_Deque.
 *
 * @note This function does not shrink the CC_Deque's capacity.

 * @param[in] deque CC_Deque from which all element are being removed
 */
void cc_deque_remove_all(CC_Deque *deque)
{
    deque->first = 0;
    deque->last  = 0;
    deque->size  = 0;
}

/**
 * Removes and frees all element from the specified CC_Deque.
 *
 * @note This function does not shrink the CC_Deque's capacity.
 * @note This function should not be called on CC_Deques that have some
 *       of their elements allocated on stack.
 *
 * @param[in] deque CC_Deque from which all elements are being removed
 */
void cc_deque_remove_all_cb(CC_Deque *deque, void (*cb) (void*))
{
    cc_deque_foreach(deque, cb);
    cc_deque_remove_all(deque);
}

/**
 * Gets a CC_Deque element from the specified index and sets the out parameter to
 * its value. The specified index must be withing the bounds of the deque.
 *
 * @param[in] deque CC_Deque from which the element is being returned
 * @param[in] index index of the CC_Deque element
 * @param[out] out Pointer to where the element is stored
 *
 * @return CC_OK if the element was found, or CC_ERR_OUT_OF_RANGE if the index
 * was out of range.
 */
enum cc_stat cc_deque_get_at(CC_Deque const * const deque, size_t index, void **out)
{
    if (index > deque->size)
        return CC_ERR_OUT_OF_RANGE;

    size_t i = (deque->first + index) & (deque->capacity - 1);
    *out = deque->buffer[i];
    return CC_OK;
}

/**
 * Gets the first (head) element of the CC_Deque.
 *
 * @param[in] deque CC_Deque whose first element is being returned
 * @param[out] out Pointer to where the element is stored
 *
 * @return CC_OK if the element was found, or CC_ERR_OUT_OF_RANGE if the
 * CC_Deque is empty.
 */
enum cc_stat cc_deque_get_first(CC_Deque const * const deque, void **out)
{
    if (deque->size == 0)
        return CC_ERR_OUT_OF_RANGE;

    *out = deque->buffer[deque->first];
    return CC_OK;
}

/**
 * Returns the last (tail) element of the CC_Deque.
 *
 * @param[in] deque the deque whose last element is being returned
 * @param[out] out Pointer to where the element is stored
 *
 * @return CC_OK if the element was found, or CC_ERR_OUT_OF_RANGE if the
 * CC_Deque is empty.
 */
enum cc_stat cc_deque_get_last(CC_Deque const * const deque, void **out)
{
    if (deque->size == 0)
        return CC_ERR_OUT_OF_RANGE;

    size_t last = (deque->last - 1) & (deque->capacity - 1);
    *out = deque->buffer[last];
    return CC_OK;
}

/**
 * Creates a shallow copy of the specified CC_Deque. A shallow copy is a copy of
 * the deque structure, but not the elements it holds.
 *
 * @note The new CC_Deque is allocated using the original CC_Deques's allocators
 *       and it also inherits the configuration of the original CC_Deque.
 *
 * @param[in] deque CC_Deque to be copied
 * @param[out] out Pointer to where the newly created copy is stored
 *
 * @return CC_OK if the copy was successfully created, or CC_ERR_ALLOC if the
 * memory allocation for the copy failed.
 */
enum cc_stat cc_deque_copy_shallow(CC_Deque const * const deque, CC_Deque **out)
{
    CC_Deque *copy = deque->mem_alloc(sizeof(CC_Deque));

    if (!copy)
        return CC_ERR_ALLOC;

    if (!(copy->buffer = deque->mem_alloc(deque->capacity * sizeof(void*)))) {
        deque->mem_free(copy);
        return CC_ERR_ALLOC;
    }
    copy->size       = deque->size;
    copy->capacity   = deque->capacity;
    copy->mem_alloc  = deque->mem_alloc;
    copy->mem_calloc = deque->mem_calloc;
    copy->mem_free   = deque->mem_free;

    copy_buffer(deque, copy->buffer, NULL);

    copy->first = 0;
    copy->last  = copy->size;

    *out = copy;
    return CC_OK;
}

/**
 * Creates a deep copy of the specified CC_Deque. A deep copy is a copy of
 * both the CC_Deque structure and the data it holds.
 *
 * @note The new CC_Deque is allocated using the original CC_Deque's allocators
 *       and also inherits the configuration of the original CC_Deque.
 *
 * @param[in] deque the deque to be copied
 * @param[in] cp   the copy function that should return a pointer to the copy of
 *                 the data.
 * @param[out] out Pointer to where the newly created copy is stored
 *
 * @return CC_OK if the copy was successfully created, or CC_ERR_ALLOC if the
 * memory allocation for the copy failed.
 */
enum cc_stat cc_deque_copy_deep(CC_Deque const * const deque, void *(*cp) (void*), CC_Deque **out)
{
    CC_Deque *copy = deque->mem_alloc(sizeof(CC_Deque));

    if (!copy)
        return CC_ERR_ALLOC;

    if (!(copy->buffer = deque->mem_alloc(deque->capacity * sizeof(void*)))) {
        deque->mem_free(copy);
        return CC_ERR_ALLOC;
    }

    copy->size       = deque->size;
    copy->capacity   = deque->capacity;
    copy->mem_alloc  = deque->mem_alloc;
    copy->mem_calloc = deque->mem_calloc;
    copy->mem_free   = deque->mem_free;

    copy_buffer(deque, copy->buffer, cp);

    copy->first = 0;
    copy->last  = copy->size;

    *out = copy;

    return CC_OK;
}

/**
 * Trims the capacity of the deque to a power of 2 that is the nearest
 * upper power of 2 to the number of elements in the deque.
 *
 * @param[in] deque CC_Deque whose capacity is being trimmed
 *
 * @return CC_OK if the capacity was trimmed successfully, or CC_ERR_ALLOC if
 * the reallocation failed.
 */
enum cc_stat cc_deque_trim_capacity(CC_Deque *deque)
{
    if (deque->capacity == deque->size)
        return CC_OK;

    size_t new_size = upper_pow_two(deque->size);

    if (new_size == deque->capacity)
        return CC_OK;

    void **new_buff = deque->mem_alloc(sizeof(void*) * new_size);

    if (!new_buff)
        return CC_ERR_ALLOC;

    copy_buffer(deque, new_buff, NULL);
    deque->mem_free(deque->buffer);

    deque->buffer   = new_buff;
    deque->first    = 0;
    deque->last     = deque->size;
    deque->capacity = new_size;
    return CC_OK;
}

/**
 * Reverses the order of elements in the specified deque.
 *
 * @param[in] deque the deque that is being reversed
 */
void cc_deque_reverse(CC_Deque *deque)
{
    size_t i;
    size_t j;
    size_t s = deque->size;
    size_t c = deque->capacity - 1;

    size_t first = deque->first;

    for (i = 0, j = s - 1; i < (s - 1) / 2; i++, j--) {
        size_t f = (first + i) & c;
        size_t l = (first + j) & c;

        void *tmp = deque->buffer[f];
        deque->buffer[f] = deque->buffer[l];
        deque->buffer[l] = tmp;
    }
}

/**
 * Returns the number of occurrences of the element within the specified CC_Deque.
 *
 * @param[in] deque CC_Deque that is being searched
 * @param[in] element the element that is being searched for
 *
 * @return the number of occurrences of the element
 */
size_t cc_deque_contains(CC_Deque const * const deque, const void *element)
{
    size_t i;
    size_t o = 0;

    for (i = 0; i < deque->size; i++) {
        size_t p = (deque->first + i) & (deque->capacity - 1);
        if (deque->buffer[p] == element)
            o++;
    }
    return o;
}

/**
 * Returns the number of occurrences of the value poined to by <code>element</code>
 * within the deque.
 *
 * @param[in] deque CC_Deque that is being searched
 * @param[in] element the element that is being searched for
 * @param[in] cmp Comparator function which returns 0 if the values passed to it are equal
 *
 * @return the number of occurrences of the element
 */
size_t cc_deque_contains_value(CC_Deque const * const deque, const void *element, int (*cmp) (const void*, const void*))
{
    size_t i;
    size_t o = 0;

    for (i = 0; i < deque->size; i++) {
        size_t p = (deque->first + i) & (deque->capacity - 1);
        if (cmp(deque->buffer[p], element) == 0)
            o++;
    }
    return o;
}

/**
 * Gets the index of the specified element. The returned index is the index
 * of the first occurrence of the element starting from the beginning of the
 * CC_Deque.
 *
 * @param[in] deque deque being searched
 * @param[in] element the element whose index is being looked up
 * @param[out] index  Pointer to where the index is stored
 *
 * @return CC_OK if the index was found, or CC_OUT_OF_RANGE if not.
 */
enum cc_stat cc_deque_index_of(CC_Deque const * const deque, const void *element, size_t *index)
{
    size_t i;

    for (i = 0; i < deque->size; i++) {
        size_t p = (deque->first + i) & (deque->capacity - 1);
        if (deque->buffer[p] == element) {
            *index = i;
            return CC_OK;
        }
    }
    return CC_ERR_OUT_OF_RANGE;
}

/**
 * Returns the size of the specified CC_Deque. The size of the CC_Deque is the
 * number of elements contained within the CC_Deque.
 *
 * @param[in] deque CC_Deque whose size is being returned
 *
 * @return the number of elements within the specified CC_Deque
 */
size_t cc_deque_size(CC_Deque const * const deque)
{
    return deque->size;
}

/**
 * Retruns the capacity of the specified deque. The capacity of the deque is
 * the maximum number of elements a CC_Deque can hold before its underlying buffer
 * needs to be resized.
 *
 * @param[in] deque CC_Deque whose capacity is being returned
 *
 * @return the capacity of the specified CC_Deque
 */
size_t cc_deque_capacity(CC_Deque const * const deque)
{
    return deque->capacity;
}

/**
 * Return the underlying deque buffer.
 *
 * @note Any direct modification of the buffer may invalidate the CC_Deque.
 *
 * @param[in] deque the deque whose underlying buffer is being returned
 *
 * @return CC_Deques internal buffer
 */
const void* const *cc_deque_get_buffer(CC_Deque const * const deque)
{
    return (const void* const*) deque->buffer;
}

/**
 * Applies the function fn to each element of the CC_Deque.
 *
 * @param[in] deque the deque on which this operation is performed
 * @param[in] fn    the operation function that is to be invoked on each CC_Deque
 *                  element
 */
void cc_deque_foreach(CC_Deque *deque, void (*fn) (void *))
{
    size_t i;

    for (i = 0; i < deque->size; i++) {
        size_t p = (deque->first + i) & (deque->capacity - 1);
        fn(deque->buffer[p]);
    }
}

/**
 * Filters the CC_Deque by modifying it. It removes all elements that don't
 * return true on pred(element).
 *
 * @param[in] deque deque that is to be filtered
 * @param[in] pred  predicate function which returns true if the element should
 *                  be kept in the CC_Deque
 *
 * @return CC_OK if the deque was filtered successfully, or CC_ERR_OUT_OF_RANGE
 * if the CC_Deque is empty.
 */
enum cc_stat cc_deque_filter_mut(CC_Deque *deque, bool (*pred) (const void*))
{
    if (cc_deque_size(deque) == 0)
        return CC_ERR_OUT_OF_RANGE;

    size_t i = 0, c = deque->capacity - 1;

    while (i < cc_deque_size(deque)) {
        size_t d_index = (deque->first + i) & c;

        if (!pred(deque->buffer[d_index])) {
            cc_deque_remove_at(deque, i, NULL);
        } else {
            i++;
        }
    }

    return CC_OK;
}

/**
 * Filters the CC_Deque by creating a new CC_Deque that contains all elements from the
 * original CC_Deque that return true on pred(element) without modifying the original
 * deque.
 *
 * @param[in] deque deque that is to be filtered
 * @param[in] deque predicate function which returns true if the element should
 *                  be kept in the filtered deque
 * @param[out] out pointer to where the new filtered deque is to be stored
 *
 * @return CC_OK if the deque was filtered successfully, CC_ERR_OUT_OF_RANGE
 * if the deque is empty, or CC_ERR_ALLOC if the memory allocation for the
 * new deque failed.
 */
enum cc_stat cc_deque_filter(CC_Deque *deque, bool (*pred) (const void*), CC_Deque **out)
{
    if (cc_deque_size(deque) == 0)
        return CC_ERR_OUT_OF_RANGE;

    size_t i;
    CC_Deque *filtered = NULL;
    cc_deque_new(&filtered);

    if (!filtered)
        return CC_ERR_ALLOC;

    for (i = 0; i < deque->size; i++) {
        size_t d_index = (deque->first + i) & (deque->capacity - 1);

        if (pred(deque->buffer[d_index])) {
            cc_deque_add(filtered, deque->buffer[d_index]);
        }
    }

    *out = filtered;
    return CC_OK;
}

/**
 * Copies the elements from the CC_Deque's buffer to the buffer buff. This function
 * only copies the elements instead of the whole buffer and also realigns the buffer
 * in the process.
 *
 * @param[in] deque The deque whose buffer is being copied to the new buffer
 * @param[in] buff The destination buffer. This buffer is expected to be at least the
 *            capacity of the original deque's buffer or greater.
 * @param[in] cp An optional copy function that returns a copy of the element passed to it.
 *            If NULL is passed, then only a shallow copy will be performed.
 */
static void copy_buffer(CC_Deque const * const deque, void **buff, void *(*cp) (void *))
{
    if (cp == NULL) {
        if (deque->last > deque->first) {
            memcpy(buff,
                   &(deque->buffer[deque->first]),
                   deque->size * sizeof(void*));
        } else {
            size_t l = deque->last;
            size_t e = deque->capacity - deque->first;

            memcpy(buff,
                   &(deque->buffer[deque->first]),
                   e * sizeof(void*));

            memcpy(&(buff[e]),
                   deque->buffer,
                   l * sizeof(void*));
        }
    } else {
        size_t i;
        for (i = 0; i < deque->size; i++) {
            size_t p = (deque->first + i) & (deque->capacity - 1);
            buff[i]  = cp(deque->buffer[p]);
        }
    }
}

/**
 * Expands the deque capacity. This operation might fail if the new buffer
 * cannot be allocated. If the capacity is already the maximum capacity,
 * no new buffer is allocated.
 *
 * @param[in] deque the deque whose capacity is being expanded
 *
 * @return CC_OK if the buffer was expanded successfully, CC_ERR_ALLOC if
 * the memory allocation for the new buffer failed, or CC_ERR_MAX_CAPACITY
 * if the CC_Deque is already at maximum capacity.
 */
static enum cc_stat expand_capacity(CC_Deque *deque)
{
    if (deque->capacity == MAX_POW_TWO)
        return CC_ERR_MAX_CAPACITY;

    size_t new_capacity = deque->capacity << 1;
    void **new_buffer = deque->mem_calloc(new_capacity, sizeof(void*));

    if (!new_buffer)
        return CC_ERR_ALLOC;

    copy_buffer(deque, new_buffer, NULL);
    deque->mem_free(deque->buffer);

    deque->first    = 0;
    deque->last     = deque->size;
    deque->capacity = new_capacity;
    deque->buffer   = new_buffer;

    return CC_OK;
}

/**
 * Rounds the integer to the nearest upper power of two.
 *
 * @param[in] the unsigned integer that is being rounded
 *
 * @return the nearest upper power of two
 */
static CC_INLINE size_t upper_pow_two(size_t n)
{
    if (n >= MAX_POW_TWO)
        return MAX_POW_TWO;

    if (n == 0)
        return 2;

    /**
     * taken from:
     * http://graphics.stanford.edu/~seander/
     * bithacks.html#RoundUpPowerOf2Float
     */
    n--;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    n++;

    return n;
}

/**
 * Initializes the iterator.
 *
 * @param[in] iter the iterator that is being initialized
 * @param[in] deque the vector to iterate over
 */
void cc_deque_iter_init(CC_DequeIter *iter, CC_Deque *deque)
{
    iter->deque = deque;
    iter->index = 0;
    iter->last_removed = false;
}

/**
 * Advances the iterator and sets the out parameter to the value of the
 * next element in the sequence.
 *
 * @param[in] iter the iterator that is being advanced
 * @param[out] out Pointer to where the next element is set
 *
 * @return CC_OK if the iterator was advanced, or CC_ITER_END if the
 * end of the CC_Deque has been reached.
 */
enum cc_stat cc_deque_iter_next(CC_DequeIter *iter, void **out)
{
    const size_t c     = (iter->deque->capacity - 1);
    const size_t last  = (iter->deque->last) & c;
    const size_t first = (iter->deque->first) & c;

    if (last == first || iter->index >= iter->deque->size)
        return CC_ITER_END;

    const size_t i = (iter->deque->first + iter->index) & c;

    iter->index++;
    iter->last_removed = false;
    *out = iter->deque->buffer[i];

    return CC_OK;
}

/**
 * Removes the last returned element by <code>cc_deque_iter_next()</code>
 * function without invalidating the iterator and optionally sets the out
 * parameter to the value of the removed element.
 *
 * @note This function should only ever be called after a call to <code>
 * cc_deque_iter_next()</code>

 * @param[out] out Pointer to where the removed element is stored, or NULL
 *                 if it is to be ignored
 * @param[in] iter the iterator on which this operation is being performed
 *
 * @return CC_OK if the element was successfully removed, CC_ERR_OUT_OF_RANGE
 * if the iterator state is invalid, or CC_ERR_VALUE_NOT_FOUND if the value
 * was already removed.
 */
enum cc_stat cc_deque_iter_remove(CC_DequeIter *iter, void **out)
{
    if (iter->last_removed)
        return CC_ERR_VALUE_NOT_FOUND;

    void *rm;
    enum cc_stat status = cc_deque_remove_at(iter->deque, iter->index, &rm);
    if (status == CC_OK) {
        iter->index--;
        iter->last_removed = true;
        if (out)
            *out = rm;
    }
    return status;
}

/**
 * Adds a new element to the CC_Deque after the last returned element by
 * <code>cc_deque_iter_next()</code> function without invalidating the
 * iterator.
 *
 * @note This function should only ever be called after a call to <code>
 * cc_deque_iter_next()</code>
 *
 * @param[in] iter the iterator on which this operation is being performed
 * @param[in] element the element being added
 *
 * @return CC_OK if the element was successfully added, or CC_ERR_ALLOC
 * if the memory allocation for the new element failed.
 */
enum cc_stat cc_deque_iter_add(CC_DequeIter *iter, void *element)
{
    enum cc_stat status = cc_deque_add_at(iter->deque, element, iter->index);
    if (status == CC_OK)
        iter->index++;

    return status;
}

/**
 * Replaces the last returned element by <code>cc_deque_iter_next()</code>
 * with the specified element and optionally sets the out parameter to
 * the value of the replaced element.
 *
 * @note This function should only ever be called after a call to <code>
 * cc_deque_iter_next()</code>
 *
 * @param[in] iter the iterator on which this operation is being performed
 * @param[in] element the replacement element
 * @param[out] out Pointer to where the replaced element is stored, or NULL
 *                if it is to be ignored
 *
 * @return  CC_OK if the element was replaced successfully, or
 * CC_ERR_VALUE_NOT_FOUND.
 */
enum cc_stat cc_deque_iter_replace(CC_DequeIter *iter, void *replacement, void **out)
{
    return cc_deque_replace_at(iter->deque, replacement, iter->index, out);
}

/**
 * Returns the index of the last returned element by <code>cc_deque_iter_next()
 * </code>.
 *
 * @note
 * This function should not be called before a call to <code>cc_deque_iter_next()
 * </code>
 *
 * @param[in] iter the iterator on which this operation is being performed
 *
 * @return the index
 */
size_t cc_deque_iter_index(CC_DequeIter *iter)
{
    return iter->index - 1;
}

/**
 * Initializes the zip iterator.
 *
 * @param[in] iter Iterator that is being initialized
 * @param[in] ar1  First deque
 * @param[in] ar2  Second deque
 */
void cc_deque_zip_iter_init(CC_DequeZipIter *iter, CC_Deque *d1, CC_Deque *d2)
{
    iter->d1    = d1;
    iter->d2    = d2;
    iter->index = 0;
    iter->last_removed = false;
}

/**
 * Outputs the next element pair in the sequence and advances the iterator.
 *
 * @param[in]  iter Iterator that is being advanced
 * @param[out] out1 Output of the first deque element
 * @param[out] out2 Output of the second deque element
 *
 * @return CC_OK if a next element pair is returned, or CC_ITER_END if the end of one
 * of the deques has been reached.
 */
enum cc_stat cc_deque_zip_iter_next(CC_DequeZipIter *iter, void **out1, void **out2)
{
    const size_t d1_capacity = (iter->d1->capacity - 1);
    const size_t d1_last     = (iter->d1->last) & d1_capacity;
    const size_t d1_first    = (iter->d1->first) & d1_capacity;

    if (d1_last == d1_first || iter->index >= iter->d1->size)
        return CC_ITER_END;

    const size_t d2_capacity = (iter->d2->capacity - 1);
    const size_t d2_last     = (iter->d2->last) & d2_capacity;
    const size_t d2_first    = (iter->d2->first) & d2_capacity;

    if (d2_last == d2_first || iter->index >= iter->d2->size)
         return CC_ITER_END;

    const size_t d1_index = (iter->d1->first + iter->index) & d1_capacity;
    const size_t d2_index = (iter->d2->first + iter->index) & d2_capacity;

    *out1 = iter->d1->buffer[d1_index];
    *out2 = iter->d2->buffer[d2_index];

    iter->index++;
    iter->last_removed = false;

    return CC_OK;
}

/**
 * Adds a new element pair to the deques after the last returned element pair by
 * <code>cc_deque_zip_iter_next()</code> and immediately before an element pair
 * that would be returned by a subsequent call to <code>cc_deque_zip_iter_next()</code>
 * without invalidating the iterator.
 *
 * @param[in] iter Iterator on which this operation is being performed
 * @param[in] e1   element added to the first deque
 * @param[in] e2   element added to the second deque
 *
 * @return CC_OK if the element pair was successfully added to the deques, or
 * CC_ERR_ALLOC if the memory allocation for the new elements failed.
 */
enum cc_stat cc_deque_zip_iter_add(CC_DequeZipIter *iter, void *e1, void *e2)
{
    if (iter->index >= iter->d1->size || iter->index >= iter->d2->size)
        return CC_ERR_OUT_OF_RANGE;

    /* While this check is performed by a call to cc_deque_add_at, it is necessary to know
       in advance whether both deque buffers have enough room before inserting new elements
       because this operation must insert either both elements, or none.*/
    if ((iter->d1->capacity == iter->d1->size && expand_capacity(iter->d1) != CC_OK) &&
        (iter->d2->capacity == iter->d2->size && expand_capacity(iter->d2) != CC_OK)) {
        return CC_ERR_ALLOC;
    }

    /* The retun status can be ignored since the checks have already been made. */
    cc_deque_add_at(iter->d1, e1, iter->index);
    cc_deque_add_at(iter->d2, e2, iter->index);

    iter->index++;
    return CC_OK;
}

/**
 * Removes and outputs the last returned element pair by <code>cc_deque_zip_iter_next()
 * </code> without invalidating the iterator.
 *
 * @param[in]  iter Iterator on which this operation is being performed
 * @param[out] out1 Output of the removed element from the first deque
 * @param[out] out2 Output of the removed element from the second deque
 *
 * @return CC_OK if the element was successfully removed, CC_ERR_OUT_OF_RANGE if the
 * iterator is in an invalid state, or CC_ERR_VALUE_NOT_FOUND if the value was already
 * removed.
 */
enum cc_stat cc_deque_zip_iter_remove(CC_DequeZipIter *iter, void **out1, void **out2)
{
    if (iter->last_removed)
        return CC_ERR_VALUE_NOT_FOUND;

    if ((iter->index - 1) >= iter->d1->size || (iter->index - 1) >= iter->d2->size)
        return CC_ERR_OUT_OF_RANGE;

    cc_deque_remove_at(iter->d1, iter->index - 1, out1);
    cc_deque_remove_at(iter->d2, iter->index - 1, out2);

    iter->index--;
    iter->last_removed = true;

    return CC_OK;
}

/**
 * Replaces the last returned element pair by <code>cc_deque_zip_iter_next()</code>
 * with the specified replacement element pair.
 *
 * @param[in] iter  Iterator on which this operation is being performed
 * @param[in]  e1   First deque's replacement element
 * @param[in]  e2   Second deque's replacement element
 * @param[out] out1 Output of the replaced element from the first deque
 * @param[out] out2 Output of the replaced element from the second deque
 *
 * @return CC_OK if the element was successfully replaced, or CC_ERR_OUT_OF_RANGE.
 */
enum cc_stat cc_deque_zip_iter_replace(CC_DequeZipIter *iter, void *e1, void *e2, void **out1, void **out2)
{
    if ((iter->index - 1) >= iter->d1->size || (iter->index - 1) >= iter->d2->size)
        return CC_ERR_OUT_OF_RANGE;

    cc_deque_replace_at(iter->d1, e1, iter->index - 1, out1);
    cc_deque_replace_at(iter->d2, e2, iter->index - 1, out2);

    return CC_OK;
}

/**
 * Returns the index of the last returned element pair by <code>cc_deque_zip_iter_next()</code>.
 *
 * @param[in] iter Iterator on which this operation is being performed
 *
 * @return current iterator index
 */
size_t cc_deque_zip_iter_index(CC_DequeZipIter *iter)
{
    return iter->index - 1;
}
