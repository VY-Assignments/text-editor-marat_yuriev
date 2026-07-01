#pragma once
#include "line.h"

class ChecklistLine : public Line
{
private:
    std::string item;
    bool checked;

public:
    explicit ChecklistLine(const std::string& item, bool checked = false);

    void Print() const override;
    std::string Serialize() const override;
    LineType GetType() const override { return LineType::Checklist; }
    std::unique_ptr<Line> Clone() const override;

    void Toggle() { checked = !checked; }
    bool IsChecked() const { return checked; }
    const std::string& GetItem() const { return item; }
};
