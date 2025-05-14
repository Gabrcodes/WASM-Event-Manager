#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include <algorithm>
#include <fstream>
#include <sstream>
#define SHIFT 32 // Caesar cipher shift
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
    vector<vector<int>> dp(m+1, vector<int>(n+1));

    for (int i = 0; i <= m; ++i) dp[i][0] = i;
    for (int j = 0; j <= n; ++j) dp[0][j] = j;

    for (int i = 1; i <= m; ++i)
        for (int j = 1; j <= n; ++j)
            dp[i][j] = (a[i-1] == b[j-1]) ? dp[i-1][j-1] : 1 + min({dp[i-1][j], dp[i][j-1], dp[i-1][j-1]});

    return dp[m][n];
}

void updateBestMatch(const string& query, const string& candidate, string& bestMatch, int& minDist) {
    int dist = levenshteinDistance(query, candidate);
    if (dist < minDist) {
        minDist = dist;
        bestMatch = candidate;
    }
}

void addIfAccurateEnough(vector<string>& vec, const string& query, const string& current, double threshold = 0.75) {
    int distance = levenshteinDistance(query, current);
    int maxLen = max(query.length(), current.length());

    if (maxLen == 0) return;  // avoid division by zero

    double accuracy = 1.0 - static_cast<double>(distance) / maxLen;

    if (accuracy >= threshold) {
        vec.push_back(current);
    }
}

enum eventType { Webinar, Conference, Workshop };

class User { // singleton
    private:
        static User* instance;
        User() { instance = nullptr; }

        string name;
        string email;
        string phoneNum;
        string companyOrSchool;

    public:
        User(const User&) = delete;
        User& operator=(const User&) = delete;

        static User* getInstance() {
            if (!instance)
                instance = new User();
            return instance;
        }

        void setName(const string& n) { name = n; }
        void setEmail(const string& e) { email = e; }
        void setPhoneNumber(const string& p) { phoneNum = p; }
        void setCompanyOrSchool(const string& c) { companyOrSchool = c; }
        string getName() const { return name; }
        string getEmail() const { return email; }
        string getPhoneNumber() const { return phoneNum; }
        string getCompanyOrSchool() const { return companyOrSchool; }
};

class attendee {
    private:
        string name;
        string email;
        string phoneNum;
        string companyOrSchool;
    public:
        attendee(string n, string e, string p, string cs) :
            name(n), email(e), phoneNum(p), companyOrSchool(cs) {}

            string getName(){
                 return name; }

            string getEmail(){
                 return email; }

            string getPhoneNum(){
                 return phoneNum; }

            string getcompanyOrSchool(){
                 return companyOrSchool; }

};

class event {
    protected:
        string title;
        string host;
        string description;
        string dateAndTime;
        string vPlatform;
        int capacity;
        vector <attendee *> attendees;
    public:
        event(string t,string h,string d,string dt,string v,int c): title(t),host(h),description(d), dateAndTime(dt), vPlatform(v),capacity(c) {}
        event(User *user) {
            host = user->getName();
            cout << "Enter title: ";
            getline(cin >> ws, title);

            cout << "Enter description: ";
            getline(cin >> ws, description);

            cout << "Enter dateAndTime: ";
            getline(cin >> ws, dateAndTime);

            cout << "Enter Virtual Platform: ";
            getline(cin >> ws, vPlatform);

            cout << "Enter Capacity: ";
            cin >> capacity;

            attendees = {};
        }



        vector<attendee*> getattendees()  {
            return attendees;

        }

          void setattendees(attendee* a) {
            attendees.push_back(a);
       }

        virtual void signUp(User *user) = 0;
        virtual eventType getType() = 0;

        string getTitle() {return title;}
        string getHost() {return host;}
        string getDescription() {return description;}
        string getdateAndTime(){return dateAndTime;}
        string getvPlatform() {return vPlatform;}
        void setTitle(string t) { title = t; }
        void setHost(string h) { host = h; }
        void setDescription(string d) { description = d; }
        void setDateAndTime(string dt) { dateAndTime = dt; }
        void setvPlatform(string v) { vPlatform = v; }
        void setCapacity(int c) { capacity = c; }

        int getcapacity() {
            return capacity;
        }

        ~event() {
            for (auto a : attendees) {
                delete a;
            }
        }

        static event* createEvent(eventType type, User *user);
};

class webinar : public event {
    private:
        eventType type;
    public:
        webinar(eventType ty, string t, string h, string d, string dt, string v, int c): event(t, h, d, dt, v, c), type(ty) {}
        webinar(User * user) : event(user){
            type = Webinar;
        }
        void signUp (User *user)  override {
            if (attendees.size() >= capacity) {
                cout << "sorry! capacity is full" << endl;
                return;
            }
            attendee *a = new attendee(user->getName(), user->getEmail(), user->getPhoneNumber(), user->getCompanyOrSchool());
            attendees.push_back(a);
            cout << "signed up for " << type << " event : " << title << endl;
            cout << "at : " << dateAndTime << ". See you there!"<< endl;
        }
        eventType getType(){
            return type;
        }
};

class conference : public event {
private:
    eventType type;
public:
    conference(eventType ty, string t, string h, string d, string dt, string v, int c) : event(t, h, d, dt, v, c), type(ty) {}
    conference(User * user) : event(user){
        type = Conference;
    }
    void signUp (User *user)  override {
        if (attendees.size() >= capacity) {
            cout << "sorry! capacity is full" << endl;
            return;
        }
        attendee *a = new attendee(user->getName(), user->getEmail(), user->getPhoneNumber(), user->getCompanyOrSchool());
        attendees.push_back(a);
        cout << "signed up for " << type << " event : " << title << endl;
        cout << "at : " << dateAndTime << ". See you there!"<< endl;
    }
    eventType getType(){
            return type;
        }
};

class workshop : public event {
    private:
        eventType type;
    public:
        workshop(eventType ty, string t, string h, string d, string dt, string v, int c): event(t, h, d, dt, v, c), type(ty) {}
        workshop(User * user) : event(user) {
            type = Workshop;
        }
        void signUp (User *user)  override {
            if (attendees.size() >= capacity) {
                cout << "sorry! capacity is full" << endl;
                return;
            }
            attendee *a = new attendee(user->getName(), user->getEmail(), user->getPhoneNumber(), user->getCompanyOrSchool());
            attendees.push_back(a);
            cout << "signed up for " << type << " event : " << title << endl;
            cout << "at : " << dateAndTime << ". See you there!"<< endl;
        }
        eventType getType(){
            return type;
        }
};

event* event::createEvent(eventType type, User *user) {
    switch (type) {
        case Webinar: return new webinar(user);
        case Conference: return new conference(user);
        case Workshop: return new workshop(user);
        default: return nullptr;
    }
}

class events {
    private:
        vector <event *> allEvents;
    public:
        events(fstream& eventFile) : allEvents({}) {getFile(eventFile);}

        ~events() {
            fstream eventFile("EventFile.txt",ios::in | ios::out);
            addToFile(eventFile);
            for (auto e : allEvents) {
                delete e;
            }
        }

        void createEvent(User * user) {
            int choice;
            cout << "Select Event Type:\n1. Webinar\n2. Conference\n3. Workshop\nChoice: ";
            cin >> choice;

            eventType type;
            switch (choice) {
                case 1: type = Webinar; break;
                case 2: type = Conference; break;
                case 3: type = Workshop; break;
                default:
                    cout << "Invalid choice!\n";
                return;
            }

            event* newEvent = event::createEvent(type, user);
            allEvents.push_back(newEvent);
            cout << "Event created successfully.\n";
        }


        void showAllEvents() const {
            if (allEvents.empty()) {
                cout << "No events created yet.\n";
                return;
            }
            cout << "Events:\n";
            for (auto e : allEvents) {
                cout << "- " << e->getTitle() << "\n";
            }
        }

        void deleteEvent() {
            string n;
            cout << "Enter event title to delete: ";
            getline(cin >> ws, n);
            string closest = "";
            int minDist = numeric_limits<int>::max();
            for (int i = 0; i < allEvents.size(); i++) {
                if (allEvents[i]->getTitle() == n) {
                    cout << "Deleted event " << allEvents[i]->getTitle() << " successfully" << endl;
                    delete allEvents[i];
                    allEvents.erase(allEvents.begin() + i);
                    return;
                }
                else updateBestMatch(n, allEvents[i]->getTitle(),  closest, minDist);
            }

            cout << "did you mean " << closest << "?(1 for yes, 0 for no): " << endl;
            int choice;
            cin >> choice;
            while (choice != 1 && choice != 0) {
                cout << "invalid choice!\nTry again: ";
                cin >> choice;
            }
            if (choice == 1) {
                for (int i = 0; i < allEvents.size(); i++) {
                    if (allEvents[i]->getTitle() == closest) {
                        cout << "Deleted event " <<allEvents[i]->getTitle() << " successfully" << endl;
                        delete allEvents[i];
                        allEvents.erase(allEvents.begin() + i);
                        return;
                    }
                }
            }
            cout << "Event not found!" << endl;
        }

        void signUp(User *user) {
            string n;
            cout << "Enter event title to sign up: ";
            getline(cin >> ws, n);
            string closest = "";
            int minDist = numeric_limits<int>::max();
            for (int i = 0; i < allEvents.size(); i++) {
                if (allEvents[i]->getTitle() == n) {
                    allEvents[i]->signUp(user);
                    return;
                }else {
                    updateBestMatch(n, allEvents[i]->getTitle(),  closest, minDist);
                }
            }

            cout << "did you mean " << closest << "?(1 for yes, 0 for no): " << endl;
            int choice;
            cin >> choice;
            while (choice != 1 && choice != 0) {
                cout << "invalid choice!\nTry again: ";
                cin >> choice;
            }
            if (choice == 1) {
                for (int i = 0; i < allEvents.size(); i++) {
                    if (allEvents[i]->getTitle() == closest) {
                        allEvents[i]->signUp(user);
                        return;
                    }
                }
            }
            cout << "Event not found!" << endl;
        }

        void search() {
            if (allEvents.empty()) {
                cout << "No events created yet.\n";
                return;
            }
            string n;
            cout << "Enter event title to search: ";
            getline(cin >> ws, n);
            vector <string> results;
            for (auto e : allEvents) {
                addIfAccurateEnough(results, n, e->getTitle());
            }
            if (results.empty()) {
                cout << "No results.\n";
                return;
            }
            cout << "results: " << endl;
            int i = 1;
            for (auto r : results) {
                cout << i++ << ". " << r << endl;
            }
        }

        bool addToFile(fstream& eventFile){
            vector<attendee*> attendees;

            if(!eventFile.is_open()) return false;
            for(auto e: allEvents){
                eventFile << to_string(e->getType()) << '|'
                          << cEncrypt(e->getTitle(), SHIFT) << '|'
                          << cEncrypt(e->getHost(), SHIFT) << '|'
                          << cEncrypt(e->getDescription(), SHIFT) << '|'
                          << cEncrypt(e->getdateAndTime(), SHIFT) << '|'
                          << cEncrypt(e->getvPlatform(), SHIFT) << '|'
                          << to_string(e->getcapacity()) << "\n";


                          attendees=e->getattendees();
                          for(int i=0;i<attendees.size();++i){
                                attendee* a=attendees[i];
                          eventFile<<cEncrypt(a->getName(),SHIFT)<<","
                                   <<cEncrypt(a->getEmail(),SHIFT)<<","
                                   <<cEncrypt(a->getPhoneNum(),SHIFT)<<","
                                   <<cEncrypt(a->getcompanyOrSchool(),SHIFT);

                                   if (i != attendees.size() - 1)
                                     eventFile << ";";
                  }
                            eventFile << '\n'; // end the event line
                    }

            return true;
        }

        void getFile(fstream& eventFile){
        string line;
        while(getline(eventFile, line)){
            char del = '|';
            stringstream s(line);
            string typeStr, title, host, description, dateAndTime, vPlatform, capStr,attendeesdata;
            getline(s, typeStr, del);
            getline(s, title, del);
            getline(s, host, del);
            getline(s, description, del);
            getline(s, dateAndTime, del);
            getline(s, vPlatform, del);
            getline(s, capStr);
            if (typeStr.empty() || title.empty() || host.empty() || description.empty() || dateAndTime.empty() || vPlatform.empty() || capStr.empty())
                continue;

            int capacity = stoi(capStr); //string to integer func.
            title = cDecrypt(title, SHIFT);
            host = cDecrypt(host, SHIFT);
            description = cDecrypt(description, SHIFT);
            dateAndTime = cDecrypt(dateAndTime, SHIFT);
            vPlatform = cDecrypt(vPlatform, SHIFT);

            eventType type;
            if (typeStr == "0") type = Webinar;
            else if (typeStr == "1") type = Conference;
            else type = Workshop;

            event* ev = nullptr;
            if (type == Webinar)
                ev = new webinar(type, title, host, description, dateAndTime, vPlatform, capacity);
            else if (type == Conference)
                ev = new conference(type, title, host, description, dateAndTime, vPlatform, capacity);
            else if (type == Workshop)
                ev = new workshop(type, title, host, description, dateAndTime, vPlatform, capacity);

            if (ev != nullptr) {
                ev->setTitle(title);
                ev->setHost(host);
                ev->setDescription(description);
                ev->setDateAndTime(dateAndTime);
                ev->setvPlatform(vPlatform);
                ev->setCapacity(capacity);

            }

            if(!attendeesdata.empty()){
                stringstream att(attendeesdata);
                string enter;

                while(getline(att,enter,';')){
                    stringstream current(enter);
                    string name;
                    string email;
                    string phone;
                    string companyorschool;

                    getline(current,name,',');
                    getline(current,email,',');
                    getline(current,phone,',');
                    getline(current,companyorschool,',');

                    name = cDecrypt(name, SHIFT);
                    email = cDecrypt(email, SHIFT);
                    phone = cDecrypt(phone, SHIFT);
                    companyorschool = cDecrypt(companyorschool, SHIFT);

                    attendee* a=new attendee(name,email,phone,companyorschool);
                    ev->setattendees(a);
                }


            }

            allEvents.push_back(ev);
            }
        }
};

User* User::instance = nullptr;

int main() {
    fstream eventFile("EventFile.txt",ios::in | ios::out);
    events eventManager(eventFile);
    User* user = User::getInstance();

    // Set user details once
    string name, email, phone, company;
    cout << "Enter your details:\n";
    cout << "Name: ";
    getline(cin >> ws, name);
    cout << "Email: ";
    getline(cin >> ws, email);
    cout << "Phone: ";
    getline(cin >> ws, phone);
    cout << "Company/School: ";
    getline(cin >> ws, company);

    user->setName(name);
    user->setEmail(email);
    user->setPhoneNumber(phone);
    user->setCompanyOrSchool(company);

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
                eventManager.signUp(user);
            break;
            case 5:
                eventManager.search();
            break;
            case 6:
                cout << "Goodbye!\n";
            return 0;
            default:
                cout << "Invalid option, try again.\n";
        }
    }
    eventFile.close();
}
