#pragma once

#include "ExpenseTracker.h"

class ExpenseTrackerRenderer
{
public:
    static ExpenseTrackerRenderer &Get(ExpenseTracker &);
    ExpenseTrackerRenderer(const ExpenseTrackerRenderer &) = delete;
    ExpenseTrackerRenderer(ExpenseTrackerRenderer &&) = delete;

    void RenderStart();
    void RenderEnd();
    void RenderTable();
    void RenderEntries();
    void RenderMenuBar();
    void RenderAddWindow();
    void RenderRemoveWindow();
    void RenderChangeNameWindow();
    void RenderChangeValueWindow();

private:
    ExpenseTrackerRenderer(ExpenseTracker &tracker);

private:
    ExpenseTracker &m_Tracker;
};