#pragma once

namespace nc 
{
    class Camera;
    class Transform;
}

namespace nc::camera
{
    class MainCameraImpl
    {
        public:
            MainCameraImpl();

            void Set(Camera* camera);
            Transform* GetTransform();
            void ClearTransform();

        private:
            Transform* m_transform;
    };
}