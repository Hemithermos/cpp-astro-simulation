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

