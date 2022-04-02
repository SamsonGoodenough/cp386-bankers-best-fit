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
bool isSafe();

// helper prototypes
bool arrayLessThan(int *array1, int *array2);
bool arrayGreaterThan(int *array1, int *array2);
bool arrayLessThanEqual(int *array1, int *array2);
bool arrayGreaterThanEqual(int *array1, int *array2);
void printError(char *error);
void printAvailable();
void printCustomer(struct Customer customer);
void printStatus();

// global variables
const char *FILENAME = "sample4_in.txt";
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
  
  // print status
  printStatus();
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
      return 1;
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
Prints an error message and exits the program
Use: printError("This is an error message");
----------------------------------------------------------
Parameters:
  char *error - the error message to print
----------------------------------------------------------
*/
void printError(char *error) {
  printf("Error: %s\n", error);
  exit(1);
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