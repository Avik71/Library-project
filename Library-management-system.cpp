#include <sqlite_orm/sqlite_orm.h>
#include <iostream>
#include <string>

using namespace sqlite_orm;

// Define the Author entity
struct Author {
    int id;
    std::string name;
};

// Define the Book entity
struct Book {
    int id;
    std::string title;
    int author_id;
    std::string genre;
    bool is_borrowed;
};

// Define the Borrower entity
struct Borrower {
    int id;
    std::string name;
    std::string email;
};

// Define the BorrowRecord entity
struct BorrowRecord {
    int id;
    int book_id;
    int borrower_id;
    std::string borrow_date;
    std::string return_date;
};

// Initialize the database schema
auto initStorage(const std::string& path) {
    return make_storage(
        path,
        make_table("authors",
            make_column("id", &Author::id, autoincrement(), primary_key()),
            make_column("name", &Author::name)
        ),
        make_table("books",
            make_column("id", &Book::id, autoincrement(), primary_key()),
            make_column("title", &Book::title),
            make_column("author_id", &Book::author_id),
            make_column("genre", &Book::genre),
            make_column("is_borrowed", &Book::is_borrowed),
            foreign_key(&Book::author_id).references(&Author::id)
        ),
        make_table("borrowers",
            make_column("id", &Borrower::id, autoincrement(), primary_key()),
            make_column("name", &Borrower::name),
            make_column("email", &Borrower::email)
        ),
        make_table("borrow_records",
            make_column("id", &BorrowRecord::id, autoincrement(), primary_key()),
            make_column("book_id", &BorrowRecord::book_id),
            make_column("borrower_id", &BorrowRecord::borrower_id),
            make_column("borrow_date", &BorrowRecord::borrow_date),
            make_column("return_date", &BorrowRecord::return_date),
            foreign_key(&BorrowRecord::book_id).references(&Book::id),
            foreign_key(&BorrowRecord::borrower_id).references(&Borrower::id)
        )
    );
}

using Storage = decltype(initStorage(""));

// CRUD Operations for Books
void addBook(Storage& storage, const std::string& title, int author_id, const std::string& genre) {
    Book book{-1, title, author_id, genre, false};
    storage.insert(book);
    std::cout << "Book added successfully!\n";
}

void listBooks(const Storage& storage) {
    auto books = storage.get_all<Book>();
    for (const auto& book : books) {
        std::cout << "ID: " << book.id << ", Title: " << book.title
                  << ", Author ID: " << book.author_id << ", Genre: " << book.genre
                  << ", Borrowed: " << (book.is_borrowed ? "Yes" : "No") << "\n";
    }
}

void updateBook(Storage& storage, int id, const std::string& new_title) {
    storage.update_all(set(c(&Book::title) = new_title), where(c(&Book::id) == id));
    std::cout << "Book updated successfully!\n";
}

void deleteBook(Storage& storage, int id) {
    storage.remove<Book>(id);
    std::cout << "Book deleted successfully!\n";
}

// CRUD Operations for Authors
void addAuthor(Storage& storage, const std::string& name) {
    Author author{-1, name};
    storage.insert(author);
    std::cout << "Author added successfully!\n";
}

void listAuthors(const Storage& storage) {
    auto authors = storage.get_all<Author>();
    for (const auto& author : authors) {
        std::cout << "ID: " << author.id << ", Name: " << author.name << "\n";
    }
}

// CRUD Operations for Borrowers
void addBorrower(Storage& storage, const std::string& name, const std::string& email) {
    Borrower borrower{-1, name, email};
    storage.insert(borrower);
    std::cout << "Borrower added successfully!\n";
}

void listBorrowers(const Storage& storage) {
    auto borrowers = storage.get_all<Borrower>();
    for (const auto& borrower : borrowers) {
        std::cout << "ID: " << borrower.id << ", Name: " << borrower.name
                  << ", Email: " << borrower.email << "\n";
    }
}

// Borrow and Return Operations
void borrowBook(Storage& storage, int book_id, int borrower_id, const std::string& borrow_date) {
    storage.update_all(set(c(&Book::is_borrowed) = true), where(c(&Book::id) == book_id));
    BorrowRecord record{-1, book_id, borrower_id, borrow_date, ""};
    storage.insert(record);
    std::cout << "Book borrowed successfully!\n";
}

void returnBook(Storage& storage, int book_id, const std::string& return_date) {
    storage.update_all(set(c(&Book::is_borrowed) = false), where(c(&Book::id) == book_id));
    storage.update_all(set(c(&BorrowRecord::return_date) = return_date),
                       where(c(&BorrowRecord::book_id) == book_id && is_null(&BorrowRecord::return_date)));
    std::cout << "Book returned successfully!\n";
}

// Console Interface
void displayMenu() {
    std::cout << "1. Add Book\n";
    std::cout << "2. List Books\n";
    std::cout << "3. Update Book\n";
    std::cout << "4. Delete Book\n";
    std::cout << "5. Add Author\n";
    std::cout << "6. List Authors\n";
    std::cout << "7. Add Borrower\n";
    std::cout << "8. List Borrowers\n";
    std::cout << "9. Borrow Book\n";
    std::cout << "10. Return Book\n";
    std::cout << "11. Exit\n";
}

int main() {
    auto storage = initStorage("library.db");
    storage.sync_schema();

    int choice;
    do {
        displayMenu();
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        switch (choice) {
            case 1: {
                std::string title, genre;
                int author_id;
                std::cout << "Enter title: ";
                std::cin >> title;
                std::cout << "Enter author ID: ";
                std::cin >> author_id;
                std::cout << "Enter genre: ";
                std::cin >> genre;
                addBook(storage, title, author_id, genre);
                break;
            }
            case 2:
                listBooks(storage);
                break;
            case 3: {
                int id;
                std::string new_title;
                std::cout << "Enter book ID: ";
                std::cin >> id;
                std::cout << "Enter new title: ";
                std::cin >> new_title;
                updateBook(storage, id, new_title);
                break;
            }
            case 4: {
                int id;
                std::cout << "Enter book ID: ";
                std::cin >> id;
                deleteBook(storage, id);
                break;
            }
            case 5: {
                std::string name;
                std::cout << "Enter author name: ";
                std::cin >> name;
                addAuthor(storage, name);
                break;
            }
            case 6:
                listAuthors(storage);
                break;
            case 7: {
                std::string name, email;
                std::cout << "Enter borrower name: ";
                std::cin >> name;
                std::cout << "Enter borrower email: ";
                std::cin >> email;
                addBorrower(storage, name, email);
                break;
            }
            case 8:
                listBorrowers(storage);
                break;
            case 9: {
                int book_id, borrower_id;
                std::string borrow_date;
                std::cout << "Enter book ID: ";
                std::cin >> book_id;
                std::cout << "Enter borrower ID: ";
                std::cin >> borrower_id;
                std::cout << "Enter borrow date (YYYY-MM-DD): ";
                std::cin >> borrow_date;
                borrowBook(storage, book_id, borrower_id, borrow_date);
                break;
            }
            case 10: {
                int book_id;
                std::string return_date;
                std::cout << "Enter book ID: ";
                std::cin >> book_id;
                std::cout << "Enter return date (YYYY-MM-DD): ";
                std::cin >> return_date;
                returnBook(storage, book_id, return_date);
                break;
            }
            case 11:
                std::cout << "Exiting...\n";
                break;
            default:
                std::cout << "Invalid choice. Try again.\n";
        }
    } while (choice != 11);

    return 0;
}