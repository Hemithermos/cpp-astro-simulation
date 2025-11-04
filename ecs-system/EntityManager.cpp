#include "EntityManager.h"

EntityManager::EntityManager()
{
    // initialise the queue with all the possible ids
    unusedIDs = std::queue<Entity>();

    // iterate over the unused ids to give them the next possible id
    //      here it just push all valid ids into the unused ids for initialization.
    for(Entity entity = 0; entity < MAX_ENTITIES; ++entity) {
        unusedIDs.push(entity);
    }
}


Entity EntityManager::createEntity()
{
    // this insure there is at least one unused id
    assert(livingEntityCount < MAX_ENTITIES && "Too many entities !!");


    // allocate the next unused ids
    Entity id = unusedIDs.front();
    // remove it from the queue
    unusedIDs.pop();
    // increment the living count
    livingEntityCount++;

    return id;
}

void EntityManager::destroyEntity(Entity entity)
{
    // this insure we dont try to access an entity outside the entity table
    assert(entity < MAX_ENTITIES && "Given entity for destruction is out of range");

    // invalidate the signature
    signaturesForEntity[entity].reset();

    // put the destroyed id at the back of the queue
    unusedIDs.push(entity);
    // decrement the living count
    livingEntityCount--;

}


void EntityManager::setSignature( Entity entity, Signature signature)
{
    // make sure entity is in range
    assert(entity < MAX_ENTITIES && "Given entity for signature attribution is out of range");

    // set the signature to the array at the entity's index
    signaturesForEntity[entity] = signature;

}

Signature EntityManager::getSignature(Entity entity)
{
    // make sure entity is in range
    assert(entity < MAX_ENTITIES && "Given entity for signature attribution is out of range");

    // get the signature from the array at the entity's index
    return signaturesForEntity[entity];
}