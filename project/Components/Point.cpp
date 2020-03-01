#include "Point.h"

#include <iostream>

Point::Point(ComponentHandle handle, EntityHandle parentHandle) : Component(handle, parentHandle){}


void Point::OnCollisionEnter(const EntityHandle other)
{
    // Entity* otherPtr = NCE::GetEntity(other);
    // if (otherPtr && otherPtr->HasComponent<Head>()) 
    // {
    //     NCE::GetEntity(SpawnerHandle)->GetComponent<PointSpawner>()->PointConsumed();
    //     NCE::DestroyEntity(GetEntityHandle());        
    // }
}

