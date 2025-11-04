#include "ComponentManager.h"

template <typename T>
std::shared_ptr<ComponentArray<T>> ComponentManager::getComponentArray()
{
    // get the string pointer of the name of the type
    const char *typeName = typeid(T).name();

    // make sure we are getting a registered component's array
    assert(mapTypeNameToComponentType(typeName) != mapTypeNameToComponentType.end() && "Component not registered. Can't get its ComponentArray");

    // return a shared pointer to the component array
    return std::static_pointer_cast<ComponentArray<T>>(mapTypeNameToComponentArray[typeName]);
}

template <typename T>
void ComponentManager::registerComponent()
{

    // get the string pointer of name of the type
    const char *typeName = typeid(T).name();

    // make sure we arent registering the same component multiple times
    assert(mapTypeNameToComponentType.find(typeName) == mapTypeNameToComponentType.end() && "Registering an already existing component");

    // add the type name to the ComponenType map
    mapTypeNameToComponentType.insert({typeName, nextComponentType});

    // create a ComponentArray of type T pointer and add it to the ComponentArray map
    mapComponentTypeToComponentArray.insert({typename, std::make_shared<ComponentArray<T>>()});

    // increment the next valid component id
    nextComponentType++;
}

template <typename T>
ComponentType ComponentManager::getComponentType()
{
    // get the string pointer of name of the type
    const char *typeName = typeid(T).name();

    // make sure we are getting a registered component type
    assert(mapTypeNameToComponentType.find(typeName) != mapTypeNameToComponentType.end() && "Component not registered. Can't get its type");

    // return the component's unique id (ComponentType)
    return mapTypeNameToComponentType[typeName];
}

template <typename T>
void ComponentManager::addComponent(Entity entity, T component)
{

    // associate a component from the good ComponentArray to an entity
    getComponentArray<T>()->insertData(entity, component);
}

template <typename T>
void ComponentManager::removeComponent(Entity entity)
{
    // unassociate a component from the good ComponentArray to an entity
    getComponentArray<T>()->removeData(entity);
}

template <typename T>
T &ComponentManager::getComponent(Entity entity)
{
    // return a reference of the T component of the entity to access its data

    return getComponentArray<T>()->getData(entity);
}

void ComponentManager::entityDestroyed(Entity entity)
{
    // for each pair {TypeName | ComponentArray} notify that entity has been destroyed
    // it will unassociate the good components if they exists
    for (auto const &pair : mapTypeNameToComponentArray)
    {
        // get the pointer to the ComponentArray
        auto const &component = pair.second;
        // notify the ComponentArray
        component->entityDestroyed(entity);
    }
}
