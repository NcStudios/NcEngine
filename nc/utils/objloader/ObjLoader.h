#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <stdint.h>

namespace nc::graphics { struct MeshData; }
namespace DirectX {struct XMFLOAT3; }

namespace nc::utils
{
    class ObjLoader
    {
        public:
            ObjLoader();
            ~ObjLoader();


            void Load(nc::graphics::MeshData* meshData);

        private:
            std::vector<DirectX::XMFLOAT3> normals;

            std::string GetFirstToken(const std::string& line);
            std::string GetLineTail(const std::string& line);

            void StoreVertFromSimpleString(const std::string& str, nc::graphics::MeshData* meshData);
            void StoreNormFromSimpleString(const std::string& str, nc::graphics::MeshData* meshData);
            void StoreIndicesFromVertNormString(const std::string& str, nc::graphics::MeshData* meshData);

            void CallProcessForToken(const std::string& token, const std::string& tail, nc::graphics::MeshData* meshData);

            void ComputeIndependentNormals(nc::graphics::MeshData* meshData);
            void ComputeAveragedNormals(nc::graphics::MeshData* meshData);
            
    };
}