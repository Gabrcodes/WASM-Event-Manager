# Event Management System

## Description

This is a simple command-line based Event Management System written in C++. It allows users to create, manage, and sign up for various types of events such as webinars, conferences, and workshops. User and event data is persisted in a local text file (`EventFile.txt`) using a basic Caesar cipher for "encryption" of text fields. The system also features a simple string similarity check (Levenshtein distance) for user input when searching or deleting events.

## Features

* **User Profile:**
    * Enter and store user details (Name, Email, Phone, Company/School).
* **Event Creation:**
    * Create different types of events: Webinar, Conference, Workshop.
    * Specify event details: Title, Host (auto-filled by current user), Description, Date & Time, Virtual Platform, Capacity.
* **Event Management:**
    * **Show All Events:** List all created events with their details.
    * **Delete Event:** Remove an event from the system. Includes a "did you mean?" suggestion for typos.
* **Event Interaction:**
    * **Sign Up for Event:** Allows the current user to register as an attendee for an event. Checks for capacity limits. Includes a "did you mean?" suggestion.
    * **Search Events:** Find events by title using a similarity search.
* **Data Persistence:**
    * Event and attendee data are saved to `EventFile.txt`.
    * Text data within the file is "encrypted" using a Caesar cipher (SHIFT = 32).
* **Basic Error Handling & Suggestions:**
    * Input validation for menu choices.
    * Suggestions for event titles during deletion or sign-up if an exact match isn't found.

## File Structure

The project is organized into the following files:

* `EventSystem.h`: Contains all class declarations (`User`, `Attendee`, `Event`, `Webinar`, `Conference`, `Workshop`, `Events`), the `eventType` enum, and utility function declarations.
* `EventSystem.cpp`: Contains the implementation for all methods of the classes defined in `EventSystem.h` and the utility functions.
* `main.cpp`: The main entry point of the application, handling user interaction via the command-line menu.
* `EventFile.txt`: The data file where event and attendee information is stored (created automatically if it doesn't exist).

## How to Compile

You can compile the project using a C++ compiler that supports C++11 or later (like g++). Navigate to the directory containing the source files and run the following command:

```bash
g++ -std=c++11 main.cpp EventSystem.cpp -o event_management_system
This will create an executable file named event_management_system (or event_management_system.exe on Windows).How to RunAfter successful compilation, run the executable from your terminal:./event_management_system
On Windows, you might run it as:event_management_system.exe
The program will then prompt you to enter your details and present a menu of options.DependenciesA C++ compiler (e.g., g++) supporting C++11 or newer.Standard C++ libraries (iostream, string, vector, fstream, sstream, algorithm, limits).NotesThe "encryption" used
