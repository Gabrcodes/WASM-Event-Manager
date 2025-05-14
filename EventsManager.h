#ifndef EVENTSMANAGER_H
#define EVENTSMANAGER_H

#include "EventSystemEntities.h" // Includes User, Attendee, Event, EventType, derived events
#include "utils.h"               // For utility functions like cEncrypt, cDecrypt, SHIFT
#include <vector>
#include <string>
#include <fstream>               // For file operations
#include <sstream>               // For string stream operations
#include <limits>                // For numeric_limits

// EventsManager class to manage a collection of events
class EventsManager {
private:
    std::vector<Event*> allEvents; // Stores pointers to Event objects
    std::string eventFilePath;     // Path to the event data file

    // Helper function to load events from a file
    void loadEventsFromFile();

    // Helper function to save events to a file
    void saveEventsToFile();

public:
    // Constructor: takes the path to the event data file
    EventsManager(const std::string& filePath);

    // Destructor: saves events to file and cleans up allocated Event objects
    ~EventsManager();

    // Method to create a new event based on user input
    void createEvent(User* user);

    // Method to display all created events
    void showAllEvents() const;

    // Method to delete an event by its title
    void deleteEvent();

    // Method for a user to sign up for an event by its title
    void signUpForEvent(User* user);

    // Method to search for events by title
    void searchEvents() const;
};

#endif // EVENTSMANAGER_H
