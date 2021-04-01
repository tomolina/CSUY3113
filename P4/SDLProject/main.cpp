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

#define PLATFORM_COUNT 45
#define ENEMY_COUNT 3

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <vector>

#include "Entity.h"

struct GameState {
    Entity *player;
    Entity *platforms;
    Entity *enemies;
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
    displayWindow = SDL_CreateWindow("Rise of AI", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
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
    
    glClearColor(0.65f, 0.1365f, 0.1365f, 1.0f);
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
   
    // Initialize Game Objects
    
    // Initialize Player
    state.player = new Entity();
    state.player->position = glm::vec3(0, 5.0f, 0);
    
    state.player->entityType = PLAYER;
    state.player->velocity = glm::vec3(0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, -9.81f, 0);
    
    state.player->speed = 3.0f;
    state.player->textureID = LoadTexture("george_0.png");
    
    state.player->height = .8f;
    state.player->width = .6f;
    state.player->jumpPower = 6.0f;
    
    
    state.player->animRight = new int[4]{3,7,11,15};
    state.player->animLeft = new int[4]{1,5,9,13};
    state.player->animUp = new int[4]{2,6,10,14};
    state.player->animDown = new int[4]{0,4,8,12};

    state.player->animIndices = state.player->animRight;
    state.player->animFrames = 4;
    state.player->animIndex = 0;
    state.player->animTime = 0;
    state.player->animRows = 4;
    state.player->animCols = 4;
    

    state.platforms = new Entity[PLATFORM_COUNT];
    
    GLuint platformTextureID1 = LoadTexture("metalBrick.png");
    GLuint platformTextureID2 = LoadTexture("metalblock.png");
    GLuint floatingPlat = LoadTexture("floatPlat.png");
    
    // Below I make it eaasier to position the ground and walls, in loops
    int groundPlat = -5.0f;
    int wallPlat1 = -3.6f;
    int wallPlat2 = -3.6f;
    
    for(int i = 0; i <= 8; i++){
        state.platforms[i].textureID = platformTextureID1;
        state.platforms[i].position = glm::vec3(-5.0f, wallPlat1, 0);
        
        state.platforms[i].entityType = PLATFORM;
        wallPlat1 += 1;
    }
    
    for(int i = 9; i <= 17; i++){
        state.platforms[i].textureID = platformTextureID1;
        state.platforms[i].position = glm::vec3(5.0f, wallPlat2, 0);
        
        state.platforms[i].entityType = PLATFORM;
        wallPlat2 += 1;
    }
    
    for(int i = 18; i <= 24; i++){
        state.platforms[i].textureID = platformTextureID2;
        state.platforms[i].position = glm::vec3(groundPlat, -3.75f, 0);
        
        state.platforms[i].entityType = PLATFORM;
        groundPlat += 1;
    }
    
    
    // These next two are the special blocks
    state.platforms[43].textureID = platformTextureID2;
    state.platforms[43].position = glm::vec3(groundPlat, -3.75f, 0);
    
    state.platforms[44].textureID = platformTextureID2;
    state.platforms[44].position = glm::vec3(groundPlat + 1, -3.75f, 0);
    
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
    state.platforms[29].position = glm::vec3(3.5f,-2.0f, 0);
    
    state.platforms[30].textureID = floatingPlat;
    state.platforms[30].position = glm::vec3(0, 2.5f, 0);
    state.platforms[30].height = .5f;
    state.platforms[30].width = .8f;

    state.platforms[31].textureID = platformTextureID2;
    state.platforms[31].position = glm::vec3(4,2.0f, 0);

    state.platforms[32].textureID = platformTextureID2;
    state.platforms[32].position = glm::vec3(3, 2.0f, 0);

    state.platforms[33].textureID = platformTextureID2;
    state.platforms[33].position = glm::vec3(4, 1.0f, 0);
    
    state.platforms[34].textureID = platformTextureID2;
    state.platforms[34].position = glm::vec3(3, 1.0f, 0);

    state.platforms[35].textureID = platformTextureID2;
    state.platforms[35].position = glm::vec3(2, .5f, 0);

    state.platforms[36].textureID = platformTextureID2;
    state.platforms[36].position = glm::vec3(-2, 1.0f, 0);

    //37
    state.platforms[37].textureID = platformTextureID2;
    state.platforms[37].position = glm::vec3(1, -1.75f, 0);

    state.platforms[38].textureID = floatingPlat;
    state.platforms[38].position = glm::vec3(0, -1.0f, 0);
    state.platforms[38].height = .5f;
    
    state.platforms[39].textureID = floatingPlat;
    state.platforms[39].position = glm::vec3(-1, -1.0f, 0);
    state.platforms[39].height = .5f;
    
    state.platforms[40].textureID = floatingPlat;
    state.platforms[40].position = glm::vec3(-2, -1.0f, 0);
    state.platforms[40].height = .5f;
    
    state.platforms[41].textureID = floatingPlat;
    state.platforms[41].position = glm::vec3(-3, -1.0f, 0);
    state.platforms[41].height = .5f;
    
    state.platforms[42].textureID = platformTextureID2;
    state.platforms[42].position = glm::vec3(-2, 1.0f, 0);
    
    

    //Update the platforms only once
    for(int i = 0; i < PLATFORM_COUNT; i++){
        state.platforms[i].Update(0, NULL, NULL, NULL, NULL, 0);
        state.platforms[i].width = .8f;
    }
    
    state.enemies = new Entity[ENEMY_COUNT];
    GLuint enemyTextureID = LoadTexture("ctg.png");
    
    state.enemies[0].entityType = ENEMY;
    state.enemies[0].textureID = enemyTextureID;
    state.enemies[0].position = glm::vec3(3, 5, 0);
    state.enemies[0].speed = 1;
    state.enemies[0].aiType = WAITGO ;
    state.enemies[0].aiState = IDLE;
    state.enemies[0].acceleration = glm::vec3(0, -9.81f, 0);
    state.enemies[0].height = 1.0f;
    state.enemies[0].width = .6f;
    
 
    
    state.enemies[1].entityType = ENEMY;
    state.enemies[1].textureID = enemyTextureID;
    state.enemies[1].position = glm::vec3(-4, 2, 0);
    state.enemies[1].speed = 1;
    state.enemies[1].aiType = JUMPER;
    state.enemies[1].aiState = JUMPING;
    state.enemies[1].acceleration = glm::vec3(0, -9.81f, 0);
    state.enemies[1].height = .8f;
    state.enemies[1].width = .6f;
    state.enemies[1].jumpPower = .25f;
    

    
    state.enemies[2].entityType = ENEMY;
    state.enemies[2].textureID = enemyTextureID;
    state.enemies[2].position = glm::vec3(-3.0f, 0, 0);
    state.enemies[2].speed = 1;
    state.enemies[2].aiType = PATROLLER;
    state.enemies[2].aiState = PATROLLING;
    state.enemies[2].acceleration = glm::vec3(0, -9.81f, 0);
    state.enemies[2].height = 1.0f;
    state.enemies[2].width = .6f;
    state.enemies[2].velocity = glm::vec3(0);
    


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
int enemyCount = 0;
int deleteFlag = 0;

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
                switch (event.key.keysym.sym) {
                    case SDLK_SPACE:
                        if(state.player->collidedBottom){
                            state.player->jump = true;
                        }
                        
                        break;

                }
                break; // SDL_KEYDOWN
        }
    }
    
    
    if((state.enemies[0].position.y <= state.player->position.y) && ( state.enemies[0].position.x-.35f <= state.player->position.x && state.player->position.x <= state.enemies[0].position.x+.35f) && state.player->collidedTop_Enemy){
        
        state.enemies[0].isActive = false;
        enemyCount += 1;
        
    }
    
    
    if(state.enemies[1].position.x > 5){
        state.enemies[1].isActive = false;
        deleteFlag += 1;
    }
    
    
    if((state.enemies[2].position.y <= state.player->position.y) && (state.enemies[2].position.x-.35f <= state.player->position.x && state.player->position.x <= state.enemies[2].position.x+.35f) && state.player->collidedTop_Enemy){

        state.enemies[2].isActive = false;
        enemyCount += 1;
    }
    
    if(deleteFlag == 1){
        enemyCount += 1;
    }

    if(state.player->collidedLeft || state.player->collidedRight || state.player->collidedTop){
        state.player->jump = false;
        state.player->movement.x = 0;
    }
    
    if(state.player->collidedLeft_Enemy || state.player->collidedRight_Enemy){
        state.player->acceleration = glm::vec3(0, 10, 0);
        
        if(state.player->collidedTop){
            state.player->isActive = false;
        }
    }
    
    
    const Uint8 *keys = SDL_GetKeyboardState(NULL);

    if (keys[SDL_SCANCODE_LEFT]) {

        state.player->movement.x = -1.0f;
        state.player->animIndices = state.player->animLeft;

    }
    else if (keys[SDL_SCANCODE_RIGHT]) {
        
        state.player->movement.x = 1.0f;
        state.player->animIndices = state.player->animRight;
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
        state.player->Update(FIXED_TIMESTEP, state.player, state.enemies, ENEMY_COUNT, state.platforms, PLATFORM_COUNT);

        deltaTime -= FIXED_TIMESTEP;
        
        for(int i = 0; i < ENEMY_COUNT; i++){
            state.enemies[i].Update(FIXED_TIMESTEP, state.player, state.player, 1, state.platforms, PLATFORM_COUNT);
            if(state.enemies[i].collidedTop){
                state.enemies[i].position = glm::vec3(10, -10, 0);
            }
        }
        
    }
    accumulator = deltaTime;
    
    
}


void Render() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Conditional statements that decide if the mission was passed or failed
    // Text is then printed accordingly
    //enemyCount == 3
    
    if(state.enemies[0].isActive == false && state.enemies[1].isActive == false && state.enemies[2].isActive == false){
        
        GLuint fontTextureID = LoadTexture("font1.png");
        DrawText(&program, fontTextureID, "You Won" , .75f, -0.25f, glm::vec3(-3.75f, 0, 0));
    }
    

    else if((state.player->collidedRight_Enemy || state.player->collidedLeft_Enemy)){
        GLuint fontTextureID = LoadTexture("font1.png");
        DrawText(&program, fontTextureID, "You Lost!!!" , .75f, -0.25f, glm::vec3(-3.75f, 0, 0));

    }
    
    
    for(int i = 0; i < PLATFORM_COUNT; i++){
        state.platforms[i].Render(&program);
    }
    for(int i = 0; i < ENEMY_COUNT; i++){
        if(state.enemies[i].collidedTop == false){
            state.enemies[i].Render(&program);
    }
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
