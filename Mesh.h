#ifndef MESH_CLASS_H
#define MESH_CLASS_H

#include<string>

#include"VAO.h"
#include"EBO.h"
#include"Texture.h"

//Tworzymy klase mesh przechowuj¹ca dane wierzcholkow, struktura to jest mesh -> VAO,texture
class Mesh {
public:
	std::vector <Vertex> vertices;
	std::vector <GLuint> indices;
	std::vector <Texture> textures;

	VAO VAO;
	
	Mesh(std::vector <Vertex>& vertices, std::vector <GLuint>& indices, std::vector <Texture> &textures);

	void Draw(Shader &Shader);

};

#endif // !MESH_CLASS_H

