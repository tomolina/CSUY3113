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
#include <SDL_mixer.h>

#include "Util.h"
#include "Map.h"
#include "Entity.h"
#include "Scene.h"

#include "OpenScreen.h"
#include "Level1.h"
#include "Level2.h"
#include "Level3.h"

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

Scene *currentScene;
Scene *sceneList[4];

Mix_Music *music;
Mix_Chunk *bounce;

int startFlag = 0;

void SwitchToScene(Scene *scene) {
    currentScene = scene;
    currentScene->Initialize();
}

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Plataforma", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 640, 480);
    
    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    
    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    
    glUseProgram(program.programID);
    glClearColor(0.1f, 0.0f, 0.1f, 1.0f);
    
    
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    sceneList[0] = new OScreen();
    sceneList[1] = new Level1();
    sceneList[2] = new Level2();
    sceneList[3] = new Level3();
    SwitchToScene(sceneList[0]);
    
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    music = Mix_LoadMUS("crypto.mp3");
    Mix_PlayMusic(music, -1);
    Mix_VolumeMusic(MIX_MAX_VOLUME / 4);
    
    bounce = Mix_LoadWAV("bounce.wav");
    Mix_Volume(-1, MIX_MAX_VOLUME / 4);
    
    GLuint heartText = Util::LoadTexture("fullHeart.png");
    
    currentScene->state.hearts = new Entity();
    currentScene->state.hearts->entityType = PLAYER;
    currentScene->state.hearts->textureID = heartText;
    currentScene->state.hearts->position = glm::vec3(6.0f, -1.5f, 0);
    
    
}


void ProcessInput() {
    currentScene->state.player->movement = glm::vec3(0);
    
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                gameIsRunning = false;
                break;
                
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                        
                    case SDLK_LEFT:
                        break;
                    case SDLK_RETURN:
                        if(startFlag == 0){
                            currentScene->state.nextScene = 1;
                            startFlag += 1;
                        }
                        break;
                    case SDLK_SPACE:
                        if(currentScene->state.player->collidedBottom){
                            currentScene->state.player->jump = true;
                            Mix_PlayChannel(-1, bounce, 0);
                        }
                        
                        
                        break;

                }
                break; // SDL_KEYDOWN
        }
    }
    
    
    const Uint8 *keys = SDL_GetKeyboardState(NULL);

    if ((keys[SDL_SCANCODE_LEFT]) && (gameOver == 0) && (winner == 0)){
        currentScene->state.player->movement.x = -1.0f;
        
        if ((currentScene->state.player->position.x > 5)  && (!currentScene->state.player->collidedLeft)) {
            currentScene->state.hearts[0].position.x -= .05f;
            currentScene->state.hearts[1].position.x -= .05f;
            currentScene->state.hearts[2].position.x -= .05f;
        }
        currentScene->state.player->animIndices = currentScene->state.player->animLeft;
    }
    
    else if ((keys[SDL_SCANCODE_RIGHT]) && (gameOver == 0) && (winner == 0)) {
        currentScene->state.player->movement.x = 1.0f;
        
        if ((currentScene->state.player->position.x > 5) && (!currentScene->state.player->collidedRight)){
            currentScene->state.hearts[0].position.x += .05f;
            currentScene->state.hearts[1].position.x += .05f;
            currentScene->state.hearts[2].position.x += .05f;
        }
        
        currentScene->state.player->animIndices = currentScene->state.player->animRight;
    }
    
    if (glm::length(currentScene->state.player->movement) > 1.0f) {
        currentScene->state.player->movement = glm::normalize(currentScene->state.player->movement);
    }
    
    if((currentScene->state.player->collidedTop_Enemy) || (currentScene->state.player->collidedLeft_Enemy) || (currentScene->state.player->collidedRight_Enemy)){
        currentScene->state.player->position.x = 1.0f;
        currentScene->state.player->position.y = -2;
        
        currentScene->state.hearts[0].position.x = 8.0f;
        currentScene->state.hearts[1].position.x = 8.5f;
        currentScene->state.hearts[2].position.x = 9.0f;
        
        heartCount -= 1;
        
    }
    
}


# define FIXED_TIMESTEP 0.0166666f
float lastTicks = 0;
float accumulator = 0.0f;

void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;
    deltaTime += accumulator;
    
    if (deltaTime < FIXED_TIMESTEP) {
        accumulator = deltaTime;
        return;
    }
    
    while (deltaTime >= FIXED_TIMESTEP) {
        // Update. Notice it's FIXED_TIMESTEP. Not deltaTime
        currentScene->Update(FIXED_TIMESTEP);
        
        deltaTime -= FIXED_TIMESTEP;
    }
    accumulator = deltaTime;

    viewMatrix = glm::mat4(1.0f);
    
    if (currentScene->state.player->position.x > 5) {
        viewMatrix = glm::translate(viewMatrix, glm::vec3(-currentScene->state.player->position.x, 3.75, 0));
    }

    else {
        viewMatrix = glm::translate(viewMatrix, glm::vec3(-5, 3.75, 0));
       // viewMatrix = glm::rotate(viewMatrix, .45f, glm::vec3(1.0f));
    }
    

}


void Render() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Conditional statements that decide if the mission was passed or failed
    // Text is then printed accordingly
    //enemyCount == 3
    
    program.SetViewMatrix(viewMatrix);
    currentScene->Render(&program);
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
        
        if (currentScene->state.nextScene >= 0){
            SwitchToScene(sceneList[currentScene->state.nextScene]);
        }
        
        Render();
    }
    
    Shutdown();
    return 0;
}
