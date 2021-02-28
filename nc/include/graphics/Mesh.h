#pragma once

#include "ResourceGroup.h"

#include <string>

namespace nc::graphics
{
    class Graphics;
    namespace d3dresource
    {
        class GraphicsResource;
        class IndexBuffer;
    }

    void LoadMesh(const std::string& path);

    class Mesh : public ResourceGroup
    {
        public:
            Mesh() = default;
            Mesh(std::string meshPath);
            Mesh(const Mesh& other) = default;
            Mesh(Mesh&& other) = default;
            Mesh& operator=(Mesh&& other) = default;
            Mesh& operator=(const Mesh& other) = default;
            ~Mesh() = default;
        
        private:
            void AddBufferResources(std::string meshPath);
    };
}