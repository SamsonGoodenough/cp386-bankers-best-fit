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

// structures
struct Block {
  void *start;        // pointer to start of block 
  void *end;          // pointer to end of block
  int size;           // size of block
  int owner;          // owner thread of block
  struct Block *next; // pointer to next block
};

// global variables
unsigned int MAX;
struct Block *head;
const int INPUT_SIZE = 512;

// method prototypes
void initBlock(struct Block *block, void *start, int size, int owner);
void setBlock(struct Block *block, int size, int owner);
void removeBlock(int ownerId);
void mergeFreeBlocks();
void bestfit(int size, int ownerID);
bool hasBlock(int ownerId);
void beginListening();

// print method prototypes
void printBlocks();
void printError(char *message);

int main(int argc, char *argv[]) {
  // check for one argument
  if (argc != 2) {
    printf("Usage: ./best-fit-algorithm <MAX>\n");
    printf("   or: make runBestfit args=\"<args>\"\n");
    return 1;
  }

  // check for valid argument
  MAX = atoi(argv[1]);
  if (MAX <= 0) {
    printError("MAX must be greater than 0, or argument is not an integer");
    return 1;
  }

  // allocate memory size of MAX
  char *memory = malloc(MAX);
  memset(memory, '.', MAX);


  // create a free block of memory
  head = (struct Block*)malloc(sizeof(struct Block));
  initBlock(head, (void*)&memory[0], MAX, -1);
  printBlocks();

  // start input loop
  beginListening();
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

  // create new remaining block
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
      printf("Successfully released memory for process P%d\n", ownerId);
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

void bestfit(int size, int ownerID) {
  struct Block *current = head;
  struct Block *bestfit = NULL;
  int bestFitSize = MAX;

  // search for best fit block
  while (current != NULL) {
    if (current->owner == -1 && current->size >= size) {
      if (current->size <= bestFitSize) {
        bestfit = current;
        bestFitSize = current->size;
      }
    }

    current = current->next;
  }

  if (bestfit == NULL) {
    printError("\n\tbestfit: no block found for given size");
    return;
  }

  // set block to given size and owner
  setBlock(bestfit, size, ownerID);
  printf("Successfully allocated %d to process P%d\n", size, ownerID);
}

void beginListening() {
  char *command;
  int id;
  int size;

  while (true) { // loop until error or exit
    // create buffers for input
    char line[INPUT_SIZE];

    // prompt for input
    printf("Enter Command: ");

    // read the input
    fgets(line, sizeof(line), stdin);
    int tokenCount = 0;

    // break into tokens
    char *token = strtok(line, " ");
    while (token != NULL) {
      if (tokenCount == 0) {
        command = token;
        command[strcspn(command, "\n")] = 0;
      } else if (tokenCount == 1) {
        if (token[0] == 'P') {
          id = atoi(token+1);
        } else {
          printError("Invalid PID, must be P followed by an integer");
          break;
        }
      } else if (tokenCount == 2) {
        size = atoi(token);
        if (size <= 0) {
          printError("Size must be greater than 0, or argument is not an integer");
          break;
        }
      } else if (tokenCount == 3) {
        if (token[0] != 'B') {
          printError("Invalid algorithm, must be B");
          break;
        }
      }

      // advance loop
      token = strtok(NULL, " ");
      tokenCount++;
    }

    // check for valid input
    if (strcmp(command, "Status") == 0) {
      printBlocks();
    } else if (strcmp(command, "Exit") == 0) {
      exit(0);
    } else if (tokenCount == 1) {
      printf("Invalid input, use one of RQ, RL, Status, Exit\n");
    } else if (strcmp(command, "RQ") == 0) {
      if (hasBlock(id)) {
        printError("PID already has a block");
      } else {
        bestfit(size, id);
      }
    } else if (strcmp(command, "RL") == 0) {
      removeBlock(id);
    } else {
      printf("Invalid input, use one of RQ, RL, Status, Exit\n");
    }

    command = NULL;
    id = -2;
    size = 0;
  }
}

bool hasBlock(int ownerId) {
  struct Block *current = head;
  while (current != NULL) {
    if (current->owner == ownerId) {
      return true;
    }
    current = current->next;
  }

  return false;
}

void printBlocks() {
  int free = 0;
  struct Block *current = head;
  printf("All Memory Blocks:\n");
  while (current != NULL) {
    if (current->owner == -1) {
      free += current->size;
      printf("\t%p - %p, size: %d, owner: NONE\n", current->start, current->end, current->size);
    } else {
      printf("\t%p - %p, size: %d, owner: P%d\n", current->start, current->end, current->size, current->owner);
    }
    
    current = current->next;
  }
  printf("Free: %d\n", free);
}

void printError(char *message) {
  printf("Error: %s\n", message);
}