#ifndef COMPONENT_H
#define COMPONENT_H
#include <cstdint>
#include <bitset>

// there can be 256 different component types because its an 8-bit integer
// easier to mention a ComponentType rather than Component number 23
using ComponentType = std::uint8_t;

// maximum size of a component array
const ComponentType MAX_COMPONENTS = 32;

// a bit set for tracking which component does an entity have
// For example an entity that has component 0 and 2
//     has a signature represented by the bit set : 101000...
//     until the MAX_COMPONENTS-th bit
using Signature = std::bitset<MAX_COMPONENTS>;



#endif