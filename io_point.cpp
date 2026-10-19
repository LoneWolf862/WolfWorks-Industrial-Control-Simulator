#include "io_point.h"

IOPoint::IOPoint(int id,
                 const std::string& tag,
                 const std::string& description)
: id(id),
tag(tag),
description(description)
{
}

int IOPoint::getID() const
{
    return id;
}

const std::string& IOPoint::getTag() const
{
    return tag;
}

const std::string& IOPoint::getDescription() const
{
    return description;
}
