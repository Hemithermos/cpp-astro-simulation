#include "SystemManager.h"

template <typename T>
std::shared_ptr<T> SystemManager::registerSystem()
{

    // cf ComponentManager.cpp for the logic
    const char *typeName = typeid(T).name();
    assert(systems.find(typeName) == systems.end() && "Can't register the same System twice.");

    auto system = std::make_shared<T>();
    systems.insert({typeName, system});
    return system
}

template <typename T>
void SystemManager::setSignature(Signature signature)
{
    // cf ComponentManager.cpp for the logic
    const char *typeName = typeid(T).name();

    assert(systems.find(typeName) != systems.end() && "Can't find the signature of an unregistered system.");

    signatures.insert({typename, signature});
}

void SystemManager::entityDestroyed(Entity entity)
{
    // erase the destroyed entity from the listOfEntities of each system
    // iterate over a map so we get a {typename, System} pair
    for (auto const &pair : systems)
    {
        auto const &system = pair.second;
        system->listOfEntities.erase(entity);
    }
}

void SystemManager::entitySignatureChanged(Entity entity, Signature signature)
{
    // notify the systems that the signature changed
    // iterate over a map so we get a {typename, System} pair
    for (auto const &pair : systems)
    {
        auto const &type = pair.first;
        auto const &system = pair.second;
        auto const &systemSignature = signatures[type];

        // if the entity signature matches at least the system signature
        // we insert the entity to the system
        // this way we dont exclude entities that have extra components

        // for this we use the bitwise and between the two signatures
        if ((systemSignature & signature) == systemSignature)
        {
            system->listOfEntities.insert(entity);
        }
        // if it doesnt match we erase the entity from the system
        else
        {
            system->listOfEntities.erase(entity);
        }
    }
}
