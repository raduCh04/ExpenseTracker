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

    inline sqlite3 *GetDatabase() const { return m_Database; }
    inline std::vector<std::string> GetTableStrings() const { return m_TableStrings; }

    inline f32 GetTempValue() const { return m_TempValue; }
    inline const char* GetTempBuffer() const { return m_TempBuffer; }

    inline f32& GetTempValue() { return m_TempValue; }
    inline char* GetTempBuffer() { return m_TempBuffer; }

private:
    void InitDatabase();

    void SaveTotalToDatabase();
    void LoadTotalFromDatabase();

private:
    f32 m_Total;
    sqlite3 *m_Database;
    std::vector<std::string> m_TableStrings;
    f32 m_TempValue;
    char m_TempBuffer[256];
};