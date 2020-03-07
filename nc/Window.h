#pragma once
#include "NCWin32.h"
//#include <windows.h>
#include <utility> //std::pair

namespace nc
{
    namespace utils::editor { class EditorManager; }

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

            void BindEditorManager(utils::editor::EditorManager* editorManager);

            void OnWindowResize();
            void ProcessSystemMessages();

            static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
        
        private:
            HWND m_hwnd;
            WNDCLASS m_wndClass;
            HDC m_deviceContext;
            HINSTANCE m_hInstance;
            std::pair<int, int> m_windowDimensions; 
            utils::editor::EditorManager* m_editorManager;
    };

} //end namespace nc