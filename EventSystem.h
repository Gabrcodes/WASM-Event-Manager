#ifndef EVENTSYSTEM_H
#define EVENTSYSTEM_H

#include <string>
#include <vector>
#include <algorithm> // For std::min, std::max
#include <limits>    // For std::numeric_limits
#include <fstream>   // For std::fstream (used by Events class)
#include <sstream>   // For std::stringstream (used by Events class)
#include <iostream>  // For Event constructor and other IO operations

// Original SHIFT macro
#define SHIFT 32

// Forward declarations to manage dependencies within this header
class User;
class Attendee; // Changed from 'attendee' for consistency
class Event;    // Changed from 'event' for consistency
// class Webinar, Conference, Workshop will be defined after Event
// class Events will be defined after all event types

// Enum Definition (original name: eventType)
// Moved the full definition before its first use in class declarations
enum eventType { Webinar, Conference, Workshop };

// Utility Function Declarations
std::string cEncrypt(const std::string& str, int shift);
std::string cDecrypt(const std::string& str, int shift);
int levenshteinDistance(const std::string& a, const std::string& b);
void updateBestMatch(const std::string& query, const std::string& candidate, std::string& bestMatch, int& minDist);
void addIfAccurateEnough(std::vector<std::string>& vec, const std::string& query, const std::string& current, double threshold = 0.75);

// User Class Declaration (singleton)
class User {
private:
    static User* instance;
    User(); // Private constructor

    std::string name;
    std::string email;
    std::string phoneNum;
    std::string companyOrSchool;

public:
    User(const User&) = delete;
    User& operator=(const User&) = delete;

    static User* getInstance();

    void setName(const std::string& n);
    void setEmail(const std::string& e);
    void setPhoneNumber(const std::string& p);
    void setCompanyOrSchool(const std::string& c);

    std::string getName() const;
    std::string getEmail() const;
    std::string getPhoneNumber() const;
    std::string getCompanyOrSchool() const;
};

// Attendee Class Declaration (original name: attendee)
class Attendee {
private:
    std::string name;
    std::string email;
    std::string phoneNum;
    std::string companyOrSchool; // Matched original casing

public:
    Attendee(std::string n, std::string e, std::string p, std::string cs);

    std::string getName(); // Matched original (non-const)
    std::string getEmail();
    std::string getPhoneNum();
    std::string getcompanyOrSchool(); // Matched original casing
};

// Event Class Declaration (base class, original name: event)
class Event {
protected:
    std::string title;
    std::string host;
    std::string description;
    std::string dateAndTime;
    std::string vPlatform;
    int capacity;
    std::vector<Attendee*> attendees; // Original: vector <attendee *>

public:
    Event(std::string t, std::string h, std::string d, std::string dt, std::string v, int c);
    Event(User* user); // Constructor taking User pointer
    virtual ~Event();

    std::vector<Attendee*> getattendees(); // Matched original casing
    void setattendees(Attendee* a);      // Matched original casing

    virtual void signUp(User* user) = 0;
    virtual eventType getType() = 0; // Now uses the fully defined eventType

    std::string getTitle();
    std::string getHost();
    std::string getDescription();
    std::string getdateAndTime();   // Matched original casing
    std::string getvPlatform();     // Matched original casing
    int getcapacity();              // Matched original casing

    void setTitle(std::string t);
    void setHost(std::string h);
    void setDescription(std::string d);
    void setDateAndTime(std::string dt);
    void setvPlatform(std::string v);
    void setCapacity(int c);

    static Event* createEvent(eventType type, User* user); // Factory method, uses fully defined eventType
};

// Derived Event Class Declarations
// Webinar Class (original name: webinar)
class Webinar : public Event {
private:
    eventType type; // As in original
public:
    Webinar(eventType ty, std::string t, std::string h, std::string d, std::string dt, std::string v, int c);
    Webinar(User* user);
    void signUp(User* user) override;
    eventType getType() override;
};

// Conference Class (original name: conference)
class Conference : public Event {
private:
    eventType type;
public:
    Conference(eventType ty, std::string t, std::string h, std::string d, std::string dt, std::string v, int c);
    Conference(User* user);
    void signUp(User* user) override;
    eventType getType() override;
};

// Workshop Class (original name: workshop)
class Workshop : public Event {
private:
    eventType type;
public:
    Workshop(eventType ty, std::string t, std::string h, std::string d, std::string dt, std::string v, int c);
    Workshop(User* user);
    void signUp(User* user) override;
    eventType getType() override;
};

// Events (Manager) Class Declaration (original name: events)
class Events {
private:
    std::vector<Event*> allEvents; // Original: vector <event *>
public:
    Events(std::fstream& eventFile); // Constructor
    ~Events();                       // Destructor

    void createEvent(User* user);
    void showAllEvents() const;
    void deleteEvent();
    void operator+(User* user);// For the collection, distinct from Event::signUp
    void search();

    bool addToFile(std::fstream& eventFile);
    void getFile(std::fstream& eventFile);
};

#endif // EVENTSYSTEM_H
