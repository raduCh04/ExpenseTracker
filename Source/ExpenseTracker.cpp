#include "ExpenseTracker.h"

ExpenseTracker::ExpenseTracker() 
    : m_Total(0.0f), m_IsRunning(true), m_Database(nullptr) {
        int exit = sqlite3_open("Expenses.db", &m_Database);
        if (exit != SQLITE_OK) {
            std::cerr << "Error open DB " << sqlite3_errmsg(m_Database) << '\n';
        } else {
            std::cout << "Opened Database Successfully!" << std::endl;
        }

        std::string table_create = "CREATE TABLE IF NOT EXISTS Expenses("
                                    "Id INTEGER PRIMARY KEY AUTOINCREMENT,"
                                    "Name TEXT,"
                                    "Value REAL)";
        char *message_error;
        exit = sqlite3_exec(m_Database, table_create.c_str(), nullptr, 0, &message_error);

        if (exit != SQLITE_OK) {
            std::cerr << "Error create table" << message_error << '\n';
            sqlite3_free(message_error);
        } else {
            std::cout << "Table created succesfully!" << '\n';
        }

        std::string totalTableCreate = "CREATE TABLE IF NOT EXISTS Total (Value REAL)";
        char *totalMessageError;
        exit = sqlite3_exec(m_Database, totalTableCreate.c_str(), nullptr, 0, &totalMessageError);
        if (exit != SQLITE_OK) {
            std::cerr << "Error creating total table: " << totalMessageError << '\n';
            sqlite3_free(totalMessageError);
        } else {
            std::cout << "Total table created successfully!" << '\n';
        }

        LoadTotalFromDatabase();

        std::string insert_value = "INSERT INTO Total (Value) VALUES (?)";
        sqlite3_stmt *statement;
        int result = sqlite3_prepare_v2(m_Database, insert_value.c_str(), -1, &statement, nullptr);
        if (result != SQLITE_OK) {
            std::cerr << "Error preparing statement: " << sqlite3_errmsg(m_Database) << '\n';
        }
        sqlite3_bind_double(statement, 1, m_Total);

        result = sqlite3_step(statement);
        if (result != SQLITE_DONE) {
        std::cerr << "Error inserting total: " << sqlite3_errmsg(m_Database) << '\n';
        } else {
            std::cout << "Total Added\n";
        }
        sqlite3_finalize(statement);

}

ExpenseTracker::ExpenseTracker(const ExpenseTracker &other)
    : m_Total(other.m_Total), m_IsRunning(other.m_IsRunning), m_Database(other.m_Database) {
}

ExpenseTracker::ExpenseTracker(ExpenseTracker &&other)
    : m_Total(other.m_Total),m_IsRunning(other.m_IsRunning), m_Database(std::move(other.m_Database)) {
        other.m_Total = 0.0f;
        other.m_Database = nullptr;
}

ExpenseTracker::~ExpenseTracker() {
    sqlite3_close(m_Database);
}

void ExpenseTracker::AddEntry(const std::string &name, float value) {
    m_Total += value;
    std::string insert = "INSERT INTO Expenses (Name, Value) VALUES (?, ?)";
    sqlite3_stmt *statement;
    int result = sqlite3_prepare_v2(m_Database, insert.c_str(), -1, &statement, nullptr);
    if (result != SQLITE_OK) {
        std::cerr << "Error preparing statement: " << sqlite3_errmsg(m_Database) << '\n';
        return;
    }
    sqlite3_bind_text(statement, 1, name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_double(statement, 2, static_cast<float>(value));

    result = sqlite3_step(statement);
    if (result != SQLITE_DONE) {
        std::cerr << "Error inserting expense: " << sqlite3_errmsg(m_Database) << '\n';
    } else {
        std::cout << "Expense Added\n";
    }
    sqlite3_finalize(statement);

    SaveTotalToDatabase();
}

void ExpenseTracker::RemoveEntry(const std::string &name) {
    std::string selectValueQuery = "SELECT Value FROM Expenses WHERE Name = ?";
    sqlite3_stmt *selectStatement;
    int result = sqlite3_prepare_v2(m_Database, selectValueQuery.c_str(), -1, &selectStatement, nullptr);
    if (result != SQLITE_OK) {
        std::cerr << "Error preparing select statement: " << sqlite3_errmsg(m_Database) << '\n';
        return;
    }
    sqlite3_bind_text(selectStatement, 1, name.c_str(), -1, SQLITE_STATIC);

    float removedValue = 0.0f;

    if (sqlite3_step(selectStatement) == SQLITE_ROW) {
        removedValue = static_cast<float>(sqlite3_column_double(selectStatement, 0));
        std::cout << "Happened!\n"; 
    }

    std::cout << "Removed value" << removedValue << '\n';

    sqlite3_finalize(selectStatement);
    std::string deleteQuery = "DELETE FROM Expenses WHERE Name = ?";
    sqlite3_stmt *statement;
    result = sqlite3_prepare_v2(m_Database, deleteQuery.c_str(), -1, &statement, nullptr);
    if (result != SQLITE_OK) {
        std::cerr << "Error preparing delete statement: " << sqlite3_errmsg(m_Database) << '\n';
        return;
    }
    sqlite3_bind_text(statement, 1, name.c_str(), -1, SQLITE_STATIC);
    result = sqlite3_step(statement);
    if (result != SQLITE_DONE) {
        std::cerr << "Error deleting expense: " << sqlite3_errmsg(m_Database) << '\n';
    } else {
        std::cout << "Expense Removed from Database\n";
    }
    sqlite3_finalize(statement);

    m_Total -= removedValue;
    SaveTotalToDatabase();
    
    std::cout << "Removed expense: " << "\"" << name << "\"" << '\n';
    return;
}

void ExpenseTracker::ChangeEntryName(const std::string &name, const std::string &new_name) {
    std::string updateQuery = "UPDATE Expenses SET Name = ? WHERE Name = ?";
    sqlite3_stmt *statement;
    int result = sqlite3_prepare_v2(m_Database, updateQuery.c_str(), -1, &statement, nullptr);
    if (result != SQLITE_OK) {
        std::cerr << "Error preparing update statement: " << sqlite3_errmsg(m_Database) << '\n';
        return;
    }
    sqlite3_bind_text(statement, 1, new_name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(statement, 2, name.c_str(), -1, SQLITE_STATIC);
    result = sqlite3_step(statement);
    if (result != SQLITE_DONE) {
        std::cerr << "Error updating expense name: " << sqlite3_errmsg(m_Database) << '\n';
    } else {
        std::cout << "Expense Name Updated in Database\n";
    }
    sqlite3_finalize(statement);
    return;
}

void ExpenseTracker::ChangeEntryValue(const std::string &name, float new_value) {
    std::string selectValueQuery = "SELECT Value FROM Expenses WHERE Name = ?";
    sqlite3_stmt *selectStatement;
    int result = sqlite3_prepare_v2(m_Database, selectValueQuery.c_str(), -1, &selectStatement, nullptr);
    if (result != SQLITE_OK) {
        std::cerr << "Error preparing select statement: " << sqlite3_errmsg(m_Database) << '\n';
        return;
    }

    sqlite3_bind_text(selectStatement, 1, name.c_str(), -1, SQLITE_STATIC);

    float oldValue = 0.0f;

    if (sqlite3_step(selectStatement) == SQLITE_ROW) {
        oldValue = static_cast<float>(sqlite3_column_double(selectStatement, 0));
    }

    sqlite3_finalize(selectStatement);
    std::string updateQuery = "UPDATE Expenses SET Value = ? WHERE Name = ?";
    sqlite3_stmt *statement;
    result = sqlite3_prepare_v2(m_Database, updateQuery.c_str(), -1, &statement, nullptr);
    if (result != SQLITE_OK) {
        std::cerr << "Error preparing update statement: " << sqlite3_errmsg(m_Database) << '\n';
        return;
    }
    sqlite3_bind_double(statement, 1, static_cast<float>(new_value));
    sqlite3_bind_text(statement, 2, name.c_str(), -1, SQLITE_STATIC);
    result = sqlite3_step(statement);
    if (result != SQLITE_DONE) {
        std::cerr << "Error updating expense value: " << sqlite3_errmsg(m_Database) << '\n';
    } else {
        std::cout << "Expense Value Updated in Database\n";
    }
    sqlite3_finalize(statement);

    m_Total -= oldValue - new_value;

    SaveTotalToDatabase();
}

void ExpenseTracker::SaveTotalToDatabase() {
    std::string updateTotalQuery = "UPDATE Total SET Value = ?";
    sqlite3_stmt *updateStatement;
    int result = sqlite3_prepare_v2(m_Database, updateTotalQuery.c_str(), -1, &updateStatement, nullptr);
    if (result != SQLITE_OK) {
        std::cerr << "Error preparing update total statement: " << sqlite3_errmsg(m_Database) << '\n';
        return;
    }
    sqlite3_bind_double(updateStatement, 1, static_cast<double>(m_Total));

    result = sqlite3_step(updateStatement);
    if (result != SQLITE_DONE) {
        std::cerr << "Error updating total value in the database: " << sqlite3_errmsg(m_Database) << '\n';
    } else {
        std::cout << "Total Value Updated in the Database\n";
        
    }
    sqlite3_finalize(updateStatement);
}


void ExpenseTracker::LoadTotalFromDatabase() {
    std::string selectTotalQuery = "SELECT Value FROM Total";
    sqlite3_stmt *selectStatement;
    int result = sqlite3_prepare_v2(m_Database, selectTotalQuery.c_str(), -1, &selectStatement, nullptr);
    if (result != SQLITE_OK) {
        std::cerr << "Error preparing select total statement: " << sqlite3_errmsg(m_Database) << '\n';
        return;
    }

    result = sqlite3_step(selectStatement);
    if (result == SQLITE_ROW) {
        m_Total = static_cast<float>(sqlite3_column_double(selectStatement, 0));
        std::cout << "Total Value Loaded from the Database\n";
    } else if (result == SQLITE_DONE) {
        // No rows found, initialize total to zero or any default value
        m_Total = 0.0f;
        std::cout << "Total Value not found in the database, initialized to zero.\n";
    } else {
        std::cerr << "Error loading total value from the database: " << sqlite3_errmsg(m_Database) << '\n';
    }

    sqlite3_finalize(selectStatement);
}

void ExpenseTracker::Run() {
    while (IsRunning()) {
        DisplayInterface();
        uint16_t input;
        if (!(std::cin >> input)) {
            continue;
        }
        HandleCommands(input);
    }
}

void ExpenseTracker::DisplayInterface()
{
    std::string text_array[] = { "1. Display expenses", "2. Add entry", "3. Remove entry", "4. Change entry name", "5. Change entry value", "6. Quit" };
    for (auto str : text_array) {
        std::cout << str << '\n';
    }
}

void ExpenseTracker::HandleCommands(uint16_t command_index) {
    switch (command_index) {
        case 1: {
            std::cout << *this;
        } break;
        case 2: {
            std::string entry_name;
            float entry_value;

            std::cout << "Enter the entry name:\n";
            std::cin >> entry_name;
            std::cout << "Enter the entry value:\n";
            std::cin >> entry_value;

            AddEntry(entry_name, entry_value);
        } break;
        case 3: {
            std::string entry_name;

            std::cout << "Enter the entry name:\n";
            std::cin >> entry_name;

            RemoveEntry(entry_name);
        } break;
        case 4: {
            std::string entry_name;
            std::string new_name;

            std::cout << "Enter the entry name:\n";
            std::cin >> entry_name;
            std::cout << "Enter the new entry name:\n";
            std::cin >> new_name;

            ChangeEntryName(entry_name, new_name);
        } break;
        case 5: {
            std::string entry_name;
            float new_value;

            std::cout << "Enter the entry name:\n";
            std::cin >> entry_name;
            std::cout << "Enter the new value:\n";
            std::cin >> new_value;

            ChangeEntryValue(entry_name, new_value);
        } break;
        case 6: {
            Stop();
        } break;

        default: {
            std::cout << "Not valid operation!\n";
        } break;
    }
}

std::ostream &operator<<(std::ostream &os, const ExpenseTracker &tracker) {
    std::string selectQuery = "SELECT * FROM Expenses";
    sqlite3_stmt* statement;
    int result = sqlite3_prepare_v2(tracker.m_Database, selectQuery.c_str(), -1, &statement, nullptr);
    if (result != SQLITE_OK) {
        std::cerr << "Error preparing select statement: " << sqlite3_errmsg(tracker.m_Database) << '\n';
        return os;
    }
    os << "######################################\n";
    while (sqlite3_step(statement) == SQLITE_ROW) {
        int id = sqlite3_column_int(statement, 0);
        const char* name = (const char*)sqlite3_column_text(statement, 1);
        double value = sqlite3_column_double(statement, 2);
        os << "ID: " << id << ", Name: " << name << ", Value: " << value << '$' << '\n';
    }
    os << "Total: " << tracker.m_Total << "$\n";
    os << "######################################\n";
    sqlite3_finalize(statement);

    std::string selectTotalQuery = "SELECT Value FROM Total";
    sqlite3_stmt *selectStatement;
    result = sqlite3_prepare_v2(tracker.m_Database, selectTotalQuery.c_str(), -1, &selectStatement, nullptr);
    if (result != SQLITE_OK) {
        std::cerr << "Error preparing select total statement: " << sqlite3_errmsg(tracker.m_Database) << '\n';
        return os;
    }

    result = sqlite3_step(selectStatement);
    if (result == SQLITE_ROW) {
        float totalValue = sqlite3_column_double(selectStatement, 0);
        os << "Total Value from the Database: " << totalValue << "$\n";
    } else if (result == SQLITE_DONE) {
        os << "Total Value not found in the database.\n";
    } else {
        std::cerr << "Error loading total value from the database: " << sqlite3_errmsg(tracker.m_Database) << '\n';
    }

    sqlite3_finalize(selectStatement);
    return os;
}
