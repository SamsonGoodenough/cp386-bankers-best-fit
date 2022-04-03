/*
==========================================================
Samson Goodenough
190723380
==========================================================
CP386 Assignment 4 - Question 1 - Banker's Algorithm
==========================================================
*/

// imports
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include <sys/stat.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>

// structs
struct Customer {
  int id;
  int *maximum;
  int *allocated;
  int *need;
} customer;

// prototypes
int init_available(int argc, char *args[]);
int init_customers();
void beginListening();
bool isSafe();

// helper prototypes
bool arrayLessThan(int *array1, int *array2);
bool arrayGreaterThan(int *array1, int *array2);
bool arrayLessThanEqual(int *array1, int *array2);
bool arrayGreaterThanEqual(int *array1, int *array2);
int  *arrayCopy(int *array);
void printError(char *error);
void printAvailable();
void printCustomer(struct Customer customer);
void printStatus();
void printOpeningStatus();

// global variables
const char *FILENAME = "sample4_in.txt";
const int INPUT_SIZE = 512;
int *available;
int numTypes;
struct Customer *customers;
int numCustomers;

/*
----------------------------------------------------------
Main of the program
Use: make bankers <args>
----------------------------------------------------------
Arguments:
  argv[i] - available resources of type i [>= 2] (required)
----------------------------------------------------------
*/
int main(int argc, char *args[]) {
  // check we have more than one argument
  if (argc < 2) {
    printError("Not enough arguments");
    return 1;
  }

  // check all arguments are integers
  for (int i = 1; i < argc; i++) {
    if (atoi(args[i]) == 0) {
      printError("At least one argument is not an integer");
      return 1;
    }
  }
  
  numTypes = init_available(argc, args);  // initialize available resources
  numCustomers = init_customers();        // initialize customers
  
  // print opening status
  printOpeningStatus();

  // begin listening for input
  beginListening();
}

// methods

/*
----------------------------------------------------------
Loads the available resources from the command line
Use: init_available(argc, args);
----------------------------------------------------------
Parameters:
  argc - number of arguments
  args - array of arguments
----------------------------------------------------------
*/
int init_available(int argc, char *args[]) {
  // allocate memory for available resources
  available = malloc((argc - 1) * sizeof(int));

  // check we have allocated memory
  if (available == NULL) {
    printError("Failed to allocate memory");
    return -1;
  }

  // fill available resource with arguments
  for (int i = 1; i < argc; i++) {
    available[i - 1] = atoi(args[i]);
  }

  return argc - 1;
}

/*
----------------------------------------------------------
Loads the customers from the file
Use: numCustomers = init_customers();
----------------------------------------------------------
Returns:
  num - number of customers
----------------------------------------------------------
*/
int init_customers() {
  // open the file
  FILE* file = fopen(FILENAME, "r");

  // check if file exists
  if (file == NULL) {
    printError("Error opening file");
    return 1;
  }

  // count the number of customers, validate the file
  char line[512];
  unsigned int numTokens = 0;
  unsigned int customerCount = 0;
  while (fgets(line, sizeof(line), file) && strcmp(line, "\n") != 0) {
    // split line into comma delimited tokens
    char *token = strtok(line, ",");
    while (token != NULL) {
      numTokens++;
      token = strtok(NULL, ",");
    }

    // check the number of tokens is correct
    if (numTokens != numTypes) {
      printError("Tokens in args don't match tokens in file");
      exit(1);
    }

    customerCount++;
    numTokens = 0;
  }

  // reset the file pointer
  rewind(file);

  // allocate memory for customers
  customers = malloc(customerCount * sizeof(struct Customer));
  unsigned int customerIndex = 0;
  while (fgets(line, sizeof(line), file) && strcmp(line, "\n") != 0) {

    // init customers
    struct Customer customer;
    customer.id = customerIndex;
    customer.maximum = malloc(numTypes * sizeof(int));
    customer.allocated = malloc(numTypes * sizeof(int));
    customer.need = malloc(numTypes * sizeof(int));

    // split line into comma delimited tokens
    unsigned int typeIndex = 0;
    char *token = strtok(line, ",");
    while (token != NULL) {
      // fill customer with maximum and need tokens
      customer.maximum[typeIndex] = atoi(token);
      customer.need[typeIndex] = atoi(token);

      token = strtok(NULL, ",");
      typeIndex++;
    }

    // add customer to customers array
    customers[customerIndex] = customer;
    customerIndex++;
  }

  fclose(file);
  return customerCount;

}

void beginListening() {
  while (true) { // loop until error or exit
    // create buffers for input
    char line[INPUT_SIZE];
    char *command;
    int customerID;
    int *types = malloc(sizeof(int) * numTypes);

    // prompt for input
    printf("Enter Command: ");

    // read the input
    fgets(line, sizeof(line), stdin);
    int typeIndex = -2; // set to -2 to skip first two tokens (command and id)

    // break into tokens
    char *token = strtok(line, " ");
    while (token != NULL) {
      if (typeIndex == -2) {
        command = token;
        command[strcspn(command, "\n")] = 0;
      } else if (typeIndex == -1) {
        customerID = atoi(token);
      } else {
        types[typeIndex] = atoi(token);
      }

      // advance loop
      token = strtok(NULL, " ");
      typeIndex++;
    }

    // check for valid input
    if (strcmp(command, "Status") == 0) {
      printStatus();
    } else if (strcmp(command, "Run") == 0) {
      // add run code
    } else if (strcmp(command, "Exit") == 0) {
      exit(0);
    } else if (typeIndex == -1) {
      printf("Invalid input, use one of RQ, RL, Status, Run, Exit\n");
    } else if (typeIndex != numTypes) {
      printf("Warning: Invalid number of types. expected %d, got %d\n", numTypes+2, typeIndex+2);
    } else if (strcmp(command, "RQ") == 0) {
      request(customerID, types);
    } else if (strcmp(command, "RL") == 0) {
      release(customerID, types);
    } else {
      printf("Invalid input, use one of RQ, RL, Status, Run, Exit\n");
    }
  }
}

/*
----------------------------------------------------------
Checks if the system is in a safe state
Use: if (isSafe()) {...}
----------------------------------------------------------
Returns:
  safe - [true/false] whether the system is safe or not
----------------------------------------------------------
*/
bool isSafe() {
  bool safe = true;

  // ===== step 1 =====
  // define work = available and finish = false
  int *work = malloc(numTypes * sizeof(int));
  bool *finish = malloc(numCustomers * sizeof(bool));
  for (unsigned int i = 0; i < numTypes; i++) {
    work[i] = available[i];
    finish[i] = available[i];
  }

  // ===== step 2 =====
  unsigned int i = 0;
  bool checkedAll = false;
  bool changeMade;
  while (checkedAll == false) {
    changeMade = false;

    if (finish[i] == false) {
      if (arrayLessThan(customers[i].need, work)) {
        // ===== step 3 =====
        for (unsigned int j = 0; j < numTypes; j++) {
          work[j] += customers[i].allocated[j];
        }

        finish[i] = true;
        changeMade = true;
      }
    }

    i++;
    if (i >= numCustomers) {
      if (changeMade) {
        i = 0;
      } else {
        checkedAll = true;
      }
    }
  }

  // ===== step 4 =====
  for (unsigned int i = 0; i < numTypes; i++) {
    if (finish[i] == false) {
      safe = false;
    }
  }

  return safe;
}

/*
----------------------------------------------------------
Requests resources
Use: request(customer, resources);
----------------------------------------------------------
Parameters:
  struct Customer customer
  int *requested - array of requested resources
----------------------------------------------------------
*/
void request(struct Customer customer, int *requested) {
  // check if the request is valid
  if (arrayGreaterThan(requested, customer.need)) {
    printError("Request is invalid - exceeds need");
    return;
  } else if (arrayGreaterThan(requested, available)) {
    printError("Request is invalid - exceeds available");
    return;
  }

  // make backup of available resources
  int *backup = arrayCopy(available);

  // temporarily allocate resources
  for (unsigned int i = 0; i < numTypes; i++) {
    available[i] -= requested[i];
  }

  if (isSafe()) {
    // update customer allocated and need
    for (unsigned int i = 0; i < numTypes; i++) {
      customer.allocated[i] += requested[i];
      customer.need[i] -= requested[i];
    }

    printf("State is safe, and request is satisfied\n");
  } else {
    // restore available resources
    available = arrayCopy(backup);

    printError("Request is invalid - system is not in a safe state");
  }
}

/*
----------------------------------------------------------
Releases resources
Use: release(customer, resources);
----------------------------------------------------------
Parameters:
  struct Customer customer
  int *released - array of resources to be released
----------------------------------------------------------
*/
void release(struct Customer customer, int *released) {
  // check if the release is valid
  if (arrayGreaterThan(released, customer.allocated)) {
    printError("Release is invalid - exceeds allocated");
    return;
  }

  // update customer available and allocated
  for (unsigned int i = 0; i < numTypes; i++) {
    available[i] += released[i];
    customer.allocated[i] -= released[i];
  }
}

// helper methods

/*
----------------------------------------------------------
Check if array is less than another array
Use: if (arrayLessThan(array1, array2)) {...}
----------------------------------------------------------
Parameters:
  int *array1 - first array to compare
  int *array2 - second array to compare against
Returns:
  lessThan - [true/false] whether the first array is less
    than the second array
----------------------------------------------------------
*/
bool arrayLessThan(int *array1, int *array2) {
  bool lessThan = true;
  unsigned int i = 0;
  while (i < numTypes && lessThan == true) {
    if (array1[i] >= array2[i]) {
      lessThan = false;
    }

    i++;
  }

  return lessThan;
}

/*
----------------------------------------------------------
Check if array is greater than another array
Use: if (arrayGreaterThan(array1, array2)) {...}
----------------------------------------------------------
Parameters:
  int *array1 - first array to compare
  int *array2 - second array to compare against
Returns:
  greaterThan - [true/false] whether the first array is
    greater than the second array
----------------------------------------------------------
*/
bool arrayGreaterThan(int *array1, int *array2) {
  bool greaterThan = true;
  unsigned int i = 0;
  while (i < numTypes && greaterThan == true) {
    if (array1[i] <= array2[i]) {
      greaterThan = false;
    }

    i++;
  }

  return greaterThan;
}

/*
----------------------------------------------------------
Check if array is less than or equal to another array
Use: if (arrayLessThan(array1, array2)) {...}
----------------------------------------------------------
Parameters:
  int *array1 - first array to compare
  int *array2 - second array to compare against
Returns:
  lessThan - [true/false] whether the first array is less
    than or equal to the second array
----------------------------------------------------------
*/
bool arrayLessThanEqual(int *array1, int *array2) {
  bool lessThanEqual = true;
  unsigned int i = 0;
  while (i < numTypes && lessThanEqual == true) {
    if (array1[i] > array2[i]) {
      lessThanEqual = false;
    }

    i++;
  }

  return lessThanEqual;
}

/*
----------------------------------------------------------
Check if array is greater than or equal to another array
Use: if (arrayGreaterThan(array1, array2)) {...}
----------------------------------------------------------
Parameters:
  int *array1 - first array to compare
  int *array2 - second array to compare against
Returns:
  greaterThan - [true/false] whether the first array is
    greater than or equal to the second array
----------------------------------------------------------
*/
bool arrayGreaterThanEqual(int *array1, int *array2) {
  bool greaterThanEqual = true;
  unsigned int i = 0;
  while (i < numTypes && greaterThanEqual == true) {
    if (array1[i] < array2[i]) {
      greaterThanEqual = false;
    }

    i++;
  }

  return greaterThanEqual;
}

/*
----------------------------------------------------------
Copies an array into a new array
Use: int *copy = arrayCopy(array);
----------------------------------------------------------
Parameters:
  int *array - source array to copy
Returns:
  int *copy - destination array
----------------------------------------------------------
*/
int *arrayCopy(int *array) {
  int *copy = malloc(numTypes * sizeof(int));
  for (unsigned int i = 0; i < numTypes; i++) {
    copy[i] = array[i];
  }

  return copy;
}

/*
----------------------------------------------------------
Prints an error message and exits the program
Use: printError("This is an error message");
----------------------------------------------------------
Parameters:
  char *error - the error message to print
----------------------------------------------------------
*/
void printError(char *error) {
  printf("Error: %s\n", error);
}

/*
----------------------------------------------------------
Prints the current available resources
Use: printAvaliable();
----------------------------------------------------------
*/
void printAvailable() {
  printf("Currently Available resources: ");
  for (unsigned int i = 0; i < numTypes; i++) {
    printf("%d ", available[i]);
  }
  printf("\n");
}

/*
----------------------------------------------------------
Prints the contents of a customer
Use: printCustomer(customers[0]);
----------------------------------------------------------
Parameters:
  customer - the customer to print
----------------------------------------------------------
*/
void printCustomer(struct Customer customer) {
  printf("Customer %d:\n", customer.id);
  printf("\tMaximum:\t");
  for (int i = 0; i < numTypes; i++) {
    printf("%d\t", customer.maximum[i]);
  }

  printf("\n\tAllocated:\t");
  for (int i = 0; i < numTypes; i++) {
    printf("%d\t", customer.allocated[i]);
  }

  printf("\n\tNeed:\t\t");
  for (int i = 0; i < numTypes; i++) {
    printf("%d\t", customer.need[i]);
  }
  printf("\n\n");
}

/*
----------------------------------------------------------
Prints the current status of the system
Use: printStatus();
----------------------------------------------------------
*/
void printStatus() {
  printf("Available Resources:\n");
  for (unsigned int i = 0; i < numTypes; i++) {
    printf("%d ", available[i]);
  }

  printf("\nMaximum Resources:");
  for (unsigned int i = 0; i < numCustomers; i++) {
    printf("\n");
    for (unsigned int j = 0; j < numTypes; j++) {
      printf("%d ", customers[i].maximum[j]);
    }
  }

  printf("\nAllocated Resources:");
  for (unsigned int i = 0; i < numCustomers; i++) {
    printf("\n");
    for (unsigned int j = 0; j < numTypes; j++) {
      printf("%d ", customers[i].allocated[j]);
    }
  }

  printf("\nNeed Resources:");
  for (unsigned int i = 0; i < numCustomers; i++) {
    printf("\n");
    for (unsigned int j = 0; j < numTypes; j++) {
      printf("%d ", customers[i].need[j]);
    }
  }
  printf("\n");
}

/*
----------------------------------------------------------
Prints the opening status of the system
Use: printOpeningStatus();
----------------------------------------------------------
*/
void printOpeningStatus() {
  printAvailable();
  printf("Maximum Resources:");
  for (unsigned int i = 0; i < numCustomers; i++) {
    printf("\n");
    for (unsigned int j = 0; j < numTypes; j++) {
      printf("%d ", customers[i].maximum[j]);
    }
  }
  printf("\n");
}