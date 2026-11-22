
#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <string>
#include <sstream>
#include <limits>
#include <algorithm>

using namespace std;

// Base class
class MediaItem {
protected:
    int id;
    string title;
    string author;
    bool checkedOut;
    int dueDay;

public:
    MediaItem(int i, string t, string a)
        : id(i), title(t), author(a),
          checkedOut(false), dueDay(0) {}

    virtual ~MediaItem() {}

    int getId() const {
        return id;
    }

    bool isCheckedOut() const {
        return checkedOut;
    }

    virtual string getType() const = 0;
    virtual double getFineRate() const = 0;

    virtual void display() const {
        cout << "\nID: " << id;
        cout << "\nTitle: " << title;
        cout << "\nAuthor: " << author;
        cout << "\nStatus: "
             << (checkedOut ? "Checked Out" : "Available");

        if (checkedOut) {
            cout << "\nDue Day: " << dueDay;
        }
    }

    void checkout(int currentDay) {
        if (checkedOut) {
            cout << "Item is already checked out.\n";
            return;
        }

        checkedOut = true;
        dueDay = currentDay + 14;

        cout << "Item checked out successfully!\n";
        cout << "Due day: " << dueDay << endl;
    }

    void returnItem(int currentDay) {
        if (!checkedOut) {
            cout << "Item is already available.\n";
            return;
        }

        int overdueDays = max(0, currentDay - dueDay);
        double fine = overdueDays * getFineRate();

        checkedOut = false;
        dueDay = 0;

        cout << "Item returned successfully!\n";
        cout << "Overdue days: " << overdueDays << endl;
        cout << "Fine: Rs. " << fine << endl;
    }

    // Restore saved checkout status without printing
    void restoreStatus(bool status, int due) {
        checkedOut = status;
        dueDay = due;
    }

    virtual void save(ofstream& file) const {
        file << getType() << "|"
             << id << "|"
             << title << "|"
             << author << "|"
             << checkedOut << "|"
             << dueDay << "\n";
    }
};

// Derived class: Book
class Book : public MediaItem {
public:
    Book(int i, string t, string a)
        : MediaItem(i, t, a) {}

    string getType() const {
        return "Book";
    }

    double getFineRate() const {
        return 2.0;
    }

    void display() const {
        cout << "\n--- BOOK ---";
        MediaItem::display();
        cout << "\nFine Rate: Rs. 2 per day\n";
    }
};

// Derived class: Journal
class Journal : public MediaItem {
public:
    Journal(int i, string t, string a)
        : MediaItem(i, t, a) {}

    string getType() const {
        return "Journal";
    }

    double getFineRate() const {
        return 3.0;
    }

    void display() const {
        cout << "\n--- JOURNAL ---";
        MediaItem::display();
        cout << "\nFine Rate: Rs. 3 per day\n";
    }
};

// Library class
class Library {
private:
    vector<unique_ptr<MediaItem> > items;
    string filename;

public:
    Library() {
        filename = "library.txt";
    }

    // Search item by ID
    MediaItem* findItem(int id) {
        for (size_t i = 0; i < items.size(); i++) {
            if (items[i]->getId() == id) {
                return items[i].get();
            }
        }
        return NULL;
    }

    // Add Book
    void addBook() {
        int id;
        string title, author;

        cout << "Enter Book ID: ";
        cin >> id;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (findItem(id) != NULL) {
            cout << "ID already exists!\n";
            return;
        }

        cout << "Enter Book Title: ";
        getline(cin, title);

        cout << "Enter Author Name: ";
        getline(cin, author);

        items.push_back(
            unique_ptr<MediaItem>(new Book(id, title, author))
        );

        saveToFile();
        cout << "Book added successfully!\n";
    }

    // Add Journal
    void addJournal() {
        int id;
        string title, author;

        cout << "Enter Journal ID: ";
        cin >> id;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (findItem(id) != NULL) {
            cout << "ID already exists!\n";
            return;
        }

        cout << "Enter Journal Title: ";
        getline(cin, title);

        cout << "Enter Publisher/Author: ";
        getline(cin, author);

        items.push_back(
            unique_ptr<MediaItem>(new Journal(id, title, author))
        );

        saveToFile();
        cout << "Journal added successfully!\n";
    }

    // Display all items
    void displayAll() const {
        if (items.empty()) {
            cout << "Library is empty.\n";
            return;
        }

        for (size_t i = 0; i < items.size(); i++) {
            items[i]->display();
            cout << "\n----------------------\n";
        }
    }

    // Search item
    void searchItem() {
        int id;

        cout << "Enter Item ID: ";
        cin >> id;

        MediaItem* item = findItem(id);

        if (item != NULL) {
            item->display();
        } else {
            cout << "Item not found!\n";
        }
    }

    // Checkout item
    void checkoutItem() {
        int id, currentDay;

        cout << "Enter Item ID: ";
        cin >> id;

        MediaItem* item = findItem(id);

        if (item == NULL) {
            cout << "Item not found!\n";
            return;
        }

        cout << "Enter current day number: ";
        cin >> currentDay;

        item->checkout(currentDay);
        saveToFile();
    }

    // Return item
    void returnItem() {
        int id, currentDay;

        cout << "Enter Item ID: ";
        cin >> id;

        MediaItem* item = findItem(id);

        if (item == NULL) {
            cout << "Item not found!\n";
            return;
        }

        cout << "Enter return day number: ";
        cin >> currentDay;

        item->returnItem(currentDay);
        saveToFile();
    }

    // Save data to file
    void saveToFile() {
        ofstream file(filename.c_str());

        if (!file) {
            cout << "Unable to save file!\n";
            return;
        }

        for (size_t i = 0; i < items.size(); i++) {
            items[i]->save(file);
        }

        file.close();
    }

    // Load data from file
    void loadFromFile() {
        ifstream file(filename.c_str());

        if (!file) {
            return;
        }

        string line;

        while (getline(file, line)) {
            stringstream ss(line);

            string type, idStr, title, author;
            string statusStr, dueStr;

            getline(ss, type, '|');
            getline(ss, idStr, '|');
            getline(ss, title, '|');
            getline(ss, author, '|');
            getline(ss, statusStr, '|');
            getline(ss, dueStr, '|');

            if (idStr.empty()) {
                continue;
            }

            int id = atoi(idStr.c_str());
            bool status = (statusStr == "1");
            int due = atoi(dueStr.c_str());

            unique_ptr<MediaItem> item;

            if (type == "Book") {
                item = unique_ptr<MediaItem>(
                    new Book(id, title, author)
                );
            }
            else if (type == "Journal") {
                item = unique_ptr<MediaItem>(
                    new Journal(id, title, author)
                );
            }
            else {
                continue;
            }

            item->restoreStatus(status, due);
            items.push_back(move(item));
        }

        file.close();
    }
};

// Main function
int main() {
    Library library;
    library.loadFromFile();

    int choice;

    do {
        cout << "\n===== LIBRARY MANAGEMENT SYSTEM =====\n";
        cout << "1. Add Book\n";
        cout << "2. Add Journal\n";
        cout << "3. Display All Items\n";
        cout << "4. Search Item\n";
        cout << "5. Checkout Item\n";
        cout << "6. Return Item\n";
        cout << "7. Exit\n";
        cout << "Enter your choice: ";

        cin >> choice;

        switch (choice) {
            case 1:
                library.addBook();
                break;

            case 2:
                library.addJournal();
                break;

            case 3:
                library.displayAll();
                break;

            case 4:
                library.searchItem();
                break;

            case 5:
                library.checkoutItem();
                break;

            case 6:
                library.returnItem();
                break;

            case 7:
                library.saveToFile();
                cout << "Data saved. Exiting program...\n";
                break;

            default:
                cout << "Invalid choice!\n";
        }

    } while (choice != 7);

    return 0;
}
