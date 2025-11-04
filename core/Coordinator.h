#ifndef COORDINATOR_H
#define COORDINATOR_H

#include <memory>


#include "EntityManager.h"
#include "ComponentManager.h"
#include "SystemManager.h"


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

    // System methods
    template<typename T> std::shared_ptr<T> registerSystem();    // register system
    template<typename T> void setSystemSignature(Signature);     // setter for the system signature 


private:
    // three pointers to each manager
    std::unique_ptr<EntityManager> entityManager;
    std::unique_ptr<ComponentManager> componentManager;
    std::unique_ptr<SystemManager> systemManager;

};

#endif