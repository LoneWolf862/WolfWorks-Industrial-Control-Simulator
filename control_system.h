#ifndef CONTROL_SYSTEM_H
#define CONTROL_SYSTEM_H

#include "io_point.h"

#include <memory>
#include <string>
#include <vector>

enum class SystemMode
{
    STOPPED,
    MANUAL,
    AUTO,
    FAULT
};

class ControlSystem
{
private:
    std::vector<std::shared_ptr<IOPoint>> ioPoints;

    SystemMode mode;
    bool running;
    bool fillRequest;
    unsigned long scanCount;

public:
    ControlSystem();

    void initialize();

    void displayAllIO() const;
    void displayStatus() const;
    void displayDiagnostics() const;

    void runScan();
    void simulateProcess();

    std::shared_ptr<IOPoint> findPoint(
        const std::string& tag) const;

        bool toggleDigitalInput(const std::string& tag);

        bool toggleDigitalOutput(const std::string& tag);

        bool setAnalogInput(
            const std::string& tag,
            float value);

        bool exportReport(
            const std::string& fileName) const;

            SystemMode getMode() const;
            void setMode(SystemMode newMode);

            bool isRunning() const;
            unsigned long getScanCount() const;
};

#endif
