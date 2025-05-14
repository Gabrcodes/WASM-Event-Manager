#ifdef __EMSCRIPTEN__
#include <emscripten.h> 
#endif

#include "app_logic.h" // This will now correctly bring in the SHIFT definition
#include <iostream> 

// Using namespace std for convenience in this .cpp file
using namespace std;

// --- Helper Function Definitions ---
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
    const size_t m = a.size(); 
    const size_t n = b.size();
    if (m == 0) return n;
    if (n == 0) return m;

    vector<vector<int>> dp(m + 1, vector<int>(n + 1));

    for (size_t i = 0; i <= m; ++i) dp[i][0] = i;
    for (size_t j = 0; j <= n; ++j) dp[0][j] = j;

    for (size_t i = 1; i <= m; ++i) {
        for (size_t j = 1; j <= n; ++j) {
            int cost = (a[i - 1] == b[j - 1]) ? 0 : 1;
            dp[i][j] = min({ dp[i - 1][j] + 1,        
                             dp[i][j - 1] + 1,        
                             dp[i - 1][j - 1] + cost }); 
        }
    }
    return dp[m][n];
}

void updateBestMatch(const string& query, const string& candidate, string& bestMatch, int& minDist) {
    if (candidate.empty() && query.empty()) { 
        minDist = 0;
        bestMatch = candidate; 
        return;
    }
    if (candidate.empty() && !query.empty()) { 
        return;
    }
    
    int dist = levenshteinDistance(query, candidate);
    if (dist < minDist) {
        minDist = dist;
        bestMatch = candidate;
    }
}

void addIfAccurateEnough(vector<string>& vec, const string& query, const string& current, double threshold) {
    if (current.empty() && query.empty()) { 
        vec.push_back(current);
        return;
    }
    if (current.empty() && !query.empty()) { 
        return;
    }
    
    int distance = levenshteinDistance(query, current);
    int maxLen = max(query.length(), current.length());

    if (maxLen == 0) { 
        if (query.empty() && current.empty()) vec.push_back(current);
        return;
    }

    double accuracy = 1.0 - static_cast<double>(distance) / maxLen;

    if (accuracy >= threshold) {
        vec.push_back(current);
    }
}

// --- User Class Method Definitions ---
User* User::instance = nullptr; 

User::User() : name(""), email(""), phoneNum(""), companyOrSchool("") {} 

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

// --- attendee Class Method Definitions ---
attendee::attendee(string n, string e, string p, string cs) :
    name(n), email(e), phoneNum(p), companyOrSchool(cs) {}

// --- event Class Method Definitions ---
event::event(string t, string h, string d, string dt, string v, int c) :
    title(t), host(h), description(d), dateAndTime(dt), vPlatform(v), capacity(c) {}

event::event(User* user, const string& t, const string& desc, const string& dt, const string& vp, int cap) :
    title(t), description(desc), dateAndTime(dt), vPlatform(vp), capacity(cap) {
    if (user) {
        host = user->getName();
    } else {
        host = "Unknown Host"; 
    }
}

event::~event() {
    for (auto a : attendees) {
        delete a;
    }
    attendees.clear();
}

string event::getTitle() const { return title; }
string event::getHost() const { return host; }
string event::getDescription() const { return description; }
string event::getdateAndTime() const { return dateAndTime; }
string event::getvPlatform() const { return vPlatform; }
int event::getcapacity() const { return capacity; }
int event::getAttendeeCount() const { return attendees.size(); }

void event::setTitle(string t) { title = t; }
void event::setHost(string h) { host = h; }
void event::setDescription(string d) { description = d; }
void event::setDateAndTime(string dt) { dateAndTime = dt; }
void event::setvPlatform(string v) { vPlatform = v; }
void event::setCapacity(int c) { capacity = c; }

event* event::createEvent(eventType type, User* user, const string& title_val, const string& desc_val, const string& dts_val, const string& vps_val, int capacity_val) {
    switch (type) {
        case Webinar: return new webinar(user, title_val, desc_val, dts_val, vps_val, capacity_val);
        case Conference: return new conference(user, title_val, desc_val, dts_val, vps_val, capacity_val);
        case Workshop: return new workshop(user, title_val, desc_val, dts_val, vps_val, capacity_val);
        default: 
            cerr << "Unknown event type in factory: " << type << endl;
            return nullptr;
    }
}

// --- webinar Class Method Definitions ---
webinar::webinar(eventType ty_enum, string t, string h, string d, string dt, string v, int c)
    : event(t, h, d, dt, v, c), type_val(ty_enum) {} 

webinar::webinar(User* user, const string& t, const string& desc, const string& date, const string& vp, int cap)
    : event(user, t, desc, date, vp, cap) {
    type_val = Webinar;
}
string webinar::signUp(User* user, string& message) {
    if (!user) { message = "Error: User details not available for sign up."; return message; }
    if (attendees.size() >= static_cast<size_t>(capacity)) {
        message = "Sorry! Capacity is full for Webinar: " + title;
        return message;
    }
    attendees.push_back(new attendee(user->getName(), user->getEmail(), user->getPhoneNumber(), user->getCompanyOrSchool()));
    message = "Signed up for Webinar: " + title + " on " + dateAndTime + ". See you there!";
    return message;
}
eventType webinar::getType() { return type_val; }

// --- conference Class Method Definitions ---
conference::conference(eventType ty_enum, string t, string h, string d, string dt, string v, int c)
    : event(t, h, d, dt, v, c), type_val(ty_enum) {}

conference::conference(User* user, const string& t, const string& desc, const string& date, const string& vp, int cap)
    : event(user, t, desc, date, vp, cap) {
    type_val = Conference;
}
string conference::signUp(User* user, string& message) {
    if (!user) { message = "Error: User details not available for sign up."; return message; }
    if (attendees.size() >= static_cast<size_t>(capacity)) {
        message = "Sorry! Capacity is full for Conference: " + title;
        return message;
    }
    attendees.push_back(new attendee(user->getName(), user->getEmail(), user->getPhoneNumber(), user->getCompanyOrSchool()));
    message = "Signed up for Conference: " + title + " on " + dateAndTime + ". See you there!";
    return message;
}
eventType conference::getType() { return type_val; }

// --- workshop Class Method Definitions ---
workshop::workshop(eventType ty_enum, string t, string h, string d, string dt, string v, int c)
    : event(t, h, d, dt, v, c), type_val(ty_enum) {}

workshop::workshop(User* user, const string& t, const string& desc, const string& date, const string& vp, int cap)
    : event(user, t, desc, date, vp, cap) {
    type_val = Workshop;
}
string workshop::signUp(User* user, string& message) {
    if (!user) { message = "Error: User details not available for sign up."; return message; }
    if (attendees.size() >= static_cast<size_t>(capacity)) {
        message = "Sorry! Capacity is full for Workshop: " + title;
        return message;
    }
    attendees.push_back(new attendee(user->getName(), user->getEmail(), user->getPhoneNumber(), user->getCompanyOrSchool()));
    message = "Signed up for Workshop: " + title + " on " + dateAndTime + ". See you there!";
    return message;
}
eventType workshop::getType() { return type_val; }

// --- events Class Method Definitions ---
events::events() : dataFilePath("/database_eventmgm/EventFile.txt") { 
    allEvents.clear(); 
    loadEventsFromFile();
}

events::~events() {
    saveEventsToFile(); 
    for (auto e : allEvents) {
        delete e;
    }
    allEvents.clear();
}

string events::createEvent(User* user, eventType type_enum, const string& title_val, const string& desc_val, const string& dt_val, const string& vp_val, const string& capStr_val) {
    if (!user) {
        return "Error: User details not available. Cannot create event.";
    }
    if (title_val.empty() || desc_val.empty() || dt_val.empty() || vp_val.empty() || capStr_val.empty()) {
        return "Error: All event detail fields must be filled.";
    }

    int capacity_val_int;
    try {
        size_t processed_chars = 0;
        capacity_val_int = stoi(capStr_val, &processed_chars);
        if (processed_chars != capStr_val.length() || capStr_val.find_first_not_of("0123456789") != string::npos) {
             throw std::invalid_argument("Capacity must be a valid whole number (digits only).");
        }
        if (capacity_val_int <= 0) {
            return "Error: Capacity must be a positive number.";
        }
    } catch (const std::invalid_argument& ia) {
        return "Error: Invalid capacity format. " + string(ia.what());
    } catch (const std::out_of_range& oor) {
        return "Error: Capacity value is out of numerical range. " + string(oor.what());
    } catch (const std::exception& e) { 
        return "Error: Problem parsing capacity. " + string(e.what());
    }

    event* newEvent = event::createEvent(type_enum, user, title_val, desc_val, dt_val, vp_val, capacity_val_int);
    if (newEvent) {
        allEvents.push_back(newEvent);
        return "Event '" + title_val + "' created successfully.";
    }
    return "Error: Could not create event instance after parsing.";
}

vector<string> events::getAllEventTitles() const {
    vector<string> titles;
    if (allEvents.empty()) {
        titles.push_back("No events created yet.");
    } else {
        for (const auto e : allEvents) {
            if (e) { 
                 titles.push_back(e->getTitle() + " (Host: " + (e->getHost().empty() ? "N/A" : e->getHost()) + ", Cap: " + to_string(e->getAttendeeCount()) + "/" + to_string(e->getcapacity()) + ")");
            }
        }
    }
    return titles;
}

string events::attemptDeleteEvent(const string& titleQuery, bool& foundDirectly, bool& foundSuggestion) {
    foundDirectly = false;
    foundSuggestion = false;
    string localSuggestedMatch = ""; 
    int minDist = numeric_limits<int>::max();

    for (size_t i = 0; i < allEvents.size(); ++i) {
        if (allEvents[i] && allEvents[i]->getTitle() == titleQuery) {
            string deletedTitle = allEvents[i]->getTitle();
            delete allEvents[i];
            allEvents.erase(allEvents.begin() + i);
            foundDirectly = true;
            return "Deleted event '" + deletedTitle + "' successfully.";
        }
        if(allEvents[i]) { 
            updateBestMatch(titleQuery, allEvents[i]->getTitle(), localSuggestedMatch, minDist);
        }
    }
    
    extern string suggestedMatch; 
    if (!localSuggestedMatch.empty() && minDist < 5 && levenshteinDistance(titleQuery, localSuggestedMatch) <= (localSuggestedMatch.length() / 2) ) {
        suggestedMatch = localSuggestedMatch; 
        foundSuggestion = true;
        return "Event '" + titleQuery + "' not found. Did you mean '" + suggestedMatch + "'?";
    }
    return "Event '" + titleQuery + "' not found.";
}

string events::confirmDeleteSuggestedEvent() {
    extern string suggestedMatch; 
    if (suggestedMatch.empty()) {
        return "Error: No suggested event to delete.";
    }
    bool fd, fs; 
    string result = attemptDeleteEvent(suggestedMatch, fd, fs);
    return result;
}

string events::attemptSignUp(User* user, const string& titleQuery, bool& foundDirectly, bool& foundSuggestion, string& signUpMessage) {
    if (!user) {
        signUpMessage = "Error: User not signed in. Cannot sign up.";
        return signUpMessage;
    }
    foundDirectly = false;
    foundSuggestion = false;
    signUpMessage = "";
    string localSuggestedMatch = "";
    int minDist = numeric_limits<int>::max();

    for (auto e : allEvents) {
        if (e && e->getTitle() == titleQuery) {
            signUpMessage = e->signUp(user, signUpMessage); 
            foundDirectly = true;
            return signUpMessage;
        }
        if(e) {
            updateBestMatch(titleQuery, e->getTitle(), localSuggestedMatch, minDist);
        }
    }
    extern string suggestedMatch; 
    if (!localSuggestedMatch.empty() && minDist < 5 && levenshteinDistance(titleQuery, localSuggestedMatch) <= (localSuggestedMatch.length() / 2)) {
        suggestedMatch = localSuggestedMatch; 
        foundSuggestion = true;
        return "Event '" + titleQuery + "' not found. Did you mean '" + suggestedMatch + "'?";
    }
    return "Event '" + titleQuery + "' not found for sign up.";
}

string events::confirmSignUpSuggestedEvent(User* user, string& signUpMessage) {
    if (!user) {
        signUpMessage = "Error: User not signed in for confirmation.";
        return signUpMessage;
    }
    extern string suggestedMatch; 
    if (suggestedMatch.empty()) {
        signUpMessage = "Error: No suggested event to sign up for.";
        return signUpMessage;
    }
    bool fd, fs; 
    string result = attemptSignUp(user, suggestedMatch, fd, fs, signUpMessage);
    return result;
}

vector<string> events::searchEvents(const string& query) {
    vector<string> results;
    if (query.empty()) {
        return getAllEventTitles(); 
    }
    if (allEvents.empty()) {
        results.push_back("No events available to search.");
        return results;
    }
    for (auto e : allEvents) {
        if (e) { 
            addIfAccurateEnough(results, query, e->getTitle());
        }
    }
    if (results.empty()) {
        results.push_back("No events found matching '" + query + "'.");
    }
    return results;
}

bool events::saveEventsToFile() {
    ofstream eventFile(dataFilePath, ios::trunc);
    if (!eventFile.is_open()) {
        cerr << "Error: Could not open " << dataFilePath << " for writing." << endl;
        return false;
    }
    for (const auto e : allEvents) {
        if (e) { 
            eventFile << to_string(e->getType()) << '|'
                      << cEncrypt(e->getTitle(), SHIFT) << '|' // SHIFT is now visible
                      << cEncrypt(e->getHost(), SHIFT) << '|'
                      << cEncrypt(e->getDescription(), SHIFT) << '|'
                      << cEncrypt(e->getdateAndTime(), SHIFT) << '|'
                      << cEncrypt(e->getvPlatform(), SHIFT) << '|'
                      << to_string(e->getcapacity()) << "\n";
        }
    }
    eventFile.close();
    cout << "Saved " << allEvents.size() << " events to " << dataFilePath << endl;
    #ifdef __EMSCRIPTEN__
    EM_ASM({ 
        if (typeof FS !== 'undefined' && FS.syncfs) {
            FS.syncfs(false, function(err) { 
                if (err) {
                    console.error("FS.syncfs error during save for " + UTF8ToString($0) + ": ", err);
                } else {
                    console.log("FS.syncfs save complete for " + UTF8ToString($0));
                }
            });
        } else {
            console.warn("FS.syncfs not available for saveEventsToFile.");
        }
    }, dataFilePath.c_str()); 
    #endif
    return true;
}

void events::loadEventsFromFile() {
    ifstream eventFile(dataFilePath);
    if (!eventFile.is_open()) {
        cout << "Info: " << dataFilePath << " not found or could not be opened. Starting with no events." << endl;
        return;
    }
    string line;
    int lineNumber = 0;
    while (getline(eventFile, line)) {
        lineNumber++;
        if (line.empty() || line.find_first_not_of(" \t\n\v\f\r") == string::npos) {
            continue; 
        }

        stringstream s(line);
        string typeStr_load, title_load, host_load, description_load, dateAndTime_load, vPlatform_load, capStr_load;
        char del = '|';

        getline(s, typeStr_load, del);
        getline(s, title_load, del);
        getline(s, host_load, del);
        getline(s, description_load, del);
        getline(s, dateAndTime_load, del);
        getline(s, vPlatform_load, del);
        getline(s, capStr_load); 

        if (s.fail() && !s.eof()){ 
             cerr << "Warn: Stream error reading line " << lineNumber << ": " << line << ". Skipping." << endl;
             continue;
        }
        
        if (typeStr_load.empty() || title_load.empty() || host_load.empty() || 
            description_load.empty() || dateAndTime_load.empty() || vPlatform_load.empty() || capStr_load.empty()) {
            if (!(typeStr_load.empty() && title_load.empty() && capStr_load.empty())) { 
                 cerr << "Warn: Malformed line " << lineNumber << " (missing fields): " << line << ". Skipping." << endl;
            }
            continue;
        }
        
        int capacity_load;
        try {
            size_t processed_chars = 0;
            capacity_load = stoi(capStr_load, &processed_chars);
            if(processed_chars != capStr_load.length() || capStr_load.find_first_not_of("0123456789") != string::npos) {
                throw std::invalid_argument("Capacity contains non-digit characters or extra data.");
            }
        } catch (const std::exception& e) {
            cerr << "Warn: Invalid capacity '" << capStr_load << "' in line " << lineNumber << ": " << line << ". Details: " << e.what() << ". Skipping." << endl;
            continue;
        }

        title_load = cDecrypt(title_load, SHIFT); // SHIFT is now visible
        host_load = cDecrypt(host_load, SHIFT);
        description_load = cDecrypt(description_load, SHIFT);
        dateAndTime_load = cDecrypt(dateAndTime_load, SHIFT);
        vPlatform_load = cDecrypt(vPlatform_load, SHIFT);

        eventType type_enum_load;
        if (typeStr_load == "0") type_enum_load = Webinar;
        else if (typeStr_load == "1") type_enum_load = Conference;
        else if (typeStr_load == "2") type_enum_load = Workshop;
        else {
            cerr << "Warn: Unknown event type '" << typeStr_load << "' in line " << lineNumber << ": " << line << ". Skipping." << endl;
            continue;
        }
        
        event* ev = nullptr;
        if (type_enum_load == Webinar)
            ev = new webinar(type_enum_load, title_load, host_load, description_load, dateAndTime_load, vPlatform_load, capacity_load);
        else if (type_enum_load == Conference)
            ev = new conference(type_enum_load, title_load, host_load, description_load, dateAndTime_load, vPlatform_load, capacity_load);
        else if (type_enum_load == Workshop)
            ev = new workshop(type_enum_load, title_load, host_load, description_load, dateAndTime_load, vPlatform_load, capacity_load);

        if (ev) {
            allEvents.push_back(ev);
        } else {
            cerr << "Error creating event object from file line " << lineNumber << ": " << line << endl;
        }
    }
    eventFile.close();
    cout << "Loaded " << allEvents.size() << " events from " << dataFilePath << endl;
}
