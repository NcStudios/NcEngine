
#include "types/EngineTypes.h"
#include "ECS.h"


namespace nc::type
{
    
    nc::Type<int>* Int = Types::Define<int>("int");

    nc::Type<bool>* Bool = Types::Define<bool>("bool");

    nc::Type<float>* Float = Types::Define<float>("float");
    
    nc::Type<std::string>* String = Types::Define<std::string>("string");

    nc::Type<nc::Vector2>* Vector2 = Types::Define<nc::Vector2>("Vector2", [](Type<nc::Vector2>* t) 
    {
        t->Add<float>("x", Float, {
            .ref = [](nc::Vector2* a) { return &a->x; },
        });
        t->Add<float>("y", Float, {
            .ref = [](nc::Vector2* a) { return &a->y; },
        });
    });

    nc::Type<nc::Vector2>* Point2 = Types::Alias(Vector2, "Point2");

    nc::Type<nc::Vector3>* Vector3 = Types::Define<nc::Vector3>("Vector3", [](Type<nc::Vector3>* t) 
    {
        t->Add<float>("x", Float, {
            .ref = [](nc::Vector3* a) { return &a->x; },
        });
        t->Add<float>("y", Float, {
            .ref = [](nc::Vector3* a) { return &a->y; },
        });
        t->Add<float>("z", Float, {
            .ref = [](nc::Vector3* a) { return &a->z; },
        });
    });

    nc::Type<std::vector<nc::Vector3>>* Vector3Vector = Types::DefineVector("Vector3[]", Vector3);

    nc::Type<nc::Vector3>* Point3 = Types::Alias(Vector3, "Point3");

    nc::Type<nc::Vector3>* Normal3 = Types::Alias(Vector3, "Normal3");

    nc::Type<nc::Vector4>* Vector4 = Types::Define<nc::Vector4>("Vector4", [](Type<nc::Vector4>* t) 
    {
        t->Add<float>("x", Float, {
            .ref = [](nc::Vector4* a) { return &a->x; },
        });
        t->Add<float>("y", Float, {
            .ref = [](nc::Vector4* a) { return &a->y; },
        });
        t->Add<float>("z", Float, {
            .ref = [](nc::Vector4* a) { return &a->z; },
        });
        t->Add<float>("w", Float, {
            .ref = [](nc::Vector4* a) { return &a->w; },
        });
    });

    nc::Type<nc::Vector4>* Point4 = Types::Alias(Vector4, "Point4");

    nc::Type<nc::Quaternion>* Quaternion = Types::Define<nc::Quaternion>("Quaternion", [](Type<nc::Quaternion>* t) 
    {
        t->Add<float>("x", Float, {
            .ref = [](nc::Quaternion* a) { return &a->x; },
        });
        t->Add<float>("y", Float, {
            .ref = [](nc::Quaternion* a) { return &a->y; },
        });
        t->Add<float>("z", Float, {
            .ref = [](nc::Quaternion* a) { return &a->z; },
        });
        t->Add<float>("w", Float, {
            .ref = [](nc::Quaternion* a) { return &a->w; },
        });
    });

    nc::Type<nc::Sphere>* Sphere = Types::Define<nc::Sphere>("Sphere", [](Type<nc::Sphere>* t) 
    {
        t->Add<nc::Vector3>("center", Vector3, {
            .ref = [](nc::Sphere* a) { return &a->center; },
        });
        t->Add<float>("radius", Float, {
            .ref = [](nc::Sphere* a) { return &a->radius; },
        });
    });

    nc::Type<nc::Box>* Box = Types::Define<nc::Box>("Box", [](Type<nc::Box>* t) 
    {
        t->Add<nc::Vector3>("center", Vector3, {
            .ref = [](nc::Box* a) { return &a->center; },
        });
        t->Add<nc::Vector3>("extents", Vector3, {
            .ref = [](nc::Box* a) { return &a->extents; },
        });
        t->Add<float>("maxExtent", Float, {
            .ref = [](nc::Box* a) { return &a->maxExtent; },
        });
    });

    nc::Type<nc::Capsule>* Capsule = Types::Define<nc::Capsule>("Capsule", [](Type<nc::Capsule>* t) 
    {
        t->Add<nc::Vector3>("pointA", Vector3, {
            .ref = [](nc::Capsule* a) { return &a->pointA; },
        });
        t->Add<nc::Vector3>("pointB", Vector3, {
            .ref = [](nc::Capsule* a) { return &a->pointB; },
        });
        t->Add<float>("radius", Float, {
            .ref = [](nc::Capsule* a) { return &a->radius; },
        });
        t->Add<float>("maxExtent", Float, {
            .ref = [](nc::Capsule* a) { return &a->maxExtent; },
        });
    });

    nc::Type<nc::ConvexHull>* ConvexHull = Types::Define<nc::ConvexHull>("ConvexHull", [](Type<nc::ConvexHull>* t) 
    {
        // TODO std::span
        // t->Add<std::vector<nc::Vector3>>("vertices",  Vector3Vector, {
        //     .ref = [](nc::ConvexHull* a) { return &a->vertices; },
        // });
        t->Add<nc::Vector3>("extents", Vector3, {
            .ref = [](nc::ConvexHull* a) { return &a->extents; },
        });
        t->Add<float>("maxExtent", Float, {
            .ref = [](nc::ConvexHull* a) { return &a->maxExtent; },
        });
    });

    nc::Type<nc::Triangle>* Triangle = Types::Define<nc::Triangle>("Triangle", [](Type<nc::Triangle>* t) 
    {
        t->Add<nc::Vector3>("a", Vector3, {
            .ref = [](nc::Triangle* a) { return &a->a; },
        });
        t->Add<nc::Vector3>("b", Vector3, {
            .ref = [](nc::Triangle* a) { return &a->b; },
        });
        t->Add<nc::Vector3>("c", Vector3, {
            .ref = [](nc::Triangle* a) { return &a->c; },
        });
    });

    nc::Type<nc::Plane>* Plane = Types::Define<nc::Plane>("Plane", [](Type<nc::Plane>* t) 
    {
        t->Add<nc::Vector3>("normal", Normal3, {
            .ref = [](nc::Plane* a) { return &a->normal; },
        });
        t->Add<float>("d", Float, {
            .ref = [](nc::Plane* a) { return &a->d; },
        });
    });

    nc::Type<nc::Frustum>* Frustum = Types::Define<nc::Frustum>("Frustum", [](Type<nc::Frustum>* t) 
    {
        t->Add<nc::Plane>("left",   Plane, {
            .ref = [](nc::Frustum* a) { return &a->left; },
        });
        t->Add<nc::Plane>("right",  Plane, {
            .ref = [](nc::Frustum* a) { return &a->right; },
        });
        t->Add<nc::Plane>("bottom", Plane, {
            .ref = [](nc::Frustum* a) { return &a->bottom; },
        });
        t->Add<nc::Plane>("top",    Plane, {
            .ref = [](nc::Frustum* a) { return &a->top; },
        });
        t->Add<nc::Plane>("front",  Plane, {
            .ref = [](nc::Frustum* a) { return &a->front; },
        });
        t->Add<nc::Plane>("back",   Plane, {
            .ref = [](nc::Frustum* a) { return &a->back; },
        });
    });

    nc::Type<nc::Transform>* Transform = Types::Define<nc::Transform>("Transform", [](){ return nullptr; }, [](Type<nc::Transform>* t)
    {
        t->Add<nc::Vector3>("position", Vector3,    {
            .get = [](nc::Transform* a, nc::Vector3* out) { *out = a->GetLocalPosition(); return true; },
            .set = [](nc::Transform* a, nc::Vector3* value) { a->SetPosition(*value); },
        });
        t->Add<nc::Quaternion>("rotation", Quaternion, {
            .get = [](nc::Transform* a, nc::Quaternion* out) { *out = a->GetLocalRotation(); return true; },
            .set = [](nc::Transform* a, nc::Quaternion* value) { a->SetRotation(*value); },
        });
        t->Add<nc::Vector3>("scale",    Vector3,    {
            .get = [](nc::Transform* a, nc::Vector3* out) { *out = a->GetLocalScale(); return true; },
            .set = [](nc::Transform* a, nc::Vector3* value) { a->SetScale(*value); },
        });
    });

    nc::Type<nc::Entity>* Entity = Types::Define<nc::Entity>("Entity", [](Type<nc::Entity>* t)
    {
        t->Add<nc::Transform>("transform", Transform, {
            .ref = [](nc::Entity* e) { return nc::ActiveRegistry()->Get<nc::Transform>(*e); },
        });
    });

}