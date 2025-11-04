#ifndef SYSTEM_H
#define SYSTEM_H

#include <set>
#include "Entity.h"

// every system should inherit that class
class System
{
public:
    std::set<Entity> listOfEntities;
};

#endif