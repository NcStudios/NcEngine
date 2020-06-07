#include "ObjLoader.h"
#include "graphics/DirectXMath/Inc/DirectXMath.h"
#include "debug/NCException.h"
#include "graphics/Model.h"

#include <iostream>
#include <algorithm>
<<<<<<< HEAD
#include "DirectXMath.h"

#include "NCException.h"
#include "Model.h"
#include "Mesh.h"
=======
>>>>>>> 0e9bead850f7a4c8ea9e19f46178bbb875064a01

namespace nc::utils
{
    const std::string COMMENT = "#";
    const std::string NAME    = "o";
    const std::string VERTEX  = "v";
    const std::string NORMAL  = "vn";
    const std::string FACE    = "f";

    ObjLoader::ObjLoader()
    {

    }

    ObjLoader::~ObjLoader()
    {

    }

    /*
    Directx uses a "Left Hand Coordinate System". What this means is that the positive direction 
    of the z-axis is facing away from the camera, while the right hand coordinate system's positive
    z-axis is facing towards the camera. positive y is facing up, positive x is facing right. A 
    lot of modeling programs such as maya and 3ds max use a right handed coordinate system. Since 
    directx uses a left had coordinate system, we will need to convert these models into a left 
    hand coordinate system. To convert from right hand to left hand, we must do a couple things. 
    First is invert the z-axis of the vertice's positions by multiplying it with -1.0f. We will 
    also need to invert the v-axis of the texture coordinats by subtracting it from 1.0f. Finally 
    we will need to convert the z-axis of the vertex normals by multiplying it by -1.0f. I have 
    put a boolean variable in the parameters of the load model function, where if it is set to 
    true, we will do the conversion, and if it is set to false we will not do the conversion.
    */


    void ObjLoader::Load(nc::graphics::MeshData* meshData)
    {

        //const float zInvert = invertCoordSystem ? -1.0f : 1.0f; //not currently using this
        std::string meshPath = meshData->MeshPath;
        const auto pathSize = meshPath.size();
        
        if(meshPath.substr(pathSize - 4, pathSize) != ".obj")
        {
            std::cerr << "Invalid file type\n";
            throw nc::DefaultException("Invalid file type");
        }

        std::ifstream infile(meshPath);

        if(!infile)
        {
            std::cerr << "Could not load file: " << meshPath << '\n';
            throw nc::DefaultException("Could not load file");
        }

        std::string currentLine = "";
        
        while(infile)
        {
            std::getline(infile, currentLine);
            
            std::string firstToken = GetFirstToken(currentLine);
            std::string tail = GetLineTail(currentLine);

            CallProcessForToken(firstToken, tail, meshData);
        }

        infile.close();

        ComputeAveragedNormals(meshData); //normals now set when reading face data
    }

    std::string ObjLoader::GetFirstToken(const std::string& line)
    {
        return line.substr(0, line.find(' '));
    }

    std::string ObjLoader::GetLineTail(const std::string& line)
    {
        return line.substr(line.find(' ') + 1, line.size());
    }

    void ObjLoader::StoreVertFromSimpleString(const std::string& str, nc::graphics::MeshData* meshData)
    {
        const auto firstSpace = str.find(' ');
        const auto lastSpace  = str.rfind(' ');
        
        float x = std::stof( str.substr(0, firstSpace) );
        float y = std::stof( str.substr(firstSpace, lastSpace - firstSpace) );
        float z = std::stof( str.substr(lastSpace, str.size() - lastSpace) );

        meshData->Vertices.push_back (graphics::Vertex{ {x,y,z}, {0, 0, 0}} );
        //model.vertices.push_back( graphics::Model::Vertex{ {x,y,z * -1.0f} } );
    }

    void ObjLoader::StoreNormFromSimpleString(const std::string& str, nc::graphics::MeshData* meshData)
    {
        (void)meshData;

        const auto firstSpace = str.find(' ');
        const auto lastSpace  = str.rfind(' ');

        float x = std::stof( str.substr(0, firstSpace) );
        float y = std::stof( str.substr(firstSpace, lastSpace - firstSpace) );
        float z = std::stof( str.substr(lastSpace, str.size() - lastSpace) );

        normals.push_back(DirectX::XMFLOAT3(x,y,z));
        //normals.push_back( {x,y,z * -1.0f} );
    }

    void ObjLoader::StoreIndicesFromVertNormString(const std::string& str, nc::graphics::MeshData* meshData)
    {
        const auto firstSpace = str.find(' ');
        const auto lastSpace  = str.rfind(' ');

        //VertNorm face formatting: vert_index//normal_index (can be different!)
        std::string f1 = str.substr(0, firstSpace);
        std::string f2 = str.substr(firstSpace + 1, lastSpace - firstSpace - 1);
        std::string f3 = str.substr(lastSpace + 1, str.size() - lastSpace - 1);

        //must subtract 1 because face data is indexed from 1 not 0
        auto firstSlash = f1.find('/');
        auto lastSlash  = f1.rfind('/');
        uint16_t p0VertIndex = std::stoi(f1.substr(0, firstSlash)) - 1;
        uint16_t p0NormIndex = std::stoi(f1.substr(lastSlash + 1, f1.size() - lastSlash - 1)) - 1;

        firstSlash = f2.find('/');
        lastSlash  = f2.rfind('/');
        uint16_t p1VertIndex = std::stoi(f2.substr(0, firstSlash)) - 1;
        uint16_t p1NormIndex = std::stoi(f2.substr(lastSlash + 1, f2.size() - lastSlash - 1)) - 1;

        firstSlash = f3.find('/');
        lastSlash  = f3.rfind('/');
        uint16_t p2VertIndex = std::stoi(f3.substr(0, firstSlash)) - 1;
        uint16_t p2NormIndex = std::stoi(f3.substr(lastSlash + 1, f3.size() - lastSlash - 1)) - 1;
        
        //add indices
        meshData->Indices.push_back(p0VertIndex);
        meshData->Indices.push_back(p1VertIndex);
        meshData->Indices.push_back(p2VertIndex);

        //add normal
        meshData->Vertices[p0VertIndex].norm = normals[p0NormIndex];
        meshData->Vertices[p1VertIndex].norm = normals[p1NormIndex];
        meshData->Vertices[p2VertIndex].norm = normals[p2NormIndex];
    }

    void ObjLoader::CallProcessForToken(const std::string& token, const std::string& tail, nc::graphics::MeshData* meshData)
    {
        if(token == COMMENT)
        {
            return;
            //std::cout << "comment found\n";
        }
        else if(token == NAME)
        {
            return;
            //std::cout << "name found\n";
        }
        else if(token == VERTEX)
        {
            StoreVertFromSimpleString(tail, meshData);
            //std::cout << "vertex found\n";
        }
        else if(token == NORMAL)
        {
            //std::cout << "normal found" << std::endl;
            StoreNormFromSimpleString(tail, meshData);
        }
        else if(token == FACE)
        {
            StoreIndicesFromVertNormString(tail, meshData);
            //std::cout << "face found" << std::endl;
        }
        else
        {
            //std::cout << "unkown found\n";
        }
        
    }

    void ObjLoader::ComputeIndependentNormals(nc::graphics::MeshData* meshData)
    {
        for(size_t i = 0; i < meshData->Indices.size() - 1; i += 3)
        {
            auto& v0 = meshData->Vertices[meshData->Indices[i]];
            auto& v1 = meshData->Vertices[meshData->Indices[i+1]];
            auto& v2 = meshData->Vertices[meshData->Indices[i+2]];
            const auto p0 = DirectX::XMLoadFloat3(&v0.pos);
            const auto p1 = DirectX::XMLoadFloat3(&v1.pos);
            const auto p2 = DirectX::XMLoadFloat3(&v2.pos);
            const auto n = DirectX::XMVector3Normalize(DirectX::XMVector3Cross( (p1 - p0), (p2 - p0) ) );
            DirectX::XMStoreFloat3(&v0.norm, n);
            DirectX::XMStoreFloat3(&v1.norm, n);
            DirectX::XMStoreFloat3(&v2.norm, n);
        }
    }

    void ObjLoader::ComputeAveragedNormals(nc::graphics::MeshData* meshData)
    {
        std::for_each(meshData->Vertices.begin(), meshData->Vertices.end(), [](auto& vert) { vert.norm = {0,0,0}; });

        DirectX::XMFLOAT3 zInverse(0, 0, 1);

        for(size_t i = 0; i < meshData->Indices.size() - 1; i+=3)
        {
            auto& v0 = meshData->Vertices[meshData->Indices[i]];
            auto& v1 = meshData->Vertices[meshData->Indices[i+1]];
            auto& v2 = meshData->Vertices[meshData->Indices[i+2]];
            const auto p0 = DirectX::XMLoadFloat3(&v0.pos);
            const auto p1 = DirectX::XMLoadFloat3(&v1.pos);
            const auto p2 = DirectX::XMLoadFloat3(&v2.pos);
            const auto faceNormal = DirectX::XMVector3Normalize(DirectX::XMVector3Cross( (p1 - p0), (p2 - p0) ) );

            auto vn0 = DirectX::XMLoadFloat3(&v0.norm);
            auto vn1 = DirectX::XMLoadFloat3(&v1.norm);
            auto vn2 = DirectX::XMLoadFloat3(&v2.norm);

            auto adjusted_vn0 = vn0 + faceNormal;
            auto adjusted_vn1 = vn1 + faceNormal;
            auto adjusted_vn2 = vn2 + faceNormal;

            DirectX::XMStoreFloat3(&v0.norm, adjusted_vn0);
            DirectX::XMStoreFloat3(&v1.norm, adjusted_vn1);
            DirectX::XMStoreFloat3(&v2.norm, adjusted_vn2);
        }

        std::for_each(meshData->Vertices.begin(), 
                      meshData->Vertices.end(),
                      [](auto& vert)
                      {
                          auto vec_norm = DirectX::XMLoadFloat3(&vert.norm);
                          DirectX::XMVector3Normalize(vec_norm);
                          DirectX::XMStoreFloat3(&vert.norm, vec_norm);
                      });
    }
}

// //duplicate vertices for flat shading
        // if(std::count(model.indices.begin(), model.indices.end(), x))
        // {
        //     auto newVertex = model.vertices.at(x);
        //     model.vertices.push_back(newVertex);
        //     x = model.vertices.size() - 1;
        // }
        // if(std::count(model.indices.begin(), model.indices.end(), y))
        // {
        //     auto newVertex = model.vertices.at(y);
        //     model.vertices.push_back(newVertex);
        //     y = model.vertices.size() - 1;
        // }
        // if(std::count(model.indices.begin(), model.indices.end(), z))
        // {
        //     auto newVertex = model.vertices.at(z);
        //     model.vertices.push_back(newVertex);
        //     z = model.vertices.size() - 1;
        // }

        // //get vetices that the indices refer to
        // auto& v0 = model.vertices[x];
        // auto& v1 = model.vertices[y];
        // auto& v2 = model.vertices[z];
        // const auto p0 = DirectX::XMLoadFloat3(&v0.pos);
        // const auto p1 = DirectX::XMLoadFloat3(&v1.pos);
        // const auto p2 = DirectX::XMLoadFloat3(&v2.pos);

        // //compute center of vertices
        // float centerX = (v0.pos.x + v1.pos.x + v2.pos.x) /3.0f;
        // float centerY = (v0.pos.y + v1.pos.y + v2.pos.y) /3.0f;
        // float centerZ = (v0.pos.z + v1.pos.z + v2.pos.z) /3.0f;
        // DirectX::XMFLOAT3 center = {centerX, centerY, centerZ};
        // DirectX::XMVECTOR vec_center = DirectX::XMLoadFloat3(&center);

        // //get angles between each vertex and center point
        // DirectX::XMVECTOR vec_angleP0 = DirectX::XMVector3AngleBetweenVectors(vec_center, p0);
        // DirectX::XMVECTOR vec_angleP1 = DirectX::XMVector3AngleBetweenVectors(vec_center, p1);
        // DirectX::XMVECTOR vec_angleP2 = DirectX::XMVector3AngleBetweenVectors(vec_center, p2);
        // DirectX::XMFLOAT3 angleP0, angleP1, angleP2;
        // DirectX::XMStoreFloat3(&angleP0, vec_angleP0);
        // DirectX::XMStoreFloat3(&angleP1, vec_angleP1);
        // DirectX::XMStoreFloat3(&angleP2, vec_angleP2);
        
        // //sort indices by largest angle to smallest
        // if(angleP0.x < angleP1.x)
        // {
        //     std::swap(x, y); std::swap(angleP0, angleP1);
        // }
        // if(angleP1.x < angleP2.x)
        // {
        //     std::swap(y, z); std::swap(angleP1, angleP2);
        // }
        // if(angleP0.x < angleP1.x)
        // {
        //     std::swap(x, y); std::swap(angleP0, angleP1);
        // }