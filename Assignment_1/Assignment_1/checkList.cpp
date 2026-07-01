#include "checkList.h"
#include <iostream>

ChecklistLine::ChecklistLine(const std::string& item, bool checked)
    : item(item), checked(checked)
{
}

void ChecklistLine::Print() const
{
    std::cout << "[ " << (checked ? "x" : " ") << " ] " << item << "\n";
}

std::string ChecklistLine::Serialize() const
{
    return std::string("CHECK|") + (checked ? "1" : "0") + "|" + item;
}

std::unique_ptr<Line> ChecklistLine::Clone() const
{
    return std::make_unique<ChecklistLine>(item, checked);
}
