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
#include<math.h>

#include "Level1.h"
#include "Scene.h"
#define LEVEL1_WIDTH 17
#define LEVEL1_HEIGHT 8
#define LEVEL1_ENEMYCOUNT 4
#define HEART_COUNT 3

glm::mat4 sawMat;

unsigned int level1_data[] =
{
   45, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 45,
   45, 0, 0, 0, 46, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 45,
   45, 0, 0, 46, 0, 46, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 45,
   45, 29, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 75, 45,
   45, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 45,
   45, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3, 3, 45,
   45, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 3, 3, 3, 45,
   45, 3, 3, 3, 3, 3, 3, 0, 0, 3, 3, 3, 3, 3, 3, 3, 45
};

void Level1::Initialize() {
    state.nextScene = -1;
    
    GLuint mapTextureID = Util::LoadTexture("newTiles.png");
    state.map = new Map(LEVEL1_WIDTH, LEVEL1_HEIGHT, level1_data, mapTextureID, 1.0f, 14, 7);
    // Move over all of the player and enemy code from initialization.
    
    // Initialize Player
    state.player = new Entity();
    state.player->position = glm::vec3(4.0f, -2.0f, 0);
    
    state.player->entityType = PLAYER;
    state.player->velocity = glm::vec3(0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, -6.81f, 0);
    
    state.player->speed = 3.0f;
    state.player->textureID = Util::LoadTexture("george_0.png");
    
    state.player->height = .8f;
    state.player->width = .6f;
    state.player->jumpPower = 5.0f;
    
    
    state.player->animRight = new int[4]{3,7,11,15};
    state.player->animLeft = new int[4]{1,5,9,13};
    state.player->animUp = new int[4]{2,6,10,14};
    state.player->animDown = new int[4]{0,4,8,12};

    state.player->animIndices = state.player->animDown;
    state.player->animFrames = 4;
    state.player->animIndex = 0;
    state.player->animTime = 0;
    state.player->animRows = 4;
    state.player->animCols = 4;
    
    
    state.enemies = new Entity[LEVEL1_ENEMYCOUNT];
    GLuint enemyTextureID = Util::LoadTexture("ctg.png");

    state.enemies[0].entityType = ENEMY;
    state.enemies[0].textureID = enemyTextureID;
    state.enemies[0].position = glm::vec3(12, 5, 0);
    state.enemies[0].speed = 1;
    state.enemies[0].aiType = WAITGO ;
    state.enemies[0].aiState = IDLE;
    state.enemies[0].acceleration = glm::vec3(0, -9.81f, 0);
    state.enemies[0].height = 1.0f;
    state.enemies[0].width = .6f;

    
    GLuint lavaText = Util::LoadTexture("lava.png");

    state.enemies[2].entityType = ENEMY;
    state.enemies[2].textureID = lavaText;
    state.enemies[2].position = glm::vec3(7, -7, 0);
    state.enemies[2].height = .8f;
    state.enemies[2].width = .6f;




    state.enemies[3].entityType = ENEMY;
    state.enemies[3].textureID = lavaText;
    state.enemies[3].position = glm::vec3(8, -7, 0);
    state.enemies[3].height = 1.0f;
    state.enemies[3].width = .6f;

    

    GLuint heartText = Util::LoadTexture("fullHeart.png");
    state.hearts = new Entity[HEART_COUNT];
    state.hearts[0].textureID = heartText;
    state.hearts[0].position = glm::vec3(8.0f, -.5f, 0);
    state.hearts[0].speed = .5;
    state.hearts[0].height = .5;
    state.hearts[0].width = .5;
    
    state.hearts[1].textureID = heartText;
    state.hearts[1].position = glm::vec3(8.5f, -.5f, 0);
    state.hearts[1].speed = .5;
    state.hearts[1].height = .5;
    state.hearts[1].width = .5;
    
    state.hearts[2].textureID = heartText;
    state.hearts[2].position = glm::vec3(9.0f, -.5f, 0);
    state.hearts[2].speed = .5;
    state.hearts[2].height = .5;
    state.hearts[2].width = .5;
    
    GLuint sawText = Util::LoadTexture("saw.png");
    state.enemies[1].entityType = ENEMY;
    state.enemies[1].aiType = SAW;
    state.enemies[1].aiState = ROTATE;
    state.enemies[1].speed = 1;
    state.enemies[1].velocity = glm::vec3(0);
    state.enemies[1].textureID = sawText;
    state.enemies[1].position = glm::vec3(8.0f, -1.5f, 0);
    
    
}
void Level1::Update(float deltaTime) {
    float playerRotate = 0;
    playerRotate += 50.0f * deltaTime;
    
    state.player->Update(deltaTime, state.player, state.enemies, LEVEL1_ENEMYCOUNT, state.map);
    
    for (int i = 0; i < HEART_COUNT; i++) {
        state.hearts[i].Update(deltaTime, state.player, state.enemies, LEVEL1_ENEMYCOUNT, state.map);

    }
    
    for(int i = 0; i < LEVEL1_ENEMYCOUNT; i++){
        state.enemies[i].Update(deltaTime, state.player, state.player, 1, state.map);
    }
    
    
    if((state.player->position.x >= 15.0f) && (state.player->position.y >= -4.0f)){
        state.nextScene = 2;
    }
}

void Level1::Render(ShaderProgram *program) {
    state.map->Render(program);
    state.player->Render(program);
    GLuint fontTextureID = Util::LoadTexture("font1.png");
    
    for (int i = 0; i < heartCount; i++) {
        state.hearts[i].Render(program);
    }
    
    for (int i = 0; i < LEVEL1_ENEMYCOUNT; i++) {
        state.enemies[i].Render(program);
    }
    
    Util::DrawText(program, fontTextureID, "LEVEL 1" , .50f, -0.25f, glm::vec3(1.0f, -.5f, 0));

    
    if(heartCount <= 0){
        Util::DrawText(program, fontTextureID, "You Lost..." , .75f, -0.25f, glm::vec3(6.0f, -1.0f, 0));
        gameOver = 1;
    }
    
    
    
}
