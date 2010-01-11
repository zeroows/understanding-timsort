#include "timsort.h"
#include <stdlib.h>
#include <string.h>
#define INSERTION_SORT_SIZE 7

void insertion_sort(int xs[], int length);

// Merge the sorted arrays p1, p2 of length l1, l2 into a single
// sorted array starting at target. target may overlap with either
// of p1 or p2 but must have enough space to store the array.
// Use the storage argument for temporary storage. It must have room for
// l1 + l2 ints.
void merge(int target[], int p1[], int l1, int p2[], int l2, int storage[]);
void integer_timsort_with_storage(int array[], int size, int storage[]);

void integer_timsort(int array[], int size){
  int *storage = malloc(sizeof(int) * size);
  integer_timsort_with_storage(array, size, storage);
  free(storage); 
}

void integer_timsort_with_storage(int array[], int size, int storage[]){
  if(size <= INSERTION_SORT_SIZE){
    insertion_sort(array, size);
    return;
  }
  
  int partition = size/2;
  integer_timsort_with_storage(array, partition, storage);
  integer_timsort_with_storage(array + partition, size - partition, storage);
  merge(array, array, partition, array + partition, size - partition, storage); 
}

void merge(int target[], int p1[], int l1, int p2[], int l2, int storage[]){
  int *merge_to = storage; 
 
  // Current index into each of the two arrays we're writing 
  // from. 
  int i1, i2;
  i1 = i2 = 0; 
 
  // The address to which we write the next element in the merge
  int *next_merge_element = merge_to;

  // Iterate over the two arrays, writing the least element at the
  // current position to merge_to. When the two are equal we prefer
  // the left one, because if we're merging left, right we want to
  // ensure stability.
  // Of course this doesn't matter for integers, but it's the thought
  // that counts. 
  while(i1 < l1 && i2 < l2){
    if(p1[i1] <= p2[i2]){
      *next_merge_element = p1[i1];
      i1++;
    } else {
      *next_merge_element = p2[i2];
      i2++;
    }
    next_merge_element++;
  }

  // If we stopped short before the end of one of the arrays
  // we now copy the rest over.
  memcpy(next_merge_element, p1 + i1, sizeof(int) * (l1 - i1)); 
  memcpy(next_merge_element, p2 + i2, sizeof(int) * (l2 - i2)); 

  // We've now merged into our additional working space. Time
  // to copy to the target. 
  memcpy(target, merge_to, sizeof(int) * (l1 + l2));
}

void insertion_sort(int xs[], int length){
  if(length <= 1) return;
  int i;
  for(i = 1; i < length; i++){
    // The array before i is sorted. Now insert xs[i] into it
    int x = xs[i];
    int j = i - 1;

    // Move j down until it's either at the beginning or on
    // something <= x, and everything to the right of it has 
    // been moved up one.
    while(j >= 0 && xs[j] > x){
      xs[j+1] = xs[j];
      j--;
    }    
    xs[j+1] = x;
  }
}
