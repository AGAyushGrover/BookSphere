// +----------------------------------+
// |          HEADER FILES            |
// +----------------------------------+

#include <iostream>
#include <sstream>
#include <limits>
#include <string> // Required for std::string and std::stoi
#include <mysql.h> // Required for MySQL functions

// For getch() on macOS/Linux
#include <termios.h>
#include <unistd.h>

#define HOST "127.0.0.1"
#define USER "root"
#define PASS "#Ayush11" // <-- IMPORTANT: PUT YOUR MYSQL PASSWORD HERE
#define DATABASE "Management"
#define PORT 3306
#define PASSWORD 1234 // <-- IMPORTANT: SET A NUMERIC PASSWORD FOR THE APP

using namespace std;

// +--------------------------------+
// |        Global Variables        |
// +--------------------------------+

MYSQL * conn;
MYSQL_RES *res_set;
MYSQL_ROW row;
stringstream stmt;
const char * q;
string query;

// +----------------------------------+
// |      Helper Functions            |
// +----------------------------------+

// A cross-platform implementation for getch()
int getch() {
    struct termios oldt, newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}

// Function to pause and wait for user to press a key
void pause_screen() {
    cout << "\nPress any key to continue...";
    getch();
}

// +----------------------------------+
// |          Classes Made            |
// +----------------------------------+

// Date structure (remains the same)
typedef struct
{
    int date;
    int month;
    int year;
} date;

class books
{
    int id;
    string name;
    string auth;
    int price;
    int qty;
public:
    void add();
    void update_price();
    void search();
    void update();
    void display();
};

class suppliers
{
    int id;
    string name;
    long int phn;
    string addr_line1;
    string addr_line2;
    string addr_city;
    string addr_state;
public:
    void add_sup();
    void remove_supplier();
    void search_id();
};

class purchases
{
    int ord_id;
    int book_id;
    int sup_id;
    int qty;
    date dt_ordered;
    int eta;
    char received;
    int inv;
public:
    void new_ord();
    void view();
    void mar_cancel();
    void mark_reciv();
};

class employees
{
    int id;
    string name;
    string addr_line1;
    string addr_line2;
    string addr_city;
    string addr_state;
    long int phn;
    date date_of_joining;
    long int salary;
public:
    void add_emp();
    void search_emp();
    void assign_mgr_stat();
    void display();
    void update_sal();
};

class members
{
    int id;
    string name;
    string addr_line1;
    string addr_line2;
    string addr_city;
    string addr_state;
    long int phn;
    date beg_date;
    date end_date;
    string valid;
public:
    void add_mem();
    void refresh();
    void search_mem();
};

class sales
{
    int invoice_id;
    int member_id;
    int book_id;
    int qty;
    int amount;
    date date_s;
public:
    void add();
    void find_total_sales();
};

// +----------------------------------+
// |        Member Functions          |
// +----------------------------------+

// ------------
// class books
// ------------

void books::add()
{
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear leftover newline
    cout << "Enter the name of the book : " ;
    getline(cin, name);
    cout << "Enter the name of the author : ";
    getline(cin, auth);
    cout << "Enter the Price : ";
    cin >> price;
    cout << "Enter the Qty Received : ";
    cin >> qty;

    stmt.str("");
    stmt << "INSERT INTO books(name, auth, price, qty) VALUES('" << name << "','" << auth << "'," << price << "," << qty << ");";
    query = stmt.str();
    q = query.c_str();

    if (mysql_query(conn, q)) {
        cout << "\nQuery Error: " << mysql_error(conn) << endl;
    } else {
        cout << "\nBook Record Inserted Successfully!" << endl;
    }
}

void books::update_price()
{
    char choice;
    cout << "Enter the id of the book for update in price : ";
    cin >> id;

    stmt.str("");
    stmt << "SELECT name, price FROM books WHERE id = " << id << ";";
    query = stmt.str();
    q = query.c_str();

    if (mysql_query(conn, q)) {
        cout << "\nQuery Error: " << mysql_error(conn) << endl;
        return;
    }

    res_set = mysql_store_result(conn);
    if ((row = mysql_fetch_row(res_set)) != NULL)
    {
        cout << "The Name of the book is : " << row[0] << endl ;
        cout << "The current price of the book is : " << row[1] << endl ;
        cout << "Do you Want to Update the Price [y/n] : " ;
        cin >> choice;
        if (choice == 'y' || choice == 'Y')
        {
            cout << "Enter the new price : ";
            cin >> price;
            stmt.str("");
            stmt << "UPDATE books SET price = " << price << " WHERE id = " << id << ";";
            query = stmt.str();
            q = query.c_str();
            if (mysql_query(conn, q)) {
                cout << "\nQuery Error: " << mysql_error(conn) << endl;
            } else {
                cout << "\nBook Price Updated Successfully!" << endl;
            }
        }
        else
        {
            cout << "No changes Made!!" << endl;
        }
    }
    else
    {
        cout << "No Book found!!!" << endl;
    }
    mysql_free_result(res_set);
}

void books::search()
{
    cout << "Enter book id for details : ";
    cin >> id;
    stmt.str("");
    stmt << "SELECT * FROM books WHERE id = " << id << ";";
    query = stmt.str();
    q = query.c_str();
    if (mysql_query(conn, q)) {
        cout << "\nQuery Error: " << mysql_error(conn) << endl;
        return;
    }

    res_set = mysql_store_result(conn);
    if ((row = mysql_fetch_row(res_set)) != NULL)
    {
        cout << "\n--- Book Details ---" << endl;
        cout << "Book ID: " << row[0] << endl;
        cout << "Name: " << row[1] << endl ;
        cout << "Author: " << row[2] << endl;
        cout << "Price: " << row[3] << endl ;
        cout << "Inventory Count: " << row[4] << endl;
    }
    else
    {
        cout << "No record Found" << endl;
    }
    mysql_free_result(res_set);
}

void books::update()
{
    cout << "Updating book quantities from received purchases..." << endl;
    
    stmt.str("");
    stmt << "SELECT book_id, qty FROM purchases WHERE recieved = 'T' AND inv IS NULL;";
    query = stmt.str();
    q = query.c_str();
    if (mysql_query(conn, q)) {
        cout << "\nQuery Error: " << mysql_error(conn) << endl;
        return;
    }

    res_set = mysql_store_result(conn);
    if (!res_set) {
        cout << "No new deliveries to update." << endl;
        return;
    }

    while ((row = mysql_fetch_row(res_set)) != NULL)
    {
        int book_id = stoi(row[0]);
        int quantity = stoi(row[1]);

        stmt.str("");
        stmt << "UPDATE books SET qty = qty + " << quantity << " WHERE id = " << book_id << ";";
        query = stmt.str();
        q = query.c_str();
        if (mysql_query(conn, q)) {
            cout << "\nError updating book ID " << book_id << ": " << mysql_error(conn) << endl;
        }
    }
    mysql_free_result(res_set);

    // Now, mark purchases as inventoried
    stmt.str("");
    stmt << "UPDATE purchases SET inv = 1 WHERE recieved = 'T' AND inv IS NULL;";
    query = stmt.str();
    q = query.c_str();
    if (mysql_query(conn, q)) {
        cout << "\nError marking purchases as inventoried: " << mysql_error(conn) << endl;
    } else {
        cout << "The orders received have been updated successfully." << endl;
    }
}

void books::display()
{
    query = "SELECT * FROM books;";
    q = query.c_str();
    if (mysql_query(conn, q)) {
        cout << "\nQuery Error: " << mysql_error(conn) << endl;
        return;
    }
    res_set = mysql_store_result(conn);
    int count = 0;
    while ((row = mysql_fetch_row(res_set)) != NULL)
    {
        cout << "\n--- Book " << ++count << " ---" << endl;
        cout << "ID: " << row[0] << endl;
        cout << "Name: " << row[1] << endl;
        cout << "Author: " << row[2] << endl;
        cout << "Price: " << row[3] << endl;
        cout << "Quantity: " << row[4] << endl;
    }
    if (count == 0) {
        cout << "No books found in the database." << endl;
    }
    mysql_free_result(res_set);
}


// ---------------
// class suppliers
// ---------------
void suppliers::add_sup()
{
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "Enter Supplier Name: ";
    getline(cin, name);
    cout << "Enter Phone No.: ";
    cin >> phn;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "Enter Address Line 1: ";
    getline(cin, addr_line1);
    cout << "Enter Address Line 2: ";
    getline(cin, addr_line2);
    cout << "Enter City: ";
    getline(cin, addr_city);
    cout << "Enter State: ";
    getline(cin, addr_state);

    stmt.str("");
    stmt << "INSERT INTO suppliers(name, phone_no, addr1, addr2, addr_city, addr_stat) VALUES('" << name << "'," << phn << ",'" << addr_line1 << "','" << addr_line2 << "','" << addr_city << "','" << addr_state << "');";
    query = stmt.str();
    q = query.c_str();
    if (mysql_query(conn, q)) {
        cout << "\nQuery Error: " << mysql_error(conn) << endl;
    } else {
        cout << "\nSupplier Record Inserted Successfully!" << endl;
    }
}

void suppliers::remove_supplier()
{
    cout << "Enter the supplier ID to remove: ";
    cin >> id;
    stmt.str("");
    stmt << "DELETE FROM suppliers WHERE id = " << id << ";";
    query = stmt.str();
    q = query.c_str();
    if (mysql_query(conn, q)) {
        cout << "\nQuery Error: " << mysql_error(conn) << endl;
    } else {
        if (mysql_affected_rows(conn) > 0) {
            cout << "\nSupplier Removed Successfully." << endl;
        } else {
            cout << "\nSupplier ID not found." << endl;
        }
    }
}

void suppliers::search_id()
{
    cout << "Enter the supplier ID to find details: ";
    cin >> id;
    stmt.str("");
    stmt << "SELECT * FROM suppliers WHERE id = " << id << ";";
    query = stmt.str();
    q = query.c_str();
    if (mysql_query(conn, q)) {
        cout << "\nQuery Error: " << mysql_error(conn) << endl;
        return;
    }

    res_set = mysql_store_result(conn);
    if ((row = mysql_fetch_row(res_set)) != NULL)
    {
        cout << "\n--- Supplier Details ---" << endl;
        cout << "Supplier ID: " << row[0] << endl;
        cout << "Name: " << row[1] << endl;
        cout << "Phone No.: " << row[2] << endl;
        cout << "Address Line 1: " << row[3] << endl;
        cout << "Address Line 2: " << row[4] << endl;
        cout << "City: " << row[5] << endl;
        cout << "State: " << row[6] << endl;
    }
    else
    {
        cout << "\nNo Supplier Found with that ID." << endl;
    }
    mysql_free_result(res_set);
}

// ------------------
// class Purchases
// ------------------
void purchases::new_ord()
{
    cout << "Enter the Book ID: ";
    cin >> book_id;
    cout << "Enter Supplier ID: ";
    cin >> sup_id;
    cout << "Enter the Quantity: ";
    cin >> qty;
    cout << "Estimated Delivery (in days): ";
    cin >> eta;

    stmt.str("");
    stmt << "INSERT INTO purchases (book_id, sup_id, qty, dt_ord, eta) VALUES (" << book_id << "," << sup_id << "," << qty << ", CURDATE(), DATE_ADD(CURDATE(), INTERVAL " << eta << " DAY));";
    query = stmt.str();
    q = query.c_str();
    if (mysql_query(conn, q)) {
        cout << "\nQuery Error: " << mysql_error(conn) << endl;
    } else {
        cout << "\nNew Order Added Successfully!" << endl;
    }
}

void purchases::mark_reciv()
{
    cout << "Enter the order ID for order received: ";
    cin >> ord_id;
    stmt.str("");
    stmt << "UPDATE purchases SET recieved = 'T' WHERE ord_id = " << ord_id << ";";
    query = stmt.str();
    q = query.c_str();
    if (mysql_query(conn, q)) {
        cout << "\nQuery Error: " << mysql_error(conn) << endl;
    } else {
        cout << "\nOrder Marked as Received." << endl;
    }
}

void purchases::mar_cancel()
{
    cout << "Enter the order ID for order cancelled: ";
    cin >> ord_id;
    stmt.str("");
    stmt << "UPDATE purchases SET recieved = 'C' WHERE ord_id = " << ord_id << ";";
    query = stmt.str();
    q = query.c_str();
    if (mysql_query(conn, q)) {
        cout << "\nQuery Error: " << mysql_error(conn) << endl;
    } else {
        cout << "\nOrder Marked as Cancelled." << endl;
    }
}

void purchases::view()
{
    int c;
    cout << "\nSelect an Option" << endl;
    cout << "1. View Pending Orders" << endl;
    cout << "2. View Cancelled Orders" << endl;
    cout << "3. View Received Orders" << endl;
    cout << "Enter Your choice: ";
    cin >> c;

    if (c == 1) received = 'F';
    else if (c == 2) received = 'C';
    else if (c == 3) received = 'T';
    else {
        cout << "Invalid choice." << endl;
        return;
    }

    stmt.str("");
    stmt << "SELECT * FROM purchases WHERE recieved = '" << received << "';";
    query = stmt.str();
    q = query.c_str();
    if (mysql_query(conn, q)) {
        cout << "\nQuery Error: " << mysql_error(conn) << endl;
        return;
    }

    res_set = mysql_store_result(conn);
    if (c == 1) cout << "\n--- Pending Orders ---" << endl;
    else if (c == 2) cout << "\n--- Cancelled Orders ---" << endl;
    else if (c == 3) cout << "\n--- Received Orders ---" << endl;
    
    int count = 0;
    while ((row = mysql_fetch_row(res_set)) != NULL)
    {
        cout << "\nOrder ID: " << row[0] << endl;
        cout << "Book ID: " << row[1] << endl;
        cout << "Supplier ID: " << row[2] << endl;
        cout << "Quantity: " << row[3] << endl;
        cout << "Date Ordered: " << row[4] << endl;
        cout << "Estimated Delivery: " << row[5] << endl;
        count++;
    }
    if (count == 0) {
        cout << "No orders found for this status." << endl;
    }
    mysql_free_result(res_set);
}

// ----------------
// class employees
// ----------------

void employees::add_emp()
{
    int verifier_id;
    cout << "Enter Your Manager ID for verification: ";
    cin >> verifier_id;

    stmt.str("");
    stmt << "SELECT mgr_stat FROM employees WHERE id = " << verifier_id << ";";
    query = stmt.str();
    q = query.c_str();
    if (mysql_query(conn, q)) {
        cout << "\nQuery Error: " << mysql_error(conn) << endl;
        return;
    }
    res_set = mysql_store_result(conn);

    if ((row = mysql_fetch_row(res_set)) == NULL)
    {
        cout << "Verifier Employee Not Found!" << endl;
        mysql_free_result(res_set);
        return;
    }
    
    // CORRECTED LOGIC: Check if the verifier is NOT a manager ('T')
    if (string(row[0]) != "T")
    {
        cout << "Access Denied! You do not have Manager Rights!" << endl;
        mysql_free_result(res_set);
        return;
    }
    mysql_free_result(res_set); // Free result before next query

    cout << "\n--- Enter Details of New Employee ---" << endl;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "Enter Name: ";
    getline(cin, name);
    cout << "Enter Address Line 1: ";
    getline(cin, addr_line1);
    cout << "Enter Address Line 2: ";
    getline(cin, addr_line2);
    cout << "Enter City: ";
    getline(cin, addr_city);
    cout << "Enter State: ";
    getline(cin, addr_state);
    cout << "Enter Phone No.: ";
    cin >> phn;
    cout << "Enter Salary: ";
    cin >> salary;

    stmt.str("");
    stmt << "INSERT INTO employees (name,addr1,addr2,addr_city,addr_stat,phone_no,doj,salary) VALUES ('" << name << "','" << addr_line1 << "','" << addr_line2 << "','" << addr_city << "','" << addr_state << "'," << phn << ",curdate()," << salary << ");";
    query = stmt.str();
    q = query.c_str();
    if (mysql_query(conn, q)) {
        cout << "\nQuery Error: " << mysql_error(conn) << endl;
    } else {
        cout << "\nEmployee Added Successfully!" << endl;
    }
}

void employees::assign_mgr_stat()
{
    int verifier_id;
    cout << "Enter Your Manager ID for verification: ";
    cin >> verifier_id;

    stmt.str("");
    stmt << "SELECT mgr_stat FROM employees WHERE id = " << verifier_id << ";";
    query = stmt.str();
    q = query.c_str();
    if (mysql_query(conn, q)) {
        cout << "\nQuery Error: " << mysql_error(conn) << endl;
        return;
    }
    res_set = mysql_store_result(conn);

    if ((row = mysql_fetch_row(res_set)) == NULL)
    {
        cout << "Verifier Employee Not Found!" << endl;
        mysql_free_result(res_set);
        return;
    }

    // CORRECTED LOGIC
    if (string(row[0]) != "T")
    {
        cout << "Access Denied! You do not have Manager Rights!" << endl;
        mysql_free_result(res_set);
        return;
    }
    mysql_free_result(res_set);

    cout << "Enter the employee id to grant Manager status: ";
    cin >> id;
    stmt.str("");
    stmt << "UPDATE employees SET mgr_stat = 'T' WHERE id = " << id << ";";
    query = stmt.str();
    q = query.c_str();
    if (mysql_query(conn, q)) {
        cout << "\nQuery Error: " << mysql_error(conn) << endl;
    } else {
        cout << "\nManager Status Granted Successfully." << endl;
    }
}

// ---------------------
// class members
// ---------------------

void members::add_mem()
{
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "Enter Member Name: ";
    getline(cin, name);
    cout << "Enter Phone No.: ";
    cin >> phn;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "Enter Address Line 1: ";
    getline(cin, addr_line1);
    cout << "Enter Address Line 2: ";
    getline(cin, addr_line2);
    cout << "Enter City: ";
    getline(cin, addr_city);
    cout << "Enter State: ";
    getline(cin, addr_state);

    stmt.str("");
    stmt << "INSERT INTO members(name, addr1, addr2, addr_city, addr_stat, phone_no, beg_date, end_date) VALUES ('" << name << "','" << addr_line1 << "','" << addr_line2 << "','" << addr_city << "','" << addr_state << "'," << phn << ", CURDATE(), DATE_ADD(CURDATE(), INTERVAL 1 YEAR));";
    query = stmt.str();
    q = query.c_str();
    if (mysql_query(conn, q)) {
        cout << "\nQuery Error: " << mysql_error(conn) << endl;
        return;
    }
    
    // Fetching the new member id
    long long new_id = mysql_insert_id(conn);
    cout << "\nMember Added Successfully!" << endl;
    cout << "New Member ID is: " << new_id << endl;
}


void members::refresh()
{
    query = "UPDATE members SET valid = 'invalid' WHERE end_date < CURDATE() AND valid = 'valid';";
    q = query.c_str();
    if (mysql_query(conn, q)) {
         cout << "\nError refreshing memberships: " << mysql_error(conn) << endl;
    } else {
        cout << "\nMembership statuses refreshed successfully. " << mysql_affected_rows(conn) << " memberships updated." << endl;
    }
}

void members::search_mem()
{
    cout << "Enter member ID to search: ";
    cin >> id;
    stmt.str("");
    stmt << "SELECT * FROM members WHERE id = " << id << ";";
    query = stmt.str();
    q = query.c_str();
    if (mysql_query(conn, q)) {
        cout << "\nQuery Error: " << mysql_error(conn) << endl;
        return;
    }

    res_set = mysql_store_result(conn);
    if ((row = mysql_fetch_row(res_set)) != NULL)
    {
        cout << "\n--- Member Details ---" << endl;
        cout << "Member ID: " << row[0] << endl;
        cout << "Name: " << row[1] << endl;
        cout << "Address: " << row[2] << ", " << row[3] << ", " << row[4] << ", " << row[5] << endl;
        cout << "Contact No.: " << row[6] << endl;
        cout << "Membership Begin: " << row[7] << endl;
        cout << "Membership End: " << row[8] << endl;
        cout << "Membership Status: " << row[9] << endl;
    }
    else
    {
        cout << "\nNo Member Found with that ID." << endl;
    }
    mysql_free_result(res_set);
}

// ------------------
// Class Sales
// ------------------
void sales::add()
{
    cout << "Enter Member id: ";
    cin >> member_id;
    cout << "Enter the book id: ";
    cin >> book_id;
    cout << "Enter the quantity: ";
    cin >> qty;

    stmt.str("");
    stmt << "SELECT price FROM books WHERE id = " << book_id << " AND qty >= " << qty << ";";
    query = stmt.str();
    q = query.c_str();

    if (mysql_query(conn, q)) {
        cout << "\nQuery Error: " << mysql_error(conn) << endl;
        return;
    }
    res_set = mysql_store_result(conn);

    if ((row = mysql_fetch_row(res_set)) != NULL)
    {
        int price_per_book = stoi(row[0]);
        amount = price_per_book * qty;
        cout << "The bill amount: " << amount << endl;
        mysql_free_result(res_set);

        // Start transaction
        mysql_query(conn, "START TRANSACTION;");

        // Insert into sales table
        stmt.str("");
        stmt << "INSERT INTO sales(mem_id, book_id, qty, amt, sales_date) VALUES (" << member_id << "," << book_id << "," << qty << "," << amount << ",curdate());";
        query = stmt.str();
        q = query.c_str();
        if (mysql_query(conn, q)) {
            cout << "\nSales Insert Error: " << mysql_error(conn) << endl;
            mysql_query(conn, "ROLLBACK;"); // Rollback on failure
            return;
        }

        // Update books quantity
        stmt.str("");
        stmt << "UPDATE books SET qty = qty - " << qty << " WHERE id = " << book_id << ";";
        query = stmt.str();
        q = query.c_str();
        if (mysql_query(conn, q)) {
            cout << "\nBook Update Error: " << mysql_error(conn) << endl;
            mysql_query(conn, "ROLLBACK;"); // Rollback on failure
            return;
        }

        mysql_query(conn, "COMMIT;"); // Commit if both succeed
        cout << "Sale recorded successfully!" << endl;
    }
    else
    {
        cout << "Book ID invalid or insufficient quantity in stock!" << endl;
        // mysql_free_result is not needed here as row was NULL
    }
}


// +----------------------------------+
// |           Menu Functions         |
// +----------------------------------+
// Forward declarations
void main_menu();
void book_menu();
void sup_menu();
void pur_menu();
void emp_menu();
void mem_menu();
void sal_menu();

// main menu
void main_menu()
{
    int c;
    cout << "\n*************************************************" << endl;
    cout << "         BOOKSHOP MANAGEMENT SYSTEM" << endl;
    cout << "*************************************************" << endl;
    cout << "   1. BOOKS" << endl;
    cout << "   2. SUPPLIERS" << endl;
    cout << "   3. PURCHASES" << endl;
    cout << "   4. EMPLOYEES" << endl;
    cout << "   5. MEMBERS" << endl;
    cout << "   6. SALES" << endl;
    cout << "   7. EXIT" << endl << endl;
    cout << "Enter Your Choice: ";
    cin >> c;
    if(cin.fail()){ // Basic input validation
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        c = 0; // Set to default case
    }

    switch (c)
    {
        case 1: system("clear"); book_menu(); break;
        case 2: system("clear"); sup_menu(); break;
        case 3: system("clear"); pur_menu(); break;
        case 4: system("clear"); emp_menu(); break;
        case 5: system("clear"); mem_menu(); break;
        case 6: system("clear"); sal_menu(); break;
        case 7: exit(0);
        default:
            cout << "\nInvalid input. Please try again." << endl;
            break;
    }
    pause_screen();
}

void book_menu()
{
    int c;
    books b;
    cout << "*************************************************" << endl;
    cout << "                  BOOK MENU" << endl;
    cout << "*************************************************" << endl;
    cout << "   1. ADD" << endl;
    cout << "   2. UPDATE PRICE" << endl;
    cout << "   3. SEARCH" << endl;
    cout << "   4. UPDATE INVENTORY FROM DELIVERIES" << endl;
    cout << "   5. DISPLAY ALL" << endl;
    cout << "   6. RETURN TO MAIN MENU" << endl << endl;
    cout << "Enter Your Choice: ";
    cin >> c;
     if(cin.fail()){
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        c = 0;
    }

    switch (c)
    {
        case 1: b.add(); break;
        case 2: b.update_price(); break;
        case 3: b.search(); break;
        case 4: b.update(); break;
        case 5: b.display(); break;
        case 6: return;
        default: cout << "Wrong Input" << endl << "Invalid input"; break;
    }
}


void sup_menu() {
    int c;
    suppliers s;
    cout << "*************************************************" << endl;
    cout << "                SUPPLIER MENU" << endl;
    cout << "*************************************************" << endl;
    cout << "   1. ADD NEW SUPPLIER" << endl;
    cout << "   2. REMOVE SUPPLIER" << endl;
    cout << "   3. SEARCH FOR SUPPLIER" << endl;
    cout << "   4. RETURN TO MAIN MENU" << endl << endl;
    cout << "Enter Your Choice: ";
    cin >> c;
     if(cin.fail()){
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        c = 0;
    }
    switch(c) {
        case 1: s.add_sup(); break;
        case 2: s.remove_supplier(); break;
        case 3: s.search_id(); break;
        case 4: return;
        default: cout << "Invalid Input" << endl; break;
    }
}

void pur_menu() {
    int c;
    purchases p;
    cout << "*************************************************" << endl;
    cout << "                PURCHASES MENU" << endl;
    cout << "*************************************************" << endl;
    cout << "   1. New Order" << endl;
    cout << "   2. View Orders" << endl;
    cout << "   3. Mark Order as Cancelled" << endl;
    cout << "   4. Mark Order as Received" << endl;
    cout << "   5. RETURN TO MAIN MENU" << endl << endl;
    cout << "Enter Your Choice: ";
    cin >> c;
     if(cin.fail()){
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        c = 0;
    }
    switch(c) {
        case 1: p.new_ord(); break;
        case 2: p.view(); break;
        case 3: p.mar_cancel(); break;
        case 4: p.mark_reciv(); break;
        case 5: return;
        default: cout << "Invalid Input" << endl; break;
    }
}

void mem_menu() {
    int c;
    members m;
    cout << "*************************************************" << endl;
    cout << "                 MEMBERS MENU" << endl;
    cout << "*************************************************" << endl;
    cout << "   1. New Member" << endl;
    cout << "   2. Search Member" << endl;
    cout << "   3. Refresh Membership Statuses" << endl;
    cout << "   4. RETURN TO MAIN MENU" << endl << endl;
    cout << "Enter Your Choice: ";
    cin >> c;
    if(cin.fail()){
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        c = 0;
    }
    switch (c) {
        case 1: m.add_mem(); break;
        case 2: m.search_mem(); break;
        case 3: m.refresh(); break;
        case 4: return;
        default: cout << "Invalid Input" << endl; break;
    }
}

void sal_menu() {
    int c;
    sales s;
    cout << "*************************************************" << endl;
    cout << "                 SALES MENU" << endl;
    cout << "*************************************************" << endl;
    cout << "   1. Add New Sale" << endl;
    cout << "   2. Total Sales Of The Year" << endl;
    cout << "   3. RETURN TO MAIN MENU" << endl << endl;
    cout << "Enter Your Choice: ";
    cin >> c;
     if(cin.fail()){
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        c = 0;
    }

    switch (c) {
        case 1: s.add(); break;
        // case 2: s.find_total_sales(); break; // This would need to be implemented
        case 3: return;
        default: cout << "Invalid Input." << endl; break;
    }
}
void emp_menu() {
    int c;
    employees e;
    cout << "*************************************************" << endl;
    cout << "                 EMPLOYEE MENU" << endl;
    cout << "*************************************************" << endl;
    cout << "   1. New Employee" << endl;
    cout << "   2. Search Employee" << endl;
    cout << "   3. Assign Manager" << endl;
    cout << "   4. View All" << endl;
    cout << "   5. Update Salary" << endl;
    cout << "   6. RETURN TO MAIN MENU" << endl << endl;
    cout << "Enter Your Choice: ";
    cin >> c;
    if(cin.fail()){
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        c = 0;
    }

    switch (c) {
        case 1: e.add_emp(); break;
        case 3: e.assign_mgr_stat(); break;
        case 6: return;
        default: cout << "Option not fully implemented in this example." << endl; break;
    }
}


// Password Function
void pass()
{
    int num = 0;
    char ch;
    cout << "Enter 4-digit password: ";
    for(int i = 0; i < 4; i++)
    {
        ch = getch();
        if (ch >= '0' && ch <= '9') {
            num = num * 10 + (ch - '0');
            cout << "*";
        } else {
            i--; // don't count non-digit input
        }
    }

    if (num == PASSWORD)
    {
        cout << "\n\nCorrect Password" << endl;
        pause_screen();
    }
    else
    {
        cout << "\n\nIncorrect Password" << endl;
        pause_screen();
        exit(1);
    }
}


// +----------------------------------+
// |        Main Function             |
// +----------------------------------+

int main()
{
    conn = mysql_init(0);
    if (!conn) {
        cout << "MySQL initialization failed." << endl;
        return 1;
    }

    conn = mysql_real_connect(conn, HOST, USER, PASS, DATABASE, PORT, NULL, 0);

    if (conn)
    {
        cout << "Connected to database successfully." << endl;
        pass(); // Ask for password only after successful connection
        while(1)
        {
            system("clear");
            main_menu();
        }
    }
    else
    {
        cout << "Connection Error: " << mysql_error(conn) << endl;
        pause_screen();
    }

    mysql_close(conn);
    return 0;
}
