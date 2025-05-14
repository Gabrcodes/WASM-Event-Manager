#pragma once // Include guard

#include <string>
#include <vector>
#include <limits>    // Required for std::numeric_limits
#include <algorithm> // Required for std::max, std::min
#include <fstream>   // Required for fstream
#include <sstream>   // Required for stringstream
#include <stdexcept> // Required for std::invalid_argument, std::out_of_range

// Using std namespace for convenience in this header, or qualify types.
// For larger projects, it's often better to qualify (e.g., std::string)
// or use 'using' declarations for specific types within .cpp files.
using namespace std;


#define SHIFT 32 

// --- Helper Function Declarations ---
string cEncrypt(const string& str, int shift);
string cDecrypt(const string& str, int shift);
int levenshteinDistance(const string& a, const string& b);
void updateBestMatch(const string& query, const string& candidate, string& bestMatch, int& minDist);
void addIfAccurateEnough(vector<string>& vec, const string& query, const string& current, double threshold = 0.75);

// --- Enum Definitions ---
enum eventType { Webinar, Conference, Workshop };

// --- Class Declarations ---

class User {
private:
    static User* instance;
    User(); // Private constructor for Singleton

    string name;
    string email;
    string phoneNum;
    string companyOrSchool;

public:
    User(const User&) = delete; // Disable copy constructor
    User& operator=(const User&) = delete; // Disable assignment operator

    static User* getInstance();

    void setName(const string& n);
    void setEmail(const string& e);
    void setPhoneNumber(const string& p);
    void setCompanyOrSchool(const string& c);

    string getName() const;
    string getEmail() const;
    string getPhoneNumber() const;
    string getCompanyOrSchool() const;
};

class attendee {
private:
    string name;
    string email;
    string phoneNum;
    string companyOrSchool;
public:
    attendee(string n, string e, string p, string cs);
};

class event {
protected:
    string title;
    string host;
    string description;
    string dateAndTime;
    string vPlatform;
    int capacity;
    vector<attendee*> attendees;

public:
    event(string t, string h, string d, string dt, string v, int c);
    event(User* user, const string& t, const string& desc, const string& dt, const string& vp, int cap);

    virtual ~event();

    virtual string signUp(User* user, string& message) = 0; 
    virtual eventType getType() = 0; 

    string getTitle() const;
    string getHost() const;
    string getDescription() const;
    string getdateAndTime() const;
    string getvPlatform() const;
    int getcapacity() const;
    int getAttendeeCount() const;

    void setTitle(string t);
    void setHost(string h);
    void setDescription(string d);
    void setDateAndTime(string dt);
    void setvPlatform(string v);
    void setCapacity(int c);

    static event* createEvent(eventType type, User* user, const string& title, const string& desc, const string& dts, const string& vps, int capacity_val);
};

class webinar : public event {
private:
    eventType type_val;
public:
    webinar(eventType ty_enum, string t, string h, string d, string dt, string v, int c);
    webinar(User* user, const string& t, const string& desc, const string& date, const string& vp, int cap);
    string signUp(User* user, string& message) override;
    eventType getType() override;
};

class conference : public event {
private:
    eventType type_val;
public:
    conference(eventType ty_enum, string t, string h, string d, string dt, string v, int c);
    conference(User* user, const string& t, const string& desc, const string& date, const string& vp, int cap);
    string signUp(User* user, string& message) override;
    eventType getType() override;
};

class workshop : public event {
private:
    eventType type_val;
public:
    workshop(eventType ty_enum, string t, string h, string d, string dt, string v, int c);
    workshop(User* user, const string& t, const string& desc, const string& date, const string& vp, int cap);
    string signUp(User* user, string& message) override;
    eventType getType() override;
};

class events {
private:
    vector<event*> allEvents;
    string dataFilePath; 

public:
    events(); 
    ~events(); 

    string createEvent(User* user, eventType type_enum, const string& title, const string& desc, const string& dt, const string& vp, const string& capStr);
    vector<string> getAllEventTitles() const;
    string attemptDeleteEvent(const string& titleQuery, bool& foundDirectly, bool& foundSuggestion);
    string confirmDeleteSuggestedEvent();
    string attemptSignUp(User* user, const string& titleQuery, bool& foundDirectly, bool& foundSuggestion, string& signUpMessage);
    string confirmSignUpSuggestedEvent(User* user, string& signUpMessage);
    vector<string> searchEvents(const string& query);
    
    bool saveEventsToFile();
    void loadEventsFromFile();
};
