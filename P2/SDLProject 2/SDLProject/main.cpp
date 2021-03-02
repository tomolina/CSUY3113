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

#include "Entity.h"



struct GameState{
    Entity *leftPad;
    Entity *square;
    Entity *rightPad;
    
};
GameState state;

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;


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
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_REPEAT);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    stbi_image_free(image);
    return textureID;
}




void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Pong!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
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
    
    glUseProgram(program.programID);
    
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glEnable(GL_BLEND);
    
    // Good setting for transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    //Textures!!
   
    state.square = new Entity();
    
    GLuint squareTextureID = LoadTexture("square.png");
    state.square->textureID = squareTextureID;
    state.square->position = glm::vec3(0.0f, 0.0f, 0.0f);
    state.square->speed = 2.0f;
    
    state.leftPad = new Entity();
    state.leftPad->position = glm::vec3(-5.0f, 0.0f, 0.0f);
    state.leftPad->movement = glm::vec3(0);
    state.leftPad->speed = 3.0f;
    state.leftPad->textureID = LoadTexture("Paddle Copy.png");
    
    state.rightPad = new Entity();
    GLuint rightPadTextureID = LoadTexture("Paddle2.png");
    state.rightPad->textureID = rightPadTextureID;
    state.rightPad->position = glm::vec3(5.0f, 0.0f, 0.0f);
    state.rightPad->movement = glm::vec3(0);
    state.rightPad->speed = 3.0f;
    
    
    
}



// y = 3.25 is max height and y = -3.25 is min height

void ProcessInput() {
   state.leftPad->movement = glm::vec3(0);
   state.rightPad->movement = glm::vec3(0);
    
   //state.square->speed = 3.0f;
   // X and Y positions for our paddles and pong
   float left_X = state.leftPad->position.x;
   float left_Y = state.leftPad->position.y;

   float right_X = state.rightPad->position.x;
   float right_Y = state.rightPad->position.y;

   float square_X = state.square->position.x;
   float square_Y = state.square->position.y;
    
    // Here we randomize how the pong ball will start
   int rand_X = rand() % 4 + 0;
   int rand_Y = rand() % 4 + 0;
    
   float myX_direc[] = {-1.5f, -1.75f, 1.5f, 2.25f, 1.75f};
   float myY_direc[] = {-1.25f, -1.0f, 1.30f, 1.25f, 1.9f};
    
   SDL_Event event;
   while (SDL_PollEvent(&event)) {
       switch (event.type) {

           case SDL_QUIT:
           case SDL_WINDOWEVENT_CLOSE:
               gameIsRunning = false;
               break;
           case SDL_KEYDOWN:
               
               switch (event.key.keysym.sym) {
                   
                   case SDLK_SPACE:
                       //state.square->speed = 3.0f;
                       state.square->movement = glm::vec3(myX_direc[rand_X], myY_direc[rand_Y],0.0f);
                       break;
               }
               break; // SDL_KEYDOWN
       }
       
   }
    
    //Creating borders for pong to bounce off of
    if(state.square->position.y > 3.6f){
        state.square->movement.y *= -1.0f;
    }
    else if(state.square->position.y < -3.6f){
        state.square->movement.y *= -1.0f;
    }
    
    //Game Over
    if(state.square->position.x >= 4.9f){
        state.square->movement = glm::vec3(0);
    }
    else if(state.square->position.x <= -4.9f){
        state.square->movement = glm::vec3(0);
    }
        
    
    //Collision!!
    float leftSquareDist_X = fabs(left_X - square_X) - ((1 + 1)/2.0f);
    float leftSquareDist_Y = fabs(left_Y - square_Y) - ((1 + 1)/2.0f);
    
    float rightSquareDist_X = fabs(right_X - square_X) - ((1 + 1)/2.0f);
    float rightSquareDist_Y = fabs(right_Y - square_Y) - ((1 + 1)/2.0f);
    
    if(leftSquareDist_X < -.7 && leftSquareDist_Y < -.7){
        state.square->movement.x *= -1.0f;
    }
    if (rightSquareDist_X < -.7 && rightSquareDist_Y < -.7) {
        state.square->movement.x *= -1.0f;
    }
    
    
    
   const Uint8 *keys = SDL_GetKeyboardState(NULL);
    
   if (keys[SDL_SCANCODE_UP]) {
       if(state.rightPad->position.y >= 3.30f){
           state.rightPad->movement.y = 0.0f;
       }
       else{
           state.rightPad->movement.y = 1.5f;
       }
   }
   else if (keys[SDL_SCANCODE_DOWN]) {
       if(state.rightPad->position.y <= -3.30f){
           state.rightPad->movement.y = 0.0f;
       }
       else{
           state.rightPad->movement.y = -1.5f;
       }
   }
    
    if (keys[SDL_SCANCODE_W]) {
        if(state.leftPad->position.y >= 3.30f){
            state.leftPad->movement.y = 0.0f;
        }
        else{
            state.leftPad->movement.y = 2.5f;
        }

    }
    else if (keys[SDL_SCANCODE_S]) {
        if(state.leftPad->position.y <= -3.30f){
            state.leftPad->movement.y = 0.0f;
        }
        else{
            state.leftPad->movement.y = -2.5f;
        }
       
    }
    if (glm::length(state.leftPad->movement) > 1.0f) {
        state.leftPad->movement = glm::normalize(state.leftPad->movement);
    }
}

//Deltatime Practice:
float lastTicks = 0.0f;

void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;
    

    state.square->Update(deltaTime);
    state.leftPad->Update(deltaTime);
    state.rightPad->Update(deltaTime);
    

}

//Rendering for Textures!
void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    state.square->Render(&program);
    state.leftPad->Render(&program);
    state.rightPad->Render(&program);
    
    SDL_GL_SwapWindow(displayWindow);
    }


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
