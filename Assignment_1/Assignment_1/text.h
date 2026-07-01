#pragma once
#include <vector>
#include <memory>
#include <string>
#include "line.h"

class Text
{
private:
    std::vector<std::unique_ptr<Line>> rows;

    std::vector<std::vector<std::unique_ptr<Line>>> undoStack;
    std::vector<std::vector<std::unique_ptr<Line>>> redoStack;
    std::vector<std::unique_ptr<Line>> clipboard;
    static const int MaxHistory = 5;

    std::vector<std::unique_ptr<Line>> CloneRows(const std::vector<std::unique_ptr<Line>>& source) const;
    void SaveStateForUndo();

public:
    Text() = default;

    void AppendText(const std::string& text);
    void StartNewLine();

    void AddChecklistItem(const std::string& item, bool checked = false);
    void AddContact(const std::string& name, const std::string& surname, const std::string& email);

    void DeleteLastChars(int count);

    void ToggleChecklistItem(int rowIndex);
    void DeleteRow(int rowIndex);

    void CutRows(int startRow, int count);
    void CopyRows(int startRow, int count);
    void PasteRows(int insertAtRow);

    void Undo();
    void Redo();

    void SearchText(const std::string& needle) const;
    void Print() const;
    void Clear();
    int RowCount() const { return static_cast<int>(rows.size()); }

    std::string ToPlainString() const;
    void LoadFromPlainString(const std::string& data);
};
