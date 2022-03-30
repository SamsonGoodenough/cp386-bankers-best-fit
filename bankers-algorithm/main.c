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
void printError(char *error);
void printCustomer(struct Customer customer);
int init_available(int argc, char *args[]);
int init_customers();

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
  
  // print all customers
  for (int i = 0; i < numCustomers; i++) {
    printCustomer(customers[i]);
  }
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