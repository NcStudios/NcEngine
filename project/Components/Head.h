#ifndef HEAD_H
#define HEAD_H

#include "NCE.h"
#include <random>

using namespace nc;

class Head : public Component
{
    private:
        float m_moveSpeed = 500;

    public:
        Head(ComponentHandle handle, EntityView parentHandle);

        void OnInitialize() override;
        void FrameUpdate() override;
        void OnDestroy() override;

        void OnCollisionEnter(const EntityHandle other) override;

        std::mt19937 rng;
        std::uniform_real_distribution<float> angleDist;
        std::uniform_real_distribution<float> posDist;
        std::uniform_real_distribution<float> scaleDist; 
        std::uniform_real_distribution<float> colorDist;
};


#endif