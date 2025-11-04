#ifndef SYSTEM_MANAGER_H
#define SYSTEM_MANAGER_H

#include <memory>
#include <unordered_map>

#include "System.h"
#include "Component.h"



// maintain system and their signatures
class SystemManager
{
    public:
    // register a new system and return its location
    template<typename T> std::shared_ptr<T> registerSystem();

    // set the signature of a system
    template<typename T> void setSignature(Signature);


    // erase an entity from all the system lists
    void entityDestroyed(Entity);

    // change the signature of an entity
    void entitySignatureChanged(Entity, Signature);



    private:

    // map the name of the systems to their signatures
    std::unordered_map<const char*, Signature> signatures;

    // map the name of the systems to their respective location
    std::unordered_map<const char*, std::shared_ptr<System>> systems;



};

#endif