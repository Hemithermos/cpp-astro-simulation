
#include "ComponentManager.h"

void ComponentManager::entityDestroyed(Entity entity)
{
    // for each pair {TypeName | ComponentArray} notify that entity has been destroyed
    // it will unassociate the good components if they exists
    for (auto const &pair : mapTypeNameToComponentArray)
    {
        // get the pointer to the ComponentArray
        auto const &component = pair.second;
        // notify the ComponentArray
        component->entityDestroyed(entity);
    }
}

