#include "ExpenseTrackerRenderer.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_glfw.h"
#include "ImGui/imgui_impl_opengl3.h"

ExpenseTrackerRenderer::ExpenseTrackerRenderer(ExpenseTracker &tracker)
    : m_Tracker(tracker)
{
}

ExpenseTrackerRenderer &ExpenseTrackerRenderer::Get(ExpenseTracker &tracker)
{
    static ExpenseTrackerRenderer instance(tracker);
    return instance;
}

void ExpenseTrackerRenderer::RenderStart()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImVec2 windowSize = ImVec2{static_cast<f32>(m_Tracker.GetWidth()), static_cast<f32>(m_Tracker.GetHeight())};
    auto flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                 ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar;
    ImGui::Begin("Expenses", 0, flags);
    ImGui::SetWindowSize(windowSize);
    ImGui::SetWindowPos(ImVec2{0.0f, 0.0f});
}

void ExpenseTrackerRenderer::RenderEnd()
{
    ImGui::End();
    ImGui::Render();
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(m_Tracker.GetWindow());
}

void ExpenseTrackerRenderer::RenderTable()
{
    constexpr i32 col_num = 3;
    if (ImGui::BeginTable("Expenses", col_num, ImGuiTableFlags_Borders))
    {
        RenderEntries();
        ImGui::EndTable();
    }
}

void ExpenseTrackerRenderer::RenderEntries()
{
    for (const std::string &header : m_Tracker.GetTableStrings())
    {
        ImGui::TableNextColumn();
        ImGui::Text(header.c_str());
    }

    std::string selectQuery = "SELECT * FROM Expenses";
    sqlite3_stmt *statement = nullptr;
    i32 result = sqlite3_prepare_v2(m_Tracker.GetDatabase(), selectQuery.c_str(), -1, &statement, nullptr);
    if (result != SQLITE_OK)
    {
        std::cerr << "Error preparing select statement: " << sqlite3_errmsg(m_Tracker.GetDatabase()) << '\n';
    }

    while (sqlite3_step(statement) == SQLITE_ROW)
    {
        for (i32 column = 0; column < m_Tracker.GetTableStrings().size(); column++)
        {
            ImGui::TableNextColumn();
            if (column == 0)
            {
                i32 id = sqlite3_column_int(statement, column);
                ImGui::Text("%d", id);
            }
            else if (column == 1)
            {
                const char *name = (const char *)sqlite3_column_text(statement, column);
                ImGui::Text(name);
            }
            else if (column == 2)
            {
                double value = sqlite3_column_double(statement, column);
                ImGui::Text("%.2f", value);
            }
        }
    }
    sqlite3_finalize(statement);
}

void ExpenseTrackerRenderer::RenderMenuBar()
{
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Add"))
        {
            RenderAddWindow();
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Remove"))
        {
            RenderRemoveWindow();
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Change Name"))
        {
            RenderChangeNameWindow();
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Change Value"))
        {
            RenderChangeValueWindow();
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
}

void ExpenseTrackerRenderer::RenderAddWindow()
{
    auto flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse;
    if (ImGui::Begin("Add", nullptr, flags))
    {
        ImGui::InputText("Name", m_Tracker.GetTempBuffer(), 256);
        ImGui::InputFloat("Value", &m_Tracker.GetTempValue(), 0.0f, 0.0f, "%.1f");
        if (ImGui::Button("Add"))
        {
            m_Tracker.AddEntry(std::string(m_Tracker.GetTempBuffer()), m_Tracker.GetTempValue());
            memset(m_Tracker.GetTempBuffer(), 0, 256);
            m_Tracker.GetTempValue() = 0.0f;
        }
        ImGui::End();
    }
}

void ExpenseTrackerRenderer::RenderRemoveWindow()
{
    auto flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse;
    if (ImGui::Begin("Remove", nullptr, flags))
    {
        ImGui::InputText("Name", m_Tracker.GetTempBuffer(), 256);
        if (ImGui::Button("Remove"))
        {
            m_Tracker.RemoveEntry(m_Tracker.GetTempBuffer());
            memset(m_Tracker.GetTempBuffer(), 0, 256);
        }
        ImGui::End();
    }
}

static char new_name[256] = {};
void ExpenseTrackerRenderer::RenderChangeNameWindow()
{
    auto flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse;
    if (ImGui::Begin("Remove", nullptr, flags))
    {
        ImGui::InputText("Name", m_Tracker.GetTempBuffer(), 256);
        ImGui::InputText("New Name", new_name, 256);
        if (ImGui::Button("Change Name"))
        {
            m_Tracker.ChangeEntryName(m_Tracker.GetTempBuffer(), new_name);
            memset(m_Tracker.GetTempBuffer(), 0, 256);
        }
        ImGui::End();
    }
}

void ExpenseTrackerRenderer::RenderChangeValueWindow()
{
    auto flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse;
    if (ImGui::Begin("Change Value", nullptr, flags))
    {
        ImGui::InputText("Name", m_Tracker.GetTempBuffer(), 256);
        ImGui::InputFloat("New Value", &m_Tracker.GetTempValue(), 0.0f, 0.0f, "%.1f");
        if (ImGui::Button("Change Value"))
        {
            m_Tracker.ChangeEntryValue(m_Tracker.GetTempBuffer(), m_Tracker.GetTempValue());
            m_Tracker.GetTempValue() = 0.0f;
            memset(m_Tracker.GetTempBuffer(), 0, 256);
        }
        ImGui::End();
    }
}
