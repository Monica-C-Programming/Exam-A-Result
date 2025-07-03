#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "passenger.h"
#include "flight.h" //Decided to put variables in flight for access 

 //Adds a passenger to a selected flight
    void addPassenger() {
        char flightID[MAX_STR];
        printf("Enter flight ID to add passenger to: ");
        fgets(flightID, MAX_STR, stdin);
        flightID[strcspn(flightID, "\n")] = '\0';
    
        // Searches for the flight
        Flight* flight = flightListHead;
        while (flight != NULL && strcmp(flight->flightID, flightID) != 0) {
            flight = flight->next;
        }
    
        if (flight == NULL) { //Gives errormessage if not found
            printf("Flight was not found.\n");
            return;
        }
    
        // creates a new passenger if the flight was found 
        Passenger* newPassenger = (Passenger*)malloc(sizeof(Passenger));
        if (!newPassenger) { //Prints an error if something goes wrong
            printf("Something went wrong.\n");
            return;
        }
    
        printf("Enter seat number for the passenger (1-%d): ", flight->seats); //Asks the user for seat number
        scanf("%d", &newPassenger->seatNumber); //Adds the number
        getchar(); //Consumes any extra lines
    
        //Checks to see if the seat inputted actually exist and is not negative
        if (newPassenger->seatNumber < 1 || newPassenger->seatNumber > flight->seats) {
            printf("Invalid seat number.\n");
            free(newPassenger);
            return;
        }
    
        printf("Enter a passenger name please: "); //Asks the user for the name
        fgets(newPassenger->name, MAX_STR, stdin);
        newPassenger->name[strcspn(newPassenger->name, "\n")] = '\0'; //Adds to the list and also consumes any extra lines
    
        printf("Enter the passengers age: "); //Asks for the users age
        scanf("%d", &newPassenger->age);

    
        newPassenger->next = NULL; //Initializes the list
    
        // Checks if the seats are doublebooked and adds it to the list in a sorted order
        Passenger* current = flight->passengerList;
        Passenger* prev = NULL;
    
        while (current != NULL && current->seatNumber < newPassenger->seatNumber) {
            prev = current;
            current = current->next;
        }
    
        // Method to check if we doublebook the seat, we can't have that!
        if (current != NULL && current->seatNumber == newPassenger->seatNumber) {
            printf("Oh no, this seat is already taken.\n");
            free(newPassenger);
            return;
        }
    
        // adds the passenger
        if (prev == NULL) {
            // in the front
            newPassenger->next = flight->passengerList;
            flight->passengerList = newPassenger;
        } else {
            // in the middle or last
            newPassenger->next = prev->next;
            prev->next = newPassenger;
        }
    
        printf("Passenger added successfully to this flight.\n");
    }
    

void changePassengerSeat() { //Methode to change the pasenger seat in the flight
    char flightID[MAX_STR]; //Sets the values we need to use for this
    char name[MAX_STR];
    int newSeat; //Adds a option for new seat

    printf("Please enter the flight ID: "); //Asks the user for flight ID 
    fgets(flightID, MAX_STR, stdin);
    flightID[strcspn(flightID, "\n")] = '\0'; //Saves and use the information gathered

    printf("Please enter the passengers name: "); //Asks the user for the name for the passenger 
    fgets(name, MAX_STR, stdin);
    name[strcspn(name, "\n")] = '\0';

    printf("Please enter the new seat number: "); //Asks for the updated seat number
    scanf("%d", &newSeat);

    // Method for searching for the flight
    Flight* flight = flightListHead; //Initializer to start at the head
    while (flight != NULL && strcmp(flight->flightID, flightID) != 0) { 
        flight = flight->next;
    }

    if (flight == NULL) { //Prints a message if the flight doesn't exist
        printf("Unfortunatly the flight was not found.\n");
        return;
    }

    // Method for searching for the passenger
    Passenger* current = flight->passengerList;
    Passenger* prev = NULL;
    while (current != NULL && strcmp(current->name, name) != 0) {
        prev = current;
        current = current->next;
    }

    if (current == NULL) { //Gives out a message if the passenger is not found
        printf("The passenger was not found.\n");
        return;
    }

    //Removes the passenger from current position, this needs to be done to keep it in a sorted order
    if (prev == NULL) {
        flight->passengerList = current->next;
    } else {
        prev->next = current->next;
    }

    // Update the seat number and add to the list
    current->seatNumber = newSeat;
    Passenger* insertPrev = NULL; //Initializer
    Passenger* insertCurrent = flight->passengerList;

    while (insertCurrent != NULL && insertCurrent->seatNumber < newSeat) {
        insertPrev = insertCurrent;
        insertCurrent = insertCurrent->next;
    }

    if (insertPrev == NULL) {
        current->next = flight->passengerList;
        flight->passengerList = current;
    } else {
        current->next = insertCurrent;
        insertPrev->next = current;
    }
    //Prints a message if the update was successfull
    printf("Seat changed successfully for %s to seat %d.\n", name, newSeat);
}

void searchPassengerByName() { //Method to search for what flights a passenger has booked using a while loop
    char name[MAX_STR];
    printf("Please enter the passengers name to search for: ");
    fgets(name, MAX_STR, stdin);
    name[strcspn(name, "\n")] = '\0';

    Flight* currentFlight = flightListHead; //Initialize startingpoint for search
    int found = 0;

    while (currentFlight != NULL) {
        Passenger* p = currentFlight->passengerList; //Initialize a pointer
        while (p != NULL) { //Searches for flights using the name of passenger
            if (strcmp(p->name, name) == 0) {
                if (!found) {
                    printf("The passenger '%s' was found on the following flight(s):\n", name);
                }
                printf("  Flight ID: %s, Destination: %s, Seat: %d, Age: %d\n", 
                       currentFlight->flightID, currentFlight->destination, p->seatNumber, p->age); //Prints the information about the flights
                found = 1;
            }
            p = p->next;
        }
        currentFlight = currentFlight->next;
    }

    if (!found) { //Prints a message if the passenger is not found in any flights
        printf("Passenger '%s' not found on any flight.\n", name);
    }
}

void multipleBookings() {
    // Method for searching passengers with multiple flights
    typedef struct PassengerInfo {
        char name[MAX_STR];
        int flightCount;
        char flightIDs[MAX_FLIGHTS][MAX_STR]; 
    } PassengerInfo;

    PassengerInfo passengerInfo[MAX_PASSENGERS];
    int passengerCount = 0;

    Flight* currentFlight = flightListHead; //Initialize to start at the front

    // Search through all registred flights
    while (currentFlight != NULL) {
        Passenger* currentPassenger = currentFlight->passengerList;

        //Search through all passengers registred
        while (currentPassenger != NULL) {
            // Check if the passenger has been found before
            int found = 0;//Counter initializer
            for (int i = 0; i < passengerCount; i++) { //Counts how many times a passenger has been found
                if (strcmp(passengerInfo[i].name, currentPassenger->name) == 0) {
                    //If anyone is found, they get added to the list
                    strcpy(passengerInfo[i].flightIDs[passengerInfo[i].flightCount], currentFlight->flightID);
                    passengerInfo[i].flightCount++;
                    found = 1;
                    break;
                }
            }

            // Checks the list for the name and if it isn't found before, it will be added
            if (!found) {
                strcpy(passengerInfo[passengerCount].name, currentPassenger->name);
                strcpy(passengerInfo[passengerCount].flightIDs[0], currentFlight->flightID);
                passengerInfo[passengerCount].flightCount = 1;
                passengerCount++;
            }

            currentPassenger = currentPassenger->next;
        }

        currentFlight = currentFlight->next;
    }

    // prints out the information if anyone is booked on multiple flights
    printf("Passengers booked on multiple flights:\n");
    for (int i = 0; i < passengerCount; i++) {
        if (passengerInfo[i].flightCount > 1) {
            printf("  Passenger '%s' is booked on flights: ", passengerInfo[i].name);
            for (int j = 0; j < passengerInfo[i].flightCount; j++) {
                printf("%s ", passengerInfo[i].flightIDs[j]);
            }
            printf("\n");
        }
    }
}


