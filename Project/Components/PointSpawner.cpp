#include "PointSpawner.h"

namespace detail
{
    enum Bound { Upper = 1, LowerLeft = 2, LowerRight = 3 };
}

PointSpawner::PointSpawner(ComponentHandle handle, EntityHandle parentHandle) : Component(handle, parentHandle) {}

void PointSpawner::OnInitialize()
{
    InitializeBounds();
    SpawnPoints();
}

void PointSpawner::InitializeBounds()
{
    int boundSize = 16;
    upperBound = Vector2(600, 50);
    lowerLeftBound = Vector2(100, 550);
    lowerRightBound = Vector2(1000, 550);

    EntityHandle boundID = NCE::CreateEntity();
    Entity* bound = NCE::GetEntityPtr(boundID);
    Transform* boundTransform = bound->GetTransform();
    boundTransform->ToggleRenderingEnabled(true);
    boundTransform->SetRect(Vector4(upperBound.GetX(), upperBound.GetY(), boundSize, boundSize));

    boundID = NCE::CreateEntity();
    bound = NCE::GetEntityPtr(boundID);
    boundTransform = bound->GetTransform();
    boundTransform->ToggleRenderingEnabled(true);
    boundTransform->SetRect(Vector4(lowerLeftBound.GetX(), lowerLeftBound.GetY(), boundSize, boundSize));

    boundID = NCE::CreateEntity();
    bound = NCE::GetEntityPtr(boundID);
    boundTransform = bound->GetTransform();
    boundTransform->ToggleRenderingEnabled(true);
    boundTransform->SetRect(Vector4(lowerRightBound.GetX(), lowerRightBound.GetY(), boundSize, boundSize));
}

void PointSpawner::SpawnPoints()
{
    srand(12);

    Vector2 lastPosition(600, 300);  //position of last point placed
    Vector2 newPosition;   //position to place new point
    Vector2 boundPosition; //position of randomly chosen bound

    EntityHandle pointID;
    Entity* point;
    Transform* pointTransform;

    int pointSize = 1;

    for(int i = 0; i < numberOfPoints; ++i)
    {
        int randBound = rand() % 3 + 1;               //random bound to move towards

        Vector2 boundPosition = GetBoundPosition(randBound);
        newPosition = (boundPosition - lastPosition) / 2;
        lastPosition = newPosition;

        pointID = NCE::CreateEntity();                //create new entity
        point = NCE::GetEntityPtr(pointID);           //get pointer to entity
        pointTransform = point->GetTransform();       //get pointer to entity transform
        pointTransform->ToggleRenderingEnabled(true); //render new entity

        pointTransform->SetRect(Vector4(newPosition.GetX(), newPosition.GetY(), pointSize, pointSize));
    }    
}

Vector2 PointSpawner::GetBoundPosition(const int boundIndex) const
{
    switch(boundIndex)
    {
        case detail::Upper:
            return upperBound;
        case detail::LowerLeft:
            return lowerLeftBound;
        case detail::LowerRight:
            return lowerRightBound;
        default:
        {
            std::cout << "default" << std::endl;
            return Vector2::Zero();
        }
    }
}