#include "EventSystem.h" // Single header for all project declarations
#include <iostream>      // For standard I/O in main
#include <string>        // For std::string in main
#include <limits>        // For std::numeric_limits
#include <fstream>       // For std::fstream

// Using namespace std as in the original global scope effect for main
using namespace std;

int main() {
    // File stream management
    fstream eventFile("EventFile.txt", ios::in | ios::out | ios::app);
    if (!eventFile.is_open()) {
        eventFile.open("EventFile.txt", ios::out); // Create if not exists
        eventFile.close();
        eventFile.open("EventFile.txt", ios::in | ios::out | ios::app);
        if (!eventFile.is_open()) {
            cerr << "Error: Could not open or create EventFile.txt. Exiting." << endl;
            return 1;
        }
    }

    Events eventManager(eventFile); // Uses the 'Events' class (original name 'events')
    User* user = User::getInstance();

    // Set user details once - exactly as in original main
    string name_main, email_main, phone_main, company_main; // Renamed to avoid conflict
    cout << "Enter your details:\n";
    cout << "Name: ";
    getline(cin >> ws, name_main);
    cout << "Email: ";
    getline(cin >> ws, email_main);
    cout << "Phone: ";
    getline(cin >> ws, phone_main);
    cout << "Company/School: ";
    getline(cin >> ws, company_main);

    user->setName(name_main);
    user->setEmail(email_main);
    user->setPhoneNumber(phone_main);
    user->setCompanyOrSchool(company_main);

    int choice;
    while (true) {
        cout << "\nMenu:\n";
        cout << "1. Create Event\n";
        cout << "2. Delete Event\n";
        cout << "3. Show All Events\n";
        cout << "4. Sign Up for an event\n";
        cout << "5. Search\n";
        cout << "6. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input, please enter a number.\n";
            continue;
        }
        // Consider adding cin.ignore() here if subsequent operations use getline

        switch (choice) {
            case 1:
                eventManager.createEvent(user);
                break;
            case 2:
                eventManager.deleteEvent();
                break;
            case 3:
                eventManager.showAllEvents();
                break;
            case 4:
                eventManager + user; // Calls Events::signUp
                break;
            case 5:
                eventManager.search();
                break;
            case 6:
                cout << "Goodbye!\n";
                if (eventFile.is_open()) {
                    eventFile.close(); // Close file before exiting
                }
                // Destructor of eventManager will also attempt to save.
                // Ensure file is handled consistently.
                return 0;
            default:
                cout << "Invalid option, try again.\n";
        }
    }
    // Should be unreachable
    // if (eventFile.is_open()) {
    //    eventFile.close();
    // }
    // return 0; 
}
