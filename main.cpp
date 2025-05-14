#include <iostream>
#include <string>
#include <vector>
#include <limits>   // Required for std::numeric_limits
// No need to include fstream here as EventsManager handles it.

// #include "utils.h" // Included via EventsManager or EventSystemEntities if needed by them directly
#include "EventSystemEntities.h" // Includes User, Attendee, Event, EventType, derived events
#include "EventsManager.h"       // Manages events and uses entities

// Function to display the main menu
void displayMenu() {
    std::cout << "\n===== Event Management System =====" << std::endl;
    std::cout << "1. Create Event" << std::endl;
    std::cout << "2. Delete Event" << std::endl;
    std::cout << "3. Show All Events" << std::endl;
    std::cout << "4. Sign Up for an Event" << std::endl;
    std::cout << "5. Search Events" << std::endl;
    std::cout << "6. View My Details" << std::endl;
    std::cout << "7. Update My Details" << std::endl;
    std::cout << "8. Exit" << std::endl;
    std::cout << "===================================" << std::endl;
    std::cout << "Enter your choice: ";
}

// Function to get/update user details
void manageUserDetails(User* user) {
    std::string name, email, phone, company;
    std::cout << "\n--- Update Your Details ---" << std::endl;
    
    std::cout << "Current Name: " << user->getName() << std::endl;
    std::cout << "New Name (leave blank to keep current): ";
    std::getline(std::cin >> std::ws, name);
    if (!name.empty()) user->setName(name);

    std::cout << "Current Email: " << user->getEmail() << std::endl;
    std::cout << "New Email (leave blank to keep current): ";
    std::getline(std::cin >> std::ws, email);
    if (!email.empty()) user->setEmail(email);

    std::cout << "Current Phone: " << user->getPhoneNumber() << std::endl;
    std::cout << "New Phone (leave blank to keep current): ";
    std::getline(std::cin >> std::ws, phone);
    if (!phone.empty()) user->setPhoneNumber(phone);

    std::cout << "Current Company/School: " << user->getCompanyOrSchool() << std::endl;
    std::cout << "New Company/School (leave blank to keep current): ";
    std::getline(std::cin >> std::ws, company);
    if (!company.empty()) user->setCompanyOrSchool(company);

    std::cout << "Details updated successfully!" << std::endl;
}

// Function to view user details
void viewUserDetails(const User* user) {
    if (!user) {
        std::cout << "User data not available." << std::endl;
        return;
    }
    std::cout << "\n--- Your Details ---" << std::endl;
    std::cout << "Name: " << user->getName() << std::endl;
    std::cout << "Email: " << user->getEmail() << std::endl;
    std::cout << "Phone: " << user->getPhoneNumber() << std::endl;
    std::cout << "Company/School: " << user->getCompanyOrSchool() << std::endl;
    std::cout << "--------------------" << std::endl;
}


int main() {
    // Define the path for the event data file
    const std::string eventDataFile = "EventFile.txt";

    // Initialize EventsManager, which will load events from the file
    EventsManager eventManager(eventDataFile);

    // Get the singleton User instance
    User* currentUser = User::getInstance();

    // Initial prompt for user details if they are not set (e.g., first run)
    if (currentUser->getName().empty() && currentUser->getEmail().empty()) {
        std::cout << "Welcome! It looks like this is your first time or your details are not set." << std::endl;
        manageUserDetails(currentUser); // Use the update function to set initial details
    }

    int choice;
    while (true) {
        displayMenu();
        // Input validation for menu choice
        while (!(std::cin >> choice)) {
            std::cout << "Invalid input. Please enter a number: ";
            std::cin.clear(); // Clear error flags
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Discard invalid input
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Consume the rest of the line after reading integer

        switch (choice) {
            case 1:
                eventManager.createEvent(currentUser);
                break;
            case 2:
                eventManager.deleteEvent();
                break;
            case 3:
                eventManager.showAllEvents();
                break;
            case 4:
                eventManager.signUpForEvent(currentUser);
                break;
            case 5:
                eventManager.searchEvents();
                break;
            case 6:
                viewUserDetails(currentUser);
                break;
            case 7:
                manageUserDetails(currentUser);
                break;
            case 8:
                std::cout << "Saving events and exiting. Goodbye!" << std::endl;
                // Destructor of eventManager will handle saving.
                return 0; // Exit the program
            default:
                std::cout << "Invalid option, please try again." << std::endl;
        }
    }

    // The User instance is a singleton and typically not deleted manually in this simple setup,
    // as its lifetime is intended to be the duration of the application.
    // OS will reclaim memory on program exit. For more complex scenarios,
    // a proper cleanup mechanism for singletons might be considered.

    return 0; // Should be unreachable due to loop and exit condition
}
