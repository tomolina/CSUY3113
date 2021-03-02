#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, marioMatrix, sunMatrix, blockMatrix, cloudMatrix1, cloudMatrix2, cloudMatrix3, ctgMatrix, projectionMatrix;

float marioX = 0;
float marioX2 = 10;
float ctgX = 0;
float playerRotate = 0;

GLuint marioTextureID;
GLuint ctgTextureID;
GLuint cloudTextureID;
GLuint sunTextureID;
GLuint blockTextureID;

//TEXTURES!
GLuint LoadTexture(const char* filePath) {
    int w, h, n;
    unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);
    if (image == NULL) {
        std::cout << "Unable to load image. Make sure the path is correct\n";
        assert(false);
}
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    stbi_image_free(image);
    return textureID;
}


void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Run Away Mario!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 640, 480);
    
    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    
    viewMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    //program.SetColor(1.0f, 0.0f, 0.0f, 1.0f);
    
    glUseProgram(program.programID);
    
    glClearColor(0.0f, 0.7f, 1.0f, 1.0f);
    glEnable(GL_BLEND);
    
    // Good setting for transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    marioTextureID = LoadTexture("Mario.png");
    ctgTextureID = LoadTexture("ctg.png");
    cloudTextureID = LoadTexture("Cloud.png");
    sunTextureID = LoadTexture("DaSun.png");
    blockTextureID = LoadTexture("blocks.png");
    
    
    
}

void ProcessInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            gameIsRunning = false;
        }
    }
}

//Deltatime Practice:
float lastTicks = 0.0f;

void Update() {
    //Deltatime cont.
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;
    
    
    //Orthographic Practice:
//    marioX += 1.5f * deltaTime;
//    marioX2 -= 1.5f * deltaTime;
    ctgX += 1.5f * deltaTime;
    playerRotate += 50.0f * deltaTime;
    
    
    //Implementing Mario!
    marioMatrix = glm::mat4(1.0f);
    marioMatrix = glm::translate(marioMatrix, glm::vec3(-5.0f, -1.0f, 0.0f));
    
   // Translate
    if(marioX > 10.0f){
        marioX -= 1.0f;
        //marioMatrix = glm::translate(marioMatrix, glm::vec3(10.0f, 0.0f, 0.0f));
        marioMatrix = glm::translate(marioMatrix, glm::vec3(marioX2, 0.0f, 0.0f));

    }
    else if (marioX2 < 0.0f){
        marioX2 += 1.0f * deltaTime;
        //marioMatrix = glm::translate(marioMatrix, glm::vec3(10.0f, 0.0f, 0.0f));
        marioMatrix = glm::translate(marioMatrix, glm::vec3(marioX, 0.0f, 0.0f));
    }
    else if(marioX >= 0.0f && marioX <= 10.0f){
        marioX += 1.0f * deltaTime;
        //marioMatrix = glm::translate(marioMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
        marioMatrix = glm::translate(marioMatrix, glm::vec3(marioX, 0.0f, 0.0f));
    }
    else{
        marioMatrix = glm::translate(marioMatrix, glm::vec3(marioX, 0.0f, 0.0f));
    }
    
    
//    if(marioX >= 0.0f && marioX <= 10.0f){
//
//    }
    
    //Implementing our Sun!
    sunMatrix = glm::mat4(1.0f);
    sunMatrix = glm::translate(sunMatrix, glm::vec3(3.75, 2.5f, 0.0f));
    sunMatrix = glm::rotate(sunMatrix, glm::radians(playerRotate), glm::vec3(0.0f, 0.0f, 1.0f));
    sunMatrix = glm::scale(sunMatrix, glm::vec3(1.5f, 1.5f, 1.0f));
    
    //Implementing the floor blocks!
    blockMatrix = glm::mat4(1.0f);
    blockMatrix = glm::scale(blockMatrix, glm::vec3(10.0f, 2.5f, 1.0f));
    blockMatrix = glm::translate(blockMatrix, glm::vec3(0.0f, -1.1f, 1.0f));
    
    //Implementing the clouds!
    cloudMatrix1 = glm::mat4(1.0f);
    cloudMatrix1 = glm::scale(cloudMatrix1, glm::vec3(1.5f, 1.0f, 1.0f));
    cloudMatrix1 = glm::translate(cloudMatrix1, glm::vec3(-2.0f, 2.0f, 1.0f));
    
    cloudMatrix2 = glm::mat4(1.0f);
    cloudMatrix2 = glm::scale(cloudMatrix2, glm::vec3(1.5f, 1.0f, 1.0f));
    cloudMatrix2 = glm::translate(cloudMatrix2, glm::vec3(1.4f, 2.0f, 1.0f));
    
    cloudMatrix3 = glm::mat4(1.0f);
    cloudMatrix3 = glm::scale(cloudMatrix3, glm::vec3(1.5f, 1.0f, 1.0f));
    cloudMatrix3 = glm::translate(cloudMatrix3, glm::vec3(-0.25f, 1.0f, 1.0f));
    
    //Implementing thr Professor!
    ctgMatrix = glm::mat4(1.0f);
    ctgMatrix = glm::scale(ctgMatrix, glm::vec3(1.0f, 1.5f, 1.0f));
    ctgMatrix = glm::translate(ctgMatrix, glm::vec3(17.0f, -0.5f, 1.0f));
    
    if(marioX >= 10.0f){
        ctgMatrix = glm::translate(ctgMatrix, glm::vec3(-ctgX, 0.0f, 0.0f));
    }
    
}

//Rendering for Textures!
void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    program.SetModelMatrix(marioMatrix);
    
    float vertices[]  = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
    
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program.positionAttribute);
    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program.texCoordAttribute);
    
    glBindTexture(GL_TEXTURE_2D, marioTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    program.SetModelMatrix(sunMatrix);
    
    glBindTexture(GL_TEXTURE_2D, sunTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    program.SetModelMatrix(blockMatrix);
    
    glBindTexture(GL_TEXTURE_2D, blockTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    program.SetModelMatrix(cloudMatrix1);
    
    glBindTexture(GL_TEXTURE_2D, cloudTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    program.SetModelMatrix(cloudMatrix2);
    
    glBindTexture(GL_TEXTURE_2D, cloudTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    program.SetModelMatrix(cloudMatrix3);
    
    glBindTexture(GL_TEXTURE_2D, cloudTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    program.SetModelMatrix(ctgMatrix);
    
    glBindTexture(GL_TEXTURE_2D, ctgTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program.positionAttribute);
    glDisableVertexAttribArray(program.texCoordAttribute);
    
    SDL_GL_SwapWindow(displayWindow);
    }



//void Render() {
//    glClear(GL_COLOR_BUFFER_BIT);
//
//    program.SetModelMatrix(modelMatrix);
//
//    float vertices[] = { 0.5f, -0.5f, 0.0f, 0.5f, -0.5f, -0.5f };
//    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
//    glEnableVertexAttribArray(program.positionAttribute);
//    glDrawArrays(GL_TRIANGLES, 0, 3);
//    glDisableVertexAttribArray(program.positionAttribute);
//
//    SDL_GL_SwapWindow(displayWindow);
//}


void Shutdown() {
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    Initialize();
    
    while (gameIsRunning) {
        ProcessInput();
        Update();
        Render();
    }
    
    Shutdown();
    return 0;
}
