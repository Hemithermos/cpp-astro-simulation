#ifndef CORE_COMPONENT_MANAGER_H
#define CORE_COMPONENT_MANAGER_H

#include <unordered_map>
#include <memory>
#include <typeinfo>
#include <cassert>

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
    void removeComponent(Entity);

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

// Template implementations moved into header so they are available to all TUs.
template <typename T>
std::shared_ptr<ComponentArray<T>> ComponentManager::getComponentArray()
{
    const char *typeName = typeid(T).name();
    assert(mapTypeNameToComponentArray.find(typeName) != mapTypeNameToComponentArray.end() && "Component not registered. Can't get its ComponentArray");
    return std::static_pointer_cast<ComponentArray<T>>(mapTypeNameToComponentArray[typeName]);
}

template <typename T>
void ComponentManager::registerComponent()
{
    const char *typeName = typeid(T).name();
    assert(mapTypeNameToComponentType.find(typeName) == mapTypeNameToComponentType.end() && "Registering an already existing component");
    mapTypeNameToComponentType.insert({typeName, nextComponentType});
    mapTypeNameToComponentArray.insert({typeName, std::make_shared<ComponentArray<T>>()});
    nextComponentType++;
}

template <typename T>
ComponentType ComponentManager::getComponentType()
{
    const char *typeName = typeid(T).name();
    assert(mapTypeNameToComponentType.find(typeName) != mapTypeNameToComponentType.end() && "Component not registered. Can't get its type");
    return mapTypeNameToComponentType[typeName];
}

template <typename T>
void ComponentManager::addComponent(Entity entity, T component)
{
    getComponentArray<T>()->insertData(entity, component);
}

template <typename T>
void ComponentManager::removeComponent(Entity entity)
{
    getComponentArray<T>()->removeData(entity);
}

template <typename T>
T &ComponentManager::getComponent(Entity entity)
{
    return getComponentArray<T>()->getData(entity);
}