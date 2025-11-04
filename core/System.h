#ifndef CORE_SYSTEM_H
#define CORE_SYSTEM_H

#include <set>
#include "Entity.h"

// every system should inherit that class
class System
{
public:
    std::set<Entity> listOfEntities;
};

#endif