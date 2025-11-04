#include "ComponentArray.h"

template <class T>
void ComponentArray<T>::insertData(Entity entity, T component)
{
    // make sure the entity to component mapping is injective
    //      eg : entity can't be mapped to two identical component
    assert(mapComponentToEntity == mapEntityToComponent && "Component added to the same entity more than once");

    // put a new entry at the end and update the maps
    size_t indexComponent = validComponentNumber;  // next after last valid
    mapEntityToComponent[entity] = indexComponent; // associate entity to new component
    mapComponentToEntity[indexComponent] = entity; // map component to associated entity

    // store the data at the good place
    componentArray[indexComponent] = component;

    // increment the number of valid associated component
    validComponentNumber++;
}

template <class T>
void ComponentArray<T>::removeData(Entity entity)
{

    // make sure the component of the entity we are trying to remove
    //      exists. If it doesnt exist then find() returns a past the end
    //      iterator (aka end())
    assert(mapEntityToComponent.find(entity) != mapEntityToComponent.end() && "Component doesnt exist and therefore can't be removed.");

    // copy the element at the end
    size_t indexOfRemovedEntity = mapEntityToComponent[entity];
    size_t indexOfLastElement = validComponentNumber - 1;
    // into the deleted data slot
    componentArray[indexOfRemovedEntity] = componentArray[indexOfLastElement];

    // update the maps
    // get the entity of the last component
    Entity entityOfLastElement = mapComponentToEntity[indexOfLastElement];
    // map the entity of the last element to the overwritten component index
    mapEntityToComponent[entityOfLastElement] = indexOfRemovedEntity;
    // map the index of the overwritten component to the former last element
    mapComponentToEntity[indexOfRemovedEntity] = entityOfLastElement;

    // erase the entity from the map to component
    mapEntityToComponent.erase(entity);
    // erase the duplicated last element at the end
    mapComponentToEntity.erase(indexOfLastElement);

    // decrement the validComponentNumber
    validComponentNumber--;
}

template <class T>
T &ComponentArray<T>::getData(Entity entity)
{
    // make sure the component of the entity we're searching for exists
    assert(mapEntityToComponent.find(entity) != mapEntityToComponent.end() && "Component doesnt exist and therefore can't be accessed.");

    // return a reference to the component
    return componentArray[mapEntityToComponent[entity]];
}

template <class T>
void ComponentArray<T>::entityDestroyed(Entity entity)
{
    if (mapEntityToComponent.find(entity) != mapEntityToComponent.end())
    {
        // if we find the component associated to this entity, kill it
        removeData(entity);
    }
}