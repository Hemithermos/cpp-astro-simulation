#ifndef CORE_COORDINATOR_H
#define CORE_COORDINATOR_H

#include <memory>


#include "EntityManager.h"
#include "ComponentManager.h"
#include "SystemManager.h"

// Avoid including concrete component/system headers here to prevent circular
// includes. Templates are defined below and will be instantiated where needed.
// this class makes sure the system, component and entity manager talk to each other

class Coordinator
{
public:
    // initialization
    void init();

    // Entity methods
    Entity createEntity();
    void destroyEntity(Entity);

    // Component methods
    template<typename T> void registerComponent();               // register component
    template<typename T> void addComponent(Entity, T);           // associate entity to component
    template<typename T> void removeComponent(Entity);           // unassociate entity from component
    template<typename T> T& getComponent(Entity);                // get the entity's component location
    template<typename T> ComponentType getComponentType();       // get type T's component type
    template<typename T> bool hasComponent(Entity);              // test if an entity has the T component

    // System methods
    template<typename T> std::shared_ptr<T> registerSystem();    // register system
    template<typename T> void setSystemSignature(Signature);     // setter for the system signature 


private:
    // three pointers to each manager
    std::unique_ptr<EntityManager> entityManager;
    std::unique_ptr<ComponentManager> componentManager;
    std::unique_ptr<SystemManager> systemManager;

};

// Template implementations for Coordinator (inside include guard)
template<typename T>
void Coordinator::registerComponent()
{
    componentManager->registerComponent<T>();
}

template<typename T>
void Coordinator::addComponent(Entity entity, T component)
{
    componentManager->addComponent(entity, component);

    auto signature = entityManager->getSignature(entity);
    signature.set(componentManager->getComponentType<T>(), true);
    entityManager->setSignature(entity, signature);

    systemManager->entitySignatureChanged(entity, signature);
}

template<typename T>
void Coordinator::removeComponent(Entity entity)
{
    componentManager->removeComponent<T>(entity);

    auto signature = entityManager->getSignature(entity);
    signature.set(componentManager->getComponentType<T>(), false);
    entityManager->setSignature(entity, signature);

    systemManager->entitySignatureChanged(entity, signature);
}

template<typename T>
T& Coordinator::getComponent(Entity entity)
{
    return componentManager->getComponent<T>(entity);
}

template<typename T>
ComponentType Coordinator::getComponentType()
{
    return componentManager->getComponentType<T>();
}

template <typename T>
bool Coordinator::hasComponent(Entity entity)
{
    Signature signature = entityManager->getSignature(entity);
    ComponentType componentType = getComponentType<T>();
    return (signature[componentType] == true);
}

template <typename T>
std::shared_ptr<T> Coordinator::registerSystem()
{
    return systemManager->registerSystem<T>();
}

template <typename T>
void Coordinator::setSystemSignature(Signature signature)
{
    systemManager->setSignature<T>(signature);
}

#endif