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
#define RESOLUTION  2
#define RENDERDISTANCE 4
#define FRACTALDEPTH 3
#define FREQUENCY .2f
#define POWER 10.0f
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



    points.resize(SIZE,std::vector<float>(SIZE,FLT_MIN));
    std::cout <<"test" << std::endl;

    glGenVertexArrays(1,&VAO);
    glGenBuffers(1,&VBO);
    glGenBuffers(1,&EBO);
    glm::vec3 position = glm::vec3(0.0f,0.0f,0.0f);
    //points.resize(SIZE,std::vector<float>(SIZE,FLT_MIN));
    glm::vec3 playerPosition = position;
    playerPosition.x += SIZE/4;
    playerPosition.y += SIZE/4;
    playerPosition.z += SIZE/4;

    std::vector<Vertex> vertices;
    int i = 0;
    Vertex vertex;
    vertex.Normal = glm::vec3(0,0,0);
    vertex.TexCoords = glm::vec2(0,0);
    //td::cout << playerPosition.x -250 << "    " << playerPosition.x +250 << std::endl;
    std::cout<<"size: " <<((int)round(playerPosition.x) -RENDERDISTANCE)*RESOLUTION <<std::endl;
    float currentFrame = glfwGetTime();
    for (int x = ((int)round(playerPosition.x) -RENDERDISTANCE)*RESOLUTION; x < (playerPosition.x +RENDERDISTANCE)*RESOLUTION; x++ ) {
        for (int z =  ((int)round(playerPosition.z) -RENDERDISTANCE)*RESOLUTION; z < (playerPosition.z +RENDERDISTANCE)*RESOLUTION; z++ ) {
            if(points.at(x).at(z) == FLT_MIN) {
                points.at(x).at(z) = getHeight(x/RESOLUTION,z/RESOLUTION);
            }

            vertex.Position = glm::vec3(x,points.at(x).at(z),z);
            vertices.push_back(vertex);
            if(points.at(x+1 ).at(z) == FLT_MIN) {
                points.at(x+1 ).at(z) = getHeight((float)x+1 ,z);
            }

            vertex.Position = glm::vec3(x/RESOLUTION+1 ,points.at(x+1 ).at(z),z/RESOLUTION);
            vertices.push_back(vertex);

            if( points.at(x).at(z+1 ) == FLT_MIN) {
                points.at(x).at(z+1 ) = getHeight((float)x,(float)z+1);
            }

            vertex.Position = glm::vec3(x/RESOLUTION,points.at(x).at(z+1 ),(z/RESOLUTION)+1 );
            vertices.push_back(vertex);
            if(points.at(x+1 ).at(z+1 ) == FLT_MIN) {
                points.at(x+1 ).at(z+1 ) = getHeight((float)x+1 ,(float)z+1 );
            }

            vertex.Position = glm::vec3((x/RESOLUTION)+1 ,points.at(x+1 ).at(z+1 ),(z/RESOLUTION)+1);
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
        vertice.Position.x -= SIZE/4;
        //vertices[j].Position.y -= SIZE/2;
        vertice.Position.z -= SIZE/4;
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
