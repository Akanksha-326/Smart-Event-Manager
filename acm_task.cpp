#include <bits/stdc++.h>
using namespace std;

struct Event {
    int id;
    string name;
    string date;     // format: DD-MM-YYYY
    string time;     // format: HH:MM
    string type;
    string location;
    string recurrence; // "none", "weekly", "monthly"
};

vector<Event> events;
int nextId = 1;
bool isAdmin = false; // admin control

// ===================== Utility Functions ===================== //

bool isValidDate(const string &date) {
    if (date.size() != 10 || date[2] != '-' || date[5] != '-') return false;
    return true;
}

bool isValidTime(const string &time) {
    if (time.size() != 5 || time[2] != ':') return false;
    return true;
}

int toMinutes(const string &t) {
    int h = stoi(t.substr(0,2));
    int m = stoi(t.substr(3,2));
    return h*60 + m;
}

bool hasConflict(const string &date, const string &time) {
    for (auto &e: events) {
        if (e.date == date && e.time == time) return true;
    }
    return false;
}

string suggestSlot(const string &date, const string &time) {
    int t = toMinutes(time);
    for (int offset=30; offset<=600; offset+=30) {
        int newT = t+offset;
        if (newT>=24*60) break;
        char buf[6];
        sprintf(buf,"%02d:%02d",newT/60,newT%60);
        if (!hasConflict(date, buf)) return string(buf);
    }
    return "No free slot today";
}

void saveEvents() {
    ofstream fout("events.csv");
    for (auto &e: events) {
        fout<<e.id<<","<<e.name<<","<<e.date<<","<<e.time<<","<<e.type<<","<<e.location<<","<<e.recurrence<<"\n";
    }
    fout.close();
}

void loadEvents() {
    ifstream fin("events.csv");
    string line;
    while (getline(fin,line)) {
        stringstream ss(line);
        Event e; string id;
        getline(ss,id,','); e.id=stoi(id);
        getline(ss,e.name,','); getline(ss,e.date,','); getline(ss,e.time,',');
        getline(ss,e.type,','); getline(ss,e.location,','); getline(ss,e.recurrence,',');
        events.push_back(e);
        nextId=max(nextId,e.id+1);
    }
    fin.close();
}

// ===================== Core Features ===================== //

void addEvent() {
    if (!isAdmin) { cout<<"[!] Only admin can add events.\n"; return; }
    Event e;
    e.id = nextId++;
    cout<<"Enter Name: "; getline(cin,e.name);
    cout<<"Enter Date (DD-MM-YYYY): "; getline(cin,e.date);
    if (!isValidDate(e.date)) { cout<<"Invalid date!\n"; return; }
    cout<<"Enter Time (HH:MM): "; getline(cin,e.time);
    if (!isValidTime(e.time)) { cout<<"Invalid time!\n"; return; }
    if (hasConflict(e.date,e.time)) {
        cout<<"Conflict detected! Suggested slot: "<<suggestSlot(e.date,e.time)<<"\n";
        return;
    }
    cout<<"Enter Type: "; getline(cin,e.type);
    cout<<"Enter Location (optional): "; getline(cin,e.location);
    cout<<"Recurrence (none/weekly/monthly): "; getline(cin,e.recurrence);

    events.push_back(e);
    saveEvents();
    cout<<"Event added successfully!\n";
}

void editEvent() {
    if (!isAdmin) { cout<<"[!] Only admin can edit events.\n"; return; }
    int id; cout<<"Enter Event ID to edit: "; cin>>id; cin.ignore();
    for (auto &e: events) {
        if (e.id==id) {
            cout<<"Editing "<<e.name<<"\n";
            cout<<"New Name (current: "<<e.name<<"): "; getline(cin,e.name);
            cout<<"New Date ("<<e.date<<"): "; getline(cin,e.date);
            cout<<"New Time ("<<e.time<<"): "; getline(cin,e.time);
            cout<<"New Type ("<<e.type<<"): "; getline(cin,e.type);
            cout<<"New Location ("<<e.location<<"): "; getline(cin,e.location);
            cout<<"New Recurrence ("<<e.recurrence<<"): "; getline(cin,e.recurrence);
            saveEvents();
            cout<<"Event updated!\n";
            return;
        }
    }
    cout<<"Event not found!\n";
}

void deleteEvent() {
    if (!isAdmin) { cout<<"[!] Only admin can delete events.\n"; return; }
    int id; cout<<"Enter Event ID to delete: "; cin>>id; cin.ignore();
    for (size_t i=0;i<events.size();i++) {
        if (events[i].id==id) {
            events.erase(events.begin()+i);
            saveEvents();
            cout<<"Deleted!\n";
            return;
        }
    }
    cout<<"Not found!\n";
}

void viewDay() {
    string date; cout<<"Enter date: "; getline(cin,date);
    vector<Event> dayEvents;
    for (auto &e: events) if (e.date==date) dayEvents.push_back(e);
    sort(dayEvents.begin(),dayEvents.end(),[](auto &a, auto &b){return a.time<b.time;});
    cout<<"Events on "<<date<<":\n";
    for (auto &e: dayEvents)
        cout<<e.time<<" - "<<e.name<<" ("<<e.type<<") at "<<e.location<<"\n";
}

// ===================== Search & Reminders ===================== //

void searchEvent() {
    string keyword; cout<<"Enter search keyword: "; getline(cin,keyword);
    transform(keyword.begin(), keyword.end(), keyword.begin(), ::tolower);
    for (auto &e: events) {
        string name=e.name, type=e.type;
        transform(name.begin(),name.end(),name.begin(),::tolower);
        transform(type.begin(),type.end(),type.begin(),::tolower);
        if (name.find(keyword)!=string::npos || type.find(keyword)!=string::npos) {
            cout<<"["<<e.id<<"] "<<e.name<<" on "<<e.date<<" "<<e.time<<" ("<<e.type<<")\n";
        }
    }
}

void sendReminders() {
    if (!isAdmin) { cout<<"[!] Only admin can send reminders.\n"; return; }
    cout<<"Pretend: Reading attendee emails from Excel file and sending reminders...\n";
    for (auto &e: events) {
        cout<<"Reminder sent for "<<e.name<<" on "<<e.date<<" "<<e.time<<"\n";
    }
}

// ===================== Analytics ===================== //

void analytics() {
    map<string,int> typeCount;
    map<string,int> weekCount;
    map<string,int> slotCount;
    for (auto &e: events) {
        typeCount[e.type]++;
        weekCount[e.date.substr(6)]+=1;
        slotCount[e.time]++;
    }
    cout<<"Total events: "<<events.size()<<"\n";
    cout<<"Frequency by type:\n";
    for (auto &p: typeCount) cout<<p.first<<": "<<p.second<<"\n";
    string maxSlot=""; int maxc=0;
    for (auto &p: slotCount) if (p.second>maxc) {maxc=p.second; maxSlot=p.first;}
    cout<<"Most common slot: "<<maxSlot<<" with "<<maxc<<" events\n";
}

// ===================== Admin Login ===================== //

void adminLogin() {
    string pass; cout<<"Enter admin password: "; getline(cin,pass);
    if (pass=="admin123") { isAdmin=true; cout<<"[+] Logged in as Admin!\n"; }
    else cout<<"Wrong password!\n";
}

// ===================== Main ===================== //

int main() {
    loadEvents();
    while (true) {
        cout<<"\n1. Add Event\n2. Edit Event\n3. Delete Event\n4. View Events by Day\n5. Search Events\n6. Send Reminders\n7. Analytics\n8. Admin Login\n9. Exit\nChoose: ";
        int ch; cin>>ch; cin.ignore();
        if (ch==1) addEvent();
        else if (ch==2) editEvent();
        else if (ch==3) deleteEvent();
        else if (ch==4) viewDay();
        else if (ch==5) searchEvent();
        else if (ch==6) sendReminders();
        else if (ch==7) analytics();
        else if (ch==8) adminLogin();
        else break;
    }
    return 0;
}
