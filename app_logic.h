#pragma once // Include guard

#define SHIFT 32 

#include <string> 
#include <vector> 
#include <limits>    
#include <algorithm> 
#include <fstream>   
#include <sstream>   
#include <stdexcept> 

// --- Enum Definitions ---
enum eventType { Webinar, Conference, Workshop };

// --- Class Declarations ---
class User; 

// --- Helper Function Declarations ---
std::string cEncrypt(const std::string& str, int shift);
std::string cDecrypt(const std::string& str, int shift);
int levenshteinDistance(const std::string& a, const std::string& b);
void updateBestMatch(const std::string& query, const std::string& candidate, std::string& bestMatch, int& minDist);
void addIfAccurateEnough(std::vector<std::string>& vec, const std::string& query, const std::string& current, double threshold = 0.75);


class User {
private:
    static User* instance;
    User(); 

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

class attendee {
private:
    std::string name;
    std::string email;
    std::string phoneNum;
    std::string companyOrSchool;
public:
    attendee(std::string n, std::string e, std::string p, std::string cs);
    std::string getName() const;      // Added const
    std::string getEmail() const;     // Added const
    std::string getPhoneNum() const;  // Added const
    std::string getcompanyOrSchool() const; // Added const
};

class event {
protected:
    std::string title;
    std::string host;
    std::string description;
    std::string dateAndTime;
    std::string vPlatform;
    int capacity;
    std::vector<attendee*> attendees;

public:
    event(std::string t, std::string h, std::string d, std::string dt, std::string v, int c);
    event(User* user, const std::string& t, const std::string& desc, const std::string& dt, const std::string& vp, int cap);

    virtual ~event();

    virtual std::string signUp(User* user, std::string& message) = 0; 
    virtual eventType getType() = 0; 

    std::string getTitle() const; // Added const
    std::string getHost() const; // Added const
    std::string getDescription() const; // Added const
    std::string getdateAndTime() const; // Added const
    std::string getvPlatform() const; // Added const
    int getcapacity() const; // Added const
    int getAttendeeCount() const;

    // New methods for attendee management
    const std::vector<attendee*>& getAttendees() const; // Get a const ref to the vector
    void addAttendee(attendee* a); // Add an attendee

    void setTitle(std::string t);
    void setHost(std::string h);
    void setDescription(std::string d);
    void setDateAndTime(std::string dt);
    void setvPlatform(std::string v);
    void setCapacity(int c);

    static event* createEvent(eventType type, User* user, const std::string& title, const std::string& desc, const std::string& dts, const std::string& vps, int capacity_val);
};

class webinar : public event {
private:
    eventType type_val;
public:
    webinar(eventType ty_enum, std::string t, std::string h, std::string d, std::string dt, std::string v, int c);
    webinar(User* user, const std::string& t, const std::string& desc, const std::string& date, const std::string& vp, int cap);
    std::string signUp(User* user, std::string& message) override;
    eventType getType() override;
};

class conference : public event {
private:
    eventType type_val;
public:
    conference(eventType ty_enum, std::string t, std::string h, std::string d, std::string dt, std::string v, int c);
    conference(User* user, const std::string& t, const std::string& desc, const std::string& date, const std::string& vp, int cap);
    std::string signUp(User* user, std::string& message) override;
    eventType getType() override;
};

class workshop : public event {
private:
    eventType type_val;
public:
    workshop(eventType ty_enum, std::string t, std::string h, std::string d, std::string dt, std::string v, int c);
    workshop(User* user, const std::string& t, const std::string& desc, const std::string& date, const std::string& vp, int cap);
    std::string signUp(User* user, std::string& message) override;
    eventType getType() override;
};

class events {
private:
    std::vector<event*> allEvents;
    std::string dataFilePath; 
    // Global suggestedMatch for "did you mean" functionality, accessed via extern in main.cpp
    // This is not ideal; better to pass it around or make it a member of a UI state class.
    // For now, keeping it as it was in the previous structure for minimal changes outside app_logic.
    // extern std::string suggestedMatch; 


public:
    events(); 
    ~events(); 

    std::string createEvent(User* user, eventType type_enum, const std::string& title, const std::string& desc, const std::string& dt, const std::string& vp, const std::string& capStr);
    std::vector<std::string> getAllEventTitles() const;
    std::string attemptDeleteEvent(const std::string& titleQuery, bool& foundDirectly, bool& foundSuggestion);
    std::string confirmDeleteSuggestedEvent();
    std::string attemptSignUp(User* user, const std::string& titleQuery, bool& foundDirectly, bool& foundSuggestion, std::string& signUpMessage);
    std::string confirmSignUpSuggestedEvent(User* user, std::string& signUpMessage);
    std::vector<std::string> searchEvents(const std::string& query);
    
    bool saveEventsToFile();
    void loadEventsFromFile();
};
