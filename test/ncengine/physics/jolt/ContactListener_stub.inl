#include "physics/jolt/ContactListener.h"

namespace nc::physics
{
void ContactListener::OnContactAdded(const JPH::Body&,
                                     const JPH::Body&,
                                     const JPH::ContactManifold&,
                                     JPH::ContactSettings&)
{
}

void ContactListener::OnContactRemoved(const JPH::SubShapeIDPair&)
{
}

void ContactListener::CommitPendingChanges()
{
}
} // namespace nc::physics
