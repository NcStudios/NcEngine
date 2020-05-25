#include "SceneManager.h"
#include "Scene.h"
#include "InitialScene.h"

namespace nc::scene
{
    SceneManager::SceneManager()
    {
        Scene* initialScene = new InitialScene();
        activeScenes.push_back(initialScene);
        LoadScene(initialScene);
    }

    void SceneManager::LoadScene(Scene* scenePtr)
    {
        scenePtr->Load();
    }

    void SceneManager::UnloadScene(Scene* scenePtr)
    {
        scenePtr->Unload();
        //remove from vector
    }

} //end namespace nc::scene
