#ifndef ANALOG_INPUT_H
#define ANALOG_INPUT_H

#include "io_point.h"

#include <string>

class AnalogInput : public IOPoint
{
private:
    float value;
    float minimum;
    float maximum;
    std::string unit;

public:
    AnalogInput(int id,
                const std::string& tag,
                const std::string& description,
                const std::string& unit,
                float minimum,
                float maximum,
                float initialValue = 0.0f);

    void setValue(float newValue);

    float getValue() const;
    float getMinimum() const;
    float getMaximum() const;
    const std::string& getUnit() const;

    void display() const override;
};

#endif
