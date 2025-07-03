#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "passenger.h"
#include "flight.h"
#include "constants.h"

#define MAX_STR 100

//Declarations for the different functions in the menu
void addFlight();
void addPassenger();
void getFlightByIndex();
void findFlightByDestination();
void deleteFlight();
void changePassengerSeat();
void searchPassengerByName();
void multipleBookings();
void freeAll();  // Releases memory used for flight and passengers


int printOptions(){ //Method for printing the different options in the menu
    int choice;
    printf("\n--- Welcome to PG3401 Airline, Please select one of the following options ---\n");
    printf("1. Add a flight to the list\n");
    printf("2. Add a passenger for departure\n");
    printf("3. Search for a flight using the flight number\n");
    printf("4. Search for a destination\n");
    printf("5. Delete a flight\n");
    printf("6. Change a passengers seat\n");
    printf("7. Search for a passenger by name\n");
    printf("8. Search for passengers with multiple bookings\n");
    printf("9. exit\n");

    printf("Enter your selection (1-9): ");
    scanf("%d", &choice);
     
    int c;
    while ((c = getchar()) != '\n' && c != EOF); //clear the stdn buffer befor reading and scanning next


    return choice;
}

void menuSelect(int choice){ // The menu to select options
    switch (choice)
    {
    case 1:
        printf("You chose to add a flight to the list.\n");
        addFlight();
        break;
    case 2:
        printf("You chose to add a passenger to the list.\n");
        addPassenger();
        break;

    case 3:
        printf("You chose to search for a flight using the flight number.\n");
        getFlightByIndex();
        break;

    case 4:
        printf("You chose to search for a destination.\n");
        findFlightByDestination();
        break;

    case 5:
        printf("You chose to delete a flight.\n");
        deleteFlight();
        break;

    case 6:
        printf("You chose to edit a passengers seat.\n");
        changePassengerSeat();
        break;

    case 7:
        printf("You chose to search for a passenger by name.\n");
        searchPassengerByName();
        break;

    case 8:
        printf("You chose to search for passengers with multiple bookings.\n");
        multipleBookings();
        break;

    case 9:
        printf("You chose to exit. Goodbye.\n");
        break;

    default:
        printf("Invalid selection. Please choose a number between 1 and 9.\n");
        break;
    }
}

   // Method to free resources
    void freeAll() {
    Flight* currentFlight = flightListHead;
    while (currentFlight != NULL) {
        Flight* tempFlight = currentFlight;
        currentFlight = currentFlight->next;

        // Free passengers
        Passenger* currentPassenger = tempFlight->passengerList;
        while (currentPassenger != NULL) {
            Passenger* tempPassenger = currentPassenger;
            currentPassenger = currentPassenger->next;
            free(tempPassenger);
        }

        free(tempFlight);
    }

    flightListHead = NULL;
}


int main() {
    int choice;
    do {
        choice = printOptions();  // Gets the users selected choice
        menuSelect(choice);  // Sends the choice to the menu
    } while (choice != 9);  // Exit when the user selects '9'
    
     freeAll(); //makes sure to free resources
    
    return 0;
}

