#include "ExpenseTracker.h"
#include "ExpenseTrackerRenderer.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_glfw.h"
#include "ImGui/imgui_impl_opengl3.h"

static bool initialized = false;

ExpenseTracker::ExpenseTracker()
    : m_Total(0.0f), m_Database(nullptr), m_TableStrings({"Id", "Name", "Value"}), m_TempBuffer(), m_TempValue(0.0f)
{
    InitDatabase();
}

ExpenseTracker::~ExpenseTracker()
{
    m_TableStrings.clear();
    sqlite3_close(m_Database);
}

void ExpenseTracker::InitDatabase()
{
    // Establish Connection
    i32 exit = sqlite3_open("Expenses.db", &m_Database);
    if (exit != SQLITE_OK)
    {
        std::cerr << "Error open DB " << sqlite3_errmsg(m_Database) << '\n';
    }
    else
    {
        std::cout << "Opened Database Successfully!" << std::endl;
    }

    // Create Expenses table
    std::string table_create = "CREATE TABLE IF NOT EXISTS Expenses("
                               "Id INTEGER PRIMARY KEY AUTOINCREMENT,"
                               "Name TEXT,"
                               "Value REAL)";
    char *message_error = nullptr;
    exit = sqlite3_exec(m_Database, table_create.c_str(), nullptr, 0, &message_error);
    if (exit != SQLITE_OK)
    {
        std::cerr << "Error create table" << message_error << '\n';
        sqlite3_free(message_error);
    }
    else
    {
        std::cout << "Table created succesfully!" << '\n';
    }

    // Create Total Value Table
    std::string totalTableCreate = "CREATE TABLE IF NOT EXISTS Total (Value REAL)";
    char *totalMessageError = nullptr;
    exit = sqlite3_exec(m_Database, totalTableCreate.c_str(), nullptr, 0, &totalMessageError);
    if (exit != SQLITE_OK)
    {
        std::cerr << "Error creating total table: " << totalMessageError << '\n';
        sqlite3_free(totalMessageError);
    }
    else
    {
        std::cout << "Total table created successfully!" << '\n';
    }

    // Setup Total Value Table
    LoadTotalFromDatabase();
    std::string insert_value = "INSERT INTO Total (Value) VALUES (?)";
    sqlite3_stmt *statement = nullptr;
    i32 result = sqlite3_prepare_v2(m_Database, insert_value.c_str(), -1, &statement, nullptr);
    if (result != SQLITE_OK)
    {
        std::cerr << "Error preparing statement: " << sqlite3_errmsg(m_Database) << '\n';
    }
    sqlite3_bind_double(statement, 1, m_Total);
    result = sqlite3_step(statement);
    if (result != SQLITE_DONE)
    {
        std::cerr << "Error inserting total: " << sqlite3_errmsg(m_Database) << '\n';
    }
    else
    {
        std::cout << "Total Added\n";
    }
    sqlite3_finalize(statement);
}

void ExpenseTracker::AddEntry(const std::string &name, f32 value)
{
    m_Total += value;
    std::string insert = "INSERT INTO Expenses (Name, Value) VALUES (?, ?)";
    sqlite3_stmt *statement = nullptr;
    i32 result = sqlite3_prepare_v2(m_Database, insert.c_str(), -1, &statement, nullptr);
    if (result != SQLITE_OK)
    {
        std::cerr << "Error preparing statement: " << sqlite3_errmsg(m_Database) << '\n';
        return;
    }
    sqlite3_bind_text(statement, 1, name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_double(statement, 2, static_cast<f32>(value));

    result = sqlite3_step(statement);
    if (result != SQLITE_DONE)
    {
        std::cerr << "Error inserting expense: " << sqlite3_errmsg(m_Database) << '\n';
    }
    else
    {
        std::cout << "Expense Added\n";
    }
    sqlite3_finalize(statement);

    SaveTotalToDatabase();
}

void ExpenseTracker::RemoveEntry(const std::string &name)
{
    std::string selectValueQuery = "SELECT Value FROM Expenses WHERE Name = ?";
    sqlite3_stmt *selectStatement;
    i32 result = sqlite3_prepare_v2(m_Database, selectValueQuery.c_str(), -1, &selectStatement, nullptr);
    if (result != SQLITE_OK)
    {
        std::cerr << "Error preparing select statement: " << sqlite3_errmsg(m_Database) << '\n';
        return;
    }
    sqlite3_bind_text(selectStatement, 1, name.c_str(), -1, SQLITE_STATIC);

    f32 removedValue = 0.0f;

    if (sqlite3_step(selectStatement) == SQLITE_ROW)
    {
        removedValue = static_cast<f32>(sqlite3_column_double(selectStatement, 0));
    }

    sqlite3_finalize(selectStatement);
    std::string deleteQuery = "DELETE FROM Expenses WHERE Name = ?";
    sqlite3_stmt *statement;
    result = sqlite3_prepare_v2(m_Database, deleteQuery.c_str(), -1, &statement, nullptr);
    if (result != SQLITE_OK)
    {
        std::cerr << "Error preparing delete statement: " << sqlite3_errmsg(m_Database) << '\n';
        return;
    }
    sqlite3_bind_text(statement, 1, name.c_str(), -1, SQLITE_STATIC);
    result = sqlite3_step(statement);
    if (result != SQLITE_DONE)
    {
        std::cerr << "Error deleting expense: " << sqlite3_errmsg(m_Database) << '\n';
    }
    else
    {
        std::cout << "Expense Removed from Database\n";
    }
    sqlite3_finalize(statement);

    m_Total -= removedValue;
    SaveTotalToDatabase();
}

void ExpenseTracker::ChangeEntryName(const std::string &name, const std::string &new_name)
{
    std::string updateQuery = "UPDATE Expenses SET Name = ? WHERE Name = ?";
    sqlite3_stmt *statement;
    i32 result = sqlite3_prepare_v2(m_Database, updateQuery.c_str(), -1, &statement, nullptr);
    if (result != SQLITE_OK)
    {
        std::cerr << "Error preparing update statement: " << sqlite3_errmsg(m_Database) << '\n';
        return;
    }
    sqlite3_bind_text(statement, 1, new_name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(statement, 2, name.c_str(), -1, SQLITE_STATIC);
    result = sqlite3_step(statement);
    if (result != SQLITE_DONE)
    {
        std::cerr << "Error updating expense name: " << sqlite3_errmsg(m_Database) << '\n';
    }
    else
    {
        std::cout << "Expense Name Updated in Database\n";
    }
    sqlite3_finalize(statement);
    return;
}

void ExpenseTracker::ChangeEntryValue(const std::string &name, f32 new_value)
{
    std::string selectValueQuery = "SELECT Value FROM Expenses WHERE Name = ?";
    sqlite3_stmt *selectStatement;
    i32 result = sqlite3_prepare_v2(m_Database, selectValueQuery.c_str(), -1, &selectStatement, nullptr);
    if (result != SQLITE_OK)
    {
        std::cerr << "Error preparing select statement: " << sqlite3_errmsg(m_Database) << '\n';
        return;
    }

    sqlite3_bind_text(selectStatement, 1, name.c_str(), -1, SQLITE_STATIC);

    f32 oldValue = 0.0f;

    if (sqlite3_step(selectStatement) == SQLITE_ROW)
    {
        oldValue = static_cast<f32>(sqlite3_column_double(selectStatement, 0));
    }

    sqlite3_finalize(selectStatement);
    std::string updateQuery = "UPDATE Expenses SET Value = ? WHERE Name = ?";
    sqlite3_stmt *statement;
    result = sqlite3_prepare_v2(m_Database, updateQuery.c_str(), -1, &statement, nullptr);
    if (result != SQLITE_OK)
    {
        std::cerr << "Error preparing update statement: " << sqlite3_errmsg(m_Database) << '\n';
        return;
    }
    sqlite3_bind_double(statement, 1, static_cast<f32>(new_value));
    sqlite3_bind_text(statement, 2, name.c_str(), -1, SQLITE_STATIC);
    result = sqlite3_step(statement);
    if (result != SQLITE_DONE)
    {
        std::cerr << "Error updating expense value: " << sqlite3_errmsg(m_Database) << '\n';
    }
    else
    {
        std::cout << "Expense Value Updated in Database\n";
    }
    sqlite3_finalize(statement);

    m_Total -= oldValue - new_value;

    SaveTotalToDatabase();
}

void ExpenseTracker::SaveTotalToDatabase()
{
    std::string updateTotalQuery = "UPDATE Total SET Value = ?";
    sqlite3_stmt *updateStatement = nullptr;
    i32 result = sqlite3_prepare_v2(m_Database, updateTotalQuery.c_str(), -1, &updateStatement, nullptr);
    if (result != SQLITE_OK)
    {
        std::cerr << "Error preparing update total statement: " << sqlite3_errmsg(m_Database) << '\n';
        return;
    }
    sqlite3_bind_double(updateStatement, 1, static_cast<double>(m_Total));

    result = sqlite3_step(updateStatement);
    if (result != SQLITE_DONE)
    {
        std::cerr << "Error updating total value in the database: " << sqlite3_errmsg(m_Database) << '\n';
    }
    else
    {
        std::cout << "Total Value Updated in the Database\n";
    }
    sqlite3_finalize(updateStatement);
}

void ExpenseTracker::LoadTotalFromDatabase()
{
    std::string selectTotalQuery = "SELECT Value FROM Total";
    sqlite3_stmt *selectStatement = nullptr;
    i32 result = sqlite3_prepare_v2(m_Database, selectTotalQuery.c_str(), -1, &selectStatement, nullptr);
    if (result != SQLITE_OK)
    {
        std::cerr << "Error preparing select total statement: " << sqlite3_errmsg(m_Database) << '\n';
        return;
    }

    result = sqlite3_step(selectStatement);
    if (result == SQLITE_ROW)
    {
        m_Total = static_cast<f32>(sqlite3_column_double(selectStatement, 0));
        std::cout << "Total Value Loaded from the Database\n";
    }
    else if (result == SQLITE_DONE)
    {
        m_Total = 0.0f;
        std::cout << "Total Value not found in the database, initialized to zero.\n";
    }
    else
    {
        std::cerr << "Error loading total value from the database: " << sqlite3_errmsg(m_Database) << '\n';
    }

    sqlite3_finalize(selectStatement);
}

void ExpenseTracker::Run()
{
    ExpenseTrackerRenderer &renderer = ExpenseTrackerRenderer::Get(*this);
    while (IsRunning())
    {
        glfwPollEvents();
        renderer.RenderStart();
        renderer.RenderMenuBar();
        renderer.RenderTable();
        renderer.RenderEnd();
    }
}
