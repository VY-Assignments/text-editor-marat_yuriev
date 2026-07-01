#pragma once
#include <vector>
#include <memory>
#include <string>
#include "line.h"

struct Cursor
{
    int row = 0;
    int col = 0;
};

class Text
{
private:
    std::vector<std::unique_ptr<Line>> rows;
    Cursor cursor;

    std::vector<std::vector<std::unique_ptr<Line>>> undoStack;
    std::vector<std::vector<std::unique_ptr<Line>>> redoStack;
    std::vector<std::unique_ptr<Line>> clipboard;
    static const int MaxHistory = 5;

    std::vector<std::unique_ptr<Line>> CloneRows(const std::vector<std::unique_ptr<Line>>& source) const;
    void SaveStateForUndo();
    void ClampCursor();

public:
    Text() = default;

    void AppendTextLine(const std::string& text);
    void StartNewLine();
    void AddChecklistItem(const std::string& item, bool checked = false);
    void AddContact(const std::string& name, const std::string& surname, const std::string& email);

    void InsertCharAtCursor(char c);
    void DeleteAtCursor(int count);
    void ToggleChecklistAtCursor();

    void CutRows(int count);
    void CopyRows(int count);
    void PasteRows();

    bool MoveCursor(int row, int col);
    Cursor GetCursor() const { return cursor; }
    void DisplayCursor() const;

    void Undo();
    void Redo();

    void SearchText(const std::string& needle) const;
    void Print() const;
    void Clear();
    int RowCount() const { return static_cast<int>(rows.size()); }

    std::string ToPlainString() const;
    void LoadFromPlainString(const std::string& data);
};
