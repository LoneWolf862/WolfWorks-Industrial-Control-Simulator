#include "control_system.h"

#include <iostream>
#include <limits>
#include <string>

void clearBadInput()
{
    std::cin.clear();
    std::cin.ignore(
        std::numeric_limits<std::streamsize>::max(),
                    '\n');
}

void displayMenu(const ControlSystem& controller)
{
    std::cout << "\n======================================================\n";
    std::cout << "       WOLFWORKS INDUSTRIAL CONTROL SIMULATOR\n";
    std::cout << "======================================================\n";
    std::cout << "Scans: " << controller.getScanCount() << '\n';
    std::cout << "\n";
    std::cout << "1. Display all I/O\n";
    std::cout << "2. Toggle digital input\n";
    std::cout << "3. Set analog input\n";
    std::cout << "4. Search I/O by tag\n";
    std::cout << "5. Change operating mode\n";
    std::cout << "6. Run one control scan\n";
    std::cout << "7. Run automatic simulation\n";
    std::cout << "8. Display diagnostics\n";
    std::cout << "9. Export system report\n";
    std::cout << "10. Toggle manual output\n";
    std::cout << "0. Exit\n";
    std::cout << "\nSelection: ";
}

int main()
{
    ControlSystem controller;
    controller.initialize();

    int choice = -1;

    while (choice != 0)
    {
        displayMenu(controller);

        if (!(std::cin >> choice))
        {
            std::cout << "Invalid input. Please enter a number.\n";
            clearBadInput();
            continue;
        }

        switch (choice)
        {
            case 0:
            {
                std::cout << "Exiting simulator.\n";
                break;
            }

            case 1:
            {
                controller.displayStatus();
                controller.displayAllIO();
                break;
            }

            case 2:
            {
                std::string tag;

                std::cout << "Enter digital input tag: ";
                std::cin >> tag;

                if (controller.toggleDigitalInput(tag))
                {
                    std::cout << tag << " toggled successfully.\n";
                }
                else
                {
                    std::cout
                    << "Digital input not found: "
                    << tag << '\n';
                }

                break;
            }

            case 3:
            {
                std::string tag;
                float value;

                std::cout << "Enter analog input tag: ";
                std::cin >> tag;

                std::cout << "Enter new value: ";

                if (!(std::cin >> value))
                {
                    std::cout << "Invalid numeric value.\n";
                    clearBadInput();
                    break;
                }

                if (controller.setAnalogInput(tag, value))
                {
                    std::cout << tag
                    << " updated successfully.\n";
                }
                else
                {
                    std::cout
                    << "Analog input not found: "
                    << tag << '\n';
                }

                break;
            }

            case 4:
            {
                std::string tag;

                std::cout << "Enter I/O tag: ";
                std::cin >> tag;

                std::shared_ptr<IOPoint> point =
                controller.findPoint(tag);

                if (point != nullptr)
                {
                    std::cout << "Found: ";
                    point->display();
                }
                else
                {
                    std::cout
                    << "I/O point not found: "
                    << tag << '\n';
                }

                break;
            }

            case 5:
            {
                int modeChoice;

                std::cout << "\nOperating Modes:\n";
                std::cout << "0. STOPPED\n";
                std::cout << "1. MANUAL\n";
                std::cout << "2. AUTO\n";
                std::cout << "Select mode: ";

                if (!(std::cin >> modeChoice))
                {
                    std::cout << "Invalid mode.\n";
                    clearBadInput();
                    break;
                }

                switch (modeChoice)
                {
                    case 0:
                        controller.setMode(SystemMode::STOPPED);
                        break;

                    case 1:
                        controller.setMode(SystemMode::MANUAL);
                        break;

                    case 2:
                        controller.setMode(SystemMode::AUTO);
                        break;

                    default:
                        std::cout << "Invalid mode selection.\n";
                        break;
                }

                controller.displayStatus();
                break;
            }

                    case 6:
                    {
                        controller.runScan();
                        controller.simulateProcess();

                        std::cout << "Control scan completed.\n";
                        controller.displayStatus();

                        break;
                    }

                    case 7:
                    {
                        int scans;

                        std::cout << "Number of scans to simulate: ";

                        if (!(std::cin >> scans) || scans <= 0)
                        {
                            std::cout << "Invalid scan count.\n";
                            clearBadInput();
                            break;
                        }

                        for (int scan = 1; scan <= scans; ++scan)
                        {
                            controller.runScan();
                            controller.simulateProcess();

                            std::cout << "\nSimulation Scan "
                            << scan << ":\n";

                            std::shared_ptr<IOPoint> level =
                            controller.findPoint("LT_101");

                            std::shared_ptr<IOPoint> pump =
                            controller.findPoint("PUMP_101");

                            std::shared_ptr<IOPoint> valve =
                            controller.findPoint("XV_101");

                            if (level != nullptr)
                            {
                                level->display();
                            }

                            if (pump != nullptr)
                            {
                                pump->display();
                            }

                            if (valve != nullptr)
                            {
                                valve->display();
                            }
                        }

                        break;
                    }

                    case 8:
                    {
                        controller.displayDiagnostics();
                        break;
                    }

                    case 9:
                    {
                        std::string fileName;

                        std::cout << "Report filename: ";
                        std::cin >> fileName;

                        if (controller.exportReport(fileName))
                        {
                            std::cout
                            << "Report exported to "
                            << fileName << '\n';
                        }
                        else
                        {
                            std::cout << "Unable to create report.\n";
                        }

                        break;
                    }

                    case 10:
                    {
                        std::string tag;

                        std::cout << "Enter digital output tag: ";
                        std::cin >> tag;

                        if (controller.toggleDigitalOutput(tag))
                        {
                            std::cout
                            << tag
                            << " toggled successfully.\n";
                        }
                        else
                        {
                            std::cout
                            << "Unable to toggle output: "
                            << tag << '\n';
                        }

                        break;
                    }

                    default:
                    {
                        std::cout << "Invalid menu selection.\n";
                        break;
                    }
        }
    }

    return 0;
}
