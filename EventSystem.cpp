#include "EventSystem.h"

using namespace std;

string cEncrypt(const string& str, int shift) {
    string result = str;
    for (char& c : result) {
        if (c != '|') {
            if (c >= 32 && c <= 126)
                c = 32 + (c - 32 + shift) % 95;
        }
    }
    return result;
}

string cDecrypt(const string& str, int shift) {
    string result = str;
    for (char& c : result) {
        if (c != '|') {
            if (c >= 32 && c <= 126)
                c = 32 + (c - 32 - shift + 95) % 95;
        }
    }
    return result;
}

int levenshteinDistance(const string& a, const string& b) {
    int m = a.size(), n = b.size();
    vector<vector<int>> dp(m + 1, vector<int>(n + 1));
    for (int i = 0; i <= m; ++i) dp[i][0] = i;
    for (int j = 0; j <= n; ++j) dp[0][j] = j;
    for (int i = 1; i <= m; ++i)
        for (int j = 1; j <= n; ++j)
            dp[i][j] = (a[i - 1] == b[j - 1]) ? dp[i - 1][j - 1] : 1 + min({dp[i - 1][j], dp[i][j - 1], dp[i - 1][j - 1]});
    return dp[m][n];
}

void updateBestMatch(const string& query, const string& candidate, string& bestMatch, int& minDist) {
    int dist = levenshteinDistance(query, candidate);
    if (dist < minDist) {
        minDist = dist;
        bestMatch = candidate;
    }
}

void addIfAccurateEnough(vector<string>& vec, const string& query, const string& current, double threshold) {
    int distance = levenshteinDistance(query, current);
    int maxLen = max(query.length(), current.length());
    if (maxLen == 0) return;
    double accuracy = 1.0 - static_cast<double>(distance) / maxLen;
    if (accuracy >= threshold) {
        vec.push_back(current);
    }
}

// --- User Class Implementation ---
User* User::instance = nullptr; // Static member initialization

User::User() {
    // Private constructor for Singleton
}

User* User::getInstance() {
    if (!instance) {
        instance = new User();
    }
    return instance;
}

void User::setName(const string& n) { name = n; }
void User::setEmail(const string& e) { email = e; }
void User::setPhoneNumber(const string& p) { phoneNum = p; }
void User::setCompanyOrSchool(const string& c) { companyOrSchool = c; }
string User::getName() const { return name; }
string User::getEmail() const { return email; }
string User::getPhoneNumber() const { return phoneNum; }
string User::getCompanyOrSchool() const { return companyOrSchool; }

// Attendee Class Implementation
Attendee::Attendee(string n, string e, string p, string cs)
    : name(n), email(e), phoneNum(p), companyOrSchool(cs) {}

string Attendee::getName() { return name; }
string Attendee::getEmail() { return email; }
string Attendee::getPhoneNum() { return phoneNum; }
string Attendee::getcompanyOrSchool() { return companyOrSchool; }

// --- Event Class Implementation ---
Event::Event(string t, string h, string d, string dt, string v, int c)
    : title(t), host(h), description(d), dateAndTime(dt), vPlatform(v), capacity(c) {}

Event::Event(User* user) { // constructor taking User input
    host = user->getName(); // assuming user is not null
    cout << "Enter title: ";
    getline(cin >> ws, title);
    cout << "Enter description: ";
    getline(cin >> ws, description);
    cout << "Enter dateAndTime: "; // Matched original prompt
    getline(cin >> ws, dateAndTime);
    cout << "Enter Virtual Platform: ";
    getline(cin >> ws, vPlatform);
    cout << "Enter Capacity: ";
    cin >> capacity;
    // original code didn't have input validation loop here or cin.ignore()
    attendees = {}; // initialize attendees vector
}

Event::~Event() {
    for (auto a : attendees) {
        delete a;
    }
}

vector<Attendee*> Event::getattendees() { return attendees; }
void Event::setattendees(Attendee* a) { attendees.push_back(a); }
string Event::getTitle() { return title; }
string Event::getHost() { return host; }
string Event::getDescription() { return description; }
string Event::getdateAndTime() { return dateAndTime; }
string Event::getvPlatform() { return vPlatform; }
int Event::getcapacity() { return capacity; }
void Event::setTitle(string t) { title = t; }
void Event::setHost(string h) { host = h; }
void Event::setDescription(string d) { description = d; }
void Event::setDateAndTime(string dt) { dateAndTime = dt; }
void Event::setvPlatform(string v) { vPlatform = v; }
void Event::setCapacity(int c) { capacity = c; }

// static factory method definition for Event
Event* Event::createEvent(eventType type, User* user) {
    switch (type) {
        // need to use the actual class names (Webinar, Conference, Workshop)
        case ::Webinar: return new class Webinar(user); // ::Webinar refers to the enum value
        case ::Conference: return new class Conference(user);
        case ::Workshop: return new class Workshop(user);
        default: return nullptr;
    }
}

// --- Webinar Class Implementation ---
Webinar::Webinar(eventType ty, string t, string h, string d, string dt, string v, int c)
    : Event(t, h, d, dt, v, c), type(ty) {}

Webinar::Webinar(User* user) : Event(user) {
    type = ::Webinar; // Assigning from the global enum eventType::Webinar
}

void Webinar::signUp(User* user) {
    if (attendees.size() >= static_cast<size_t>(getcapacity())) { // Use getter for capacity
        cout << "sorry! capacity is full" << endl;
        return;
    }
    Attendee* a = new Attendee(user->getName(), user->getEmail(), user->getPhoneNumber(), user->getCompanyOrSchool());
    // attendees.push_back(a); // Use protected member as in original, or use setattendees
    setattendees(a); // Using the public method is generally safer if available
    cout << "signed up for " << type << " event : " << getTitle() << endl; // Use getter
    cout << "at : " << getdateAndTime() << ". See you there!" << endl; // Use getter
}
eventType Webinar::getType() { return type; }

// --- Conference Class Implementation ---
Conference::Conference(eventType ty, string t, string h, string d, string dt, string v, int c)
    : Event(t, h, d, dt, v, c), type(ty) {}

Conference::Conference(User* user) : Event(user) {
    type = ::Conference;
}
void Conference::signUp(User* user) {
    if (attendees.size() >= static_cast<size_t>(getcapacity())) {
        cout << "sorry! capacity is full" << endl;
        return;
    }
    Attendee* a = new Attendee(user->getName(), user->getEmail(), user->getPhoneNumber(), user->getCompanyOrSchool());
    setattendees(a);
    cout << "signed up for " << type << " event : " << getTitle() << endl;
    cout << "at : " << getdateAndTime() << ". See you there!" << endl;
}
eventType Conference::getType() { return type; }

// --- Workshop Class Implementation ---
Workshop::Workshop(eventType ty, string t, string h, string d, string dt, string v, int c)
    : Event(t, h, d, dt, v, c), type(ty) {}

Workshop::Workshop(User* user) : Event(user) {
    type = ::Workshop;
}
void Workshop::signUp(User* user) {
    if (attendees.size() >= static_cast<size_t>(getcapacity())) {
        cout << "sorry! capacity is full" << endl;
        return;
    }
    Attendee* a = new Attendee(user->getName(), user->getEmail(), user->getPhoneNumber(), user->getCompanyOrSchool());
    setattendees(a);
    cout << "signed up for " << type << " event : " << getTitle() << endl;
    cout << "at : " << getdateAndTime() << ". See you there!" << endl;
}
eventType Workshop::getType() { return type; }

// --- Events (Manager) Class Implementation ---
Events::Events(fstream& eventFile) : allEvents({}) {
    getFile(eventFile); // Load events from file
}

Events::~Events() {
    fstream eventFile("EventFile.txt", ios::in | ios::out); // Original re-opened file
    if (eventFile.is_open()) {
        addToFile(eventFile); // Save events
        eventFile.close();
    } else {
        // cerr << "Error: Could not open EventFile.txt in Events destructor for writing." << endl;
    }
    for (auto e : allEvents) {
        delete e;
    }
}

void Events::createEvent(User* user) {
    int choice;
    cout << "Select Event Type:\n1. Webinar\n2. Conference\n3. Workshop\nChoice: ";
    cin >> choice;
    // No extensive input validation in original
    eventType type_val;
    switch (choice) {
        case 1: type_val = ::Webinar; break;
        case 2: type_val = ::Conference; break;
        case 3: type_val = ::Workshop; break;
        default: cout << "Invalid choice!\n"; return;
    }
    Event* newEvent = Event::createEvent(type_val, user); // Use static factory
    if (newEvent) {
        allEvents.push_back(newEvent);
        cout << "Event created successfully.\n";
    }
}


void Events::showAllEvents() const {
    if (allEvents.empty()) {
        cout << "No events created yet.\n";
        return;
    }
    cout << "Events:\n";
    for (const auto e : allEvents) { // Use const_auto for const method
        if (e) cout << "- " << e->getTitle() << "\n";
    }
}

void Events::deleteEvent() {
    string n;
    cout << "Enter event title to delete: ";
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear buffer
    getline(cin, n);

    string closest = "";
    int minDist = numeric_limits<int>::max();
    bool found = false;
    for (size_t i = 0; i < allEvents.size(); ++i) {
        if (allEvents[i] && allEvents[i]->getTitle() == n) {
            cout << "Deleted event " << allEvents[i]->getTitle() << " successfully" << endl;
            delete allEvents[i];
            allEvents.erase(allEvents.begin() + i);
            found = true;
            return;
        } else if (allEvents[i]) {
            updateBestMatch(n, allEvents[i]->getTitle(), closest, minDist);
        }
    }
    if (!found && !closest.empty()) {
        cout << "did you mean " << closest << "?(1 for yes, 0 for no): " << endl;
        int choice_del; // Renamed to avoid conflict
        cin >> choice_del;
        while (choice_del != 1 && choice_del != 0) {
            cout << "invalid choice!\nTry again: ";
            cin >> choice_del;
        }
        if (choice_del == 1) {
            for (size_t i = 0; i < allEvents.size(); ++i) {
                if (allEvents[i] && allEvents[i]->getTitle() == closest) {
                    cout << "Deleted event " << allEvents[i]->getTitle() << " successfully" << endl;
                    delete allEvents[i];
                    allEvents.erase(allEvents.begin() + i);
                    return;
                }
            }
        }
    }
    if (!found) cout << "Event not found!" << endl;
}

void Events::operator+(User* user) { // signing up the user using operator overloading
    string n;
    cout << "Enter event title to sign up: ";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    getline(cin, n);

    string closest = "";
    int minDist = numeric_limits<int>::max();
    bool found_direct = false;
    for (size_t i = 0; i < allEvents.size(); ++i) {
        if (allEvents[i] && allEvents[i]->getTitle() == n) {
            allEvents[i]->signUp(user); // Call Event's signUp
            found_direct = true;
            return;
        } else if (allEvents[i]) {
            updateBestMatch(n, allEvents[i]->getTitle(), closest, minDist);
        }
    }
    if (!found_direct && !closest.empty()) {
        cout << "did you mean " << closest << "?(1 for yes, 0 for no): " << endl;
        int choice_su; // Renamed
        cin >> choice_su;
        while (choice_su != 1 && choice_su != 0) {
            cout << "invalid choice!\nTry again: ";
            cin >> choice_su;
        }
        if (choice_su == 1) {
            for (size_t i = 0; i < allEvents.size(); ++i) {
                if (allEvents[i] && allEvents[i]->getTitle() == closest) {
                    allEvents[i]->signUp(user);
                    return;
                }
            }
        }
    }
    if (!found_direct) cout << "Event not found!" << endl;
}

void Events::search() {
    if (allEvents.empty()) {
        cout << "No events created yet.\n"; return;
    }
    string n;
    cout << "Enter event title to search: ";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    getline(cin, n);
    vector<string> results;
    for (const auto e : allEvents) { // Use const_auto for const method
        if (e) addIfAccurateEnough(results, n, e->getTitle());
    }
    if (results.empty()) {
        cout << "No results.\n"; return;
    }
    cout << "results: " << endl;
    int i = 1;
    for (const auto& r : results) { // Use const_auto&
        cout << i++ << ". " << r << endl;
    }
}

bool Events::addToFile(fstream& eventFile) {
    eventFile.clear(); // Clear EOF flags
    eventFile.seekp(0, ios::beg); // Go to the beginning for overwrite
    // eventFile.truncate(); // This might be needed if overwriting and new content is shorter.
                           // However, original just seekp(0).

    if (!eventFile.is_open()) return false;

    for (const auto e : allEvents) { 
        if (!e) continue;
        eventFile << to_string(e->getType()) << '|'
                  << cEncrypt(e->getTitle(), SHIFT) << '|'
                  << cEncrypt(e->getHost(), SHIFT) << '|'
                  << cEncrypt(e->getDescription(), SHIFT) << '|'
                  << cEncrypt(e->getdateAndTime(), SHIFT) << '|'
                  << cEncrypt(e->getvPlatform(), SHIFT) << '|'
                  << to_string(e->getcapacity()) << "\n";

        vector<Attendee*> current_event_attendees = e->getattendees();
        for (size_t i = 0; i < current_event_attendees.size(); ++i) {
            Attendee* a = current_event_attendees[i];
            if (!a) continue;
            eventFile << cEncrypt(a->getName(), SHIFT) << ","
                      << cEncrypt(a->getEmail(), SHIFT) << ","
                      << cEncrypt(a->getPhoneNum(), SHIFT) << ","
                      << cEncrypt(a->getcompanyOrSchool(), SHIFT);
            if (i != current_event_attendees.size() - 1) {
                eventFile << ";";
            }
        }
        eventFile << '\n'; // End the attendees line
    }
    eventFile.flush();
    return true;
}

void Events::getFile(fstream& eventFile) {
    string line, attendeesdata_line;
    if (!eventFile.is_open()) return;
    eventFile.clear();
    eventFile.seekg(0, ios::beg);

    while (getline(eventFile, line)) {
        if (line.empty()) continue;

        char del = '|';
        stringstream s(line);
        string typeStr, title_str, host_str, desc_str, datetime_str, platform_str, capStr;
        
        getline(s, typeStr, del);
        getline(s, title_str, del);
        getline(s, host_str, del);
        getline(s, desc_str, del);
        getline(s, datetime_str, del);
        getline(s, platform_str, del);
        getline(s, capStr);

        if (typeStr.empty() || title_str.empty() || host_str.empty() || desc_str.empty() || datetime_str.empty() || platform_str.empty() || capStr.empty()) {
            if(getline(eventFile, attendeesdata_line)){} // Consume attendee line
            continue;
        }
        
        int capacity_val;
        try {
            capacity_val = stoi(capStr);
        } catch (const exception&) { // Catch generic std::exception
            if(getline(eventFile, attendeesdata_line)){}
            continue;
        }

        title_str = cDecrypt(title_str, SHIFT);
        host_str = cDecrypt(host_str, SHIFT);
        desc_str = cDecrypt(desc_str, SHIFT);
        datetime_str = cDecrypt(datetime_str, SHIFT);
        platform_str = cDecrypt(platform_str, SHIFT);

        eventType type_val_enum;
        if (typeStr == "0") type_val_enum = ::Webinar;
        else if (typeStr == "1") type_val_enum = ::Conference;
        else if (typeStr == "2") type_val_enum = ::Workshop;
        else {
            if(getline(eventFile, attendeesdata_line)){}
            continue;
        }

        Event* ev = nullptr;
        // Use constructor that takes all details for loading
        if (type_val_enum == ::Webinar)
            ev = new class Webinar(type_val_enum, title_str, host_str, desc_str, datetime_str, platform_str, capacity_val);
        else if (type_val_enum == ::Conference)
            ev = new class Conference(type_val_enum, title_str, host_str, desc_str, datetime_str, platform_str, capacity_val);
        else if (type_val_enum == ::Workshop)
            ev = new class Workshop(type_val_enum, title_str, host_str, desc_str, datetime_str, platform_str, capacity_val);

        if (ev) {
            if (getline(eventFile, attendeesdata_line) && !attendeesdata_line.empty()) {
                stringstream att_stream(attendeesdata_line);
                string single_attendee_data;
                char att_del = ';', detail_del = ',';
                while (getline(att_stream, single_attendee_data, att_del)) {
                    if (single_attendee_data.empty()) continue;
                    stringstream current_att(single_attendee_data);
                    string name_att, email_att, phone_att, company_att;
                    getline(current_att, name_att, detail_del);
                    getline(current_att, email_att, detail_del);
                    getline(current_att, phone_att, detail_del);
                    getline(current_att, company_att);

                    if (name_att.empty() || email_att.empty() || phone_att.empty() || company_att.empty()) continue;

                    Attendee* loaded_attendee = new Attendee(
                        cDecrypt(name_att, SHIFT), cDecrypt(email_att, SHIFT),
                        cDecrypt(phone_att, SHIFT), cDecrypt(company_att, SHIFT)
                    );
                    ev->setattendees(loaded_attendee);
                }
            }
            allEvents.push_back(ev);
        } else { // ev was null, type unknown
             if (eventFile.peek() != EOF && eventFile.peek() != '\n') {
                 if(getline(eventFile, attendeesdata_line)) { /* consumed */ }
            }
        }
    }
}
