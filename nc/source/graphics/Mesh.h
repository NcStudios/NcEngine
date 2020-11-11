#pragma once

#include "ResourceGroup.h"

#include <string>
#include <vector>

namespace nc::graphics
{
    class Graphics;
    namespace d3dresource
    {
        class GraphicsResource;
        class IndexBuffer;
    }

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