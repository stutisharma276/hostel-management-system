#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <stdexcept>

using namespace std;

// Forward declarations
class Complaint;
class Student;

// Friend function declaration (Demonstrating Friend Function concept)
void printStudentAudit(const Student& s);

// ==========================================
// 1. BASE CLASS: User (Abstract Class & Polymorphism)
// ==========================================
class User {
protected:
    string userID;
    string name;
    string email;

public:
    User(string id, string n, string mail) 
        : userID(id), name(n), email(mail) {}
    
    virtual ~User() {} // Virtual destructor

    // Pure virtual function making User an abstract base class
    virtual void displayDashboard() const = 0;

    string getUserID() const { return userID; }
    string getName() const { return name; }
};

// ==========================================
// 2. DERIVED CLASS: Student
// ==========================================
class Student : public User {
private:
    int roomNumber;
    string hostelBlock;

public:
    Student(string id, string n, string mail, int room, string block)
        : User(id, n, mail), roomNumber(room), hostelBlock(block) {}

    int getRoomNumber() const { return roomNumber; }
    string getHostelBlock() const { return hostelBlock; }

    void displayDashboard() const override {
        cout << "\n[Student Dashboard]" << endl;
        cout << "Name: " << name << " | ID: " << userID << endl;
        cout << "Block: " << hostelBlock << " | Room: " << roomNumber << endl;
    }

    // Declaring a friend function to access private/protected member variables
    friend void printStudentAudit(const Student& s);
};

// Friend Function Definition (Can access private fields like roomNumber directly)
void printStudentAudit(const Student& s) {
    cout << "[Audit Log] Student " << s.name 
         << " (ID: " << s.userID << ") is officially assigned to " 
         << s.hostelBlock << ", Room No: " << s.roomNumber << endl;
}

// ==========================================
// 3. DERIVED CLASS: MaintenanceStaff
// ==========================================
class MaintenanceStaff : public User {
private:
    string specialization;
    bool isAvailable;

public:
    MaintenanceStaff(string id, string n, string mail, string spec)
        : User(id, n, mail), specialization(spec), isAvailable(true) {}

    string getSpecialization() const { return specialization; }
    bool getAvailability() const { return isAvailable; }

    void displayDashboard() const override {
        cout << "\n[Staff Dashboard]" << endl;
        cout << "Staff Name: " << name << " | Specialization: " << specialization << endl;
        cout << "Status: " << (isAvailable ? "Available" : "Busy") << endl;
    }
};

// ==========================================
// 4. COMPLAINT CLASS (Copy Constructor & Operator Overloading)
// ==========================================
class Complaint {
private:
    int complaintID;
    string studentID;
    string category;
    string description;
    string status;
    static int counter;

public:
    // Parameterized Constructor
    Complaint(string sID, string cat, string desc)
        : studentID(sID), category(cat), description(desc), status("Pending") {
        complaintID = ++counter;
    }

    // Copy Constructor (Demonstrating custom Copy Constructor for backup/duplication)
    Complaint(const Complaint& other) {
        complaintID = other.complaintID + 1000; // Marking duplicate/backup ID
        studentID = other.studentID;
        category = other.category;
        description = other.description + " (Cloned/Backed-up)";
        status = other.status;
        cout << "-> [Copy Constructor Called] Complaint backed up successfully.\n";
    }

    int getComplaintID() const { return complaintID; }
    string getStatus() const { return status; }
    void setStatus(string newStatus) { status = newStatus; }

    // Overloading the Insertion Operator (<<) to display complaints cleanly
    friend ostream& operator<<(ostream& os, const Complaint& c) {
        os << "Complaint #" << c.complaintID 
           << " | Student ID: " << c.studentID
           << " | Category: " << c.category 
           << " | Status: " << c.status 
           << "\nDescription: " << c.description;
        return os;
    }
};

// Initialize static counter
int Complaint::counter = 0;

// ==========================================
// 5. SYSTEM CONTROLLER CLASS
// ==========================================
class HostelSystem {
private:
    vector<shared_ptr<Student>> students;
    vector<shared_ptr<MaintenanceStaff>> staffList;
    vector<Complaint> complaints;

public:
    void registerStudent(string id, string name, string email, int room, string block) {
        auto newStudent = make_shared<Student>(id, name, email, room, block);
        students.push_back(newStudent);
        cout << "-> Student registered successfully!\n";
        // Triggering the Friend function to test it
        printStudentAudit(*newStudent);
    }

    void registerStaff(string id, string name, string email, string specialization) {
        staffList.push_back(make_shared<MaintenanceStaff>(id, name, email, specialization));
        cout << "-> Staff member registered successfully!\n";
    }

    void lodgeComplaint(string studentID, string category, string description) {
        bool found = false;
        for (const auto& s : students) {
            if (s->getUserID() == studentID) {
                found = true;
                break;
            }
        }
        if (!found) {
            throw runtime_error("Error: Student ID not found in database!"); // Exception handling
        }

        complaints.emplace_back(studentID, category, description);
        cout << "-> Complaint lodged successfully! Total complaints: " << complaints.size() << "\n";
    }

    void viewAllComplaints() const {
        if (complaints.empty()) {
            cout << "\nNo complaints registered yet.\n";
            return;
        }
        cout << "\n--- ALL HOSTEL COMPLAINTS ---\n";
        for (const auto& c : complaints) {
            cout << c << endl; // Utilizing overloaded << operator
            cout << "-----------------------------\n";
        }
    }

    void backupLastComplaint() {
        if (complaints.empty()) {
            cout << "No complaints to back up.\n";
            return;
        }
        // Invoking Copy Constructor explicitly to create a backup copy
        Complaint backup(complaints.back()); 
        cout << backup << endl;
    }

    void resolveComplaint(int cID) {
        for (auto& c : complaints) {
            if (c.getComplaintID() == cID) {
                c.setStatus("Resolved");
                cout << "-> Complaint #" << cID << " marked as Resolved.\n";
                return;
            }
        }
        cout << "-> Error: Complaint ID not found.\n";
    }
};

// ==========================================
// 6. MAIN FUNCTION (Driver Code)
// ==========================================
int main() {
    HostelSystem system;

    // Pre-loading sample data
    system.registerStudent("S101", "Rohan", "rohan@hostel.com", 204, "Block-A");
    system.registerStaff("M01", "Amit", "amit@maintenance.com", "Electrical");

    int choice;
    do {
        cout << "\n===== HOSTEL MAINTENANCE DESK =====" << endl;
        cout << "1. Register Student" << endl;
        cout << "2. Lodge a Complaint" << endl;
        cout << "3. View All Complaints" << endl;
        cout << "4. Resolve a Complaint" << endl;
        cout << "5. Test Copy Constructor (Backup Last Complaint)" << endl;
        cout << "6. Exit" << endl;
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1: {
                string id, name, email, block;
                int room;
                cout << "Enter ID, Name, Email, Room No, Block: ";
                cin >> id >> name >> email >> room >> block;
                system.registerStudent(id, name, email, room, block);
                break;
            }
            case 2: {
                string sID, category, desc;
                cout << "Enter Student ID: ";
                cin >> sID;
                cout << "Enter Category (Electrical/Plumbing): ";
                cin >> category;
                cout << "Enter Description (no spaces/use underscores): ";
                cin >> desc;
                try {
                    system.lodgeComplaint(sID, category, desc);
                } catch (const exception& e) {
                    cout << e.what() << endl;
                }
                break;
            }
            case 3:
                system.viewAllComplaints();
                break;
            case 4: {
                int cID;
                cout << "Enter Complaint ID to resolve: ";
                cin >> cID;
                system.resolveComplaint(cID);
                break;
            }
            case 5:
                system.backupLastComplaint();
                break;
            case 6:
                cout << "Exiting system. Good luck with your project presentation!\n";
                break;
            default:
                cout << "Invalid choice! Try again.\n";
        }
    } while (choice != 6);

    return 0;
}