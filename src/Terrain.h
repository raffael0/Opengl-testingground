//
// Created by raffael on 11/22/19.
//

#ifndef UNO5_LINUX_TERRAIN_H
#define UNO5_LINUX_TERRAIN_H

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <vector>
#include <glm/gtc/noise.hpp>
#include <glad/glad.h>

#define SIZE 10000
#define RESOLUTION  0.3
#define RENDERDISTANCE 50
#define FRACTALDEPTH 3
#define FREQUENCY .2f
#define POWER 10.0f

class Terrain{
public: Terrain();
    void Draw(glm::vec3 playerPosition,Shader shader);
private:
    std::vector<std::vector<float>> points;
    struct Vertex{
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoords;
    };
    unsigned int VAO,VBO,EBO;
    float lastFrame;

private:
    float getHeight(float x,float z);

};

Terrain::Terrain() {
    points.resize(SIZE,std::vector<float>(SIZE,FLT_MIN));

    glGenVertexArrays(1,&VAO);
    glGenBuffers(1,&VBO);
    glGenBuffers(1,&EBO);

    //glBindVertexArray(0);
}

void Terrain::Draw(glm::vec3 position, Shader ourShader) {
    glm::vec3 playerPosition = position;
    playerPosition.x += SIZE/2;
    playerPosition.y += SIZE/2;
    playerPosition.z += SIZE/2;

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    int i = 0;
    Vertex vertex;
    vertex.Normal = glm::vec3(0,0,0);
    vertex.TexCoords = glm::vec2(0,0);
    //td::cout << playerPosition.x -250 << "    " << playerPosition.x +250 << std::endl;

    float currentFrame = glfwGetTime();
    for (float x = playerPosition.x -RENDERDISTANCE -.2f; x < (playerPosition.x +RENDERDISTANCE + .2f); x=x+RESOLUTION ) {
        for (float z =  playerPosition.z -RENDERDISTANCE -.2f; z < (playerPosition.z +RENDERDISTANCE +.2f); z=z+RESOLUTION ) {
            if(points.at(x).at(z) == FLT_MIN) {
                points.at(x).at(z) = glm::perlin(glm::vec2(x,z))*3;
            }

            vertex.Position = glm::vec3(x,points.at(x).at(z),z);
            vertices.push_back(vertex);
            if(points.at(x+RESOLUTION ).at(z) == FLT_MIN) {
                points.at(x+RESOLUTION ).at(z) = glm::perlin(glm::vec2(x+RESOLUTION ,z));
            }

            vertex.Position = glm::vec3(x+RESOLUTION ,points.at(x+RESOLUTION ).at(z),z);
            vertices.push_back(vertex);

            if( points.at(x).at(z+RESOLUTION ) == FLT_MIN) {
                points.at(x).at(z+RESOLUTION ) = glm::perlin(glm::vec2(x,z+RESOLUTION ));
            }

            vertex.Position = glm::vec3(x,points.at(x).at(z+RESOLUTION ),z+RESOLUTION );
            vertices.push_back(vertex);
            if(points.at(x+RESOLUTION ).at(z+RESOLUTION ) == FLT_MIN) {
                points.at(x+RESOLUTION ).at(z+RESOLUTION ) = glm::perlin(glm::vec2(x+RESOLUTION ,z+RESOLUTION ));
            }

            vertex.Position = glm::vec3(x+RESOLUTION ,points.at(x+RESOLUTION ).at(z+RESOLUTION ),z+RESOLUTION);
            vertices.push_back(vertex);
            indices.push_back(i);
            indices.push_back(i+3);
            indices.push_back(i+1);
            indices.push_back(i+3);
            indices.push_back(i);
            indices.push_back(i+2);
            i = i+4;
        }


    }
    float deltaTime =  glfwGetTime()- currentFrame ;
    std::cout<<1/deltaTime<< std::endl;

    for (auto & vertice : vertices) {
        vertice.Position.x -= SIZE/2;
        //vertices[j].Position.y -= SIZE/2;
        vertice.Position.z -= SIZE/2;
    }
    std::cout << vertices.size()<<std::endl;

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);

    glBufferData(GL_ARRAY_BUFFER,vertices.size()* sizeof(Vertex),&vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()* sizeof(unsigned int),
                 &indices[0],GL_STATIC_DRAW);

    // vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)nullptr);
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

    glBindVertexArray(0);
    ourShader.use();
    // world transformation
    glm::mat4 model = glm::mat4(1.0f);
    //model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
    model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));	// it's a bit too big for our scene, so scale it down
    ourShader.setMat4("model", model);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES,indices.size(),GL_UNSIGNED_INT,0);

}
//This function uses generates the Height for a given x and z value
float Terrain::getHeight(float x,float z) {
    float height= 0;
    for (int i = 0; i < FRACTALDEPTH; ++i) {
        height += (1.0f/i)*glm::perlin(glm::vec2(FREQUENCY*x*i, FREQUENCY * z*i));
    }
    height = powf(height,POWER);
    return 0;
}

#endif //UNO5_LINUX_TERRAIN_H
