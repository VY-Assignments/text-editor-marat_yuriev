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

void Text::AppendText(const std::string& textToAdd)
{
    SaveStateForUndo();

    if (canContinueTextBlock && !rows.empty() && rows.back()->GetType() == LineType::Text)
    {
        static_cast<TextLine*>(rows.back().get())->AppendText(textToAdd);
    }
    else
    {
        rows.push_back(std::make_unique<TextLine>(textToAdd));
    }

    canContinueTextBlock = true;
    std::cout << "Text appended\n";
}

void Text::StartNewLine()
{
    SaveStateForUndo();

    if (!rows.empty() && rows.back()->GetType() == LineType::Text)
    {
        static_cast<TextLine*>(rows.back().get())->AppendText("\n");
    }
    else
    {
        rows.push_back(std::make_unique<TextLine>(""));
    }

    canContinueTextBlock = false;
    std::cout << "New line started\n";
}

void Text::AddChecklistItem(const std::string& item, bool checked)
{
    SaveStateForUndo();
    rows.push_back(std::make_unique<ChecklistLine>(item, checked));
    std::cout << "Checklist item added\n";
}

void Text::AddContact(const std::string& name, const std::string& surname, const std::string& email)
{
    SaveStateForUndo();
    rows.push_back(std::make_unique<ContactLine>(name, surname, email));
    std::cout << "Contact added\n";
}

void Text::DeleteLastChars(int count)
{
    if (rows.empty() || rows.back()->GetType() != LineType::Text)
    {
        std::cout << "Last row is not text; nothing to delete\n";
        return;
    }
    if (count <= 0)
    {
        std::cout << "Invalid number of characters to delete\n";
        return;
    }

    SaveStateForUndo();
    auto* line = static_cast<TextLine*>(rows.back().get());
    int len = line->Length();
    int toDelete = std::min(count, len);
    line->DeleteChars(len - toDelete, toDelete);
    std::cout << "Deleted " << toDelete << " character(s)\n";
}

void Text::ToggleChecklistItem(int rowIndex)
{
    if (rowIndex < 0 || rowIndex >= static_cast<int>(rows.size()))
    {
        std::cout << "Row does not exist\n";
        return;
    }
    if (rows[rowIndex]->GetType() != LineType::Checklist)
    {
        std::cout << "Row " << rowIndex << " is not a checklist item\n";
        return;
    }

    SaveStateForUndo();
    static_cast<ChecklistLine*>(rows[rowIndex].get())->Toggle();
    std::cout << "Checklist item toggled\n";
}

void Text::DeleteRow(int rowIndex)
{
    if (rowIndex < 0 || rowIndex >= static_cast<int>(rows.size()))
    {
        std::cout << "Row does not exist\n";
        return;
    }

    SaveStateForUndo();
    rows.erase(rows.begin() + rowIndex);
    std::cout << "Row " << rowIndex << " deleted\n";
}

void Text::CutRows(int startRow, int count)
{
    if (rows.empty())
    {
        std::cout << "Nothing to cut\n";
        return;
    }
    if (startRow < 0 || startRow >= static_cast<int>(rows.size()) || count <= 0)
    {
        std::cout << "Invalid row range\n";
        return;
    }

    SaveStateForUndo();
    int end = std::min(startRow + count, static_cast<int>(rows.size()));

    clipboard.clear();
    for (int i = startRow; i < end; i++)
        clipboard.push_back(rows[i]->Clone());

    rows.erase(rows.begin() + startRow, rows.begin() + end);
    std::cout << "Cut " << clipboard.size() << " row(s)\n";
}

void Text::CopyRows(int startRow, int count)
{
    if (rows.empty())
    {
        std::cout << "Nothing to copy\n";
        return;
    }
    if (startRow < 0 || startRow >= static_cast<int>(rows.size()) || count <= 0)
    {
        std::cout << "Invalid row range\n";
        return;
    }

    int end = std::min(startRow + count, static_cast<int>(rows.size()));
    clipboard.clear();
    for (int i = startRow; i < end; i++)
        clipboard.push_back(rows[i]->Clone());

    std::cout << "Copied " << clipboard.size() << " row(s)\n";
}

void Text::PasteRows(int insertAtRow)
{
    if (clipboard.empty())
    {
        std::cout << "Clipboard is empty\n";
        return;
    }

    int insertAt = insertAtRow;
    if (insertAt < 0 || insertAt > static_cast<int>(rows.size()))
        insertAt = static_cast<int>(rows.size()); 

    SaveStateForUndo();
    for (size_t i = 0; i < clipboard.size(); i++)
        rows.insert(rows.begin() + insertAt + i, clipboard[i]->Clone());

    std::cout << "Pasted " << clipboard.size() << " row(s)\n";
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
}