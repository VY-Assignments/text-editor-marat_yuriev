#include "Line.h"
#include "textLine.h"
#include "checkList.h"
#include "contactLine.h"
#include <stdexcept>

namespace
{
    std::string ExtractTag(const std::string& raw, std::string& rest)
    {
        size_t pos = raw.find('|');
        if (pos == std::string::npos)
        {
            rest.clear();
            return raw;
        }
        rest = raw.substr(pos + 1);
        return raw.substr(0, pos);
    }

    std::string TakeField(std::string& rest)
    {
        size_t pos = rest.find('|');
        if (pos == std::string::npos)
        {
            std::string field = rest;
            rest.clear();
            return field;
        }
        std::string field = rest.substr(0, pos);
        rest = rest.substr(pos + 1);
        return field;
    }
}

std::unique_ptr<Line> Line::Deserialize(const std::string& raw)
{
    std::string rest;
    std::string tag = ExtractTag(raw, rest);

    if (tag == "TEXT")
    {
        return std::make_unique<TextLine>(rest);
    }
    if (tag == "CHECK")
    {
        std::string checkedFlag = TakeField(rest);
        std::string item = rest;
        return std::make_unique<ChecklistLine>(item, checkedFlag == "1");
    }
    if (tag == "CONTACT")
    {
        std::string name = TakeField(rest);
        std::string surname = TakeField(rest);
        std::string email = rest;
        return std::make_unique<ContactLine>(name, surname, email);
    }

    throw std::runtime_error("Unknown row tag while deserializing: " + tag);
}
