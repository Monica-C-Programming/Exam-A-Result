#ifndef FLIGHT_H
#define FLIGHT_H

#include "passenger.h"
#include "constants.h"

typedef struct Flight { //Structure for flight
    char flightID[MAX_STR];
    char destination[MAX_STR];
    int seats;
    Passenger* passengerList;
    struct Flight* next;
    struct Flight* prev;  
    float time;  
} Flight;

extern Flight* flightListHead; //Extern to be shared across multiple sourcefiles

// Function declarations
void addFlight();
void getFlightByIndex();
void findFlightByDestination();
void deleteFlight();

#endif



