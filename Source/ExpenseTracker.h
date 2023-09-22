#pragma once

#include <iostream>
#include <memory>
#include <string>

#include "sqlite3.h"

class ExpenseTracker {
    public:
        ExpenseTracker();
        ExpenseTracker(const ExpenseTracker& other);
        ExpenseTracker(ExpenseTracker&& other);
        ~ExpenseTracker();

        void AddEntry(const std::string& name, float value);
        void RemoveEntry(const std::string& name);
        void ChangeEntryName(const std::string& name, const std::string& new_name);
        void ChangeEntryValue(const std::string& name, float new_value);

        void SaveTotalToDatabase();
        void LoadTotalFromDatabase();

        void Run();
        void DisplayInterface();
        void HandleCommands(uint16_t command_index);

        inline void Stop() { m_IsRunning = false; }
        inline bool IsRunning() const { return m_IsRunning; }

        friend std::ostream& operator<<(std::ostream& os, const ExpenseTracker &tracker);
    private:
        float m_Total;
        bool m_IsRunning;
        sqlite3* m_Database;
};