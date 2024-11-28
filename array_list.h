//! A contiguous, growable list of items in memory.
//!
//! ARRAY_LIST_IMPL
//!     Provide definition of functions. Do not use this option multiple times
//!     with the same ARRAY_LIST option.
//! ARRAY_LIST_STATIC
//!     Mark functions as static.
//! ARRAY_LIST_T
//!     Type of the item inside the array list. This is by default int.
//! ARRAY_LIST
//!     Name of the array list structure and prefix for functions. This is
//!     by default Array_List.
//! ARRAY_LIST_GROW_FACTOR
//!     Amount by which capacity will grow when list is resized in append
//!     functions. This is by default 2.

#ifndef ARRAY_LIST_H
#define ARRAY_LIST_H

// clang-format off
#ifndef ARRAY_LIST
#define ARRAY_LIST Array_List
#endif

#ifndef ARRAY_LIST_T
#define ARRAY_LIST_T int
#endif

#ifndef ARRAY_LIST_GROW_FACTOR
#define ARRAY_LIST_GROW_FACTOR 2
#endif

#define CONCATENATE(a, b) CONCATENATE1(a, b)
#define CONCATENATE1(a, b) CONCATENATE2(a, b)
#define CONCATENATE2(a, b) a ## b

#define ARRAY_LIST_INIT CONCATENATE(ARRAY_LIST, _Init)
#define ARRAY_LIST_RESIZE CONCATENATE(ARRAY_LIST, _Resize)
#define ARRAY_LIST_APPEND CONCATENATE(ARRAY_LIST, _Append)
#define ARRAY_LIST_APPEND_N_TIMES CONCATENATE(ARRAY_LIST, _Append_N_Times)
#define ARRAY_LIST_APPEND_BUFFER CONCATENATE(ARRAY_LIST, _Append_Buffer)
#define ARRAY_LIST_APPEND_BUFFER_N_TIMES CONCATENATE(ARRAY_LIST, _Append_Buffer_N_Times)
#define ARRAY_LIST_POP CONCATENATE(ARRAY_LIST, _Pop)
#define ARRAY_LIST_SWAP_REMOVE CONCATENATE(ARRAY_LIST, _Swap_Remove)
#define ARRAY_LIST_SWAP_REMOVE_U CONCATENATE(ARRAY_LIST, _Swap_Remove_U)
#define ARRAY_LIST_ORDERED_REMOVE CONCATENATE(ARRAY_LIST, _Ordered_Remove)
#define ARRAY_LIST_ORDERED_REMOVE_U CONCATENATE(ARRAY_LIST, _Ordered_Remove_U)
#define ARRAY_LIST_CLONE CONCATENATE(ARRAY_LIST, _Clone)
#define ARRAY_LIST_DEINIT_U CONCATENATE(ARRAY_LIST, _Deinit_U)
#define ARRAY_LIST_DEINIT CONCATENATE(ARRAY_LIST, _Deinit)

#ifdef ARRAY_LIST_STATIC
#   define ARRAY_LIST_FN static
#else
#   define ARRAY_LIST_FN extern
#endif

#include <string.h>
#include <stddef.h>
#include <stdlib.h>

struct ARRAY_LIST {
    typeof(ARRAY_LIST_T) *buf;
    size_t len;
    size_t cap;
};

/// Initialize an array list.
/// On failure, buf will be NULL.
ARRAY_LIST_FN struct ARRAY_LIST
ARRAY_LIST_INIT(size_t initial_capacity);

/// Resize list to provided size.
/// On success returns 0.
/// On failure returns non-zero value.
/// Invalidates pointers to elements.
ARRAY_LIST_FN int
ARRAY_LIST_RESIZE(struct ARRAY_LIST *list, size_t new_capacity);

/// Append item to end of list.
/// Resizes if needed, invaldating pointers to list's buffer.
/// On success returns 0.
/// On failure returns non-zero value.
ARRAY_LIST_FN int
ARRAY_LIST_APPEND(struct ARRAY_LIST *list, typeof(ARRAY_LIST_T) item);

/// Append item to end of list N times.
/// Resizes if needed, invaldating pointers to list's buffer.
/// On success returns 0.
/// On failure returns non-zero value.
ARRAY_LIST_FN int
ARRAY_LIST_APPEND_N_TIMES(struct ARRAY_LIST *list, typeof(ARRAY_LIST_T) item, size_t n);

/// Append each item of a buffer to end of list.
/// Resizes if needed, invaldating pointers to list's buffer.
/// On success returns 0.
/// On failure returns non-zero value.
ARRAY_LIST_FN int
ARRAY_LIST_APPEND_BUFFER(struct ARRAY_LIST *list, typeof(ARRAY_LIST_T) *buffer, size_t count);

/// Append each item of a buffer to end of list N times.
/// Resizes if needed, invaldating pointers to list's buffer.
///
/// On success returns 0.
/// On failure returns non-zero value.
ARRAY_LIST_FN int
ARRAY_LIST_APPEND_BUFFER_N_TIMES(struct ARRAY_LIST *list, typeof(ARRAY_LIST_T) *buffer, size_t count, size_t n);

/// Remove last item from array list and fill out with it.
/// Verifies list length is non-zero.
///
/// On success returns 0.
/// On failure returns non-zero value.
ARRAY_LIST_FN int
ARRAY_LIST_POP(struct ARRAY_LIST *list, typeof(ARRAY_LIST_T) *out);

/// Remove item at specified index and fill out with it.
/// Empty slot is filled from the end of the list.
///
/// Verifies list length is non-zero.
/// Verifies index is in bounds.
///
/// On success returns 0.
/// On failure returns non-zero value.
///
/// See Array_List_Ordered_Remove for order preserving removal.
/// See Array_List_Swap_Remove_U for same behavior without safety checks.
ARRAY_LIST_FN int
ARRAY_LIST_SWAP_REMOVE(struct ARRAY_LIST *list, size_t index, typeof(ARRAY_LIST_T) *out);

/// Remove item at specified index and fill out with it.
/// Empty slot is filled from the end of the list.
///
/// See Array_List_Ordered_Remove for order preserving removal.
/// See Array_List_Swap_Remove for same behavior with safety checks.
ARRAY_LIST_FN void
ARRAY_LIST_SWAP_REMOVE_U(struct ARRAY_LIST *list, size_t index, typeof(ARRAY_LIST_T) *out);

/// Remove item at specified index and fill out with it.
/// Empty slot is filled by shifting elements after index.
///
/// Verifies list length is non-zero.
/// Verifies index is in bounds.
///
/// On success returns 0.
/// On failure returns non-zero value.
///
/// See Array_List_Swap_Remove for non-order preserving removal.
/// See Array_List_Ordered_Remove_U for same behavior without safety checks.
ARRAY_LIST_FN int
ARRAY_LIST_ORDERED_REMOVE(struct ARRAY_LIST *list, size_t index, typeof(ARRAY_LIST_T) *out);

/// Remove item at specified index and fill out with it.
/// Empty slot is filled by shifting elements after index.
///
/// On success returns 0.
/// On failure returns non-zero value.
///
/// See Array_List_Swap_Remove for non-order preserving removal.
/// See Array_List_Ordered_Remove for same behavior with safety checks.
ARRAY_LIST_FN void
ARRAY_LIST_ORDERED_REMOVE_U(struct ARRAY_LIST *list, size_t index, typeof(ARRAY_LIST_T) *out);

/// Create a copy of this list.
///
/// On success returns 0.
/// On failure returns non-zero value.
ARRAY_LIST_FN int
ARRAY_LIST_CLONE(struct ARRAY_LIST *list);

/// Frees list's buffer only.
///
/// See Array_List_Deinit for function that frees and zeros.
ARRAY_LIST_FN void
ARRAY_LIST_DEINIT_U(struct ARRAY_LIST *list);

/// Free list's buffer and zero out items.
///
/// See Array_List_Deinit_U for function that only frees.
ARRAY_LIST_FN void
ARRAY_LIST_DEINIT(struct ARRAY_LIST *list);

#ifdef ARRAY_LIST_IMPL

/// Initialize an array list.
/// On failure, buf will be NULL.
ARRAY_LIST_FN struct ARRAY_LIST
ARRAY_LIST_INIT(size_t initial_capacity)
{
    typeof(ARRAY_LIST_T) *buf = malloc(sizeof *buf * initial_capacity);
    return (struct ARRAY_LIST){
        .buf = buf,
        .len = 0,
        .cap = buf ? initial_capacity : 0
    };
}

/// Resize list to provided size.
/// On success returns 0.
/// On failure returns non-zero value.
/// Invalidates pointers to elements.
ARRAY_LIST_FN int
ARRAY_LIST_RESIZE(struct ARRAY_LIST *list, size_t new_capacity)
{
    typeof(ARRAY_LIST_T) *buf_resized = realloc(list->buf, sizeof *buf_resized * new_capacity);
    if (buf_resized < 0) {
        return 1;
    }
    list->buf = buf_resized;
    list->cap = new_capacity;
    if (new_capacity < list->len) {
        list->len = new_capacity;
    }
    return 0;
}

/// Append item to end of list.
/// Resizes if needed, invaldating pointers to list's buffer.
/// On success returns 0.
/// On failure returns non-zero value.
ARRAY_LIST_FN int
ARRAY_LIST_APPEND(struct ARRAY_LIST *list, typeof(ARRAY_LIST_T) item)
{
    if (list->len + 1 > list->cap) {
        size_t new_capacity = list->len * ARRAY_LIST_GROW_FACTOR;
        if (list->len == 0) {
            new_capacity = 4;
        }
        int ret = ARRAY_LIST_RESIZE(list, new_capacity);
        if (ret != 0) {
            return ret;
        }
    }

    list->buf[list->len++] = item;
    return 0;
}

/// Append item to end of list N times.
/// Resizes if needed, invaldating pointers to list's buffer.
/// On success returns 0.
/// On failure returns non-zero value.
ARRAY_LIST_FN int
ARRAY_LIST_APPEND_N_TIMES(struct ARRAY_LIST *list, typeof(ARRAY_LIST_T) item, size_t n)
{
    if (list->len + n > list->cap) {
        int ret = ARRAY_LIST_RESIZE(list, list->len * ARRAY_LIST_GROW_FACTOR);
        if (ret != 0) {
            return ret;
        }
    }

    while (n-- > 0) {
        list->buf[list->len++] = item;
    }
    return 0;
}

/// Append each item of a buffer to end of list.
/// Resizes if needed, invaldating pointers to list's buffer.
/// On success returns 0.
/// On failure returns non-zero value.
ARRAY_LIST_FN int
ARRAY_LIST_APPEND_BUFFER(struct ARRAY_LIST *list, typeof(ARRAY_LIST_T) *buffer, size_t count)
{
    if (list->len + count > list->cap) {
        int ret = ARRAY_LIST_RESIZE(list, list->len * ARRAY_LIST_GROW_FACTOR);
        if (ret != 0) {
            return ret;
        }
    }

    memmove(&list->buf[list->len], buffer, sizeof(*buffer) * count);
    return 0;
}

/// Append each item of a buffer to end of list N times.
/// Resizes if needed, invaldating pointers to list's buffer.
///
/// On success returns 0.
/// On failure returns non-zero value.
ARRAY_LIST_FN int
ARRAY_LIST_APPEND_BUFFER_N_TIMES(struct ARRAY_LIST *list, typeof(ARRAY_LIST_T) *buffer, size_t count, size_t n)
{
    if (list->len + n * count > list->cap) {
        int ret = ARRAY_LIST_RESIZE(list, list->len * ARRAY_LIST_GROW_FACTOR);
        if (ret != 0) {
            return ret;
        }
    }

    while (n-- > 0) {
        memmove(&list->buf[list->len], buffer, sizeof(*buffer) * count);
    }
    return 0;
}

/// Remove last item from array list and fill out with it.
/// Verifies list length is non-zero.
///
/// On success returns 0.
/// On failure returns non-zero value.
ARRAY_LIST_FN int
ARRAY_LIST_POP(struct ARRAY_LIST *list, typeof(ARRAY_LIST_T) *out)
{
    if (list->len == 0) {
        return 1;
    }
    *out = list->buf[--list->len];
    return 0;
}

/// Remove item at specified index and fill out with it.
/// Empty slot is filled from the end of the list.
///
/// Verifies list length is non-zero.
/// Verifies index is in bounds.
///
/// On success returns 0.
/// On failure returns non-zero value.
///
/// See Array_List_Ordered_Remove for order preserving removal.
/// See Array_List_Swap_Remove_U for same behavior without safety checks.
ARRAY_LIST_FN int
ARRAY_LIST_SWAP_REMOVE(struct ARRAY_LIST *list, size_t index, typeof(ARRAY_LIST_T) *out)
{
    if (list->len == 0 || index >= list->len) {
        return 1;
    }
    *out = list->buf[index];
    list->buf[index] = list->buf[--list->len];
    return 0;
}

/// Remove item at specified index and fill out with it.
/// Empty slot is filled from the end of the list.
///
/// See Array_List_Ordered_Remove for order preserving removal.
/// See Array_List_Swap_Remove for same behavior with safety checks.
ARRAY_LIST_FN void
ARRAY_LIST_SWAP_REMOVE_U(struct ARRAY_LIST *list, size_t index, typeof(ARRAY_LIST_T) *out)
{
    *out = list->buf[index];
    list->buf[index] = list->buf[--list->len];
}

/// Remove item at specified index and fill out with it.
/// Empty slot is filled by shifting elements after index.
///
/// Verifies list length is non-zero.
/// Verifies index is in bounds.
///
/// On success returns 0.
/// On failure returns non-zero value.
///
/// See Array_List_Swap_Remove for non-order preserving removal.
/// See Array_List_Ordered_Remove_U for same behavior without safety checks.
ARRAY_LIST_FN int
ARRAY_LIST_ORDERED_REMOVE(struct ARRAY_LIST *list, size_t index, typeof(ARRAY_LIST_T) *out)
{
    if (list->len == 0 || index >= list->len) {
        return 1;
    }
    *out = list->buf[index];
    memmove(&list->buf[index], &list->buf[index + 1], sizeof(*list->buf) * (list->len - index));
    list->buf[index] = list->buf[--list->len];
    return 0;
}

/// Remove item at specified index and fill out with it.
/// Empty slot is filled by shifting elements after index.
///
/// On success returns 0.
/// On failure returns non-zero value.
///
/// See Array_List_Swap_Remove for non-order preserving removal.
/// See Array_List_Ordered_Remove for same behavior with safety checks.
ARRAY_LIST_FN void
ARRAY_LIST_ORDERED_REMOVE_U(struct ARRAY_LIST *list, size_t index, typeof(ARRAY_LIST_T) *out)
{
    *out = list->buf[index];
    memmove(&list->buf[index], &list->buf[index + 1], sizeof(*list->buf) * (list->len - index));
    list->buf[index] = list->buf[--list->len];
}

/// Create a copy of this list.
///
/// On success returns 0.
/// On failure returns non-zero value.
ARRAY_LIST_FN int
ARRAY_LIST_CLONE(struct ARRAY_LIST *list)
{
    struct ARRAY_LIST list_clone = ARRAY_LIST_INIT(list->len);
    if (!list_clone.buf) {
        return 1;
    }
    memcpy(list_clone.buf, list->buf, sizeof(*list->buf) * list->len);
    return 0;
}


/// Frees list's buffer only.
///
/// See Array_List_Deinit for function that frees and zeros.
ARRAY_LIST_FN void
ARRAY_LIST_DEINIT_U(struct ARRAY_LIST *list)
{
    free(list->buf);
}

/// Free list's buffer and zero out items.
///
/// See Array_List_Deinit_U for function that only frees.
ARRAY_LIST_FN void
ARRAY_LIST_DEINIT(struct ARRAY_LIST *list)
{
    free(list->buf);
    *list = (struct ARRAY_LIST){};
}

#endif

#undef ARRAY_LIST_INIT
#undef ARRAY_LIST_RESIZE
#undef ARRAY_LIST_APPEND
#undef ARRAY_LIST_APPEND_N_TIMES
#undef ARRAY_LIST_APPEND_BUFFER
#undef ARRAY_LIST_APPEND_BUFFER_N_TIMES
#undef ARRAY_LIST_POP
#undef ARRAY_LIST_SWAP_REMOVE
#undef ARRAY_LIST_SWAP_REMOVE_U
#undef ARRAY_LIST_ORDERED_REMOVE
#undef ARRAY_LIST_ORDERED_REMOVE_U
#undef ARRAY_LIST_CLONE
#undef ARRAY_LIST_DEINIT_U
#undef ARRAY_LIST_DEINIT

#undef ARRAY_LIST_IMPL
#undef ARRAY_LIST_STATIC
#undef ARRAY_LIST_T
#undef ARRAY_LIST
#undef ARRAY_LIST_GROW_FACTOR

#endif
