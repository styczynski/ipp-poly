/** @file
*
*  Unidirectional list implementation based on arrays (C99 standard)
*  Usage:
*  @code
*     #include <array_lists.h>
*      ...
*     ArrayList l = ArrayListNew();
*     int a = 42;
*     int b = 64;
*
*     ArrayListPushBack(&l, &a);
*     ArrayListPushBack(&l, &b);
*
*     printf("%d", *((int*)ArrayListPopBack(&l))); //Output: 64
*     printf("%d", ArrayListSize(&l)); //Output: 1
*
*     ArrayListDestroy(&l);
*  @endcode
*
*
*  @author Piotr Styczyński <piotrsty1@gmail.com>
*  @copyright MIT
*  @date 2017-05-13
*/
#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "memalloc.h"
#include "generics.h"

#ifndef __STY_COMMON_ARRAY_LISTS_H__
#define __STY_COMMON_ARRAY_LISTS_H__

/**
 * @def ARRAY_LIST_AUTORESIZE_FACTOR
 *
 * The list is always resized to its current size * resize factor
 * when theres no place for elements.
 *
 * This take place only through ArrayListResize calls.
 * See ArrayListResize for further details.
 */
#define ARRAY_LIST_AUTORESIZE_FACTOR 2

/**
* @def LOOP_ARRAY_LIST(LIST, VAR_NAME)
* Macro for interating through ArrayList
*
* Usage:
*
* @code
*   ArrayList l;
*   LOOP_ARRAY_LIST(&l, i) {
*         // Capture ArrayListData from iterator:
*         ArrayListData element = ArrayListsGetValue(i);
*
*         // Convert the ArrayListData to the int pointer:
*         int element_value = *((*int) element_value);
*
*         // Print data from the list:
*         printf("void_ptr = %p\n", element_value);
*    }
* @endcode
*
* @param[in] LIST     : ArrayList name to be iterated
* @param[in] VAR_NAME : name of iterator variable
*/
#define LOOP_ARRAY_LIST(LIST, VAR_NAME) \
  for(ArrayListIterator VAR_NAME = ArrayListBegin(LIST); \
  !ArrayListIsEnd(VAR_NAME); VAR_NAME = ArrayListNext(VAR_NAME))

/*
* Declare data types needed for ArrayLists implementation
*/

/** Data type held by array list */
typedef void* ArrayListData;

/** Type of array list */
typedef struct ArrayList ArrayList;

/**
* ArrayList iterator
*/
typedef struct ArrayListIterator ArrayListIterator;


/**
* Root element of the ArrayList containing pointer
* to the first element and needed metadata like
* size and capacity
*/
struct ArrayList {
  ArrayListData* data; ///< Pointer to the begining of the ArrayList
  int alloc_size; ///< Allocation size of list (capacity)
  int size; ///< Actual size of a list
};

/** Iterator type for array lists **/
struct ArrayListIterator {
  ArrayList* target; ///< Parent list pointer
  int position; ///< Index on the parent list
};

/**
* Function of type
* ArrayListData -> ArrayListData
* Used as ArrayList data manipulators/iterators
*
* @param[in] ArrayListData : modified data
* @return ArrayListData
*/
typedef ArrayListData (*ArrayListModifierFn)(ArrayListData);


/**
* Create new ArrayList
* All ArrayLists must be then freed with ArrayListDestroy(ArrayList).
*
* @return ArrayList
*/
static inline ArrayList ArrayListNew() {
  ArrayList l = (ArrayList){
    .data = NULL,
    .alloc_size = 0,
    .size = 0
  };
  return l;
}

/**
* Function resizing capacity of ArrayList to at least @p min_size.
* The actual allocation size might be greater due to performance
* boost on less allocation invokes.
*
* More details:
*   If the new_size = ARRAY_LIST_AUTORESIZE_FACTOR * current size is
*   enought space i.e. new_size >= min_size
*   then everything is done if not the list is resized more to match the
*   size of min_size.
*
* WARN:
*   No functionality should depend on the size of allocated array list
*   as this function provides only list at least the size of @p min_size.
*
* @param[in] l        : ArrayList*
* @param[in] min_size : const int
*/
void ArrayListResize(ArrayList* l, const int min_size);

/**
* Destroy given ArrayList freeing up memory.
*
* WARN: Invalidates all ArrayListIterators of the list
*
* @param[in] l : ArrayList*
*/
void ArrayListDestroy(ArrayList* l);



/**
* Push @p element to the end of a given ArrayList.
* Method returns iterator to the newly created element.
*
* NOTICE: All ArrayListIterators are valid until used operations do
*         keep pointers validity.
*
* @param[in] l       : ArrayList*
* @param[in] element : ArrayListData
* @return ArrayListIterator
*/
ArrayListIterator ArrayListPushBack( ArrayList* l, ArrayListData element );

/**
* Removes last element of the given ArrayList or does nothing if it's empty.
* Returns data pointer held by the removed element.
* If no element was removed (ArrayList is empty) undefined value is returned.
*
* WARN: Invalidates ArrayListIterators when elment under pointer
*       is popped.
*
* @param[in] l : ArrayList*
* @return ArrayListData (popped element)
*/
ArrayListData ArrayListPopBack( ArrayList* l );

/**
* Clears the entire ArrayList.
*
* WARN:
*   This function does not provide deep deallcoation functionality.
*   If the contents is erased then firstly all elements must be
*   deallocated manually.
*
* WARN: Invalidates ArrayListIterators for all elements of list
*
* @param[in] l : ArrayList*
*/
void ArrayListClear( ArrayList* l );

/**
* Obtain first element of the ArrayList.
* Function return ArrayListData pointer to the data under first element.
*
* If the ArrayList is empty undefined value is returned.
*
* @param[in] l : const ArrayList*
* @return first element if exists
*/
ArrayListData ArrayListFirst( const ArrayList* l );

/**
* Obtain last element of the ArrayList.
* Function return ArrayListData pointer to the data under first element.
*
* If the ArrayList is empty undefined value is returned.
*
* @param[in] l : const ArrayList*
* @return last element if exists
*/
ArrayListData ArrayListLast( const ArrayList* l );

/**
* Obtain the size of a ArrayList.
*
* @param[in] l : const ArrayList*
* @return size of the ArrayList
*/
int ArrayListSize( const ArrayList* l );

/**
* Check if ArrayList is empty.
*
* @param[in] l : const ArrayList*
* @return If ArrayList is empty?
*/
static inline int ArrayListEmpty( const ArrayList* l ) {
  if(l == NULL) return 1;
  return (l->size == 0);
}

/**
* Shallow-copy the ArrayList into a new one.
*
* WARN: Each element will be a new one, but the data
*       pointers will be still pointing to the same
*       memory locations (there're still the same
*       objects under ArrayListData pointers)
*
* NOTE: To create new standalone copy use ArrayListDeepCopy
*
* @param[in] l : const ArrayList*
* @return shallow copy of a given ArrayList
*/
ArrayList ArrayListCopy( const ArrayList* l );

/**
* Performs deep copy of the ArrayList returning new one.
* The given (ArrayListData)->(ArrayListData) function is used as assigner.
* The function should create new element value, copy the value of
* the given one and return pointer to this element.
*
* Example correct data allocator for list of integers:
* @code
*    ArrayListData allocator(ArrayListData data) {
*       // Obtain int* out of given element's data
*       int* elem = (int*) data;
*
*       // Allocate new copy of element
*       int* elem_copy = (int*) malloc(sizeof(int));
*
*       // Copy underlying data into the copy
*       *elem_copy = *elem;
*
*       // Return new standalone copy
*       return (ArrayListData) elem_copy;
*    }
* @endcode
*
* @param[in] l                : const ArrayList*
* @param[in] elementAllocator : ArrayListModifierFn
* @return deep copy of a given ArrayList
*/
ArrayList ArrayListDeepCopy( const ArrayList* l, ArrayListModifierFn elementAllocator );

/**
* Copy the ArrayList into other ArrayList (shallow-copy into target).
*
* WARN: Each element will be a new one, but the data
*       pointers will be still pointing to the same
*       memory locations (there're still the same
*       objects under ArrayListData)
*
* @param[in] source : const ArrayList*
* @param[in] target : ArrayList*
*/
void ArrayListCopyInto( const ArrayList* source, ArrayList* target );

/**
* Iterate through ArrayList using given
* (ArrayListData)->(ArrayListData) function.
* Function is executed for every ArrayList element value
* The return value is ignored.
*
* @param[in] l        : const ArrayList*
* @param[in] iterator : ArrayListModifierFn
*/
void ArrayListIterate( const ArrayList* l, ArrayListModifierFn iterator );

/**
* Map ArrayList values using given
* (ArrayListData)->(ArrayListData) function
* Function is executed for every ArrayList element value
* Then the result of function is assigned to the
* element's data pointer.
*
* NOTICE: Mapping is made in place.
*         There's no additional list created.
*         If you want to swap values you must remember to
*         deallocate needed resources!
*
* @param[in] l       : const ArrayList*
* @param[in] mapping : ArrayListModifierFn
*/
void ArrayListMap( ArrayList* l, ArrayListModifierFn mapping );

/**
* Get the first element iterator.
* If the ArrayList is empty then NULL is returned.
*
* NOTICE: All ArrayListIterators are valid until used operation does not
*         keep pointers validity.
*
* @param[in] l : const ArrayList*
* @return ArrayListIterator pointing to the ArrayList begining
*/
static inline ArrayListIterator ArrayListBegin( const ArrayList* l ) {
  if(l == NULL) return (ArrayListIterator) {
    .target = NULL,
    .position = 0
  };
  if(l->size == 0) return (ArrayListIterator) {
    .target = NULL,
    .position = 0
  };
  return (ArrayListIterator) {
    .target = (ArrayList*) l,
    .position = 0
  };
}

/**
* Get the last element interator.
* If the ArrayList is empty then NULL is returned.
*
* NOTICE: All ArrayListIterators are valid until used operation does not
*         keep pointers validity.
*
* @param[in] l : const ArrayList*
* @return ArrayListIterator pointing to the ArrayList end
*/
static inline ArrayListIterator ArrayListEnd( const ArrayList* l ) {
  if(l == NULL) return (ArrayListIterator) {
    .target = NULL,
    .position = 0
  };
  if(l->size == 0) return (ArrayListIterator) {
    .target = NULL,
    .position = 0
  };
  return (ArrayListIterator) {
    .target = (ArrayList*) l,
    .position = (l->size)-1
  };
}


/**
* Checks if given iterator is the last element
*
* @param[in] iterator : ArrayListIterator
* @return If the iterator is on the ArrayList's end?
*/
static inline int ArrayListIsEnd( ArrayListIterator iterator ) {
  if(iterator.target == NULL) return 1;
  return iterator.position >= ((iterator.target)->size);
}

/**
* Checks if given iterator is the first element.
*
* @param[in] iterator : ArrayListIterator
* @return If the iterator is on the ArrayList's begining?
*/
static inline int ArrayListIsBegin( ArrayListIterator iterator ) {
  if(iterator.target == NULL) return 1;
  return iterator.position <= 0;
}

/**
* Checks if given iterator is the last or first element.
*
* @param[in] iterator : ArrayListIterator
* @return If the iterator is on the left/side of the ArrayList
*/
static inline int ArrayListIsSideElement( ArrayListIterator iterator ) {
  return ArrayListIsBegin(iterator) || ArrayListIsEnd(iterator);
}

/**
* Get next element on the ArrayList.
* Returns NULL if iterator is the last element.
*
* NOTICE: All ArrayListIterators are valid until used operation does not
*         keep pointers validity.
*
* @param[in] iterator : ArrayListIterator
* @return next iterator (the right neighbour of the current iterator)
*/
static inline ArrayListIterator ArrayListNext( ArrayListIterator iterator ) {
  if(ArrayListIsEnd(iterator)) return (ArrayListIterator) { .target = NULL, .position = 0 };
  return (ArrayListIterator) { .target = iterator.target, .position = iterator.position+1 };
}

/**
* Get prevous element on the ArrayList.
* Returns NULL if iterator is the last element.
*
* NOTICE: All ArrayListIterators are valid until used operation does not
*         keep pointers validity.
*
* @param[in] iterator : ArrayListIterator
* @return previous iterator (the left neighbour of the current iterator)
*/
static inline ArrayListIterator ArrayListPrevious( ArrayListIterator iterator ) {
  if(ArrayListIsBegin(iterator)) return (ArrayListIterator) { .target = NULL, .position = 0 };
  return (ArrayListIterator) { .target = iterator.target, .position = iterator.position-1 };
}

/**
* Get value of the ArrayList element. Returns underlying data.
* Returns NULL if element is NULL.
*
* NOTICE: All ArrayListIterators are valid until used operation does not
*         keep pointers validity.
*
* @param[in] iterator : ArrayListIterator
* @return value under the given iterator
*/
static inline ArrayListData ArrayListGetValue( ArrayListIterator iterator ) {
  if(iterator.target == NULL) return NULL;
  if(iterator.position < 0) return NULL;
  return ((iterator.target)->data)[iterator.position];
}

/**
* Sets value of the ArrayList element.
* Does nothing if element is NULL.
*
* NOTICE: All ArrayListIterators are valid until used operation does not
*         keep pointers validity.
*
* @param[in] iterator : ArrayListIterator
* @param[in] value    : ArrayListData
*/
static inline void ArrayListSetValue( ArrayListIterator iterator, ArrayListData value ) {
  if(iterator.target == NULL) return;
  if(iterator.position < 0) return;
  (iterator.target->data)[iterator.position] = value;
}

/**
* Destroy given ArrayList freeing up memory.
*
* WARN: Invalidates ArrayListIterators
*
* @param[in] l           : ArrayList*
* @param[in] deallocator : ArrayListModifierFn
*/
static inline void ArrayListDestroyDeep(ArrayList* l, ArrayListModifierFn deallocator) {
  LOOP_ARRAY_LIST(l, iter) {
    deallocator(ArrayListGetValue(iter));
  }
  ArrayListDestroy(l);
}

/**
* Print given ArrayList to stdout.
* Prints only adresses of values not exact values.
*
* @param[in] l       : const ArrayList*
* @param[in] printer : ArrayListModifierFn
*/
void ArrayListPrint( const ArrayList* l, GenericsPrinter printer );

/**
* Print given ArrayList to stdout.
* Prints only adresses of values not exact values.
* Variant displaying new line at the end of stringified ArrayList.
*
* @param[in] l       : const ArrayList*
* @param[in] printer : ArrayListModifierFn
*/
static inline void ArrayListPrintln( const ArrayList* l, GenericsPrinter printer ) {
  ArrayListPrint(l, printer);
  printf("\n");
}

/**
* Print given ArrayList to stdout.
* Prints only adresses of values not exact values.
*
* @param[in] l : const ArrayList*
*/
static inline void ArrayListPrintData( const ArrayList* l ) {
  ArrayListPrint(l, GenericsPtrPrinter);
}

/**
* Print given ArrayList to stdout.
* Prints only adresses of values not exact values.
* Variant displaying new line at the end of stringified ArrayList.
*
* @param[in] l : const ArrayList*
*/
static inline void ArrayListPrintlnData( const ArrayList* l ) {
  ArrayListPrintData(l);
  printf("\n");
}

#endif /* __STY_ARRAY_LISTS_H__ */
