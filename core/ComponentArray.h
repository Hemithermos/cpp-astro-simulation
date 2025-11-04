#ifndef COMPONENTARRAY_H
#define COMPONENTARRAY_H

#include <array>
#include <unordered_map>
#include <cassert>


#include "Component.h"
#include "Entity.h"

// interface for the component arrays to abstract the template
//      and call all templated component arrays by their interface
//      to destroy entities
class InterfaceComponentArray
{
public:
    virtual ~InterfaceComponentArray() = default;
    virtual void entityDestroyed(Entity) = 0;
};

// the turbo packed array of component T
template <class T>
class ComponentArray : public InterfaceComponentArray
{
public:
    // insert a component for an entity in the component array
    //      and make sure to map them good
    void insertData(Entity, T);

    // remove an entity's stored data from the component
    //      and make sure the array stays dense/packed
    //      for this we will overwrite the component to be
    //      destroyed with the data of the last component
    //      and remap everything good to keep links
    void removeData(Entity);

    // getter for the data of the component associated to the
    //      entity. It returns a reference to it so it can be
    //      edited.
    T& getData(Entity);

    // destroy the component of an entity
    void entityDestroyed(Entity) override;

private:
    // the packed array of type T having MAX_ENTITIES allowed entry
    // each unique entity has its own slot
    std::array<T, MAX_ENTITIES> componentArray;

    // map the entity to the component array index
    // ie : the entity has the given component and its data are stored at this index
    std::unordered_map<Entity, size_t> mapEntityToComponent;

    // map the indices of the component array to entities
    // ie : the component at the given index is associated to entity
    std::unordered_map<size_t, Entity> mapComponentToEntity;


    // count of valid associated components in component array aka size of
    //      valid entries in componentArray
    size_t validComponentNumber;
};

#endif