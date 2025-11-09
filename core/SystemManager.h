#ifndef CORE_SYSTEM_MANAGER_H
#define CORE_SYSTEM_MANAGER_H

#include <memory>
#include <unordered_map>

#include <typeinfo>
#include <cassert>

#include "System.h"
#include "Component.h"

// maintain system and their signatures
class SystemManager
{
public:
    // register a new system and return its location
    template <typename T>
    std::shared_ptr<T> registerSystem();

    // set the signature of a system
    template <typename T>
    void setSignature(Signature);

    // erase an entity from all the system lists
    void entityDestroyed(Entity);

    // change the signature of an entity
    void entitySignatureChanged(Entity, Signature);

private:
    // map the name of the systems to their signatures
    std::unordered_map<const char *, Signature> signatures;

    // map the name of the systems to their respective location
    std::unordered_map<const char *, std::shared_ptr<System>> systems;
};

#endif

// Template implementations for SystemManager
template <typename T>
std::shared_ptr<T> SystemManager::registerSystem()
{
    const char *typeName = typeid(T).name();
    assert(systems.find(typeName) == systems.end() && "Can't register the same System twice.");
    auto system = std::make_shared<T>();
    systems.insert({typeName, system});
    return system;
}

template <typename T>
void SystemManager::setSignature(Signature signature)
{
    const char *typeName = typeid(T).name();
    assert(systems.find(typeName) != systems.end() && "Can't find the signature of an unregistered system.");
    signatures.insert({typeName, signature});
}