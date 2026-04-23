#include <iostream>
#include <queue>
#include <deque>
#include <string>
#include <fstream>
using namespace std;

// Node structure for linked list
struct DonorNode {
    string name;
    string bloodType;
    string donationDate;
    DonorNode* next;
    
    DonorNode(string n, string bt, string dd) 
        : name(n), bloodType(bt), donationDate(dd), next(NULL) {}
};

// Binary Tree Node for blood type organization
struct BloodTypeNode {
    string bloodType;
    int count;
    BloodTypeNode* left;
    BloodTypeNode* right;
    
    BloodTypeNode(string bt) 
        : bloodType(bt), count(1), left(NULL), right(NULL) {}
};

// Global pointers
DonorNode* donorList = NULL;
BloodTypeNode* bloodTypeTree = NULL;
queue<string> donorQueue;
deque<string> menuDeque;

// Function prototypes
void showMainMenu();
void registerDonor();
void processDonation();
void showDonors();
void checkBloodAvailability();
void requestBlood();
void generateDonationID();
void saveToFile();
void loadFromFile();
void addToBloodTypeTree(string bloodType);
void displayBloodTypeTree(BloodTypeNode* root);
BloodTypeNode* searchBloodType(BloodTypeNode* root, string bloodType);

// Linked List Operations
void insertDonor(string name, string bloodType, string donationDate) {
    DonorNode* newNode = new DonorNode(name, bloodType, donationDate);
    if (!donorList) {
        donorList = newNode;
    } else {
        DonorNode* temp = donorList;
        while (temp->next) {
            temp = temp->next;
        }
        temp->next = newNode;
    }
    addToBloodTypeTree(bloodType);
}

void displayDonors() {
    DonorNode* temp = donorList;
    cout << "\nRegistered Donors:\n";
    while (temp) {
        cout << "Name: " << temp->name 
             << ", Blood Type: " << temp->bloodType 
             << ", Last Donation: " << temp->donationDate << endl;
        temp = temp->next;
    }
}

// Binary Tree Operations
void addToBloodTypeTree(string bloodType) {
    if (!bloodTypeTree) {
        bloodTypeTree = new BloodTypeNode(bloodType);
        return;
    }
    
    BloodTypeNode* current = bloodTypeTree;
    while (true) {
        if (bloodType == current->bloodType) {
            current->count++;
            return;
        } else if (bloodType < current->bloodType) {
            if (!current->left) {
                current->left = new BloodTypeNode(bloodType);
                return;
            }
            current = current->left;
        } else {
            if (!current->right) {
                current->right = new BloodTypeNode(bloodType);
                return;
            }
            current = current->right;
        }
    }
}

void displayBloodTypeTree(BloodTypeNode* root) {
    if (root) {
        displayBloodTypeTree(root->left);
        cout << "Blood Type: " << root->bloodType 
             << ", Donors: " << root->count << endl;
        displayBloodTypeTree(root->right);
    }
}

BloodTypeNode* searchBloodType(BloodTypeNode* root, string bloodType) {
    if (!root || root->bloodType == bloodType) {
        return root;
    }
    if (bloodType < root->bloodType) {
        return searchBloodType(root->left, bloodType);
    }
    return searchBloodType(root->right, bloodType);
}

// File Handling
void saveToFile() {
    ofstream outFile("donors.dat");
    DonorNode* temp = donorList;
    while (temp) {
        outFile << temp->name << "," 
                << temp->bloodType << "," 
                << temp->donationDate << "\n";
        temp = temp->next;
    }
    outFile.close();
    cout << "Donor data saved to file.\n";
}

void loadFromFile() {
    ifstream inFile("donors.dat");
    if (!inFile) {
        cout << "No existing donor data found.\n";
        return;
    }
    
    string line;
    while (getline(inFile, line)) {
        size_t pos1 = line.find(',');
        size_t pos2 = line.find(',', pos1+1);
        string name = line.substr(0, pos1);
        string bloodType = line.substr(pos1+1, pos2-pos1-1);
        string donationDate = line.substr(pos2+1);
        insertDonor(name, bloodType, donationDate);
    }
    inFile.close();
    cout << "Donor data loaded from file.\n";
}

// Menu Functions
void showMainMenu() {
    cout << "\n===== Blood Bank Management System =====\n";
    cout << "1. Register Blood Donor\n";
    cout << "2. Process Donation\n";
    cout << "3. Show Waiting Donors\n";
    cout << "4. Check Blood Availability\n";
    cout << "5. Request Blood\n";
    cout << "6. Generate Donation ID\n";
    cout << "7. Display All Donors (Linked List)\n";
    cout << "8. Display Blood Type Stats (Tree)\n";
    cout << "9. Save Data to File\n";
    cout << "10. Back (Remove Last Menu)\n";
    cout << "11. Exit\n";
    cout << "Choose an option: ";
}

void registerDonor() {
    string name, bloodType, donationDate;
    cout << "Enter donor name: ";
    cin >> name;
    cout << "Enter blood type: ";
    cin >> bloodType;
    cout << "Enter donation date (DD-MM-YYYY): ";
    cin >> donationDate;
    
    donorQueue.push(name + " (" + bloodType + ")");
    insertDonor(name, bloodType, donationDate);
    cout << "Donor registered for screening.\n";
}

void processDonation() {
    if (donorQueue.empty()) {
        cout << "No donors in queue.\n";
    } else {
        cout << "Processing donation for: " << donorQueue.front() << endl;
        donorQueue.pop();
    }
}

void showDonors() {
    if (donorQueue.empty()) {
        cout << "No donors in queue.\n";
    } else {
        queue<string> temp = donorQueue;
        cout << "Donors in queue:\n";
        while (!temp.empty()) {
            cout << "- " << temp.front() << endl;
            temp.pop();
        }
    }
}

void checkBloodAvailability() {
    string bloodType;
    cout << "Enter blood type to check: ";
    cin >> bloodType;
    
    BloodTypeNode* result = searchBloodType(bloodTypeTree, bloodType);
    if (result) {
        cout << "Blood type " << bloodType << " has " << result->count << " donors.\n";
    } else {
        cout << "No donors found for blood type " << bloodType << ".\n";
    }
}

void requestBlood() {
    string patientName, bloodType;
    int units;
    cout << "Enter patient name: ";
    cin >> patientName;
    cout << "Enter required blood type: ";
    cin >> bloodType;
    cout << "Enter units required: ";
    cin >> units;
    
    BloodTypeNode* result = searchBloodType(bloodTypeTree, bloodType);
    if (result && result->count >= units) {
        cout << "Blood request for " << patientName << ": " 
             << units << " units of " << bloodType << " processed.\n";
    } else {
        cout << "Insufficient blood available for this request.\n";
    }
}

void generateDonationID() {
    static int donationID = 1000;
    donationID++;
    cout << "Your donation ID is: BBD-" << donationID << endl;
}

int main() {
    loadFromFile(); // Load existing data at startup
    
    int choice;
    do {
        menuDeque.push_back("Main Menu");
        showMainMenu();
        cin >> choice;

        switch (choice) {
            case 1:
                menuDeque.push_back("Register Donor");
                registerDonor();
                break;
            case 2:
                menuDeque.push_back("Process Donation");
                processDonation();
                break;
            case 3:
                menuDeque.push_back("Show Donors");
                showDonors();
                break;
            case 4:
                menuDeque.push_back("Check Blood Availability");
                checkBloodAvailability();
                break;
            case 5:
                menuDeque.push_back("Request Blood");
                requestBlood();
                break;
            case 6:
                menuDeque.push_back("Generate Donation ID");
                generateDonationID();
                break;
            case 7:
                menuDeque.push_back("Display All Donors");
                displayDonors();
                break;
            case 8:
                menuDeque.push_back("Display Blood Type Stats");
                cout << "\nBlood Type Statistics:\n";
                displayBloodTypeTree(bloodTypeTree);
                break;
            case 9:
                menuDeque.push_back("Save Data to File");
                saveToFile();
                break;
            case 10:
                if (!menuDeque.empty()) {
                    cout << "Going back from: " << menuDeque.back() << endl;
                    menuDeque.pop_back();
                } else {
                    cout << "Menu history is empty.\n";
                }
                break;
            case 11:
                saveToFile(); // Save data before exiting
                cout << "Exiting Blood Bank Management System...\n";
                break;
            default:
                cout << "Invalid option.\n";
        }
    } while (choice != 11);

    // Clean up memory
    DonorNode* current = donorList;
    while (current) {
        DonorNode* next = current->next;
        delete current;
        current = next;
    }

    return 0;
}
