#include "Model.h"
#include <iostream>

Model::Model(const std::string& path) {
    loadModel(path);
}

void Model::Draw(Shader& shader) {
    for (unsigned int i = 0; i < meshes.size(); i++)
        meshes[i].Draw(shader);
}

void Model::loadModel(const std::string& path) {
    Assimp::Importer import;
    const aiScene * scene = import.ReadFile(path,
        aiProcess_Triangulate |
        aiProcess_FlipUVs |
        aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
        return;
    }

    size_t slash = path.find_last_of("/\\");
    directory = (slash != std::string::npos) ? path.substr(0, slash) : ".";

    std::cout << "[INFO] £adowanie modelu z: " << path << std::endl;
    std::cout << "[INFO] Katalog tekstur ustawiony na: " << directory << std::endl;


    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene) {
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) {
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    std::vector<Texture> textures;
    int coloredVertexCount = 0;
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;

        vertex.position = glm::vec3(
            mesh->mVertices[i].x,
            mesh->mVertices[i].y,
            mesh->mVertices[i].z
        );
        if (mesh->HasVertexColors(0)) {
            vertex.color = glm::vec3(
                mesh->mColors[0][i].r,
                mesh->mColors[0][i].g,
                mesh->mColors[0][i].b
            );
            coloredVertexCount++;
        }
        else {
            vertex.color = glm::vec3(1.0f); // fallback
        }


        // Jeœli UV istniej¹
        if (mesh->mTextureCoords[0]) {
            vertex.TexUV = glm::vec2(
                mesh->mTextureCoords[0][i].x,
                mesh->mTextureCoords[0][i].y
            );
        }
        else {
            vertex.TexUV = glm::vec2(0.0f);
        }

        vertices.push_back(vertex);
    }
    if (coloredVertexCount > 0) {
        std::cout << "[INFO] Siatka zawiera kolory wierzcho³ków (" << coloredVertexCount << " z " << mesh->mNumVertices << ")\n";
    }
    else {
        std::cout << "[INFO] Brak kolorów wierzcho³ków – u¿ywany fallback (bia³y)\n";
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    if (mesh->mMaterialIndex >= 0) {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    }
    std::cout << "Loaded mesh with " << vertices.size() << " vertices and " << indices.size() << " indices\n";


    return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName) {
    std::vector<Texture> textures;

    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);
        std::string texPath = directory + "/" + std::string(str.C_Str());
        std::ifstream texFile(texPath);
        if (!texFile.good()) {
            std::cerr << "[WARNING] Nie znaleziono tekstury: " << texPath << std::endl;
        }
        else {
            std::cout << "[INFO] Znaleziono teksturê: " << texPath << std::endl;
        }

        std::cout << "Trying to load texture: " << texPath << std::endl;

        Texture texture(texPath.c_str(), typeName.c_str(), GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
        textures.push_back(texture);
    }

    return textures;
}
