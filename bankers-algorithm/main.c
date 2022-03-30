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
void init_available(int argc, char *args[]);
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
void init_available(int argc, char *args[]) {
  // allocate memory for available resources
  available = malloc((argc - 1) * sizeof(int));

  // check we have allocated memory
  if (available == NULL) {
    printError("Failed to allocate memory");
    return;
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