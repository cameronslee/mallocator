#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define ALIGNMENT 8
#define MAX_MALLOC_SIZE 1000 

struct malloc_stc
{
  struct malloc_stc *next;
  struct malloc_stc *prev;
  int size;
  unsigned char *buffer; //points to beginning address of the block
};

static struct malloc_stc *FreeList; 
static unsigned char Buffer[MAX_MALLOC_SIZE];

void InitMallocator() {
  FreeList = (struct malloc_stc *)Buffer; 
  FreeList->next = NULL;
  FreeList->prev = NULL;
  FreeList->size = sizeof(Buffer) - sizeof(struct malloc_stc);

  // set buffer to point to the location after first bookkeeping block
  FreeList->buffer = (unsigned char *)(Buffer + sizeof(struct malloc_stc)); 
}

void PrintNode(void *node) {
  struct malloc_stc *current = (struct malloc_stc *)node;
  printf("\n%s\n", "###################");
  printf("%p current \n", current);
  printf("%p next \n", current->next);
  printf("%p prev \n", current->prev);
  printf("%d size \n", current->size);
  printf("%p buffer \n", current->buffer);
  printf("%s\n", "###################");
}

void *Mallocator(int size) {
  // round up to nearest multiple of 8 for alignment
	if (size % ALIGNMENT != 0) size = size + 8 - (size % ALIGNMENT); 
  if (size <= 0 || size > FreeList->size) return NULL;

  struct malloc_stc *current = FreeList;

  //points to the addr after the allocated portion of data
  struct malloc_stc *alloc_block = (struct malloc_stc *)(current->buffer);
  alloc_block->next = current->next;
  alloc_block->prev = current;
  alloc_block->size = size;
  alloc_block->buffer = (unsigned char *)current->buffer + sizeof(struct malloc_stc);
  
	struct malloc_stc *free_block = (struct malloc_stc *)(alloc_block->buffer + alloc_block->size);
  free_block->next = current->next;
  free_block->prev = current;
  free_block->size = current->size - sizeof(struct malloc_stc) - size;
  free_block->buffer = (unsigned char *)alloc_block + sizeof(struct malloc_stc);

  PrintNode(current);

  // update list
  FreeList = free_block;

  //update last node in freelist
  if (FreeList->prev != NULL) FreeList->prev->next = free_block;
  return alloc_block->buffer;
}

// Prints from head to beginning (stack like)
void PrintMyMallocFreeList() {
  struct malloc_stc *current = FreeList;
  
  while (current->next != NULL) current = current->next;
  printf("\n%s", "---- FREE LIST ----");
  while (current != NULL) {
    printf("\n%s\n", "###################");
    printf("%p current \n", current);
    printf("%p next \n", current->next);
    printf("%p prev \n", current->prev);
    printf("%d size \n", current->size);
    printf("%p buffer \n", current->buffer);
    printf("%s\n", "###################");

    current = current->prev;

    if (current != NULL) {
      printf("%s\n", "         ^");
      printf("%s", "         |");
    }
  }
  printf("%s\n\n", "-------------------");
}

int main(int argc, char *argv[])
{
  char *array1;
  char *array2;
  int i;

  InitMallocator();

  array1 = Mallocator(128);
  if(array1 == NULL)
  {
    fprintf(stderr,"call to Mallocator() failed\n");
    fflush(stderr);
    exit(1);
  }

  array2 = Mallocator(32);
  if(array2 == NULL)
  {
    fprintf(stderr,"call to Mallocator() failed\n");
    fflush(stderr);
    exit(1);
  }

  PrintMyMallocFreeList();

  return(0);
}
