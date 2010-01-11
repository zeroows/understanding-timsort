#include "timsort.h"
#include <stdlib.h>
#include <string.h>
#define INSERTION_SORT_SIZE 7

// We use a fixed size stack. This size is far larger than there is
// any reasonable expectation of overflowing. Of course, we do still
// need to check for overflows.
#define STACK_SIZE 1024

typedef struct {
  int *index;
  int length;
} run;

typedef struct {
  int *storage;
  // Storage for the stack of runs we've got so far.
  run runs[STACK_SIZE];
  // The index of the first unwritten element of the stack.
  int stack_height;

  // We keep track of how far we've partitioned up to so we know where to start the next partition. 
  // The idea is that everything < partioned_up_to is on the stack, everything >= partioned_up_to 
  // is not yet on the stack. When partitioned_up_to == length we'll have put everything on the stack.
  int *partitioned_up_to;

  int *array;
  int length;

} sort_state_struct;

typedef sort_state_struct *sort_state;

void integer_timsort_with_state(int array[], int size, sort_state state);

void insertion_sort(int xs[], int length);

// Merge the sorted arrays p1, p2 of length l1, l2 into a single
// sorted array starting at target. target may overlap with either
// of p1 or p2 but must have enough space to store the array.
// Use the storage argument for temporary storage. It must have room for
// l1 + l2 ints.
void merge(int target[], int p1[], int l1, int p2[], int l2, int storage[]);

int next_partition(sort_state state);
int should_collapse(sort_state state);
void merge_collapse(sort_state state);

void integer_timsort(int array[], int size){
  sort_state_struct state;
  state.storage = malloc(sizeof(int) * size);
  state.stack_height = 0;
  state.partitioned_up_to = array;
  state.array = array;
  state.length = size; 

  while(next_partition(&state)){
    while(should_collapse(&state)) merge_collapse(&state);
  }

  while(state.stack_height > 1) merge_collapse(&state);

  free(state.storage); 
}

int next_partition(sort_state state){
  if(state->partitioned_up_to >= state->array + state->length) return 0;
 
  int *start_index = state->partitioned_up_to;
  
  // Find an increasing run starting from start_index

  int *next_start_index = start_index + 1;

  while(next_start_index < state->array + state->length){
    if(*next_start_index >= *(next_start_index - 1)) next_start_index++;
    else break;
  }
 
  // So now [start_index, next_start_index) is an increasing run. Push it onto the stack.

  run run_to_add;
  run_to_add.index = start_index;
  run_to_add.length = (next_start_index - start_index);
  state->runs[state->stack_height] = run_to_add;

  state->stack_height++;
  state->partitioned_up_to = next_start_index;

  return 1;
}

int should_collapse(sort_state state){
  return (state->stack_height > 2) && ((state->stack_height >= STACK_SIZE) || (rand() % 2));
}

void merge_collapse(sort_state state){
  run A = state->runs[state->stack_height - 2];
  run B = state->runs[state->stack_height - 1];

  merge(A.index, A.index, A.length, B.index, B.length, state->storage);

  state->stack_height--;
  A.length += B.length;
  state->runs[state->stack_height - 1] = A;
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
