#include "digital_output.h"

#include <iomanip>
#include <iostream>

DigitalOutput::DigitalOutput(int id,
                             const std::string& tag,
                             const std::string& description,
                             bool initialState)
: IOPoint(id, tag, description),
state(initialState)
{
}

void DigitalOutput::setState(bool newState)
{
    state = newState;
}

bool DigitalOutput::getState() const
{
    return state;
}

void DigitalOutput::display() const
{
    std::cout << std::left
    << std::setw(5) << getID()
    << std::setw(16) << getTag()
    << std::setw(28) << getDescription()
    << (state ? "ON" : "OFF")
    << '\n';
}
