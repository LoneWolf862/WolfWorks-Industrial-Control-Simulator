#include "control_system.h"

#include "analog_input.h"
#include "digital_input.h"
#include "digital_output.h"

#include <fstream>
#include <iomanip>
#include <algorithm>
#include <iostream>
#include <memory>

ControlSystem::ControlSystem()
: mode(SystemMode::STOPPED),
running(false),
fillRequest(false),
scanCount(0)
{
}

void ControlSystem::initialize()
{
    ioPoints.clear();

    ioPoints.push_back(
        std::make_shared<DigitalInput>(
            0, "PB_START", "Start Pushbutton"));

    ioPoints.push_back(
        std::make_shared<DigitalInput>(
            1, "PB_STOP", "Stop Pushbutton"));

    ioPoints.push_back(
        std::make_shared<DigitalInput>(
            2, "ESTOP", "Emergency Stop"));

    ioPoints.push_back(
        std::make_shared<AnalogInput>(
            3, "LT_101", "Tank Level",
            "%", 0.0f, 100.0f, 43.5f));

    ioPoints.push_back(
        std::make_shared<AnalogInput>(
            4, "PT_101", "Tank Pressure",
            "PSI", 0.0f, 100.0f, 28.2f));

    ioPoints.push_back(
        std::make_shared<DigitalOutput>(
            5, "PUMP_101", "Fill Pump"));

    ioPoints.push_back(
        std::make_shared<DigitalOutput>(
            6, "XV_101", "Fill Valve"));

    ioPoints.push_back(
        std::make_shared<DigitalOutput>(
            7, "ALARM", "General Alarm"));
}

std::shared_ptr<IOPoint> ControlSystem::findPoint(
    const std::string& tag) const
    {
        auto it = std::find_if(
            ioPoints.begin(),
                               ioPoints.end(),
                               [&tag](const std::shared_ptr<IOPoint>& point)
                               {
                                   return point->getTag() == tag;
                               });

        if (it != ioPoints.end())
        {
            return *it;
        }

        return nullptr;
    }

    bool ControlSystem::toggleDigitalInput(const std::string& tag)
    {
        std::shared_ptr<IOPoint> point = findPoint(tag);

        std::shared_ptr<DigitalInput> input =
        std::dynamic_pointer_cast<DigitalInput>(point);

        if (input == nullptr)
        {
            return false;
        }

        input->toggle();
        return true;
    }

    bool ControlSystem::setAnalogInput(
        const std::string& tag,
        float value)
    {
        std::shared_ptr<IOPoint> point = findPoint(tag);

        std::shared_ptr<AnalogInput> input =
        std::dynamic_pointer_cast<AnalogInput>(point);

        if (input == nullptr)
        {
            return false;
        }

        input->setValue(value);
        return true;
    }

    void ControlSystem::runScan()
    {
        ++scanCount;

        auto startButton =
        std::dynamic_pointer_cast<DigitalInput>(
            findPoint("PB_START"));

        auto stopButton =
        std::dynamic_pointer_cast<DigitalInput>(
            findPoint("PB_STOP"));

        auto estop =
        std::dynamic_pointer_cast<DigitalInput>(
            findPoint("ESTOP"));

        auto tankLevel =
        std::dynamic_pointer_cast<AnalogInput>(
            findPoint("LT_101"));

        auto pump =
        std::dynamic_pointer_cast<DigitalOutput>(
            findPoint("PUMP_101"));

        auto fillValve =
        std::dynamic_pointer_cast<DigitalOutput>(
            findPoint("XV_101"));

        auto alarm =
        std::dynamic_pointer_cast<DigitalOutput>(
            findPoint("ALARM"));

        // Safety logic always has highest priority.
        if (estop->getState())
        {
            mode = SystemMode::FAULT;
            running = false;
            fillRequest = false;

            pump->setState(false);
            fillValve->setState(false);
            alarm->setState(true);

            return;
        }

        // Clear the alarm when the emergency stop is no longer active.
        alarm->setState(false);

        // Stop pushbutton.
        if (stopButton->getState())
        {
            running = false;
            fillRequest = false;

            pump->setState(false);
            fillValve->setState(false);

            // Treat both pushbuttons as momentary inputs.
            stopButton->setState(false);
            startButton->setState(false);

            return;
        }
        // Start pushbutton.
        if (startButton->getState())
        {
            if (mode != SystemMode::STOPPED &&
                mode != SystemMode::FAULT)
            {
                running = true;
            }

            startButton->setState(false);
        }

        if (mode == SystemMode::STOPPED)
        {
            running = false;
            fillRequest = false;

            pump->setState(false);
            fillValve->setState(false);

            return;
        }

        if (mode == SystemMode::FAULT)
        {
            running = false;
            fillRequest = false;

            pump->setState(false);
            fillValve->setState(false);

            return;
        }

        // AUTO mode tank control.
        if (mode == SystemMode::AUTO && running)
        {
            if (tankLevel->getValue() <= 20.0f)
            {
                fillRequest = true;
            }
            else if (tankLevel->getValue() >= 80.0f)
            {
                fillRequest = false;
            }

            pump->setState(fillRequest);
            fillValve->setState(fillRequest);
        }

        // In MANUAL mode, the operator directly controls
        // the pump and valve. The scan leaves their states unchanged.
        if (mode == SystemMode::MANUAL && running)
        {
            return;
        }
    }

    void ControlSystem::simulateProcess()
    {
        if (!running)
        {
            return;
        }

        auto tankLevel =
        std::dynamic_pointer_cast<AnalogInput>(
            findPoint("LT_101"));

        auto pressure =
        std::dynamic_pointer_cast<AnalogInput>(
            findPoint("PT_101"));

        auto pump =
        std::dynamic_pointer_cast<DigitalOutput>(
            findPoint("PUMP_101"));

        auto fillValve =
        std::dynamic_pointer_cast<DigitalOutput>(
            findPoint("XV_101"));

        if (tankLevel == nullptr ||
            pressure == nullptr ||
            pump == nullptr ||
            fillValve == nullptr)
        {
            return;
        }

        float level = tankLevel->getValue();

        if (pump->getState() && fillValve->getState())
        {
            level += 5.0f;
        }
        else
        {
            level -= 2.0f;
        }

        tankLevel->setValue(level);

        float simulatedPressure =
        10.0f + (tankLevel->getValue() * 0.4f);

        pressure->setValue(simulatedPressure);
    }

    void ControlSystem::displayAllIO() const
    {
        std::cout << "\n--------------- ALL I/O ----------------\n";

        std::for_each(
            ioPoints.begin(),
                      ioPoints.end(),
                      [](const std::shared_ptr<IOPoint>& point)
                      {
                          point->display();
                      });

        std::cout << "----------------------------------------\n";
    }

    void ControlSystem::displayStatus() const
    {
        std::cout << "\n============= SYSTEM STATUS ============\n";

        std::cout << "Mode:       ";

        switch (mode)
        {
            case SystemMode::STOPPED:
                std::cout << "STOPPED";
                break;

            case SystemMode::MANUAL:
                std::cout << "MANUAL";
                break;

            case SystemMode::AUTO:
                std::cout << "AUTO";
                break;

            case SystemMode::FAULT:
                std::cout << "FAULT";
                break;
        }

        std::cout << '\n';

        std::cout << "State:      "
        << (running ? "RUNNING" : "STOPPED")
        << '\n';

        std::cout << "Scan Count: "
        << scanCount
        << '\n';

        std::cout << "I/O Points: "
        << ioPoints.size()
        << '\n';

        std::cout << "========================================\n";
    }

    SystemMode ControlSystem::getMode() const
    {
        return mode;
    }

    void ControlSystem::setMode(SystemMode newMode)
    {
        // A fault cannot be cleared while the emergency stop is active.
        if (mode == SystemMode::FAULT)
        {
            auto estop =
            std::dynamic_pointer_cast<DigitalInput>(
                findPoint("ESTOP"));

            if (estop != nullptr && estop->getState())
            {
                std::cout
                << "Cannot reset FAULT: Emergency Stop is active.\n";
                return;
            }

            // Fault recovery must pass through STOPPED.
            if (newMode != SystemMode::STOPPED)
            {
                std::cout
                << "FAULT must be reset to STOPPED before "
                << "another mode can be selected.\n";
                return;
            }
        }

        mode = newMode;

        // Changing operating modes does not automatically start
        // the process. The START pushbutton controls the run state.
        running = false;
        fillRequest = false;

        auto pump =
        std::dynamic_pointer_cast<DigitalOutput>(
            findPoint("PUMP_101"));

        auto fillValve =
        std::dynamic_pointer_cast<DigitalOutput>(
            findPoint("XV_101"));

        if (pump != nullptr)
        {
            pump->setState(false);
        }

        if (fillValve != nullptr)
        {
            fillValve->setState(false);
        }
    }

    bool ControlSystem::isRunning() const
    {
        return running;
    }

    unsigned long ControlSystem::getScanCount() const
    {
        return scanCount;
    }

    bool ControlSystem::toggleDigitalOutput(const std::string& tag)
    {
        if (mode != SystemMode::MANUAL)
        {
            std::cout
            << "Outputs can only be manually controlled "
            << "in MANUAL mode.\n";

            return false;
        }

        if (!running)
        {
            std::cout
            << "Manual control is not enabled. "
            << "Press START and run a control scan first.\n";

            return false;
        }

        std::shared_ptr<IOPoint> point = findPoint(tag);

        std::shared_ptr<DigitalOutput> output =
        std::dynamic_pointer_cast<DigitalOutput>(point);

        if (output == nullptr)
        {
            return false;
        }

        // The alarm is controlled by the safety logic,
        // not manually by the operator.
        if (tag == "ALARM")
        {
            std::cout
            << "ALARM is controlled by the safety system.\n";

            return false;
        }

        output->setState(!output->getState());

        return true;
    }

    bool ControlSystem::exportReport(
        const std::string& fileName) const
        {
            std::ofstream file(fileName);

            if (!file)
            {
                return false;
            }

            file << "WolfWorks Industrial Control Simulator\n";
            file << "System Report\n";
            file << "========================================\n\n";

            file << "Mode: ";

            switch (mode)
            {
                case SystemMode::STOPPED:
                    file << "STOPPED";
                    break;

                case SystemMode::MANUAL:
                    file << "MANUAL";
                    break;

                case SystemMode::AUTO:
                    file << "AUTO";
                    break;

                case SystemMode::FAULT:
                    file << "FAULT";
                    break;
            }

            file << '\n';

            file << "State: "
            << (running ? "RUNNING" : "STOPPED")
            << '\n';

            file << "Scan Count: "
            << scanCount
            << "\n\n";

            file << "I/O POINTS\n";
            file << "----------------------------------------\n";

            for (const auto& point : ioPoints)
            {
                file << "ID: " << point->getID()
                << "  Tag: " << point->getTag()
                << "  Description: "
                << point->getDescription();

                if (auto input =
                    std::dynamic_pointer_cast<DigitalInput>(point))
                {
                    file << "  State: "
                    << (input->getState() ? "ON" : "OFF");
                }
                else if (auto analog =
                    std::dynamic_pointer_cast<AnalogInput>(point))
                {
                    file << "  Value: "
                    << std::fixed << std::setprecision(1)
                    << analog->getValue()
                    << ' '
                    << analog->getUnit();
                }
                else if (auto output =
                    std::dynamic_pointer_cast<DigitalOutput>(point))
                {
                    file << "  State: "
                    << (output->getState() ? "ON" : "OFF");
                }

                file << '\n';
            }

            return true;
        }

    void ControlSystem::displayDiagnostics() const
    {
        int digitalInputs = 0;
        int analogInputs = 0;
        int digitalOutputs = 0;
        int activeDigitalInputs = 0;
        int activeDigitalOutputs = 0;

        for (const auto& point : ioPoints)
        {
            if (auto input =
                std::dynamic_pointer_cast<DigitalInput>(point))
            {
                ++digitalInputs;

                if (input->getState())
                {
                    ++activeDigitalInputs;
                }
            }
            else if (std::dynamic_pointer_cast<AnalogInput>(point))
            {
                ++analogInputs;
            }
            else if (auto output =
                std::dynamic_pointer_cast<DigitalOutput>(point))
            {
                ++digitalOutputs;

                if (output->getState())
                {
                    ++activeDigitalOutputs;
                }
            }
        }

        std::cout << "\n=============== DIAGNOSTICS ===============\n";
        std::cout << "Total I/O Points:       " << ioPoints.size() << '\n';
        std::cout << "Digital Inputs:         " << digitalInputs << '\n';
        std::cout << "Analog Inputs:          " << analogInputs << '\n';
        std::cout << "Digital Outputs:        " << digitalOutputs << '\n';
        std::cout << "Active Digital Inputs:  "
        << activeDigitalInputs << '\n';
        std::cout << "Active Digital Outputs: "
        << activeDigitalOutputs << '\n';
        std::cout << "Total Control Scans:    " << scanCount << '\n';

        std::cout << "Controller Fault:       "
        << (mode == SystemMode::FAULT ? "YES" : "NO")
        << '\n';

        std::cout << "===========================================\n";
    }
