#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "flight.h"

Flight* flightListHead = NULL; // Global initializer

void addFlight() { //Function to add the flight to the list
    Flight* newFlight = (Flight*)malloc(sizeof(Flight));
    if (!newFlight) {
        printf("Something went wrong.\n"); //Prints  an error if something goes wrong
        return;
    }

    // Gets information regarding the flight from the user
    printf("Enter a flight ID (example: BA-42): ");
    fgets(newFlight->flightID, MAX_STR, stdin); //Reads input and store in FlightID
    newFlight->flightID[strcspn(newFlight->flightID, "\n")] = '\0'; // Removes extra newline

    printf("Enter the flight's destination: "); //Gets information about the destination
    fgets(newFlight->destination, MAX_STR, stdin); //Reads input and store in destination
    newFlight->destination[strcspn(newFlight->destination, "\n")] = '\0'; // Removes extra newline
    
    printf("Please enter how many available seats there are on this flight: "); //Gets information about how manu seats
    if (scanf("%d", &newFlight->seats) != 1) { //Reads input and store in seats
        printf("Invalid input for number of seats.\n"); //Prints an error if there is invalid input
        free(newFlight);
        return;
    }

    printf("Enter a departure time, as a float (example: 10.45): "); //Gets information about time
    if (scanf("%f", &newFlight->time) != 1) { //Reads input and put it in time
        printf("Invalid input for departure time.\n"); //Prints error if invalid syntax
        free(newFlight);
        return;
    }

    // Initializer
    newFlight->passengerList = NULL;
    newFlight->next = NULL;
    newFlight->prev = NULL;

    // Insert at end of the doubly linked list or middle
    if (flightListHead == NULL) {
        flightListHead = newFlight; // Adds at the front
    } else {
        Flight* temp = flightListHead; // Adds in the middle
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = newFlight;
        newFlight->prev = temp;
    }

    printf("Flight was added successfully.\n"); //Prints a success message
}

void getFlightByIndex() { //Function to search for flight by using index
    int index; //Initialize a index
    printf("Enter a number to show information about the flight(starting from 1): ");
    if (scanf("%d", &index) != 1) { //Collects user input and store it in index to use for search
        printf("Invalid input.\n"); //Prints error if invalid input
        return;
    }

    if (index < 1) { //Gives error if index is invalid
        printf("Invalid flight ID.\n");
        return;
    }
//Flight search for information
    Flight* current = flightListHead;
    int count = 1;

    while (current != NULL && count < index) {
        current = current->next;
        count++;
    }

    if (current == NULL) { //Prints error if flight is not found
        printf("The flight you are looking for couldn't be found at the position %d.\n", index);
        return;
    }

    // Prints out information about the flight
    printf("\nFlight number #%d:\n", index);
    printf("  Flight ID: %s\n", current->flightID);
    printf("  Destination: %s\n", current->destination);
    printf("  Seats: %d\n", current->seats);
    printf("  Departure Time: %.2f\n", current->time);

    // Prints out the passengers list for the flight
    printf("Passengers:\n");
    if (current->passengerList == NULL) {
        printf("No passengers are registered for this flight yet.\n"); //Message if passenger is not found
    } else {
        Passenger* p = current->passengerList; //Prints if passenger is found
        while (p != NULL) {
            printf("Seat %d: %s, Age %d\n", p->seatNumber, p->name, p->age);
            p = p->next;
        }
    }
    printf("\n");
}

void findFlightByDestination() { //Searches for flights using destination
    char destination[MAX_STR]; //Max str is defined in headerfile
    printf("Please enter the destination to search for: "); //Asks for destionation to search for
    fgets(destination, MAX_STR, stdin);
    destination[strcspn(destination, "\n")] = '\0'; // Removes extra newline

    Flight* current = flightListHead; //Initializers 
    int index = 1;
    int found = 0;

    while (current != NULL) { //Prints out information if a flight is found
        if (strcmp(current->destination, destination) == 0) {
            printf("Flight to '%s' found at position %d in the list.\n", destination, index);
            printf("Flight ID: %s\n", current->flightID);
            printf("Seats: %d\n", current->seats);
            printf("Departure Time: %.2f\n\n", current->time);
            found = 1;
        }
        current = current->next;
        index++;
    }

    if (!found) { //Prints if a flight is not found
        printf("No flight found with destination '%s'.\n", destination);
    }
}

void deleteFlight() { //Function to delete a flight
    char flightID[MAX_STR]; //collects input and gives a warning before deleting
    printf("Enter a flight ID to delete please (This cannot be undone!): ");
    fgets(flightID, MAX_STR, stdin);
    flightID[strcspn(flightID, "\n")] = '\0';

    Flight* current = flightListHead;
 //Search for flights
    while (current != NULL && strcmp(current->flightID, flightID) != 0) {
        current = current->next;
    }

    if (current == NULL) {
        printf("The flight you were searching for is not found.\n"); //Prints an error if flight isn't found
        return;
    }

    // Removes the flight from the doubly linked list
    if (current->prev != NULL)
        current->prev->next = current->next;
    else
        flightListHead = current->next;

    if (current->next != NULL)
        current->next->prev = current->prev;

    // Removes the passengers from the passenger list
    Passenger* p = current->passengerList;
    while (p != NULL) {
        Passenger* toDelete = p;
        p = p->next;
        free(toDelete);
    }

    // Free the flight
    free(current);
//Prints a message if deletion was successfull
    printf("Flight ID '%s' and all its passenger reservations have been deleted.\n", flightID);
}
