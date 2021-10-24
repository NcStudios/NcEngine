#pragma once

#include <string>
#include <vector>

#include "types/Types.h"
#include "math/Vector.h"
#include "math/Quaternion.h"
#include "physics/Geometry.h"
#include "component/Transform.h"
#include "Entity.h"


namespace nc::type
{
    
    extern nc::Type<int>* Int;
    extern nc::Type<bool>* Bool;
    extern nc::Type<float>* Float;
    extern nc::Type<std::string>* String;

    extern nc::Type<nc::Vector2>* Vector2;
    extern nc::Type<nc::Vector2>* Point2;

    extern nc::Type<nc::Vector3>* Vector3;
    extern nc::Type<std::vector<nc::Vector3>>* Vector3Vector;
    extern nc::Type<nc::Vector3>* Point3;
    extern nc::Type<nc::Vector3>* Normal3;

    extern nc::Type<nc::Vector4>* Vector4;
    extern nc::Type<nc::Vector4>* Point4;

    extern nc::Type<nc::Quaternion>* Quaternion;
    extern nc::Type<nc::Sphere>* Sphere;
    extern nc::Type<nc::Box>* Box;
    extern nc::Type<nc::Capsule>* Capsule;
    extern nc::Type<nc::ConvexHull>* ConvexHull;
    extern nc::Type<nc::Triangle>* Triangle;
    extern nc::Type<nc::Plane>* Plane;
    extern nc::Type<nc::Frustum>* Frustum;

    extern nc::Type<nc::Transform>* Transform;

    extern nc::Type<nc::Entity>* Entity;

}