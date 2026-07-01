#pragma once
#include <string>
#include <memory>

enum class LineType
{
    Text,
    Checklist,
    Contact
};

class Line
{
public:
    virtual ~Line() = default;

    virtual void Print() const = 0;

    virtual std::string Serialize() const = 0;

    virtual LineType GetType() const = 0;

    virtual std::unique_ptr<Line> Clone() const = 0;

    static std::unique_ptr<Line> Deserialize(const std::string& raw);
};
