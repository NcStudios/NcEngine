#pragma once

#include "ResourceGroup.h"

#include <string>

namespace nc::graphics
{
    void LoadMeshAsset(const std::string& path);

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
    };
}