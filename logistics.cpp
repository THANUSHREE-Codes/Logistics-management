// MENU-DRIVEN VERSION WITH USER INPUT + PACKAGE REMOVAL BY ID
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <stdexcept>

using namespace std;

namespace Logistics {
    const double BASE_RATE = 5.0;
    const double DIMENSIONAL_FACTOR = 5000.0;
}

// ---------------- BASE CLASS ----------------
class Package {
protected:
    string trackingID, senderName, recipientName;
    double weight;
    static int nextID;

public:
    Package(double w, string s, string r) : weight(w), senderName(s), recipientName(r) {
        if (w <= 0) throw invalid_argument("Weight must be positive");
        trackingID = "PKG" + to_string(nextID++);
    }

    virtual double calculateShippingCost() const = 0;

    virtual void display() const {
        cout << left << setw(10) << trackingID
             << setw(15) << senderName
             << setw(15) << recipientName
             << setw(8) << weight;
    }

    virtual ~Package() {}

    string getID() const { return trackingID; }
};

int Package::nextID = 1000;

// ---------------- LETTER CLASS ----------------
class Letter : public Package {
public:
    Letter(double w, string s, string r) : Package(w, s, r) {}

    double calculateShippingCost() const override {
        return Logistics::BASE_RATE + 2.0;
    }

    void display() const override {
        Package::display();
        cout << setw(12) << "Letter" << endl;
    }
};

// ---------------- BOX CLASS ----------------
class Box : public Package {
    double l, w, h;
public:
    Box(double wt, string s, string r, double L, double W, double H)
        : Package(wt, s, r), l(L), w(W), h(H) {
        if (L <= 0 || W <= 0 || H <= 0)
            throw invalid_argument("Invalid dimensions");
    }

    double calculateShippingCost() const override {
        double dimensionalWeight = (l * w * h * 1000000) / Logistics::DIMENSIONAL_FACTOR;
        return max(weight, dimensionalWeight) * Logistics::BASE_RATE;
    }

    void display() const override {
        Package::display();
        cout << setw(12) << "Box" << l << "x" << w << "x" << h << endl;
    }
};

// ---------------- SHIPMENT CLASS ----------------
class Shipment {
    vector<Package*> packages;
public:
    ~Shipment() {
        for (auto p : packages) delete p;
    }

    void add(Package* p) {
        packages.push_back(p);
    }

    // 🔍 SEARCH PACKAGE BY ID
    Package* searchByID(const string& id) const {
        for (auto p : packages) {
            if (p->getID() == id) {
                return p; // found
            }
        }
        return nullptr; // not found
    }

    // ❌ REMOVE PACKAGE BY ID
    void removeByID(const string& id) {
        for (auto it = packages.begin(); it != packages.end(); ++it) {
            if ((*it)->getID() == id) {
                delete *it;               // free memory
                packages.erase(it);       // remove from vector
                cout << "Package " << id << " removed successfully.";
                return;
            }
        }
        throw runtime_error("Package ID not found");
    }

    void show() const {
        if (packages.empty()) {
            cout << "No packages in shipment.";
            return;
        }
        cout << "ID        SENDER         RECEIVER       WT     TYPE        DETAILS\n";
        
        for (auto p : packages) p->display();
    }

    double totalCost() const {
        double sum = 0;
        for (auto p : packages) sum += p->calculateShippingCost();
        return sum;
    }

    void save(const string& file) {
        ofstream fout(file);
        for (auto p : packages)
            fout << p->getID() << "," << p->calculateShippingCost() << endl;
        fout.close();
    }
};

// ---------------- MAIN FUNCTION ----------------
int main() {
    Shipment shipment;
    int choice;

    do {
        cout << "\n=== Automated Shipping & Logistics Manager ===\n";
        cout << "1. Add Letter\n";
        cout << "2. Add Box\n";
        cout << "3. Display Packages\n";
        cout << "4. Show Total Cost\n";
        cout << "5. Remove Package by ID\n";
        cout << "6. Search Package by ID\n";
        cout << "7. Save to File\n";
        cout << "8. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        try {
            if (choice == 1) {
                string s, r;
                double w;
                cout << "Sender: "; cin >> s;
                cout << "Receiver: "; cin >> r;
                cout << "Weight: "; cin >> w;
                shipment.add(new Letter(w, s, r));
                cout << "Letter added successfully\n";
            }
            else if (choice == 2) {
                string s, r;
                double wt, l, w, h;
                cout << "Sender: "; cin >> s;
                cout << "Receiver: "; cin >> r;
                cout << "Weight: "; cin >> wt;
                cout << "Length Width Height: "; cin >> l >> w >> h;
                shipment.add(new Box(wt, s, r, l, w, h));
                cout << "Box added successfully\n";
            }
            else if (choice == 3) {
                shipment.show();
            }
            else if (choice == 4) {
                cout << "Total Shipping Cost: $" << fixed << setprecision(2)
                     << shipment.totalCost() << endl;
            }
            else if (choice == 5) {
                string id;
                cout << "Enter Package ID to remove: ";
                cin >> id;
                shipment.removeByID(id);
            }
            else if (choice == 7) {
                string id;
                cout << "Enter Package ID to search: ";
                cin >> id;
                Package* p = shipment.searchByID(id);
                if (p) {
                    cout << "Package Found:";
                    cout << "ID        SENDER         RECEIVER       WT     TYPE        DETAILS";
                    cout << "-------------------------------------------------------------";
                    p->display();
                } else {
                    cout << "Package not found.";
                }
            }
            else if (choice == 6) {
                shipment.save("shipment_log.txt");
                cout << "Saved to shipment_log.txt\n";
            }
        }
        catch (exception& e) {
            cout << "Error: " << e.what() << endl;
        }

    } while (choice != 0);

    return 0;
}
