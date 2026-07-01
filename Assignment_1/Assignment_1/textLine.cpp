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

    std::string escaped;
    escaped.reserve(text.size());
    for (char c : text)
    {
        if (c == '\n')      escaped += "\\n";
        else if (c == '\\') escaped += "\\\\";
        else                escaped += c;
    }
    return "TEXT|" + escaped;
}

std::unique_ptr<Line> TextLine::Clone() const
{
    return std::make_unique<TextLine>(text);
}

void TextLine::AppendText(const std::string& more)
{
    text += more;
}

void TextLine::DeleteChars(int index, int count)
{
    if (index < 0 || index >= static_cast<int>(text.size()) || count <= 0)
        return;

    int end = std::min(index + count, static_cast<int>(text.size()));
    text.erase(text.begin() + index, text.begin() + end);
}
