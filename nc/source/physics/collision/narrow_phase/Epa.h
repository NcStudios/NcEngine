#pragma once

#include "Common.h"
#include "physics/collision/CollisionState.h"

#include <iostream> // remove once minDistance error is figured out

namespace nc::physics
{
    constexpr float FloatMax = std::numeric_limits<float>::max();
    constexpr size_t EpaMaxIterations = 32u;
    constexpr float EpaTolerance = 0.001f;

    template<class BVA, class BVB>
    bool Epa(const BVA& a, const BVB& b, DirectX::FXMMATRIX aMatrix, DirectX::FXMMATRIX bMatrix, CollisionState* stateOut);

    template<class BVA>
    bool EpaVsTriangle(const BVA& a, const TriangleXM& b, DirectX::FXMMATRIX aMatrix, CollisionState* stateOut);

    template<class BVA, class BVB>
    bool Epa(const BVA& a, const BVB& b, DirectX::FXMMATRIX aMatrix, DirectX::FXMMATRIX bMatrix, CollisionState* state)
    {
        using namespace DirectX;

        state->polytope.Initialize(state->simplex);
        auto minFace = state->polytope.ComputeNormalData();
        NormalData minNorm{Vector3::Zero(), FloatMax};
        size_t iterations = 0u;

        while(minNorm.distance == FloatMax)
        {
            // Normal data for the face closest to the origin.
            minNorm = state->polytope.GetNormalData(minFace);

            if(iterations++ > EpaMaxIterations)
                break;

            // Find a point on the Minkowski hull in the direction of the closest face's normal.
            auto direction_v = XMLoadVector3(&minNorm.normal);
            auto supports = MinkowskiSupports(a, b, state->rotationA, state->rotationB, aMatrix, bMatrix, direction_v);

            Vector3 support;
            XMStoreVector3(&support, supports.worldCSO);
            XMStoreVector3(&(state->contact.worldPointA), supports.worldA);
            XMStoreVector3(&(state->contact.worldPointB), supports.worldB);
            XMStoreVector3(&(state->contact.localPointA), supports.localA);
            XMStoreVector3(&(state->contact.localPointB), supports.localB);

            if(abs(Dot(minNorm.normal, support) - minNorm.distance) > EpaTolerance)
            {
                // The closest face is not on the hull, so we expand towards the hull.
                if(!state->polytope.Expand(support, state->contact, &minFace))
                {
                    /** @todo Need to determine if this can happen under normal circumstances.
                     *  It has thrown here a few times, but always when there is wonk elsewhere. */
                    //throw std::runtime_error("Epa - minDistance not found");
                    std::cout << "Epa - minDistance not found\n";
                    state->contact.depth = 0.0f;
                    state->contact.lambda = 0.0f;
                    state->contact.muTangent = 0.0f;
                    state->contact.muBitangent = 0.0f;
                    return false;
                }

                minNorm.distance = FloatMax;
            }
        }

        auto success = state->polytope.GetContacts(minFace, &state->contact);
        state->contact.normal = Normalize(minNorm.normal);
        state->contact.depth = minNorm.distance + EpaTolerance;
        state->contact.lambda = 0.0f;
        state->contact.muTangent = 0.0f;
        state->contact.muBitangent = 0.0f;
        return success;
    }

    template<class BVA>
    bool EpaVsTriangle(const BVA& a, const TriangleXM& b, DirectX::FXMMATRIX aMatrix, CollisionState* stateOut)
    {
        using namespace DirectX;

        stateOut->polytope.Initialize(stateOut->simplex);
        auto minFace = stateOut->polytope.ComputeNormalData();
        NormalData minNorm{Vector3::Zero(), FloatMax};
        size_t iterations = 0u;

        while(minNorm.distance == FloatMax)
        {
            // Normal data for the face closest to the origin.
            minNorm = stateOut->polytope.GetNormalData(minFace);

            if(iterations++ > EpaMaxIterations)
                break;

            // Find a point on the Minkowski hull in the direction of the closest face's normal.
            auto direction_v = XMLoadVector3(&minNorm.normal);
            auto aDirection_v = XMVector3InverseRotate(direction_v, stateOut->rotationA);
            auto aSupportLocal_v = MinkowskiSupport(a, aDirection_v);
            auto aSupportWorld_v = XMVector3Transform(aSupportLocal_v, aMatrix);

            auto bDirection_v = XMVectorNegate(direction_v);
            auto bSupportWorld_v = MinkowskiSupport(b, bDirection_v);
            
            auto support_v = aSupportWorld_v - bSupportWorld_v;
            Vector3 support;
            XMStoreVector3(&support, support_v);

            XMStoreVector3(&(stateOut->contact.worldPointA), aSupportWorld_v);
            XMStoreVector3(&(stateOut->contact.worldPointB), bSupportWorld_v);
            XMStoreVector3(&(stateOut->contact.localPointA), aSupportLocal_v);
            XMStoreVector3(&(stateOut->contact.localPointB), bSupportWorld_v);

            if(abs(Dot(minNorm.normal, support) - minNorm.distance) > EpaTolerance)
            {
                // The closest face is not on the hull, so we expand towards the hull.
                if(!stateOut->polytope.Expand(support, stateOut->contact, &minFace))
                {
                    /** @todo Need to determine if this can happen under normal circumstances.
                     *  It has thrown here a few times, but always when there is wonk elsewhere. */
                    //throw std::runtime_error("Epa - minDistance not found");
                    std::cout << "Epa - minDistance not found\n";
                    stateOut->contact.depth = 0.0f;
                    stateOut->contact.lambda = 0.0f;
                    stateOut->contact.muTangent = 0.0f;
                    stateOut->contact.muBitangent = 0.0f;
                    return false;
                }

                minNorm.distance = FloatMax;
            }
        }

        auto success = stateOut->polytope.GetContacts(minFace, &stateOut->contact);
        stateOut->contact.normal = Normalize(minNorm.normal);
        stateOut->contact.depth = minNorm.distance + EpaTolerance;
        stateOut->contact.lambda = 0.0f;
        stateOut->contact.muTangent = 0.0f;
        stateOut->contact.muBitangent = 0.0f;
        return success;
    }
}