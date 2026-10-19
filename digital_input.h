#ifndef DIGITAL_INPUT_H
#define DIGITAL_INPUT_H

#include "io_point.h"

class DigitalInput : public IOPoint
{
private:
    bool state;

public:
    DigitalInput(int id,
                 const std::string& tag,
                 const std::string& description,
                 bool initialState = false);

    void setState(bool newState);
    void toggle();

    bool getState() const;

    void display() const override;
};

#endif
