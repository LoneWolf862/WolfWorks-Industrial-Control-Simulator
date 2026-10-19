#ifndef DIGITAL_OUTPUT_H
#define DIGITAL_OUTPUT_H

#include "io_point.h"

class DigitalOutput : public IOPoint
{
private:
    bool state;

public:
    DigitalOutput(int id,
                  const std::string& tag,
                  const std::string& description,
                  bool initialState = false);

    void setState(bool newState);
    bool getState() const;

    void display() const override;
};

#endif
