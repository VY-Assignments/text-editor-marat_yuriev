#pragma once
#include "line.h"


class ContactLine : public Line
{
private:
    std::string name;
    std::string surname;
    std::string email;

public:
    ContactLine(const std::string& name, const std::string& surname, const std::string& email);

    void Print() const override;
    std::string Serialize() const override;
    LineType GetType() const override { return LineType::Contact; }
    std::unique_ptr<Line> Clone() const override;

    const std::string& GetName() const { return name; }
    const std::string& GetSurname() const { return surname; }
    const std::string& GetEmail() const { return email; }
};
