#include "glad/glad.h"
#include "Shader.h"
#include <GLFW/glfw3.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "stb_image.h"
#define STB_IMAGE_IMPLEMENTATION

#include "Camera.h"
#include "Model.h"
#include <glm/gtc/noise.hpp>
#include "Terrain.h"
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
unsigned  int loadTexture(const char* path);
void DrawTerrain(Shader ourShader);
//settings
const unsigned int screenWidth = 600;
const unsigned int screenHeight = 600;

float lastX= (float)screenWidth/2, lastY = (float)screenHeight/2;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
bool wireframes{false};
glm::vec3 lightPos(0.8f,1.6f,1.5f);

Camera camera(glm::vec3(0.0f,0.0f,3.0f));

//glm

bool firstMouse = true;
int main() {
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif
    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(screenWidth,screenHeight, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------

    //csvHelper csvHelper("/home/raffael/Documents/Schule/19-20/ANDERE PROJEKTE/Opengl/UNO5_Linux/src/data/SRTM_RAMP2_TOPO_2000-02-11_gs_3600x1800.SS.CSV");

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    auto terrain = new Terrain();

    // build and compile our shader program
    // ------------------------------------

    Model ourModel("/home/raffael/Documents/Schule/19-20/ANDERE PROJEKTE/Opengl/UNO5_Linux/src/textures/nanosuit/nanosuit.obj");
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    // render loop:
    // -----------
    Shader ourShader("/home/raffael/Documents/Schule/19-20/ANDERE PROJEKTE/Opengl/UNO5_Linux/src/shader/model.vs", "/home/raffael/Documents/Schule/19-20/ANDERE PROJEKTE/Opengl/UNO5_Linux/src/shader/model.fs");

    //std::cout<< (1.0f/3.0f) <<std::endl;
    //std::cout <<terrain->getHeight(2,3)<< std::endl;
    while (!glfwWindowShouldClose(window))
    {

        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        //std::cout<< 1/deltaTime << std::endl;
        // input
        // -----
        processInput(window);
        //lightmovement
        //lightPos.x = 1.0f + sin(glfwGetTime())*2;
        //lightPos.y = 0.5f + sin(glfwGetTime()/2) *2;

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // be sure to activate shader when setting uniforms/drawing objects

        std::cout << "X: "<<camera.Front.x << " Y: " << camera.Front.y << " Z:" << camera.Front.z << std::endl;
        ourShader.use();
        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);
        /*
        // world transformation
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	// it's a bit too big for our scene, so scale it down
        ourShader.setMat4("model", model);
        ourModel.Draw(ourShader);
        //lightingShader.setMat3("normalInverse",glm::mat3(transpose(inverse(model))));
        //lightingShader.setMat4("model", model);
        */
        // render the loaded model

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f)); // translate it down so it's at the center of the scene
        //model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	// it's a bit too big for our scene, so scale it down
        ourShader.setMat4("model", model);
        ourModel.Draw(ourShader);

        terrain->Draw(camera.Position, ourShader);
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------

        glfwSwapBuffers(window);
        glfwPollEvents();
        //std::cout << "fps:" << 1/deltaTime << std::endl;
    }
    delete(terrain);
    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;

}
void processInput(GLFWwindow *window){

    if(glfwGetKey(window,GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwWindowShouldClose(window);
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD,deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) ==  GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD,deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT,deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT,deltaTime);
    if(glfwGetKey(window,GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS){
        camera.ProcessKeyboard(DOWN,deltaTime);
    }
    if(glfwGetKey(window,GLFW_KEY_SPACE) == GLFW_PRESS){
        camera.ProcessKeyboard(UP,deltaTime);
    }
    if (glfwGetKey(window,GLFW_KEY_TAB) == GLFW_PRESS) {
        wireframes = !wireframes;
        if (wireframes)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
glViewport(0,0,width,height);
}
void mouse_callback(GLFWwindow* window, double xpos, double ypos){

    if(firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset,yoffset);

}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset){
   camera.ProcessMouseScroll(yoffset);
}
unsigned int loadTexture(char const * path)
{

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

void DrawTerrain(Shader ourShader){

    unsigned int VAO,VBO;
    std::vector<Vertex> vertices;
    Vertex vertex;
    for (float x = camera.Position.x - 250; x < camera.Position.x + 250; x = x+0.1f) {
        for (float z = camera.Position.z - 250; z < camera.Position.z + 250; z = z+0.1f) {
            vertex.Position.x = x;
            vertex.Position.y =  glm::perlin(glm::vec2(x, z));;
            vertex.Position.z = z;

            vertex.Normal.x = 0;
            vertex.Normal.y = 0;
            vertex.Normal.z = 0;

            vertex.TexCoords.x = x;
            vertex.TexCoords.y = z;
            vertices.push_back(vertex);
        }
    }
    glGenVertexArrays(1,&VAO);
    glGenBuffers(1,&VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);

    glBufferData(GL_ARRAY_BUFFER,vertices.size()* sizeof(Vertex),&vertices[0], GL_STATIC_DRAW);

    // vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

    glBindVertexArray(0);


    ourShader.use();
    // view/projection transformations
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
    glm::mat4 view = camera.GetViewMatrix();
    ourShader.setMat4("projection", projection);
    ourShader.setMat4("view", view);

    // world transformation
    glm::mat4 model = glm::mat4(1.0f);
    //model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
    ourShader.setMat4("model", model);


    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES,0,vertices.size());
}