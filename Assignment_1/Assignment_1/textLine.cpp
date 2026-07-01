#include "textLine.h"
#include <iostream>
#include <algorithm>

TextLine::TextLine(const std::string& initialText)
    : text(initialText)
{
}

void TextLine::Print() const
{
    std::cout << "Text: " << text << "\n";
}

std::string TextLine::Serialize() const
{
    return "TEXT|" + text;
}

std::unique_ptr<Line> TextLine::Clone() const
{
    return std::make_unique<TextLine>(text);
}

void TextLine::InsertChar(int index, char c)
{
    if (index < 0) index = 0;
    if (index > static_cast<int>(text.size())) index = static_cast<int>(text.size());
    text.insert(text.begin() + index, c);
}

void TextLine::DeleteChars(int index, int count)
{
    if (index < 0 || index >= static_cast<int>(text.size()) || count <= 0)
        return;

    int end = std::min(index + count, static_cast<int>(text.size()));
    text.erase(text.begin() + index, text.begin() + end);
}
