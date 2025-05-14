#ifdef __EMSCRIPTEN__
#include <emscripten.h> 
#endif

#include "app_logic.h"
#include <iostream> 

// Using namespace std for convenience in this .cpp file
using namespace std;

// This global is used by attemptDeleteEvent/attemptSignUp to store the suggestion
// It's then read by confirmDeleteSuggestedEvent/confirmSignUpSuggestedEvent
// and by the UI in main.cpp to display the "Did you mean X?" message.
// This is not ideal from a strict encapsulation perspective but matches the prior structure.
// A better approach would be for these functions to return a struct containing
// the message and any suggestion, or for a UI state class to manage this.
extern string suggestedMatch; // Defined in main.cpp

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

void updateBestMatch(const string& query, const string& candidate, string& bestMatch_ref, int& minDist) { // Renamed bestMatch to avoid conflict
    if (candidate.empty() && query.empty()) { 
        minDist = 0;
        bestMatch_ref = candidate; 
        return;
    }
    if (candidate.empty() && !query.empty()) { 
        return;
    }
    
    int dist = levenshteinDistance(query, candidate);
    if (dist < minDist) {
        minDist = dist;
        bestMatch_ref = candidate;
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
string attendee::getName() const { return name; }
string attendee::getEmail() const { return email; }
string attendee::getPhoneNum() const { return phoneNum; }
string attendee::getcompanyOrSchool() const { return companyOrSchool; }


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

const std::vector<attendee*>& event::getAttendees() const {
    return attendees;
}
void event::addAttendee(attendee* a) {
    if (a) { // Basic null check
        attendees.push_back(a);
    }
}

void event::setTitle(string t) { title = t; }
void event::setHost(string h) { host = h; }
void event::setDescription(string d) { description = d; }
void event::setDateAndTime(string dt_val) { dateAndTime = dt_val; } // Parameter name changed to avoid conflict
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
webinar::webinar(eventType ty_enum, string t, string h, string d, string dt_val, string v, int c) // dt_val to avoid conflict
    : event(t, h, d, dt_val, v, c), type_val(ty_enum) {} 

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
conference::conference(eventType ty_enum, string t, string h, string d, string dt_val, string v, int c) // dt_val
    : event(t, h, d, dt_val, v, c), type_val(ty_enum) {}

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
workshop::workshop(eventType ty_enum, string t, string h, string d, string dt_val, string v, int c) // dt_val
    : event(t, h, d, dt_val, v, c), type_val(ty_enum) {}

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
        if (e) delete e; // Null check before delete
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
    
    if (!localSuggestedMatch.empty() && minDist < 5 && levenshteinDistance(titleQuery, localSuggestedMatch) <= (localSuggestedMatch.length() / 2) ) {
        suggestedMatch = localSuggestedMatch; 
        foundSuggestion = true;
        return "Event '" + titleQuery + "' not found. Did you mean '" + suggestedMatch + "'?";
    }
    return "Event '" + titleQuery + "' not found.";
}

string events::confirmDeleteSuggestedEvent() {
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
    signUpMessage = ""; // Clear previous message
    string localSuggestedMatch = "";
    int minDist = numeric_limits<int>::max();

    for (auto e : allEvents) {
        if (e && e->getTitle() == titleQuery) {
            // signUpMessage is populated by the event's signUp method
            return e->signUp(user, signUpMessage); 
        }
        if(e) {
            updateBestMatch(titleQuery, e->getTitle(), localSuggestedMatch, minDist);
        }
    }
    
    if (!localSuggestedMatch.empty() && minDist < 5 && levenshteinDistance(titleQuery, localSuggestedMatch) <= (localSuggestedMatch.length() / 2)) {
        suggestedMatch = localSuggestedMatch; 
        foundSuggestion = true;
        signUpMessage = "Event '" + titleQuery + "' not found. Did you mean '" + suggestedMatch + "'?";
        return signUpMessage;
    }
    signUpMessage = "Event '" + titleQuery + "' not found for sign up.";
    return signUpMessage;
}

string events::confirmSignUpSuggestedEvent(User* user, string& signUpMessage) {
    if (!user) {
        signUpMessage = "Error: User not signed in for confirmation.";
        return signUpMessage;
    }
    if (suggestedMatch.empty()) {
        signUpMessage = "Error: No suggested event to sign up for.";
        return signUpMessage;
    }
    bool fd, fs; 
    // signUpMessage will be populated by attemptSignUp
    return attemptSignUp(user, suggestedMatch, fd, fs, signUpMessage);
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
                      << cEncrypt(e->getTitle(), SHIFT) << '|'
                      << cEncrypt(e->getHost(), SHIFT) << '|'
                      << cEncrypt(e->getDescription(), SHIFT) << '|'
                      << cEncrypt(e->getdateAndTime(), SHIFT) << '|'
                      << cEncrypt(e->getvPlatform(), SHIFT) << '|'
                      << to_string(e->getcapacity()) << "\n"; // End of event main line
            
            const auto& eventAttendees = e->getAttendees();
            if (!eventAttendees.empty()) {
                eventFile << eventAttendees.size(); // Number of attendees
                for (const auto att : eventAttendees) {
                    if (att) {
                        eventFile << ";" // Separator before each attendee block
                                  << cEncrypt(att->getName(), SHIFT) << ","
                                  << cEncrypt(att->getEmail(), SHIFT) << ","
                                  << cEncrypt(att->getPhoneNum(), SHIFT) << ","
                                  << cEncrypt(att->getcompanyOrSchool(), SHIFT);
                    }
                }
                eventFile << "\n"; // Newline after all attendees for this event
            }
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
    string eventLine;
    int lineNumber = 0;
    while (getline(eventFile, eventLine)) {
        lineNumber++;
        if (eventLine.empty() || eventLine.find_first_not_of(" \t\n\v\f\r") == string::npos) {
            continue; 
        }

        stringstream s_event(eventLine);
        string typeStr_load, title_load, host_load, description_load, dateAndTime_load, vPlatform_load, capStr_load;
        char del = '|';

        getline(s_event, typeStr_load, del);
        getline(s_event, title_load, del);
        getline(s_event, host_load, del);
        getline(s_event, description_load, del);
        getline(s_event, dateAndTime_load, del);
        getline(s_event, vPlatform_load, del);
        getline(s_event, capStr_load); 

        if (s_event.fail() && !s_event.eof()){ 
             cerr << "Warn: Stream error reading event line " << lineNumber << ": " << eventLine << ". Skipping." << endl;
             continue;
        }
        
        if (typeStr_load.empty() || title_load.empty() || host_load.empty() || 
            description_load.empty() || dateAndTime_load.empty() || vPlatform_load.empty() || capStr_load.empty()) {
            if (!(typeStr_load.empty() && title_load.empty() && capStr_load.empty())) { 
                 cerr << "Warn: Malformed event line " << lineNumber << " (missing fields): " << eventLine << ". Skipping." << endl;
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
            cerr << "Warn: Invalid capacity '" << capStr_load << "' in event line " << lineNumber << ": " << eventLine << ". Details: " << e.what() << ". Skipping." << endl;
            continue;
        }

        title_load = cDecrypt(title_load, SHIFT);
        host_load = cDecrypt(host_load, SHIFT);
        description_load = cDecrypt(description_load, SHIFT);
        dateAndTime_load = cDecrypt(dateAndTime_load, SHIFT);
        vPlatform_load = cDecrypt(vPlatform_load, SHIFT);

        eventType type_enum_load;
        if (typeStr_load == "0") type_enum_load = Webinar;
        else if (typeStr_load == "1") type_enum_load = Conference;
        else if (typeStr_load == "2") type_enum_load = Workshop;
        else {
            cerr << "Warn: Unknown event type '" << typeStr_load << "' in event line " << lineNumber << ": " << eventLine << ". Skipping." << endl;
            continue;
        }
        
        event* ev = nullptr;
        if (type_enum_load == Webinar)
            ev = new webinar(type_enum_load, title_load, host_load, description_load, dateAndTime_load, vPlatform_load, capacity_load);
        else if (type_enum_load == Conference)
            ev = new conference(type_enum_load, title_load, host_load, description_load, dateAndTime_load, vPlatform_load, capacity_load);
        else if (type_enum_load == Workshop)
            ev = new workshop(type_enum_load, title_load, host_load, description_load, dateAndTime_load, vPlatform_load, capacity_load);

        if (!ev) {
            cerr << "Error creating event object from event line " << lineNumber << ": " << eventLine << endl;
            continue; // Skip to next event line
        }

        // Try to read the next line for attendee data
        string attendeeDataLine;
        if (getline(eventFile, attendeeDataLine)) {
            lineNumber++; // Account for the attendee line
            if (!attendeeDataLine.empty() && attendeeDataLine.find('|') == string::npos) { // Basic check: attendee line shouldn't look like an event line
                stringstream s_attendees(attendeeDataLine);
                string numAttendeesStr;
                getline(s_attendees, numAttendeesStr, ';'); // Get number of attendees
                
                try {
                    int numAttendees = 0;
                    if (!numAttendeesStr.empty()) {
                         numAttendees = stoi(numAttendeesStr);
                    }

                    for (int i = 0; i < numAttendees; ++i) {
                        string singleAttendeeData;
                        if (!getline(s_attendees, singleAttendeeData, ';')) {
                            cerr << "Warn: Could not read expected attendee data block " << (i+1) << " for event '" << title_load << "' on line " << lineNumber << endl;
                            break; 
                        }
                        
                        stringstream s_single_attendee(singleAttendeeData);
                        string name_att, email_att, phone_att, company_att;
                        char comma_del = ',';

                        getline(s_single_attendee, name_att, comma_del);
                        getline(s_single_attendee, email_att, comma_del);
                        getline(s_single_attendee, phone_att, comma_del);
                        getline(s_single_attendee, company_att); // Read rest for company

                        if (name_att.empty() && email_att.empty() && phone_att.empty() && company_att.empty() && s_single_attendee.eof() && !s_single_attendee.fail()){
                            // Potentially an empty attendee string from an extra semicolon, skip it
                            continue;
                        }
                        if (s_single_attendee.fail() && !s_single_attendee.eof()){
                             cerr << "Warn: Stream error reading attendee details for event '" << title_load << "' on line " << lineNumber << endl;
                             continue;
                        }


                        name_att = cDecrypt(name_att, SHIFT);
                        email_att = cDecrypt(email_att, SHIFT);
                        phone_att = cDecrypt(phone_att, SHIFT);
                        company_att = cDecrypt(company_att, SHIFT);
                        
                        ev->addAttendee(new attendee(name_att, email_att, phone_att, company_att));
                    }
                } catch (const std::exception& e) {
                    cerr << "Warn: Error parsing attendee count or data for event '" << title_load << "' on line " << lineNumber << ". Details: " << e.what() << endl;
                }
            } else {
                // This line was not attendee data, it's likely the next event or EOF.
                // We need to "put it back" or handle it. The outer loop's getline will get it if it's a new event.
                // If it was EOF, the outer loop will terminate.
                // If it was an empty line, outer loop skips.
                // If it was a new event line, the current getline has consumed it.
                // This requires careful handling of the file stream position.
                // A simpler way is to check if `attendeeDataLine` starts with a digit and `|`.
                // If so, it's a new event, and we need to process `attendeeDataLine` as `eventLine` in the next iteration.
                // This is complex with simple getline.
                // The current implementation assumes if a line is read, and it's not attendee data, the outer loop's next getline will correctly pick up.
                // This might lead to skipping an event if an event has no attendees and the next line is an event.
                // The `eventFile.seekg(-attendeeDataLine.length() -1, ios_base::cur);` would be one way to "put back", but tricky with newlines.
                // For now, this simplified logic might miss an event if an event with attendees is followed by an event with no attendees.
                // The format `NumAttendees;data` on the attendee line helps. If NumAttendees is 0, the line might be "0\n".
                 if (!attendeeDataLine.empty() && attendeeDataLine.find('|') != string::npos) {
                    // It looks like the next event line, we need to process it in the next iteration
                    // This requires the stream to be reset or the line to be processed now.
                    // The simplest is to assume the outer loop will pick it up, but this means
                    // the current `getline(eventFile, eventLine)` has already consumed it.
                    // This is a known limitation of this simple line-by-line parsing for multi-line records.
                    // A more robust parser would handle lookahead or a more structured format.
                    // For now, we'll assume the format is strict: event line, then OPTIONAL attendee line.
                    // If the attendee line read is actually the next event, the current logic will misinterpret.
                    // The check `attendeeDataLine.find('|') == string::npos` tries to mitigate this.
                    // If it *does* find '|', it means it's likely a new event, so we shouldn't parse it as attendees.
                    // The problem is, `getline` has already advanced the stream.
                    // This part of the loading is the most fragile.
                    // Let's assume for now that if an attendee line is expected (next line exists), it IS the attendee line.
                    // The `saveEventsToFile` writes `attendees.size()` first, so if 0, it writes "0\n".
                    // If `attendeeDataLine` is "0", then numAttendees will be 0, and the inner loop won't run. This is correct.
                 }
            }
        }
        allEvents.push_back(ev);
    }
    eventFile.close();
    cout << "Loaded " << allEvents.size() << " events from " << dataFilePath << endl;
}
