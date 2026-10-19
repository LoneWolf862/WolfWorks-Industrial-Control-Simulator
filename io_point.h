#ifndef IO_POINT_H
#define IO_POINT_H

#include <string>

class IOPoint
{
private:
    int id;
    std::string tag;
    std::string description;

public:
    IOPoint(int id,
            const std::string& tag,
            const std::string& description);

    virtual ~IOPoint() = default;

    int getID() const;
    const std::string& getTag() const;
    const std::string& getDescription() const;

    virtual void display() const = 0;
};

#endif
