#include "EventsManager.h"
#include <iostream>
#include <algorithm> // For std::remove if needed, though erase is used with iterators

// Constructor for EventsManager
// Initializes the event file path and loads existing events from the file.
EventsManager::EventsManager(const std::string& filePath) : eventFilePath(filePath) {
    loadEventsFromFile();
}

// Destructor for EventsManager
// Saves all current events to the file and deallocates memory for each Event object.
EventsManager::~EventsManager() {
    saveEventsToFile();
    for (Event* e : allEvents) {
        delete e; // Deallocate each Event
    }
    allEvents.clear(); // Clear the vector of pointers
}

// Loads events from the specified event file
void EventsManager::loadEventsFromFile() {
    std::ifstream eventFile(eventFilePath);
    if (!eventFile.is_open()) {
        // std::cerr << "Warning: Could not open event file for reading: " << eventFilePath << ". Starting fresh." << std::endl;
        return; // File might not exist on first run, which is fine.
    }

    std::string eventLine;
    std::string attendeesLine; // Expect attendees on the next line

    while (std::getline(eventFile, eventLine)) { // Read event data
        if (eventLine.empty()) continue;

        std::stringstream s_event(eventLine);
        std::string typeStr, title, host, description, dateAndTime, vPlatform, capStr;
        char del = '|';

        std::getline(s_event, typeStr, del);
        std::getline(s_event, title, del);
        std::getline(s_event, host, del);
        std::getline(s_event, description, del);
        std::getline(s_event, dateAndTime, del);
        std::getline(s_event, vPlatform, del);
        std::getline(s_event, capStr); // Rest of the line is capacity

        if (typeStr.empty() || title.empty() || host.empty() || description.empty() || dateAndTime.empty() || vPlatform.empty() || capStr.empty()) {
            std::cerr << "Warning: Skipped malformed event line during load: " << eventLine << std::endl;
            // Try to consume the attendees line if this event line was malformed
            if (std::getline(eventFile, attendeesLine)) {} 
            continue;
        }
        
        int capacity;
        try {
            capacity = std::stoi(capStr);
        } catch (const std::invalid_argument& ia) {
            std::cerr << "Warning: Invalid capacity format for event (title might be decrypted): '" << cDecrypt(title, SHIFT) << "'. Skipping. Details: " << ia.what() << std::endl;
            if (std::getline(eventFile, attendeesLine)) {}
            continue;
        } catch (const std::out_of_range& oor) {
            std::cerr << "Warning: Capacity out of range for event (title might be decrypted): '" << cDecrypt(title, SHIFT) << "'. Skipping. Details: " << oor.what() << std::endl;
            if (std::getline(eventFile, attendeesLine)) {}
            continue;
        }


        title = cDecrypt(title, SHIFT);
        host = cDecrypt(host, SHIFT);
        description = cDecrypt(description, SHIFT);
        dateAndTime = cDecrypt(dateAndTime, SHIFT);
        vPlatform = cDecrypt(vPlatform, SHIFT);

        EventType eventTypeVal;
        if (typeStr == "0") eventTypeVal = EventType::Webinar;
        else if (typeStr == "1") eventTypeVal = EventType::Conference;
        else if (typeStr == "2") eventTypeVal = EventType::Workshop;
        else {
            std::cerr << "Warning: Unknown event type '" << typeStr << "' for event '" << title << "'. Skipping." << std::endl;
            if (std::getline(eventFile, attendeesLine)) {} // Consume attendees line
            continue;
        }

        Event* currentEvent = nullptr;
        // Create event using the constructor that takes all details (not the User* one for loading)
        if (eventTypeVal == EventType::Webinar) {
            currentEvent = new Webinar(eventTypeVal, title, host, description, dateAndTime, vPlatform, capacity);
        } else if (eventTypeVal == EventType::Conference) {
            currentEvent = new Conference(eventTypeVal, title, host, description, dateAndTime, vPlatform, capacity);
        } else if (eventTypeVal == EventType::Workshop) {
            currentEvent = new Workshop(eventTypeVal, title, host, description, dateAndTime, vPlatform, capacity);
        }

        if (currentEvent) {
            // Now read the next line for attendees
            if (std::getline(eventFile, attendeesLine) && !attendeesLine.empty()) {
                std::stringstream s_attendees(attendeesLine);
                std::string attendeeRecord;
                char attendeeDel = ';';
                char detailDel = ',';

                while (std::getline(s_attendees, attendeeRecord, attendeeDel)) {
                    if (attendeeRecord.empty()) continue;

                    std::stringstream s_attendee_details(attendeeRecord);
                    std::string attName, attEmail, attPhone, attCompany;

                    std::getline(s_attendee_details, attName, detailDel);
                    std::getline(s_attendee_details, attEmail, detailDel);
                    std::getline(s_attendee_details, attPhone, detailDel);
                    std::getline(s_attendee_details, attCompany); // Rest for company

                     if (attName.empty() || attEmail.empty() || attPhone.empty() || attCompany.empty()) {
                         std::cerr << "Warning: Skipped malformed attendee record for event '" << title << "': " << attendeeRecord << std::endl;
                        continue;
                    }

                    attName = cDecrypt(attName, SHIFT);
                    attEmail = cDecrypt(attEmail, SHIFT);
                    attPhone = cDecrypt(attPhone, SHIFT);
                    attCompany = cDecrypt(attCompany, SHIFT);

                    Attendee* newAttendee = new Attendee(attName, attEmail, attPhone, attCompany);
                    currentEvent->addAttendee(newAttendee);
                }
            }
            allEvents.push_back(currentEvent);
        } else {
             // If currentEvent is null, it means an unknown type was encountered earlier
             // The attendees line for this problematic event still needs to be consumed if it exists
             if (eventFile.peek() != EOF && eventFile.peek() != '\n') { // Check if there's an attendees line to consume
                if (std::getline(eventFile, attendeesLine)) { /* consumed */ }
             }
        }
    }
    eventFile.close();
}

// Saves all current events to the specified event file
void EventsManager::saveEventsToFile() {
    std::ofstream eventFile(eventFilePath, std::ios::trunc); // Truncate to overwrite
    if (!eventFile.is_open()) {
        std::cerr << "Error: Could not open event file for writing: " << eventFilePath << std::endl;
        return;
    }

    for (const Event* e : allEvents) {
        if (!e) continue;

        eventFile << static_cast<int>(e->getType()) << '|'
                  << cEncrypt(e->getTitle(), SHIFT) << '|'
                  << cEncrypt(e->getHost(), SHIFT) << '|'
                  << cEncrypt(e->getDescription(), SHIFT) << '|'
                  << cEncrypt(e->getDateAndTime(), SHIFT) << '|'
                  << cEncrypt(e->getVPlatform(), SHIFT) << '|'
                  << e->getCapacity() << "\n";

        const auto& attendees = e->getAttendees();
        for (size_t i = 0; i < attendees.size(); ++i) {
            const Attendee* a = attendees[i];
            if (!a) continue;
            eventFile << cEncrypt(a->getName(), SHIFT) << ','
                      << cEncrypt(a->getEmail(), SHIFT) << ','
                      << cEncrypt(a->getPhoneNum(), SHIFT) << ','
                      << cEncrypt(a->getCompanyOrSchool(), SHIFT);
            if (i < attendees.size() - 1) {
                eventFile << ";";
            }
        }
        eventFile << "\n"; // Newline after each event's attendees (even if none)
    }
    eventFile.close();
}

// Creates a new event based on user input
void EventsManager::createEvent(User* user) {
    if (!user) {
        std::cout << "Cannot create event: No user context provided." << std::endl;
        return;
    }
    std::cout << "Select Event Type:\n1. Webinar\n2. Conference\n3. Workshop\nChoice: ";
    int choice;
    while (!(std::cin >> choice) || choice < 1 || choice > 3) {
        std::cout << "Invalid choice. Please enter 1, 2, or 3: ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 

    EventType type;
    switch (choice) {
        case 1: type = EventType::Webinar; break;
        case 2: type = EventType::Conference; break;
        case 3: type = EventType::Workshop; break;
        default: std::cout << "Invalid type selected somehow.\n"; return; // Should be caught by loop
    }

    // The Event::createEvent factory method uses the Event(User*) constructor,
    // which handles prompting for event details.
    Event* newEvent = Event::createEvent(type, user); 
    if (newEvent) {
        allEvents.push_back(newEvent);
        std::cout << "Event '" << newEvent->getTitle() << "' created successfully.\n";
    } else {
        std::cout << "Failed to create event.\n";
    }
}

// Displays all events
void EventsManager::showAllEvents() const {
    if (allEvents.empty()) {
        std::cout << "No events created yet.\n";
        return;
    }
    std::cout << "\n--- All Events ---" << std::endl;
    int count = 1;
    for (const Event* e : allEvents) {
        if (!e) continue;
        std::cout << count++ << ". Title: " << e->getTitle() << std::endl;
        std::cout << "   Host: " << e->getHost() << std::endl;
        std::cout << "   Date: " << e->getDateAndTime() << std::endl;
        std::cout << "   Platform: " << e->getVPlatform() << std::endl;
        std::cout << "   Registered: " << e->getAttendees().size() << "/" << e->getCapacity() << std::endl;
        std::cout << "   Description: " << e->getDescription() << std::endl;
        std::cout << "   Attendees:" << std::endl;
        if (e->getAttendees().empty()) {
            std::cout << "     No attendees yet." << std::endl;
        } else {
            for (const Attendee* att : e->getAttendees()) {
                if (att) { // Should always be true if addAttendee checks
                    std::cout << "     - " << att->getName() << " (" << att->getEmail() << ")" << std::endl;
                }
            }
        }
        std::cout << "--------------------" << std::endl;
    }
}

// Deletes an event by its title
void EventsManager::deleteEvent() {
    if (allEvents.empty()) {
        std::cout << "No events to delete.\n";
        return;
    }
    std::string titleToDelete;
    std::cout << "Enter the exact title of the event to delete: ";
    std::getline(std::cin >> std::ws, titleToDelete);

    std::string bestMatchTitle = "";
    int minDist = std::numeric_limits<int>::max();
    Event* eventFound = nullptr;
    auto it = allEvents.begin();
    auto foundIt = allEvents.end();

    for (it = allEvents.begin(); it != allEvents.end(); ++it) {
        if ((*it)->getTitle() == titleToDelete) {
            eventFound = *it;
            foundIt = it;
            break;
        }
        updateBestMatch(titleToDelete, (*it)->getTitle(), bestMatchTitle, minDist);
    }

    if (eventFound) {
        std::cout << "Event '" << eventFound->getTitle() << "' found. Confirm deletion (1 for yes, 0 for no): ";
        int confirm;
        while (!(std::cin >> confirm) || (confirm != 0 && confirm != 1)) {
            std::cout << "Invalid input. Enter 1 for yes or 0 for no: ";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (confirm == 1) {
            delete eventFound; // Deallocate Event and its Attendees
            allEvents.erase(foundIt); // Remove pointer from vector
            std::cout << "Event '" << titleToDelete << "' deleted successfully.\n";
        } else {
            std::cout << "Deletion cancelled.\n";
        }
    } else {
        std::cout << "Event with title '" << titleToDelete << "' not found.\n";
        if (!bestMatchTitle.empty() && minDist < 5) { // Suggest if a close match exists (threshold for closeness)
            std::cout << "Did you mean '" << bestMatchTitle << "'? (1 for yes, 0 for no): ";
            int choice;
            while (!(std::cin >> choice) || (choice != 0 && choice != 1)) {
                 std::cout << "Invalid input. Enter 1 for yes or 0 for no: ";
                 std::cin.clear();
                 std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            if (choice == 1) {
                // Find and delete the suggested event
                for (auto innerIt = allEvents.begin(); innerIt != allEvents.end(); ++innerIt) {
                    if ((*innerIt)->getTitle() == bestMatchTitle) {
                        std::cout << "Deleting event '" << bestMatchTitle << "'." << std::endl;
                        delete *innerIt;
                        allEvents.erase(innerIt);
                        std::cout << "Event '" << bestMatchTitle << "' deleted successfully.\n";
                        return; // Exit after deletion
                    }
                }
            }
        }
    }
}

// Allows a user to sign up for an event
void EventsManager::signUpForEvent(User* user) {
    if (allEvents.empty()) {
        std::cout << "No events available to sign up for.\n";
        return;
    }
     if (!user || user->getName().empty()) { // Check if user details are set
        std::cout << "User details are not set. Please update your details first (Option 7 in main menu).\n";
        return;
    }

    std::string eventTitleQuery;
    std::cout << "Enter the exact title of the event to sign up for: ";
    std::getline(std::cin >> std::ws, eventTitleQuery);

    Event* targetEvent = nullptr;
    std::string bestMatchTitle = "";
    int minDist = std::numeric_limits<int>::max();

    for (Event* e : allEvents) {
        if (e->getTitle() == eventTitleQuery) {
            targetEvent = e;
            break;
        }
        updateBestMatch(eventTitleQuery, e->getTitle(), bestMatchTitle, minDist);
    }

    if (targetEvent) {
        targetEvent->signUp(user); // Delegate to the event's signUp method
    } else {
        std::cout << "Event with title '" << eventTitleQuery << "' not found.\n";
        if (!bestMatchTitle.empty() && minDist < 5) { // Suggestion threshold
            std::cout << "Did you mean '" << bestMatchTitle << "'? (1 for yes, 0 for no): ";
            int choice;
            while (!(std::cin >> choice) || (choice != 0 && choice != 1)) {
                 std::cout << "Invalid input. Enter 1 for yes or 0 for no: ";
                 std::cin.clear();
                 std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            if (choice == 1) {
                for (Event* e : allEvents) { // Find the suggested event
                    if (e->getTitle() == bestMatchTitle) {
                        e->signUp(user);
                        return; // Exit after sign up
                    }
                }
            }
        }
    }
}

// Searches for events by title and displays matching results
void EventsManager::searchEvents() const {
    if (allEvents.empty()) {
        std::cout << "No events to search.\n";
        return;
    }
    std::string query;
    std::cout << "Enter event title to search (can be partial): ";
    std::getline(std::cin >> std::ws, query);

    std::vector<std::string> foundTitles; // Store titles of matching events
    for (const Event* e : allEvents) {
        if(e) { // Ensure event pointer is not null
            // Using addIfAccurateEnough for fuzzy search
            addIfAccurateEnough(foundTitles, query, e->getTitle(), 0.6); // Adjust threshold as needed (e.g., 0.6 for more lenient search)
        }
    }

    if (foundTitles.empty()) {
        std::cout << "No events found matching your query '" << query << "'.\n";
    } else {
        std::cout << "\n--- Search Results for '" << query << "' ---" << std::endl;
        int count = 1;
        // To avoid duplicates if addIfAccurateEnough adds the same title multiple times from different checks (unlikely with current usage)
        // Or if multiple events somehow had the exact same title (which should be avoided by design)
        // A std::set could be used here for unique titles before display if that becomes an issue.
        for (const std::string& title : foundTitles) {
            // Iterate through allEvents to find the event matching the title to display its details
            for (const Event* e : allEvents) {
                if (e && e->getTitle() == title) {
                    std::cout << count++ << ". Title: " << e->getTitle() << std::endl;
                    std::cout << "   Host: " << e->getHost() << std::endl;
                    std::cout << "   Date: " << e->getDateAndTime() << std::endl;
                    std::cout << "   Platform: " << e->getVPlatform() << std::endl;
                    std::cout << "   Registered: " << e->getAttendees().size() << "/" << e->getCapacity() << std::endl;
                    // Do not display full description or attendee list in search results for brevity
                    std::cout << "--------------------" << std::endl;
                    break; // Found the event, move to the next title in foundTitles
                }
            }
        }
    }
}
