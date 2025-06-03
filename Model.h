#ifndef MODEL_CLASS_H
#define MODEL_CLASS_H

#include <vector>
#include <string>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <glm/gtc/type_ptr.hpp>
#include "Mesh.h"
#include "shaderClass.h"

struct Node {
    std::string name;
    glm::mat4 transformation;
    std::vector<Mesh> meshes;
    std::vector<Node> children;
};

class Model {
public:
    Node rootNode;
    Model(const std::string& path);
    void Draw(Shader& shader);
    void drawNode(Node& node, Shader& shader, glm::mat4 parentTransform);
    Node* findNodeByName(Node& node, const std::string& name);
    glm::vec3 getGlobalPosition(Node& node, glm::mat4 parentTransform, const std::string& name);
    void printAllGlobalPositions(Node& node, glm::mat4 parentTransform);

private:
    std::string directory;

    void loadModel(const std::string& path);
    Node processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};

#endif
