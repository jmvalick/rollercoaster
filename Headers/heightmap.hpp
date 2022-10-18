#ifndef HEIGHTMAP_H
#define HEIGHTMAP_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <iostream>

#include <shader.hpp>

// Reference: https://github.com/nothings/stb/blob/master/stb_image.h#L4
// To use stb_image, add this in *one* C++ source file.
#include <stb_image.h>

struct Vertex {
	// position
	glm::vec3 Position;
	// position
	glm::vec3 Normal;
	// texCoords
	glm::vec2 TexCoords;
};

class Heightmap
{
public:

	// constructor
	Heightmap(const char* heightmapPath)
	{
		// load Heightmap data
		load_heightmap(heightmapPath);

		// create Heightmap verts from the data
		create_heightmap();

		// free image data once no longer needed
		stbi_image_free(data);

		// Now create indices
		create_indices();

		// Create buffers for rendering
		setup_heightmap();
	}

	// render the mesh
	void Draw(Shader shader, unsigned int textureID)
	{
		// Set the shader properties
		shader.use();
		glm::mat4 heightmap_model;
		heightmap_model = glm::translate(heightmap_model, glm::vec3(0.0f, -10.0f, 0.0f));
		heightmap_model = glm::scale(heightmap_model, glm::vec3(20.0f, 10.0f, 20.0f));
		shader.setMat4("model", heightmap_model);

		// Set material properties
		shader.setVec3("material.specular", 0.3f, 0.3f, 0.3f);
		shader.setFloat("material.shininess", 64.0f);

		// activate proper texture unit before binding
		glActiveTexture(GL_TEXTURE0);
		// and finally bind the textures
		glBindTexture(GL_TEXTURE_2D, textureID);

		// draw mesh using EBO
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		// always good practice to set everything back to defaults once configured.
		glActiveTexture(GL_TEXTURE0);
	}

	/*
	Perform cleanup by deleting the buffers
	*/
	void delete_buffers()
	{
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
	}

private:

	// Render data
	unsigned int VAO, VBO, EBO;
	//Heightmap attributes
	int width, height, nrChannels;
	// Pointer to input data buffer
	unsigned char* data;
	// Heightmap data
	std::vector<Vertex> vertices;
	// indices for EBO
	std::vector<unsigned int> indices;


	/*
	Load data from the heightmap, saving important metadata
	*/
	void load_heightmap(const char* heightmapPath)
	{
		// Save number of channels to avoid problems with 8-bit vs. 24-bit vs. 32-bit images
		data = stbi_load(heightmapPath, &width, &height, &nrChannels, 0);
		if (!data)
		{
			std::cout << "Failed to load heightmap" << std::endl;
		}
	}



	/*
	Helper function for creating individual vertices
	*/
	Vertex make_vertex(int img_x, int img_y)
	{
		Vertex v;
		//XYZ coords
		v.Position.x = 2.0f*(float(img_x) / float(width - 1)) - 1.0f;
		v.Position.y = float(data[(img_x *width + img_y)*nrChannels]) / 255.0f;
		v.Position.z = 2.0f*(float(img_y) / float(height - 1)) - 1.0f;

		// Setting normal to default, calculate later.  
		v.Normal = glm::vec3(0.0f, 0.0f, 0.0f);

		//Texture Coords
		v.TexCoords.x = float(img_x) / float(width - 1);
		v.TexCoords.y = float(img_y) / float(height - 1);
		return v;
	}


	/*
	Create all vertices for the heightmap by iterating 
	Vertices made
	*/
	void create_heightmap()
	{
		// convert heightmap to floats and set texture coordinates
		for (int x = 0; x < width; x++)
		{
			for (int y = 0; y < height; y++)
			{
				vertices.push_back(make_vertex(x, y));
			}

		}
	}

	// Find the normal for each triangle uisng the cross product and then add it to all three vertices of the triangle.  
	//   The normalization of all the triangles happens in the shader which averages all norms of adjacent triangles.   
	//   Order of the triangles matters here since you want to normal facing out of the object.  
	void set_normals(Vertex &p1, Vertex &p2, Vertex &p3)
	{
		glm::vec3 normal =  glm::cross(p2.Position - p1.Position, p3.Position - p1.Position);
		p1.Normal += normal;
		p2.Normal += normal;
		p3.Normal += normal;
	}

	/*
	Create all indices for the heightmap
	*/
	void create_indices()
	{
		// convert heightmap to floats and set texture coordinates.  Also set normals for each triangle we define.
		for (int x = 0; x < width - 1; x++)
		{
			for (int y = 0; y < height - 1; y++)
			{
				// Get indices of square "cell" in heightmap setup
				unsigned int a, b, c, d;
				a = x*width + y;
				b = x*width + y + 1;
				c = (x + 1)*width + y;
				d = (x + 1)*width + y + 1;

				// Triangle 1
				indices.push_back(a); // 0
				indices.push_back(b); // 1
				indices.push_back(c); // 3

				//while here, add normals
				set_normals(vertices[a], vertices[b], vertices[c]);

				// Triangle 2
				indices.push_back(b); // 1
				indices.push_back(d); // 2
				indices.push_back(c); // 3

				//And again, add normals. 
				set_normals(vertices[b], vertices[d], vertices[c]);
 
			}

		}
	}
	

	void setup_heightmap()
	{
		// create buffers/arrays
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);
		// load data into vertex buffers
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		// A great thing about structs is that their memory layout is sequential for all its items.
		// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/3/2 array which
		// again translates to 3/3/2 floats which translates to a byte array.
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

		// set the vertex attribute pointers
		// vertex Positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		// vertex normal coords
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

		// vertex texture coords
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

		glBindVertexArray(0);
	}

};
#endif
