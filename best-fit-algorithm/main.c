/*
==========================================================
Samson Goodenough
190723380
==========================================================
CP386 Assignment 4 - Question 2 - Best-Fit Algorithm
==========================================================
*/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

unsigned int MAX;
struct Block {
  void *start;        // pointer to start of block 
  void *end;          // pointer to end of block
  int size;           // size of block
  int owner;          // owner thread of block
  struct Block *next; // pointer to next block
};

void initBlock(struct Block *block, void *start, int size, int owner);
void setBlock(struct Block *block, int size, int owner);
void removeBlock(int ownerId);
void mergeFreeBlocks();
void printBlocks();
void printError(char *message);

struct Block *head;

int main(int argc, char *argv[]) {
  // check for one argument
  if (argc != 2) {
    printf("Usage: ./best-fit-algorithm <MAX>\n");
    return 1;
  }

  MAX = atoi(argv[1]);

  // allocate memory size of MAX
  char *memory = malloc(MAX);
  memset(memory, '.', MAX);

  // create a free block of memory
  head = (struct Block*)malloc(sizeof(struct Block));
  initBlock(head, (void*)&memory[0], MAX, -1);
  printBlocks();

  setBlock(head, 20, 0);
  printBlocks();

  setBlock(head->next, 30, 1);
  printBlocks();

  removeBlock(1);
  printBlocks();
}

void initBlock(struct Block *block, void *start, int size, int owner) {
  block->start = start;
  block->end = block->start + size;
  block->size = size;
  block->owner = owner;
  block->next = NULL;
}

void setBlock(struct Block *block, int size, int owner) {
  if (block->owner != -1) {
    printError("\n\tsetBlock: block is not free");
    return;
  }

  struct Block *newBlock = (struct Block*)malloc(sizeof(struct Block));
  initBlock(newBlock, block->start + size, block->size - size, -1);

  block->owner = owner;
  block->size = size;
  block->end = block->start + size-1;
  block->next = newBlock;
}

void removeBlock(int ownerId) {
  // search for block in list
  struct Block *current = head;
  while (current->next != NULL) {
    if (current->owner == ownerId) {
      current->owner = -1;
      break;
    }
    current = current->next;
  }

  mergeFreeBlocks();
}

void mergeFreeBlocks() {
  struct Block *current = head;
  while (current->next != NULL) {
    if (current->owner == -1 && current->next->owner == -1) {
      current->size += current->next->size;
      current->end = current->next->end;
      current->next = current->next->next;
    } else {
      current = current->next;
    }
  }
}

void printBlocks() {
  struct Block *current = head;
  printf("All Memory Blocks:\n");
  while (current != NULL) {
    printf("\t%p - %p, size: %d, owner: %d\n", current->start, current->end, current->size, current->owner);
    current = current->next;
  }
}

void printError(char *message) {
  printf("Error: %s\n", message);
}