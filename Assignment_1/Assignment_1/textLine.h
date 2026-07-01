#pragma once
#include "line.h"

class TextLine : public Line
{
private:
    std::string text;

public:
    explicit TextLine(const std::string& initialText = "");

    void Print() const override;
    std::string Serialize() const override;
    LineType GetType() const override { return LineType::Text; }
    std::unique_ptr<Line> Clone() const override;

    const std::string& GetText() const { return text; }

    void InsertChar(int index, char c);
    void DeleteChars(int index, int count);
    int Length() const { return static_cast<int>(text.size()); }
};
