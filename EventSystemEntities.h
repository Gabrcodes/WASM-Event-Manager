#ifndef EVENTSYSTEMENTITIES_H
#define EVENTSYSTEMENTITIES_H

#include <string>
#include <vector>
#include <iostream> // For cin, cout, ws, getline in Event constructor
#include <limits>   // For numeric_limits in Event constructor input validation

// Forward declaration for User in Event constructor
class User; 
// Forward declaration for Attendee in Event class
class Attendee;


// Enum to define different types of events
enum class EventType {
    Webinar,
    Conference,
    Workshop
};

// User class implemented as a Singleton
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

// Attendee class to store information about an event attendee
class Attendee {
private:
    std::string name;
    std::string email;
    std::string phoneNum;
    std::string companyOrSchool;

public:
    Attendee(std::string n, std::string e, std::string p, std::string cs);

    std::string getName() const;
    std::string getEmail() const;
    std::string getPhoneNum() const;
    std::string getCompanyOrSchool() const;
};

// Base class for events
class Event {
protected:
    std::string title;
    std::string host;
    std::string description;
    std::string dateAndTime;
    std::string vPlatform;
    int capacity;
    std::vector<Attendee*> attendees;

public:
    // Constructor for pre-defined event details
    Event(std::string t, std::string h, std::string d, std::string dt, std::string v, int c);
    // Constructor for user-input driven event creation
    Event(User* user);
    virtual ~Event();

    const std::vector<Attendee*>& getAttendees() const;
    void addAttendee(Attendee* a);

    virtual void signUp(User* user) = 0;
    virtual EventType getType() const = 0;

    std::string getTitle() const;
    std::string getHost() const;
    std::string getDescription() const;
    std::string getDateAndTime() const;
    std::string getVPlatform() const;
    int getCapacity() const;

    void setTitle(const std::string& t);
    void setHost(const std::string& h);
    void setDescription(const std::string& d);
    void setDateAndTime(const std::string& dt);
    void setVPlatform(const std::string& v);
    void setCapacity(int c);
    
    // Static factory method declaration (implementation in .cpp)
    static Event* createEvent(EventType type, User* user);
};

// Derived event classes
class Webinar : public Event {
public:
    Webinar(EventType ty, std::string t, std::string h, std::string d, std::string dt, std::string v, int c);
    Webinar(User* user);
    void signUp(User* user) override;
    EventType getType() const override;
};

class Conference : public Event {
public:
    Conference(EventType ty, std::string t, std::string h, std::string d, std::string dt, std::string v, int c);
    Conference(User* user);
    void signUp(User* user) override;
    EventType getType() const override;
};

class Workshop : public Event {
public:
    Workshop(EventType ty, std::string t, std::string h, std::string d, std::string dt, std::string v, int c);
    Workshop(User* user);
    void signUp(User* user) override;
    EventType getType() const override;
};

#endif // EVENTSYSTEMENTITIES_H
