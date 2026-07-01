#include "contactLine.h"
#include <iostream>

ContactLine::ContactLine(const std::string& name, const std::string& surname, const std::string& email)
    : name(name), surname(surname), email(email)
{
}

void ContactLine::Print() const
{
    std::cout << "Contact - " << name << " " << surname << ", E-mail: " << email << "\n";
}

std::string ContactLine::Serialize() const
{
    return "CONTACT|" + name + "|" + surname + "|" + email;
}

std::unique_ptr<Line> ContactLine::Clone() const
{
    return std::make_unique<ContactLine>(name, surname, email);
}
