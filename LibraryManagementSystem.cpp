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

    bool addBook(Book newBook);
    // Adds a new book to the library collection.
    // Ensures the book ID is unique and initializes available copies.

    bool updateBookDetails(int bookID, string newTitle, string newAuthor, string newCategory);
    // Updates title, author, and category of an existing book.
    // Does not change copy counts.

    bool removeBook(int bookID);
    // Removes a book from the system if it is not currently borrowed or reserved.
    // Maintains data consistency.

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

    bool reserveBook(int bookID, string studentID);
    // Allows a student to reserve a book if all copies are currently borrowed.
    // Enforces reservation limits.
    // Should store in a queue

    Reserve *getNextReservation(int bookID);
    // Retrieves the earliest reservation made for a specific book.
    // Used to fairly assign returned copies.

    void processReservations(int bookID);
    // Automatically assigns a returned book to the next student in the reservation queue.
    // Removes the fulfilled reservation.

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
};

int main()
{
    return 0;
}
