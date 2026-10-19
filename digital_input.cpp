#include "digital_input.h"

#include <iomanip>
#include <iostream>

DigitalInput::DigitalInput(int id,
                           const std::string& tag,
                           const std::string& description,
                           bool initialState)
: IOPoint(id, tag, description),
state(initialState)
{
}

void DigitalInput::setState(bool newState)
{
    state = newState;
}

void DigitalInput::toggle()
{
    state = !state;
}

bool DigitalInput::getState() const
{
    return state;
}

void DigitalInput::display() const
{
    std::cout << std::left
    << std::setw(5) << getID()
    << std::setw(16) << getTag()
    << std::setw(28) << getDescription()
    << (state ? "ON" : "OFF")
    << '\n';
}
