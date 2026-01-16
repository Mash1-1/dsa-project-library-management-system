#include <iostream>
#include <string>
#include <bits/stdc++.h>
using namespace std;

// Global variables
const int maxBorrows = 10;  // Maximum books a student can borrow at once
const int maxReserve = 5;   // Maximum books a student can reserve
const int loanDuration = 3; // Loan duration in days

struct Reserve
{
    int bookID;       // ID of the reserved book
    string studentID; // ID of the student who made the reservation
};

struct Loan
{
    int bookID;        // ID of the borrowed book
    string studentID;  // ID of the borrowing student
    string returnDate; // Date when the book was returned (used for fine calculation)
};

struct Book
{
    string title;        // Book title
    string author;       // Book author
    string category;     // Book category or genre
    string description;  // Short description of the book
    int id;              // Unique book ID
    int totalCopies;     // Total copies owned by the library
    int availableCopies; // Copies currently available for borrowing
};

struct Student
{
    string name;                    // Student name
    string id;                      // Unique student ID
    string phoneNumber;             // Student phone number
    string email;                   // Student email
    Loan borrowedBooks[maxBorrows]; // Fixed-size array of borrowed books
    int fine = 0;                   // Total fine owed by the student
};

class LibraryManagementSystem
{
private:
    vector<Book> books;            // Container holding all books in the library
    vector<Reserve> reservedBooks; // Container holding all active reservations
    vector<Student> students;      // Students

public:
    // =====================================================
    // Searching & Filtering Operations
    // =====================================================

    Book *searchBookById(int bookID);
    // Searches for a book using its unique ID.
    // Returns a pointer to the book if found, otherwise returns nullptr.

    vector<Book *> searchBooksByTitle(string titleKeyword);
    // Searches for books whose titles contain the given keyword.
    // Returns a list of pointers to matching books.

    vector<Book *> filterBooksByCategory(string category);
    // Filters books based on their category.
    // Returns a list of books belonging to the specified category.

    // =====================================================
    // Book Management Operations
    // =====================================================

    bool addBook(Book newBook){

        for(int i=0;i<books.size();i++)
        {
            if(books[i].id==newBook.id)
            {
                return false;
            }
        }

        newBook.availableCopies=newBook.totalCopies;
        books.push_back(newBook);

        return true;
    }
    // Adds a new book to the library collection.
    // Ensures the book ID is unique and initializes available copies.

    bool updateBookDetails(int bookID, string newTitle, string newAuthor, string newCategory){

        for(int i=0;i<books.size();i++)
        {
            if(books[i].id==bookID)
            {
                books[i].title=newTitle;
                books[i].author=newAuthor;
                books[i].category=newCategory;
                return true;
            }
        }

        return false;
    }
    // Updates title, author, and category of an existing book.
    // Does not change copy counts.

    bool removeBook(int bookID){
        
        for(int i=0;i<books.size();i++)
        {
            if(books[i].id==bookID)
            {
                if(books[i].availableCopies!=books[i].totalCopies)
                {
                    return false;
                }
            }

            for(int j=0;j<reservedBooks.size();j++)
            {
                if(reservedBooks[j].bookID==bookID)
                {
                    return false;
                }
            }

            books.erase(books.begin()+i);
            return true;
        }

        return false;
    }

    // =====================================================
    // Borrowing, Returning & Renewal Operations
    // =====================================================

    bool borrowBook(int bookID, string studentID);
    // Allows a student to borrow a book if copies are available.
    // Updates student borrowing records and available copy count.

    bool returnBook(int bookID, string studentID, string returnDate);
    // Processes the return of a book.
    // Calculates fines if the book is returned late and updates availability.

    bool renewBook(int bookID, string studentID);
    // Renews a borrowed book by extending its due date.
    // Renewal is denied if the book is reserved by another student.

    // =====================================================
    // Reservation & Queue-Based Operations
    // =====================================================
    
    bool reserveBook(int bookID, string studentID) {
        // Check if the student exists
        Student* student = findStudentById(studentID);
        if (!student) {
            // Assuming some output or logging, but for now, just return false
            return false;
        }

        // Check if the book exists
        Book* book = searchBookById(bookID);
        if (!book) {
            return false;
        }

        // Reservations are only allowed if all copies are currently borrowed
        if (book->availableCopies > 0) {
            // Cannot reserve a book that has available copies
            return false;
        }

        // Enforce reservation limit: count current reservations for this student
        int currentReservations = 0;
        for (const auto& res : reservedBooks) {
            if (res.studentID == studentID) {
                currentReservations++;
            }
        }
        if (currentReservations >= maxReserve) {
            // Student has reached the maximum number of reservations
            return false;
        }

        // Prevent duplicate reservations: check if the student already reserved this book
        for (const auto& res : reservedBooks) {
            if (res.bookID == bookID && res.studentID == studentID) {
                // Already reserved
                return false;
            }
        }

        // Add the reservation to the end of the vector (FIFO queue behavior)
        Reserve newReserve;
        newReserve.bookID = bookID;
        newReserve.studentID = studentID;
        reservedBooks.push_back(newReserve);

        return true;
    }
    // Allows a student to reserve a book if all copies are currently borrowed.
    // Enforces reservation limits.
    // Should store in a queue

    Reserve *getNextReservation(int bookID) {
        // Iterate through the reservations in order (FIFO)
        for (auto it = reservedBooks.begin(); it != reservedBooks.end(); ++it) {
            if (it->bookID == bookID) {
                // Return pointer to the earliest (first) reservation for this book
                return &(*it);
            }
        }
        // No reservation found for this book
        return nullptr;
    }
    // Retrieves the earliest reservation made for a specific book.
    // Used to fairly assign returned copies.

    void processReservations(int bookID) {
        // Get the next (earliest) reservation for this book
        Reserve* nextRes = getNextReservation(bookID);
        if (!nextRes) {
            // No reservations to process
            return;
        }

        // Automatically assign the book to the reserving student by borrowing it for them
        // Assuming borrowBook handles availability, limits, fines, etc.
        bool assigned = borrowBook(bookID, nextRes->studentID);

        if (assigned) {
            // Remove the fulfilled reservation from the vector
            for (auto it = reservedBooks.begin(); it != reservedBooks.end(); ++it) {
                if (it->bookID == bookID && it->studentID == nextRes->studentID) {
                    reservedBooks.erase(it);
                    break; // Erase only the first matching one
                }
            }
        } else {
            // If assignment failed (e.g., student now has fines or reached limits),
            // we could optionally remove it or notify, but for simplicity, leave it in queue
            // In a real system, might notify admin or student
        }
    }

    // =====================================================
    // Student Management & Fine Handling
    // =====================================================

    bool registerStudent(Student newStudent);
    // Registers a new student in the library system.
    // Ensures student ID uniqueness.

    Student *findStudentById(string studentID);
    // Searches for and returns a student by ID.
    // Returns nullptr if the student does not exist.

    int calculateTotalFine(string studentID);
    // Calculates and returns the total fine owed by a student
    // based on overdue borrowed books.

    // =====================================================
    // Reports, Sorting & Persistence Preparation
    // =====================================================

    vector<Book> getOverdueBooks();
    // Collects all books that are currently overdue.
    // Used for reporting and monitoring late returns.

    void sortBooksByTitle();
    // Sorts the library's books alphabetically by title.
    // Intended to be implemented using a basic sorting algorithm.

    void saveLibraryData();
    // Prepares all library data (books, students, reservations)
    // for saving to persistent storage such as a file.

    // =====================================================
    // Menu System & Quality-of-Life Utilities
    // =====================================================

    void displayMainMenu();
    // Menu with option asking if the user is a student or admin (no passwords and stuff, just 1. admin 2. user then lead to uses)
    // Displays all available actions in a user-friendly menu format.

    void handleUserChoice(int choice);
    // Handles user menu selection and calls the corresponding system operation.

    vector<Book> getAllBooks();
    // Returns a list of all books in the library.
    // Used for browsing, reporting, and UI display.

    void displayAllBooks();
    // Displays all books with formatted details for better readability.

    void clearScreen();
    // Clears the console screen to improve user experience.

    void pauseForUser();
    // Pauses program execution until the user presses Enter.
    // Prevents information from disappearing too quickly.    
    void displayMainMenu() {
        clearScreen();
        cout << "Welcome to the Library Management System" << endl;
        cout << "Please select your role:" << endl;
        cout << "1. Admin" << endl;
        cout << "2. Student" << endl;
        cout << "0. Exit" << endl;
        cout << "Enter your choice: ";
    }
    // Menu with option asking if the user is a student or admin (no passwords and stuff, just 1. admin 2. user then lead to uses)
    // Displays all available actions in a user-friendly menu format.

    void handleUserChoice(int choice) {
        if (choice == 0) {
            cout << "Exiting the system. Goodbye!" << endl;
            return;
        } else if (choice == 1) {
            // Admin role
            handleAdminMenu();
        } else if (choice == 2) {
            // Student role
            handleStudentMenu();
        } else {
            cout << "Invalid choice. Please try again." << endl;
            pauseForUser();
        }
    }
    // Handles user menu selection and calls the corresponding system operation.

    // Helper function for Admin menu (not part of public interface)
    void handleAdminMenu() {
        int adminChoice;
        do {
            clearScreen();
            cout << "Admin Menu:" << endl;
            cout << "1. Add Book" << endl;
            cout << "2. Update Book Details" << endl;
            cout << "3. Remove Book" << endl;
            cout << "4. Register Student" << endl;
            cout << "5. Display All Books" << endl;
            cout << "6. Sort Books by Title" << endl;
            cout << "7. Save Library Data" << endl;
            cout << "0. Back to Main Menu" << endl;
            cout << "Enter your choice: ";
            cin >> adminChoice;
            cin.ignore(); // Clear input buffer

            switch (adminChoice) {
                case 1: {
                    Book newBook;
                    cout << "Enter Book ID: ";
                    cin >> newBook.id;
                    cin.ignore();
                    cout << "Enter Title: ";
                    getline(cin, newBook.title);
                    cout << "Enter Author: ";
                    getline(cin, newBook.author);
                    cout << "Enter Category: ";
                    getline(cin, newBook.category);
                    cout << "Enter Description: ";
                    getline(cin, newBook.description);
                    cout << "Enter Total Copies: ";
                    cin >> newBook.totalCopies;
                    newBook.availableCopies = newBook.totalCopies;
                    if (addBook(newBook)) {
                        cout << "Book added successfully." << endl;
                    } else {
                        cout << "Failed to add book." << endl;
                    }
                    pauseForUser();
                    break;
                }
                case 2: {
                    int bookID;
                    string newTitle, newAuthor, newCategory;
                    cout << "Enter Book ID to update: ";
                    cin >> bookID;
                    cin.ignore();
                    cout << "Enter New Title: ";
                    getline(cin, newTitle);
                    cout << "Enter New Author: ";
                    getline(cin, newAuthor);
                    cout << "Enter New Category: ";
                    getline(cin, newCategory);
                    if (updateBookDetails(bookID, newTitle, newAuthor, newCategory)) {
                        cout << "Book updated successfully." << endl;
                    } else {
                        cout << "Failed to update book." << endl;
                    }
                    pauseForUser();
                    break;
                }
                case 3: {
                    int bookID;
                    cout << "Enter Book ID to remove: ";
                    cin >> bookID;
                    if (removeBook(bookID)) {
                        cout << "Book removed successfully." << endl;
                    } else {
                        cout << "Failed to remove book." << endl;
                    }
                    pauseForUser();
                    break;
                }
                case 4: {
                    Student newStudent;
                    cout << "Enter Student ID: ";
                    getline(cin, newStudent.id);
                    cout << "Enter Name: ";
                    getline(cin, newStudent.name);
                    cout << "Enter Phone Number: ";
                    getline(cin, newStudent.phoneNumber);
                    cout << "Enter Email: ";
                    getline(cin, newStudent.email);
                    if (registerStudent(newStudent)) {
                        cout << "Student registered successfully." << endl;
                    } else {
                        cout << "Failed to register student." << endl;
                    }
                    pauseForUser();
                    break;
                }
                case 5: {
                    displayAllBooks();
                    pauseForUser();
                    break;
                }
                case 6: {
                    sortBooksByTitle();
                    cout << "Books sorted by title." << endl;
                    pauseForUser();
                    break;
                }
                case 7: {
                    saveLibraryData();
                    cout << "Library data saved." << endl;
                    pauseForUser();
                    break;
                }
                case 0:
                    break;
                default:
                    cout << "Invalid choice. Please try again." << endl;
                    pauseForUser();
            }
        } while (adminChoice != 0);
    }

    // Helper function for Student menu (not part of public interface)
    void handleStudentMenu() {
        string studentID;
        cout << "Enter your Student ID: ";
        getline(cin, studentID);
        Student* student = findStudentById(studentID);
        if (!student) {
            cout << "Student ID not found. Returning to main menu." << endl;
            pauseForUser();
            return;
        }

        int studentChoice;
        do {
            clearScreen();
            cout << "Student Menu (ID: " << studentID << "):" << endl;
            cout << "1. Search Books by Title" << endl;
            cout << "2. Filter Books by Category" << endl;
            cout << "3. Borrow Book" << endl;
            cout << "4. Return Book" << endl;
            cout << "5. Renew Book" << endl;
            cout << "6. Reserve Book" << endl;
            cout << "7. View Fine" << endl;
            cout << "8. Display All Books" << endl;
            cout << "0. Back to Main Menu" << endl;
            cout << "Enter your choice: ";
            cin >> studentChoice;
            cin.ignore(); // Clear input buffer

            switch (studentChoice) {
                case 1: {
                    string titleKeyword;
                    cout << "Enter title keyword: ";
                    getline(cin, titleKeyword);
                    vector<Book*> results = searchBooksByTitle(titleKeyword);
                    if (results.empty()) {
                        cout << "No books found." << endl;
                    } else {
                        cout << "Found books:" << endl;
                        for (auto* b : results) {
                            cout << "ID: " << b->id << ", Title: " << b->title << ", Author: " << b->author << endl;
                        }
                    }
                    pauseForUser();
                    break;
                }
                case 2: {
                    string category;
                    cout << "Enter category: ";
                    getline(cin, category);
                    vector<Book*> results = filterBooksByCategory(category);
                    if (results.empty()) {
                        cout << "No books found in this category." << endl;
                    } else {
                        cout << "Books in category " << category << ":" << endl;
                        for (auto* b : results) {
                            cout << "ID: " << b->id << ", Title: " << b->title << ", Author: " << b->author << endl;
                        }
                    }
                    pauseForUser();
                    break;
                }
                case 3: {
                    int bookID;
                    cout << "Enter Book ID to borrow: ";
                    cin >> bookID;
                    if (borrowBook(bookID, studentID)) {
                        cout << "Book borrowed successfully." << endl;
                    } else {
                        cout << "Failed to borrow book." << endl;
                    }
                    pauseForUser();
                    break;
                }
                case 4: {
                    int bookID;
                    string returnDate;
                    cout << "Enter Book ID to return: ";
                    cin >> bookID;
                    cin.ignore();
                    cout << "Enter Return Date (YYYY-MM-DD): ";
                    getline(cin, returnDate);
                    if (returnBook(bookID, studentID, returnDate)) {
                        cout << "Book returned successfully." << endl;
                    } else {
                        cout << "Failed to return book." << endl;
                    }
                    pauseForUser();
                    break;
                }
                case 5: {
                    int bookID;
                    cout << "Enter Book ID to renew: ";
                    cin >> bookID;
                    if (renewBook(bookID, studentID)) {
                        cout << "Book renewed successfully." << endl;
                    } else {
                        cout << "Failed to renew book." << endl;
                    }
                    pauseForUser();
                    break;
                }
                case 6: {
                    int bookID;
                    cout << "Enter Book ID to reserve: ";
                    cin >> bookID;
                    if (reserveBook(bookID, studentID)) {
                        cout << "Book reserved successfully." << endl;
                    } else {
                        cout << "Failed to reserve book." << endl;
                    }
                    pauseForUser();
                    break;
                }
                case 7: {
                    int fine = calculateTotalFine(studentID);
                    cout << "Total fine: $" << fine << endl;
                    pauseForUser();
                    break;
                }
                case 8: {
                    displayAllBooks();
                    pauseForUser();
                    break;
                }
                case 0:
                    break;
                default:
                    cout << "Invalid choice. Please try again." << endl;
                    pauseForUser();
            }
        } while (studentChoice != 0);
    }

    vector<Book> getAllBooks() {
        return books;
    }
    // Returns a list of all books in the library.
    // Used for browsing, reporting, and UI display.

    void displayAllBooks() {
        if (books.empty()) {
            cout << "No books in the library." << endl;
            return;
        }
        cout << "All Books in the Library:" << endl;
        cout << left << setw(10) << "ID" << setw(30) << "Title" << setw(20) << "Author"
             << setw(15) << "Category" << setw(10) << "Available" << setw(10) << "Total" << endl;
        cout << string(95, '-') << endl;
        for (const auto& book : books) {
            cout << left << setw(10) << book.id << setw(30) << book.title << setw(20) << book.author
                 << setw(15) << book.category << setw(10) << book.availableCopies << setw(10) << book.totalCopies << endl;
        }
    }
    // Displays all books with formatted details for better readability.

	    void clearScreen() {
	        system("cls");
	    }
    // Clears the console screen to improve user experience.

    void pauseForUser() {
        cout << "Press Enter to continue...";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    // Pauses program execution until the user presses Enter.
    // Prevents information from disappearing too quickly.
};

int main()
{
    return 0;
}

bool LibraryManagementSystem::borrowBook(int bookID, string studentID)
{
    Book *book = searchBookById(bookID);
    Student *student = findStudentById(studentID);

    if (!book || !student)
        return false;

    if (book->availableCopies <= 0)
        return false;

    // Find empty loan slot
    for (int i = 0; i < maxBorrows; i++)
    {
        if (student->borrowedBooks[i].bookID == 0)
        {
            student->borrowedBooks[i].bookID = bookID;
            student->borrowedBooks[i].studentID = studentID;
            student->borrowedBooks[i].returnDate = calculateDueDate(loanDuration);

            book->availableCopies--;
            return true;
        }
    }

    return false; // borrow limit reached
}

bool LibraryManagementSystem::returnBook(int bookID, string studentID, string returnDate)
{
    Book *book = searchBookById(bookID);
    Student *student = findStudentById(studentID);

    if (!book || !student)
        return false;

    for (int i = 0; i < maxBorrows; i++)
    {
        if (student->borrowedBooks[i].bookID == bookID)
        {
            // Fine calculation (simple)
            if (returnDate > student->borrowedBooks[i].returnDate)
                student->fine += 10;

            // Clear loan record
            student->borrowedBooks[i].bookID = 0;
            student->borrowedBooks[i].studentID = "";
            student->borrowedBooks[i].returnDate = "";

            book->availableCopies++;

            processReservations(bookID);
            return true;
        }
    }

    return false; // book not found in student's loans
}

bool LibraryManagementSystem::renewBook(int bookID, string studentID)
{
    // Check if another student has reserved this book
    for (const auto &r : reservedBooks)
    {
        if (r.bookID == bookID && r.studentID != studentID)
            return false;
    }

    Student *student = findStudentById(studentID);
    if (!student)
        return false;

    for (int i = 0; i < maxBorrows; i++)
    {
        if (student->borrowedBooks[i].bookID == bookID)
        {
            student->borrowedBooks[i].returnDate = calculateDueDate(loanDuration);
            return true;
        }
    }

    return false; // book not borrowed by this student
}

// Helper function for calculating due date
string calculateDueDate(int daysToAdd)
{
    time_t now = time(0);
    tm *future = localtime(&now);

    future->tm_mday += daysToAdd;
    mktime(future); // normalizes date

    char buffer[11];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d", future);

    return string(buffer);
}
