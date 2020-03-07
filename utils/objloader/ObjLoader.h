#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <stdint.h>

namespace nc::graphics { class Model; }
namespace DirectX {struct XMFLOAT3; }

namespace nc::utils
{
    class ObjLoader
    {
        public:
            ObjLoader();
            ~ObjLoader();

            nc::graphics::Model Load(const std::string& path);

        private:
            std::vector<DirectX::XMFLOAT3> normals;

            std::string GetFirstToken(const std::string& line);
            std::string GetLineTail(const std::string& line);

            void StoreVertFromSimpleString(const std::string& str, nc::graphics::Model& model);
            void StoreNormFromSimpleString(const std::string& str, nc::graphics::Model& model);
            void StoreIndicesFromVertNormString(const std::string& str, nc::graphics::Model& model);

            void CallProcessForToken(const std::string& token, const std::string& tail, nc::graphics::Model& model);

            void ComputeIndependentNormals(nc::graphics::Model& model);
            void ComputeAveragedNormals(nc::graphics::Model& model);
            
    };
}