#pragma once

namespace nc 
{
    class Camera;
    class Transform;
}

namespace nc::camera
{
    class MainCamera
    {
        public:
            MainCamera();
            ~MainCamera();

            static Transform* GetTransform();
            static void Register(Camera* camera);

            Transform* GetTransform_();
            void ClearTransform();

        private:
            static MainCamera* m_instance;
            Transform* m_transform;
    };
}