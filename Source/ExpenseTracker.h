#pragma once

#include <iostream>
#include <memory>
#include <string>

#include "SQLite3/sqlite3.h"
#include "Application.h"

class ExpenseTrackerRenderer;

class ExpenseTracker : public Application
{
public:
    ExpenseTracker();
    ExpenseTracker(const ExpenseTracker &) = delete;
    ExpenseTracker(ExpenseTracker &&) = delete;
    virtual ~ExpenseTracker();

    virtual void Run() override;

    void AddEntry(const std::string &name, f32 value);
    void RemoveEntry(const std::string &name);
    void ChangeEntryName(const std::string &name, const std::string &new_name);
    void ChangeEntryValue(const std::string &name, f32 new_value);

    auto GetDatabase() const { return m_Database; }
    auto GetTableStrings() const { return m_TableStrings; }
private:
    void InitDatabase();

    void SaveTotalToDatabase();
    void LoadTotalFromDatabase();

private:
    f32 m_Total;
    sqlite3 *m_Database;
    std::vector<std::string> m_TableStrings;
};