#include "Coordinator.h"



void Coordinator::init()
{
    // create three unique ptrs to each manager
    entityManager = std::make_unique<EntityManager>();
    componentManager = std::make_unique<ComponentManager>();
    systemManager = std::make_unique<SystemManager>();

}

// entity methods

Entity Coordinator::createEntity()
{
    return entityManager->createEntity();
}

void Coordinator::destroyEntity(Entity entity)
{
    entityManager->destroyEntity(entity);
    componentManager->entityDestroyed(entity);
    systemManager->entityDestroyed(entity);
}

// component methods

template<typename T> void Coordinator::registerComponent()
{
    componentManager->registerComponent<T>();
}
template<typename T> void Coordinator::addComponent(Entity entity, T component)
{
    // associate the entity to the component
    componentManager->addComponent(entity, component);

    auto signature = entityManager->getSignature(entity);
    // change the signature of the entity at the component bit to 1
    signature.set(componentManager->getComponentType<T>(), true);
    // set the entity signature
    entityManager->setSignature(entity, signature);

    // send a "signature changed for entity" signal in the system to perhaps include the entity to some systems
    systemManager->entitySignatureChanged(entity, signature);

}
template<typename T> void Coordinator::removeComponent(Entity entity)
{
    // disassociate the entity to the component
    componentManager->removeComponent<T>(entity);

    auto signature = entityManager->getSignature(entity);
    // change the signature of the entity at the component bit to 0
    signature.set(componentManager->getComponentType<T>(), false);
    // set the entity signature
    entityManager->setSignature(entity, signature);

    // send a "signature changed for entity" signal in the system to perhaps exclude the entity from some systems
    systemManager->entitySignatureChanged(entity, signature);
}
template<typename T> T& Coordinator::getComponent(Entity entity)
{
    return componentManager->getComponent<T>(entity);

}
template<typename T> ComponentType Coordinator::getComponentType()
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

// system methods


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
