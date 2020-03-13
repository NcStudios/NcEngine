#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <stdint.h>

namespace nc::graphics { class MeshType; }
namespace DirectX {struct XMFLOAT3; }

namespace nc::utils
{
    class ObjLoader
    {
        public:
            ObjLoader();
            ~ObjLoader();


            void Load(nc::graphics::MeshType* mesh);

        private:
            std::vector<DirectX::XMFLOAT3> normals;

            std::string GetFirstToken(const std::string& line);
            std::string GetLineTail(const std::string& line);

            void StoreVertFromSimpleString(const std::string& str, nc::graphics::MeshType* mesh);
            void StoreNormFromSimpleString(const std::string& str, nc::graphics::MeshType* mesh);
            void StoreIndicesFromVertNormString(const std::string& str, nc::graphics::MeshType* mesh);

            void CallProcessForToken(const std::string& token, const std::string& tail, nc::graphics::MeshType* mesh);

            void ComputeIndependentNormals(nc::graphics::MeshType* mesh);
            void ComputeAveragedNormals(nc::graphics::MeshType* mesh);
            
    };
}