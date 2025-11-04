#ifndef COMPONENTMANAGER_H
#define COMPONENTMANAGER_H

#include <unordered_map>
#include <memory>

#include "ComponentArray.h"

// like the Entity manager, this class role is to associate unique componentType ids to components.
// It can also :
//          - get the unique id of a component
//          - associate and unassociate a component from an entity
//          - get the component data of an entity (ex : the position of a particle)
//          - notify the component array that an entity has been destroyed

class ComponentManager
{
public:
    // register a new component to the component manager. A new unique component id (aka ComponentType)
    // is created. The component array associated to this component is also created.
    template <typename T>
    void registerComponent();

    // get the component type of the type T. Used to create signatures
    template <typename T>
    ComponentType getComponentType();

    // associate the given component to the given entity
    template <typename T>
    void addComponent(Entity, T);

    // un-associate the given component to the given entity and vice-versa
    template <typename T>
    void removeComponent(Entity, T);

    // get the reference to the component of an entity
    template <typename T>
    T &getComponent(Entity);

    // notify each ComponentArray that the given entity has been destroyed
    void entityDestroyed(Entity);

private:
    // map name of the type to the unique id of the component (componentType)
    std::unordered_map<const char *, ComponentType> mapTypeNameToComponentType;

    // map the name of the type to the ComponentArray
    std::unordered_map<const char *, std::shared_ptr<InterfaceComponentArray>> mapTypeNameToComponentArray;

    // the component id (ComponentType) to be attributed next
    ComponentType nextComponentType;

    // get the string pointer of the ComponentArray of type T
    template <typename T>
    std::shared_ptr<ComponentArray<T>> getComponentArray();
};

#endif