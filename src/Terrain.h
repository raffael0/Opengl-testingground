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

#define SIZE 41000
#define RESOLUTION  10
#define RENDERDISTANCE 30
#define FRACTALDEPTH 4
#define FREQUENCY .2f
#define POWER 3.0f
#define SCALE 1.0f
class Terrain{

public:
    Terrain();
    void Draw(glm::vec3 playerPosition,Shader shader);
    float getHeight(float x,float z);

private:
    std::vector<std::vector<float>> points;
    struct Vertex{
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoords;
    };
    unsigned int VAO,VBO,EBO;
    float lastFrame;
    std::vector<unsigned int> indices;

private:

};

Terrain::Terrain() {
    glGenVertexArrays(1,&VAO);
    glGenBuffers(1,&VBO);
    glGenBuffers(1,&EBO);

    glm::vec3 position = glm::vec3(0.0f,0.0f,0.0f);
    glm::vec3 playerPosition = position;

    std::vector<Vertex> vertices;
    int i = 0;
    Vertex vertex;
    vertex.Normal = glm::vec3(0,0,0);
    vertex.TexCoords = glm::vec2(0,0);

    for (float x = (round(playerPosition.x) -RENDERDISTANCE); x < (playerPosition.x +RENDERDISTANCE); x=x+(1.0f/RESOLUTION) ) {
        for (float z =  (round(playerPosition.z) -RENDERDISTANCE); z < (playerPosition.z +RENDERDISTANCE); z=z+(1.0f/RESOLUTION) ) {

            vertex.Position = glm::vec3(x,getHeight(x,z),z);
            vertices.push_back(vertex);
            vertex.Position = glm::vec3(x+(1.0f/RESOLUTION) ,getHeight(x+(1.0f/RESOLUTION),z),z);
            vertices.push_back(vertex);
            vertex.Position = glm::vec3(x,getHeight(x,z+(1.0f/RESOLUTION)),(z)+(1.0f/RESOLUTION) );
            vertices.push_back(vertex);
            vertex.Position = glm::vec3(x+(1.0f/RESOLUTION) ,getHeight(x+(1.0f/RESOLUTION),z+(1.0f/RESOLUTION)),z+(1.0f/RESOLUTION));
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



    //glBindVertexArray(0);

}

void Terrain::Draw(glm::vec3 position, Shader ourShader) {



   
    glBindVertexArray(0);
    ourShader.use();
    // world transformation
    glm::mat4 model = glm::mat4(1.0f);
    //model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
    model = glm::scale(model, glm::vec3(SCALE, SCALE, SCALE));	// it's a bit too big for our scene, so scale it down
    ourShader.setMat4("model", model);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES,indices.size(),GL_UNSIGNED_INT,0);



    //for(auto & a : points){
    //    a.clear();
    //}
    //glDeleteBuffers(1,&EBO);
    //glDeleteBuffers(1,&VBO);
    //glDeleteVertexArrays(1,&VAO);
    //points.clear();
}

//This function uses generates the Height for a given x and z value
float Terrain::getHeight(float x,float z) {
    //return glm::perlin(glm::vec2(x,z));
    x+=0.2f;
    z+=0.2f;
    float height= 0;
    for (float i = 1; i < FRACTALDEPTH+1; ++i) {
        height += (1.0f/i)*abs(glm::perlin(glm::vec2(FREQUENCY*x*i, FREQUENCY * z*i)));
    }
    height = powf(height,POWER);
    return height;

}

#endif //UNO5_LINUX_TERRAIN_H
