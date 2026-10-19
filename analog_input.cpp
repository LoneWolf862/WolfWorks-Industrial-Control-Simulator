#include "analog_input.h"

#include <iomanip>
#include <iostream>

AnalogInput::AnalogInput(int id,
                         const std::string& tag,
                         const std::string& description,
                         const std::string& unit,
                         float minimum,
                         float maximum,
                         float initialValue)
: IOPoint(id, tag, description),
value(initialValue),
minimum(minimum),
maximum(maximum),
unit(unit)
{
}

void AnalogInput::setValue(float newValue)
{
    if (newValue < minimum)
    {
        value = minimum;
    }
    else if (newValue > maximum)
    {
        value = maximum;
    }
    else
    {
        value = newValue;
    }
}

float AnalogInput::getValue() const
{
    return value;
}

float AnalogInput::getMinimum() const
{
    return minimum;
}

float AnalogInput::getMaximum() const
{
    return maximum;
}

const std::string& AnalogInput::getUnit() const
{
    return unit;
}

void AnalogInput::display() const
{
    std::cout << std::left
    << std::setw(5) << getID()
    << std::setw(16) << getTag()
    << std::setw(28) << getDescription()
    << std::fixed << std::setprecision(1)
    << value << ' ' << unit
    << '\n';
}
