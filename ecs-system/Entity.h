#ifndef ENTITY_H
#define ENTITY_H

#include <cstdint>

// an Entity alias treated as an unsigned 32bit integer
using Entity = std::uint32_t; 

// max entity, used for arrays
const Entity MAX_ENTITIES = 5000;



#endif