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

#define PLATFORM_COUNT 43
#define WINNER_COUNT  2

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <vector>

#include "Entity.h"

struct GameState {
    Entity *player;
    Entity *platforms;
    Entity *correctPlat;
};

GameState state;

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

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
    displayWindow = SDL_CreateWindow("Plant the Pine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
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
    
    glClearColor(0.0f, 0.5f, 0.5f, 1.0f);
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
   
    // Initialize Game Objects
    
    // Initialize Player
    state.player = new Entity();
    state.player->position = glm::vec3(0, 5.0f, 0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, -0.05f, 0);
    
    state.player->speed = 1.0f;
    state.player->textureID = LoadTexture("pine.png");
    
    state.player->height = .8f;
    state.player->width = .6f;
    
    state.player->jumpPower = 2.0f;
    
    state.correctPlat = new Entity[WINNER_COUNT];
    state.platforms = new Entity[PLATFORM_COUNT];
    
    GLuint platformTextureID1 = LoadTexture("platformPack_tile001.png");
    GLuint platformTextureID2 = LoadTexture("platformPack_tile003.png");

    
    // Below I make it eaasier to position the ground and walls, in loops
    int groundPlat = -5.0f;
    int wallPlat1 = -3.6f;
    int wallPlat2 = -3.6f;
    
    for(int i = 0; i <= 8; i++){
        state.platforms[i].textureID = platformTextureID2;
        state.platforms[i].position = glm::vec3(-5.0f, wallPlat1, 0);
        wallPlat1 += 1;
    }
    
    for(int i = 9; i <= 17; i++){
        state.platforms[i].textureID = platformTextureID2;
        state.platforms[i].position = glm::vec3(5.0f, wallPlat2, 0);
        wallPlat2 += 1;
    }
    
    for(int i = 18; i <= 24; i++){
        state.platforms[i].textureID = platformTextureID2;
        state.platforms[i].position = glm::vec3(groundPlat, -3.75f, 0);
        groundPlat += 1;
    }
    
    
    // These next two are the special blocks
    state.correctPlat[0].textureID = platformTextureID1;
    state.correctPlat[0].position = glm::vec3(groundPlat, -3.75f, 0);
    
    state.correctPlat[1].textureID = platformTextureID1;
    state.correctPlat[1].position = glm::vec3(groundPlat + 1, -3.75f, 0);
    
    //From here on, I customize the placement of blocks one by one
    state.platforms[25].textureID = platformTextureID2;
    state.platforms[25].position = glm::vec3(-3, 1.0f, 0);
    
    state.platforms[26].textureID = platformTextureID2;
    state.platforms[26].position = glm::vec3(-4, 1.0f, 0);
    
    state.platforms[27].textureID = platformTextureID2;
    state.platforms[27].position = glm::vec3(groundPlat + 2, -3.75f, 0);
    
    state.platforms[28].textureID = platformTextureID2;
    state.platforms[28].position = glm::vec3(groundPlat + 3 , -3.75f, 0);
    
    state.platforms[29].textureID = platformTextureID2;
    state.platforms[29].position = glm::vec3(-4.0f,2.0f, 0);
    
    state.platforms[30].textureID = platformTextureID2;
    state.platforms[30].position = glm::vec3(4,3.0f, 0);

    state.platforms[31].textureID = platformTextureID2;
    state.platforms[31].position = glm::vec3(4,2.0f, 0);

    state.platforms[32].textureID = platformTextureID2;
    state.platforms[32].position = glm::vec3(3, 2.0f, 0);

    state.platforms[33].textureID = platformTextureID2;
    state.platforms[33].position = glm::vec3(4, 1.0f, 0);
    
    state.platforms[34].textureID = platformTextureID2;
    state.platforms[34].position = glm::vec3(3, 1.0f, 0);

    state.platforms[35].textureID = platformTextureID2;
    state.platforms[35].position = glm::vec3(2, 1.0f, 0);

    state.platforms[36].textureID = platformTextureID2;
    state.platforms[36].position = glm::vec3(-2, 1.0f, 0);

    state.platforms[37].textureID = platformTextureID2;
    state.platforms[37].position = glm::vec3(-3, 2.0f, 0);

    state.platforms[38].textureID = platformTextureID2;
    state.platforms[38].position = glm::vec3(0, -1.0f, 0);
    
    state.platforms[39].textureID = platformTextureID2;
    state.platforms[39].position = glm::vec3(-1, -1.0f, 0);
    
    state.platforms[40].textureID = platformTextureID2;
    state.platforms[40].position = glm::vec3(-2, -1.0f, 0);
    
    state.platforms[41].textureID = platformTextureID2;
    state.platforms[41].position = glm::vec3(-3, -1.0f, 0);

    state.platforms[42].textureID = platformTextureID2;
    state.platforms[42].position = glm::vec3(-2, 1.0f, 0);
    
    

    //Update the platforms only once
    for(int i = 0; i < PLATFORM_COUNT; i++){
        state.platforms[i].Update(0, NULL, 0);
    }
    for(int i = 0; i < WINNER_COUNT; i++){
        state.correctPlat[i].Update(0, NULL, 0);
    }
}

//Text!
void DrawText(ShaderProgram *program, GLuint fontTextureID, std::string text,
                                 float size, float spacing, glm::vec3 position)
{
    float width = 1.0f / 16.0f;
    float height = 1.0f / 16.0f;
    
    std::vector<float> vertices;
    std::vector<float> texCoords;
    
    for(int i = 0; i < text.size(); i++) {
        
        int index = (int)text[i];
        float offset = (size + spacing) * i;
        float u = (float)(index % 16) / 16.0f;
        float v = (float)(index / 16) / 16.0f;
        
        vertices.insert(vertices.end(), {
                offset + (-0.5f * size), 0.5f * size,
                offset + (-0.5f * size), -0.5f * size,
                offset + (0.5f * size), 0.5f * size,
                offset + (0.5f * size), -0.5f * size,
                offset + (0.5f * size), 0.5f * size,
                offset + (-0.5f * size), -0.5f * size,
        });
        
        texCoords.insert(texCoords.end(), {
            u, v,
            u, v + height,
            u + width, v,
            u + width, v + height,
            u + width, v,
            u, v + height,
        });
        
    }
    
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    program->SetModelMatrix(modelMatrix);
    
    glUseProgram(program->programID);
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices.data());
    glEnableVertexAttribArray(program->positionAttribute);
    
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords.data());
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    glBindTexture(GL_TEXTURE_2D, fontTextureID);
    glDrawArrays(GL_TRIANGLES, 0, (int)(text.size() * 6));
    
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
    
}

void ProcessInput() {
    
    state.player->movement = glm::vec3(0);
    
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                gameIsRunning = false;
                break;
                
            case SDL_KEYDOWN:
                break; // SDL_KEYDOWN
        }
    }
    
    // Makes sure we dont move at all once we collide with an unwanted block
    if(state.player->collidedBottom == true || state.player->collidedLeft == true || state.player->collidedRight == true){
        state.player->acceleration.x = 0;
        state.player->acceleration.y = 0;
        
        state.player->velocity.x = 0;
        state.player->velocity.y = 0;
        
        state.player->movement.x = 0;
        state.player->speed = 0;
    }

    
    const Uint8 *keys = SDL_GetKeyboardState(NULL);

    if (keys[SDL_SCANCODE_LEFT]) {
        // Player only moves if there is no collsion yet
        if(state.player->collidedBottom == true){
            state.player->acceleration.x = 0;
            state.player->velocity.x = 0;
            state.player->movement.x = 0;
            state.player->speed = 0;
            
        }
        else {
            state.player->movement.x = -1.0f;
            state.player->animIndices = state.player->animLeft;
            state.player->acceleration.x -= 2.25f;
        }
    }
    else if (keys[SDL_SCANCODE_RIGHT]) {
        // Player only moves if there is no collsion yet
        if(state.player->collidedBottom == true ){
            state.player->acceleration.x = 0;
            state.player->velocity.x = 0;
            state.player->movement.x = 0;
            state.player->speed = 0;
            
        }
        else{
            state.player->movement.x = 1.0f;
            state.player->animIndices = state.player->animRight;
            state.player->acceleration.x += 2.25f;
        }
    }
    
    if (glm::length(state.player->movement) > 1.0f) {
        state.player->movement = glm::normalize(state.player->movement);
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
        state.player->Update(FIXED_TIMESTEP, state.platforms, PLATFORM_COUNT);
        state.player->Update(FIXED_TIMESTEP, state.correctPlat, WINNER_COUNT);
        deltaTime -= FIXED_TIMESTEP;
    }
    accumulator = deltaTime;
}


void Render() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Conditional statements that decide if the mission was passed or failed
    // Text is then printed accordingly
    if(state.player->collidedBottom == true && (state.player->position.y < -2.75 && state.player->position.x >= 1.75 && state.player->position.x <= 3.35)){
        
        GLuint fontTextureID = LoadTexture("font1.png");
        DrawText(&program, fontTextureID, "Mission Passed..." , .75f, -0.25f, glm::vec3(-3.75f, 0, 0));
        DrawText(&program, fontTextureID, "The pine will survive." , .5f, -0.25f, glm::vec3(-3.75f, -2.15f, 0));
    }
    
    else if(((state.player->collidedLeft == true) || (state.player->collidedRight == true ) || (state.player->collidedBottom == true ))  && state.player->position.y > -3.0f){
        
        GLuint fontTextureID = LoadTexture("font1.png");
        DrawText(&program, fontTextureID, "Mission Failed..." , .75f, -0.25f, glm::vec3(-3.75f, 0, 0));
        DrawText(&program, fontTextureID, "The pine tree has died." , .5f, -0.25f, glm::vec3(-3.75f, -2.15f, 0));

    }
                    
    for(int i = 0; i < WINNER_COUNT; i++){
        state.correctPlat[i].Render(&program);
    }
    for(int i = 0; i < PLATFORM_COUNT; i++){
        state.platforms[i].Render(&program);
    }
    
    state.player->Render(&program);
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
