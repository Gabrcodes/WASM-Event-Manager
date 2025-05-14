#include "EventSystemEntities.h"
#include <iostream> // For cout, cin, ws, getline
#include <limits>   // For numeric_limits

// --- User Class Implementation ---
User* User::instance = nullptr;

User::User() {
    // Private constructor for Singleton
}

User* User::getInstance() {
    if (!instance) {
        instance = new User();
    }
    return instance;
}

void User::setName(const std::string& n) { name = n; }
void User::setEmail(const std::string& e) { email = e; }
void User::setPhoneNumber(const std::string& p) { phoneNum = p; }
void User::setCompanyOrSchool(const std::string& c) { companyOrSchool = c; }

std::string User::getName() const { return name; }
std::string User::getEmail() const { return email; }
std::string User::getPhoneNumber() const { return phoneNum; }
std::string User::getCompanyOrSchool() const { return companyOrSchool; }

// --- Attendee Class Implementation ---
Attendee::Attendee(std::string n, std::string e, std::string p, std::string cs)
    : name(n), email(e), phoneNum(p), companyOrSchool(cs) {}

std::string Attendee::getName() const { return name; }
std::string Attendee::getEmail() const { return email; }
std::string Attendee::getPhoneNum() const { return phoneNum; }
std::string Attendee::getCompanyOrSchool() const { return companyOrSchool; }

// --- Event Class Implementation ---
Event::Event(std::string t, std::string h, std::string d, std::string dt, std::string v, int c)
    : title(t), host(h), description(d), dateAndTime(dt), vPlatform(v), capacity(c) {}

Event::Event(User* user) {
    if (user) {
        host = user->getName();
    } else {
        host = "Unknown Host"; // Fallback if user is somehow null
    }

    std::cout << "Enter title: ";
    std::getline(std::cin >> std::ws, title);

    std::cout << "Enter description: ";
    std::getline(std::cin >> std::ws, description);

    std::cout << "Enter date and time (e.g., YYYY-MM-DD HH:MM): ";
    std::getline(std::cin >> std::ws, dateAndTime);

    std::cout << "Enter Virtual Platform (e.g., Zoom, Google Meet): ";
    std::getline(std::cin >> std::ws, vPlatform);

    std::cout << "Enter Capacity: ";
    while (!(std::cin >> capacity) || capacity < 0) {
        std::cout << "Invalid input. Please enter a non-negative number for Capacity: ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Consume trailing newline
}

Event::~Event() {
    for (Attendee* a : attendees) {
        delete a;
    }
    attendees.clear();
}

const std::vector<Attendee*>& Event::getAttendees() const {
    return attendees;
}

void Event::addAttendee(Attendee* a) {
    if (a) {
        attendees.push_back(a);
    }
}

std::string Event::getTitle() const { return title; }
std::string Event::getHost() const { return host; }
std::string Event::getDescription() const { return description; }
std::string Event::getDateAndTime() const { return dateAndTime; }
std::string Event::getVPlatform() const { return vPlatform; }
int Event::getCapacity() const { return capacity; }

void Event::setTitle(const std::string& t) { title = t; }
void Event::setHost(const std::string& h) { host = h; }
void Event::setDescription(const std::string& d) { description = d; }
void Event::setDateAndTime(const std::string& dt) { dateAndTime = dt; }
void Event::setVPlatform(const std::string& v) { vPlatform = v; }
void Event::setCapacity(int c) { capacity = c; }

// Static factory method for Event
Event* Event::createEvent(EventType type, User* user) {
    // Note: The derived class constructors (Webinar, Conference, Workshop)
    // that take a User* will call the Event(User*) constructor,
    // which prompts for details.
    switch (type) {
        case EventType::Webinar:
            return new Webinar(user); // This constructor calls Event(user)
        case EventType::Conference:
            return new Conference(user); // This constructor calls Event(user)
        case EventType::Workshop:
            return new Workshop(user); // This constructor calls Event(user)
        default:
            std::cerr << "Error: Unknown event type in Event::createEvent factory." << std::endl;
            return nullptr;
    }
}


// --- Webinar Class Implementation ---
Webinar::Webinar(EventType ty, std::string t, std::string h, std::string d, std::string dt, std::string v, int c)
    : Event(t, h, d, dt, v, c) {
    // 'ty' is implicitly EventType::Webinar due to class type, but passed for consistency with file loading.
}

Webinar::Webinar(User* user) : Event(user) {
    // Base class constructor Event(user) handles input
}

void Webinar::signUp(User* user) {
    if (!user) {
        std::cout << "Error: Cannot sign up null user." << std::endl;
        return;
    }
    if (attendees.size() >= static_cast<size_t>(capacity)) {
        std::cout << "Sorry! Capacity for the webinar '" << title << "' is full." << std::endl;
        return;
    }
    Attendee* a = new Attendee(user->getName(), user->getEmail(), user->getPhoneNumber(), user->getCompanyOrSchool());
    addAttendee(a);
    std::cout << "Successfully signed up " << user->getName() << " for the Webinar: " << title << std::endl;
    std::cout << "Event Date & Time: " << getDateAndTime() << ". See you there!" << std::endl;
}

EventType Webinar::getType() const {
    return EventType::Webinar;
}

// --- Conference Class Implementation ---
Conference::Conference(EventType ty, std::string t, std::string h, std::string d, std::string dt, std::string v, int c)
    : Event(t, h, d, dt, v, c) {}

Conference::Conference(User* user) : Event(user) {}

void Conference::signUp(User* user) {
    if (!user) {
        std::cout << "Error: Cannot sign up null user." << std::endl;
        return;
    }
    if (attendees.size() >= static_cast<size_t>(capacity)) {
        std::cout << "Sorry! Capacity for the conference '" << title << "' is full." << std::endl;
        return;
    }
    Attendee* a = new Attendee(user->getName(), user->getEmail(), user->getPhoneNumber(), user->getCompanyOrSchool());
    addAttendee(a);
    std::cout << "Successfully signed up " << user->getName() << " for the Conference: " << title << std::endl;
    std::cout << "Event Date & Time: " << getDateAndTime() << ". See you there!" << std::endl;
}

EventType Conference::getType() const {
    return EventType::Conference;
}

// --- Workshop Class Implementation ---
Workshop::Workshop(EventType ty, std::string t, std::string h, std::string d, std::string dt, std::string v, int c)
    : Event(t, h, d, dt, v, c) {}

Workshop::Workshop(User* user) : Event(user) {}

void Workshop::signUp(User* user) {
    if (!user) {
        std::cout << "Error: Cannot sign up null user." << std::endl;
        return;
    }
    if (attendees.size() >= static_cast<size_t>(capacity)) {
        std::cout << "Sorry! Capacity for the workshop '" << title << "' is full." << std::endl;
        return;
    }
    Attendee* a = new Attendee(user->getName(), user->getEmail(), user->getPhoneNumber(), user->getCompanyOrSchool());
    addAttendee(a);
    std::cout << "Successfully signed up " << user->getName() << " for the Workshop: " << title << std::endl;
    std::cout << "Event Date & Time: " << getDateAndTime() << ". See you there!" << std::endl;
}

EventType Workshop::getType() const {
    return EventType::Workshop;
}
