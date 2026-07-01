#include "text.h"
#include "textLine.h"
#include "checkList.h"
#include "contactLine.h"
#include <iostream>
#include <sstream>
#include <algorithm>

std::vector<std::unique_ptr<Line>> Text::CloneRows(const std::vector<std::unique_ptr<Line>>& source) const
{
    std::vector<std::unique_ptr<Line>> copy;
    copy.reserve(source.size());
    for (const auto& row : source)
        copy.push_back(row->Clone());
    return copy;
}

void Text::SaveStateForUndo()
{
    if (static_cast<int>(undoStack.size()) >= MaxHistory)
        undoStack.erase(undoStack.begin());

    undoStack.push_back(CloneRows(rows));
    redoStack.clear();
}

void Text::ClampCursor()
{
    if (rows.empty())
    {
        cursor = { 0, 0 };
        return;
    }
    if (cursor.row >= static_cast<int>(rows.size()))
        cursor.row = static_cast<int>(rows.size()) - 1;
    if (cursor.row < 0)
        cursor.row = 0;

    if (rows[cursor.row]->GetType() != LineType::Text)
    {
        cursor.col = 0; 
    }
    else
    {
        int len = static_cast<TextLine*>(rows[cursor.row].get())->Length();
        cursor.col = std::min(std::max(cursor.col, 0), len);
    }
}

void Text::AppendTextLine(const std::string& text)
{
    SaveStateForUndo();
    rows.push_back(std::make_unique<TextLine>(text));
    cursor.row = static_cast<int>(rows.size()) - 1;
    cursor.col = static_cast<TextLine*>(rows[cursor.row].get())->Length();
    std::cout << "Text row added\n";
}

void Text::StartNewLine()
{
    SaveStateForUndo();
    rows.push_back(std::make_unique<TextLine>(""));
    cursor.row = static_cast<int>(rows.size()) - 1;
    cursor.col = 0;
    std::cout << "New line started\n";
}

void Text::AddChecklistItem(const std::string& item, bool checked)
{
    SaveStateForUndo();
    rows.push_back(std::make_unique<ChecklistLine>(item, checked));
    cursor.row = static_cast<int>(rows.size()) - 1;
    cursor.col = 0;
    std::cout << "Checklist item added\n";
}

void Text::AddContact(const std::string& name, const std::string& surname, const std::string& email)
{
    SaveStateForUndo();
    rows.push_back(std::make_unique<ContactLine>(name, surname, email));
    cursor.row = static_cast<int>(rows.size()) - 1;
    cursor.col = 0;
    std::cout << "Contact added\n";
}

void Text::InsertCharAtCursor(char c)
{
    if (rows.empty())
    {
        AppendTextLine("");
    }

    if (rows[cursor.row]->GetType() != LineType::Text)
    {
        std::cout << "Cursor is on a non-text row; cannot insert characters here\n";
        return;
    }

    SaveStateForUndo();
    auto* line = static_cast<TextLine*>(rows[cursor.row].get());
    line->InsertChar(cursor.col, c);
    cursor.col++;
    std::cout << "Character inserted\n";
}

void Text::DeleteAtCursor(int count)
{
    if (rows.empty())
    {
        std::cout << "Buffer is empty\n";
        return;
    }
    if (rows[cursor.row]->GetType() != LineType::Text)
    {
        std::cout << "Cursor is on a non-text row; cannot delete characters here\n";
        return;
    }
    if (count <= 0)
    {
        std::cout << "Invalid number of characters to delete\n";
        return;
    }

    SaveStateForUndo();
    auto* line = static_cast<TextLine*>(rows[cursor.row].get());
    int available = line->Length() - cursor.col;
    int toDelete = std::min(count, available);
    line->DeleteChars(cursor.col, toDelete);
    std::cout << "Deleted " << toDelete << " character(s)\n";
}

void Text::ToggleChecklistAtCursor()
{
    if (rows.empty() || rows[cursor.row]->GetType() != LineType::Checklist)
    {
        std::cout << "Cursor is not on a checklist row\n";
        return;
    }

    SaveStateForUndo();
    static_cast<ChecklistLine*>(rows[cursor.row].get())->Toggle();
    std::cout << "Checklist item toggled\n";
}

void Text::CutRows(int count)
{
    if (rows.empty() || count <= 0)
    {
        std::cout << "Nothing to cut\n";
        return;
    }

    SaveStateForUndo();
    int end = std::min(cursor.row + count, static_cast<int>(rows.size()));

    clipboard.clear();
    for (int i = cursor.row; i < end; i++)
        clipboard.push_back(rows[i]->Clone());

    rows.erase(rows.begin() + cursor.row, rows.begin() + end);
    ClampCursor();
    std::cout << "Cut " << clipboard.size() << " row(s)\n";
}

void Text::CopyRows(int count)
{
    if (rows.empty() || count <= 0)
    {
        std::cout << "Nothing to copy\n";
        return;
    }

    int end = std::min(cursor.row + count, static_cast<int>(rows.size()));
    clipboard.clear();
    for (int i = cursor.row; i < end; i++)
        clipboard.push_back(rows[i]->Clone());

    std::cout << "Copied " << clipboard.size() << " row(s)\n";
}

void Text::PasteRows()
{
    if (clipboard.empty())
    {
        std::cout << "Clipboard is empty\n";
        return;
    }

    SaveStateForUndo();
    int insertAt = rows.empty() ? 0 : cursor.row;

    for (size_t i = 0; i < clipboard.size(); i++)
        rows.insert(rows.begin() + insertAt + i, clipboard[i]->Clone());

    std::cout << "Pasted " << clipboard.size() << " row(s)\n";
}

bool Text::MoveCursor(int row, int col)
{
    if (row < 0 || row >= static_cast<int>(rows.size()))
    {
        std::cout << "Row does not exist\n";
        return false;
    }

    cursor.row = row;
    if (rows[row]->GetType() == LineType::Text)
    {
        int len = static_cast<TextLine*>(rows[row].get())->Length();
        if (col < 0 || col > len)
        {
            std::cout << "Column out of range\n";
            cursor.col = 0;
            return false;
        }
        cursor.col = col;
    }
    else
    {
        cursor.col = 0;
    }

    std::cout << "Cursor moved to Row " << row << ", Col " << cursor.col << "\n";
    return true;
}

void Text::DisplayCursor() const
{
    if (rows.empty())
    {
        std::cout << "Cursor is at the start (buffer is empty)\n";
        return;
    }
    std::cout << "Cursor at Row " << cursor.row << ", Col " << cursor.col << "\n";
}

void Text::Undo()
{
    if (undoStack.empty())
    {
        std::cout << "Nothing to undo\n";
        return;
    }

    redoStack.push_back(CloneRows(rows));
    rows = std::move(undoStack.back());
    undoStack.pop_back();
    ClampCursor();
    std::cout << "Undo executed\n";
}

void Text::Redo()
{
    if (redoStack.empty())
    {
        std::cout << "Nothing to redo\n";
        return;
    }

    undoStack.push_back(CloneRows(rows));
    rows = std::move(redoStack.back());
    redoStack.pop_back();
    ClampCursor();
    std::cout << "Redo executed\n";
}

void Text::SearchText(const std::string& needle) const
{
    bool found = false;
    for (int i = 0; i < static_cast<int>(rows.size()); i++)
    {
        if (rows[i]->Serialize().find(needle) != std::string::npos)
        {
            std::cout << "Found on row " << i << "\n";
            found = true;
        }
    }
    if (!found)
        std::cout << "Text not found\n";
}

void Text::Print() const
{
    if (rows.empty())
    {
        std::cout << "Buffer is empty\n";
        return;
    }
    for (const auto& row : rows)
        row->Print();
}

void Text::Clear()
{
    rows.clear();
    undoStack.clear();
    redoStack.clear();
    clipboard.clear();
    cursor = { 0, 0 };
}

std::string Text::ToPlainString() const
{
    std::ostringstream out;
    for (size_t i = 0; i < rows.size(); i++)
    {
        out << rows[i]->Serialize();
        if (i + 1 < rows.size())
            out << "\n";
    }
    return out.str();
}

void Text::LoadFromPlainString(const std::string& data)
{
    Clear();

    std::istringstream in(data);
    std::string line;
    while (std::getline(in, line))
    {
        if (line.empty())
            continue;
        rows.push_back(Line::Deserialize(line));
    }

    cursor = { 0, 0 };
}
