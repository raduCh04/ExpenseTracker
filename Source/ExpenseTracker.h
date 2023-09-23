#pragma once

#include <iostream>
#include <memory>
#include <string>

#include "sqlite3.h"
#include "Application.h"

class ExpenseTracker : public Application {
    public:
        ExpenseTracker();
        ExpenseTracker(const ExpenseTracker& other);
        ExpenseTracker(ExpenseTracker&& other);
        ~ExpenseTracker();
        
        virtual void Run() override;

        friend std::ostream& operator<<(std::ostream& os, const ExpenseTracker &tracker);
    private:
        void DisplayInterface();
        void HandleCommands(uint16_t command_index);
        void AddEntry(const std::string& name, float value);
        void RemoveEntry(const std::string& name);
        void ChangeEntryName(const std::string& name, const std::string& new_name);
        void ChangeEntryValue(const std::string& name, float new_value);

        void SaveTotalToDatabase();
        void LoadTotalFromDatabase();
    private:
        float m_Total;
        sqlite3* m_Database;
};