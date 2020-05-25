#pragma once
#include "NCWin32.h"
#include <utility> //std::pair

namespace nc
{
    #ifdef NC_DEBUG
    namespace utils::editor { class EditorManager; }
    #endif

    class Window
    {
        public:
            static Window* Instance;

            Window(HINSTANCE instance) noexcept;
            ~Window() noexcept;
            Window(const Window& other) = delete;
            Window(Window&& other) = delete;
            Window& operator=(const Window& other) = delete;
            Window& operator=(Window&& other) = delete;

            HWND GetHWND() const noexcept;
            std::pair<int, int> GetWindowDimensions() const noexcept;

            #ifdef NC_DEBUG
            void BindEditorManager(utils::editor::EditorManager* editorManager);
            #endif

            void OnWindowResize();
            void ProcessSystemMessages();

            static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
        
        private:
            HWND m_hwnd;
            WNDCLASS m_wndClass;
            HDC m_deviceContext;
            HINSTANCE m_hInstance;
            std::pair<int, int> m_windowDimensions; 
            
            #ifdef NC_DEBUG
            utils::editor::EditorManager* m_editorManager;
            #endif
    };

} //end namespace nc