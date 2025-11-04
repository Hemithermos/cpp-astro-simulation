#ifndef ENTITYMANAGER_H
#define ENTITYMANAGER_H

#include <queue>
#include <array>
#include <cassert>


#include "Entity.h"
#include "Component.h" 

// distribute the entity ids, keep record of the used ids
class EntityManager {
    public:

    // initialise the queue with all the possible ids
    EntityManager();

    // allocate if possible an id to a new entity, taking an id from the queue
    Entity createEntity();

    // destroy an entity, giving back its id to the queue
    void destroyEntity(Entity);

    // setter for the signature of an entity
    void setSignature(Entity, Signature);

    // getter for the signature of an entity
    Signature getSignature(Entity);


    private:
    // id allocation queue
    std::queue<Entity> unusedIDs;

    // array of signature where array[entity] access the signature of entity
    //      size is MAX_ENTITIES
    std::array<Signature, MAX_ENTITIES> signaturesForEntity;


    // total living entities
    uint32_t livingEntityCount;

};



#endif