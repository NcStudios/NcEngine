#include "engine_context.h"

namespace nc
{
    void clear(engine_context* context)
    {
        if(context->application) context->application->clear();
        context->registry->Clear();
        context->particleSystem->Clear();
        context->pointLightSystem->Clear();
        if(context->physicsSystem) context->physicsSystem->ClearState();
        context->audioSystem->Clear();
        context->mainCamera->Set(nullptr);
        context->time->ResetFrameDeltaTime();
        context->time->ResetAccumulatedTime();
        context->environment->Clear();
    }

    auto build_engine_context(engine_init_flags flags) -> engine_context
    {
        V_LOG("Constructing engine_context");
        bool enableApplicationModule = !(flags & engine_init_flags_headless_mode);
        bool enablePhysicsModule = !(flags & engine_init_flags_disable_physics);
        const auto& memorySettings = config::GetMemorySettings();
        engine_context context;
        context.registry = std::make_unique<Registry>(memorySettings.maxTransforms);
        context.mainCamera = std::make_unique<camera::MainCameraImpl>();
        attach_application(&context, context.mainCamera.get(), enableApplicationModule);
        context.particleSystem = std::make_unique<ecs::ParticleEmitterSystem>(context.registry.get());
        context.pointLightSystem = std::make_unique<ecs::PointLightSystem>(context.registry.get());
        physics::attach_physics_system(&context, context.registry.get(), enablePhysicsModule);
        context.sceneSystem = std::make_unique<scene::SceneSystemImpl>();
        context.time = std::make_unique<time::Time>();
        context.audioSystem = std::make_unique<audio::AudioSystemImpl>(context.registry.get());
        context.environment = std::make_unique<EnvironmentImpl>();
        context.random = std::make_unique<Random>();
        return context;
    }
}