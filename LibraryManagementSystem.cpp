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

    vector<Book> getOverdueBooks(){
    // Collects all books that are currently overdue.
    // Used for reporting and monitoring late returns.
   // Loop over all students using index
   vector<Book> overdueBooks;
    for (int s = 0; s < students.size(); s++) {
        Student &student = students[s]; // get current student

        // Loop over borrowed books
        for (int i = 0; i < maxBorrows; i++) {
            if (student.borrowedBooks[i].bookID != 0) {
                // Check if the book is overdue
                if (student.borrowedBooks[i].returnDate < "TODAY_DATE") {
                    Book *book = searchBookById(student.borrowedBooks[i].bookID);
                    if (book) {
                        overdueBooks.push_back(*book);
                    }
                }
            }
        }
    }

    return overdueBooks;  
}

    void sortBooksByTitle(){
    // Sorts the library's books alphabetically by title.
    // Intended to be implemented using a basic sorting algorithm.
    int n=books.size();
    for(int i=0;i<n-1;i++){
        for(int j=0;j<n-i-1;j++){
          if(books[j].title>books[j+1].title){
            // swaps the book
            Book temp=books[j];
            books[j]=books[j+1];
            books[j+1]=temp;
          }
        }
    }
    }

    void saveLibraryData(){
    // Prepares all library data (books, students, reservations)
    // for saving to persistent storage such as a file.
        ofstream file("library_data.txt");
        if(!file.is_open()){
            cout<<"Failed to open file for saving .\n";
            return;
        }
        // save books
         file << "Books:\n";
    for (int i = 0; i < books.size(); i++) {
        file << books[i].id << "," 
             << books[i].title << "," 
             << books[i].author << "," 
             << books[i].category << "," 
             << books[i].totalCopies << "," 
             << books[i].availableCopies << "\n";
    }
        // Step 3: Save students
    file << "\nStudents:\n";
    for (int i = 0; i < students.size(); i++) {
        file << students[i].id << "," 
             << students[i].name << "," 
             << students[i].email << "," 
             << students[i].phoneNumber << "," 
             << students[i].fine << "\n";
    }
        // Step 4: Save reservations
    file << "\nReservations:\n";
    for (int i = 0; i < reservedBooks.size(); i++) {
        file << reservedBooks[i].bookID << "," 
             << reservedBooks[i].studentID << "\n";
    }
    file.close();
    cout<<"Library data saved successfully \n";
    }

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
            student->borrowedBooks[i].returnDate = "DUE_DATE";

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
            student->borrowedBooks[i].returnDate = "EXTENDED_DUE_DATE";
            return true;
        }
    }

    return false; // book not borrowed by this student
}
