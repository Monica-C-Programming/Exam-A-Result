#ifndef PASSENGER_H
#define PASSENGER_H

#include "constants.h"  // Include constants.h

#define MAX_STR 100 //Defined name size

typedef struct Passenger { //Structure for passengers
    char name[MAX_STR];
    int seatNumber;
    int age;
    struct Passenger* next;
} Passenger;

// Declarations for passenger functions
void addPassenger();
void changePassengerSeat();
void searchPassengerByName();
void multipleBookings();

#endif



