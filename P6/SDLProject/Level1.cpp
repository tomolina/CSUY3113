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
#define LEVEL1_WIDTH 11
#define LEVEL1_HEIGHT 30
#define LEVEL1_ENEMYCOUNT 7
//#define LEVEL1_COINS 15
#define HEART_COUNT 3

glm::mat4 sawMat;

unsigned int level1_data[] =
{
    92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92,
    92, 64, 64, 64, 64, 64, 64, 64, 64, 64, 92,
    92, 64, 64, 64, 64, 64, 64, 64, 64, 64, 92,
    92, 64, 64, 64, 64, 64, 64, 64, 64, 64, 92,
    92, 64, 64, 64, 64, 64, 64, 64, 64, 64, 92,
    92, 64, 64, 64, 64, 64, 64, 64, 64, 64, 92,
    92, 64, 64, 64, 64, 64, 64, 64, 64, 64, 92,
    92, 38, 38, 38, 38, 38, 64, 64, 38, 38, 92,
    92, 64, 64, 64, 64, 64, 64, 64, 64, 64, 92,
    92, 64, 64, 64, 64, 64, 64, 64, 64, 64, 92,
    92, 64, 64, 64, 64, 64, 64, 64, 38, 38, 92,
    92, 64, 64, 64, 64, 64, 64, 64, 38, 38, 92,
    92, 64, 64, 64, 64, 64, 64, 38, 38, 38, 92,
    92, 38, 38, 38, 64, 64, 38, 38, 38, 38, 92,
    92, 38, 38, 38, 64, 64, 64, 64, 64, 64, 92,
    92, 64, 64, 64, 64, 64, 64, 64, 64, 64, 92,
    92, 64, 64, 64, 64, 64, 64, 64, 64, 30, 92,
    92, 64, 30, 30, 30, 64, 64, 64, 30, 30, 92,
    92, 64, 30, 30, 30, 64, 64, 30, 30, 30, 92,
    92, 64, 64, 30, 30, 64, 64, 64, 64, 64, 92,
    92, 64, 64, 30, 30, 64, 64, 64, 64, 64, 92,
    92, 64, 64, 30, 30, 64, 64, 64, 64, 64, 92,
    92, 64, 30, 30, 64, 64, 64, 64, 64, 64, 92,
    92, 64, 64, 64, 64, 64, 64, 64, 64, 64, 92,
    92, 64, 64, 64, 64, 64, 64, 64, 64, 64, 92,
    92, 64, 64, 64, 64, 64, 64, 64, 64, 64, 92,
    92, 64, 64, 64, 64, 64, 64, 64, 64, 64, 92,
    92, 64, 30, 30, 30, 30, 30, 30, 30, 30, 92,
    92, 64, 64, 64, 64, 64, 64, 64, 64, 64, 92,
    92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92
};

void Level1::Initialize() {
    state.nextScene = -1;
    
    GLuint mapTextureID = Util::LoadTexture("basictiles.png");
    state.map = new Map(LEVEL1_WIDTH, LEVEL1_HEIGHT, level1_data, mapTextureID, 1.0f, 8, 15);
    // Move over all of the player and enemy code from initialization.
    
    // Initialize Player
    state.player = new Entity();
    state.player->position = glm::vec3(4.0f, -1.5f, 0);
    
    state.player->entityType = PLAYER;
    state.player->velocity = glm::vec3(0);
    state.player->movement = glm::vec3(0);
    //state.player->acceleration = glm::vec3(0, 0, 0);
    
    state.player->speed = 3.0f;
    state.player->textureID = Util::LoadTexture("log.png");
    
    state.player->height = .8f;
    state.player->width = .6f;
    state.player->jumpPower = 5.0f;
    
    
    state.player->animRight = new int[4]{12,13,14,15};
    state.player->animLeft = new int[4]{18,19,20,21};
    state.player->animUp = new int[4]{6,7,8,9};
    state.player->animDown = new int[4]{0,1,2,3};

    state.player->animIndices = state.player->animDown;
    state.player->animFrames = 4;
    state.player->animIndex = 0;
    state.player->animTime = 0;
    state.player->animRows = 4;
    state.player->animCols = 6;
    
    
    state.enemies = new Entity[LEVEL1_ENEMYCOUNT];
    GLuint ghost = Util::LoadTexture("ghost.png");

    state.enemies[1].entityType = ENEMY;
    state.enemies[1].textureID = ghost;
    state.enemies[1].position = glm::vec3(3, -4, 0);
    state.enemies[1].speed = 1;
    state.enemies[1].aiType = WAITGO ;
    state.enemies[1].aiState = IDLE;
    //state.enemies[0].acceleration = glm::vec3(0, -9.81f, 0);
    state.enemies[1].height = 1.0f;
    state.enemies[1].width = .6f;

    state.enemies[2].entityType = ENEMY;
    state.enemies[2].textureID = ghost;
    state.enemies[2].aiType = WAITGO ;
    state.enemies[2].aiState = IDLE;
    state.enemies[2].position = glm::vec3(7, -9, 0);
    state.enemies[2].speed = 2.25;
    state.enemies[2].height = .5f;
    state.enemies[2].width = .8f;
    
    state.enemies[3].entityType = ENEMY;
    state.enemies[3].textureID = ghost;
    state.enemies[3].aiType = WAITGO ;
    state.enemies[3].aiState = IDLE;
    state.enemies[3].position = glm::vec3(7, -15, 0);
    state.enemies[3].height = .5f;
    state.enemies[3].width = .8f;




    state.enemies[4].entityType = ENEMY;
    state.enemies[4].aiType = WAITGO ;
    state.enemies[4].aiState = IDLE;
    state.enemies[4].textureID = ghost;
    state.enemies[4].position = glm::vec3(5, -20, 0);
    state.enemies[4].speed = 2;
    state.enemies[4].height = 1.0f;
    state.enemies[4].width = .6f;

    
    //state.coins = new Entity[15];
    GLuint coinTex = Util::LoadTexture("coin.png");
    state.coins = new Entity();
    
    state.coins->aiType = COIN;
    state.coins->aiState = IDLE;
    state.coins->textureID = coinTex;
    
    state.coins->position = glm::vec3(9, -28, 0);

    
    
    GLuint heartText = Util::LoadTexture("fullHeart.png");
    state.hearts = new Entity[HEART_COUNT];
    state.hearts[0].textureID = heartText;
    state.hearts[0].position = glm::vec3(8.0f, -7.0f, 0);
    state.hearts[0].speed = .5;
    state.hearts[0].height = .5;
    state.hearts[0].width = .5;
    
    state.hearts[1].textureID = heartText;
    state.hearts[1].position = glm::vec3(8.5f, -7.0f, 0);
    state.hearts[1].speed = .5;
    state.hearts[1].height = .5;
    state.hearts[1].width = .5;
    
    state.hearts[2].textureID = heartText;
    state.hearts[2].position = glm::vec3(9.0f, -7.0f, 0);
    state.hearts[2].speed = .5;
    state.hearts[2].height = .5;
    state.hearts[2].width = .5;

    state.enemies[0].entityType = ENEMY;
    state.enemies[0].aiType = WAITGO;
    state.enemies[0].aiState = IDLE;
    state.enemies[0].speed = 1;
    state.enemies[0].velocity = glm::vec3(0);
    state.enemies[0].textureID = ghost;
    state.enemies[0].position = glm::vec3(8.0f, -1.5f, 0);
    
    state.enemies[5].entityType = ENEMY;
    state.enemies[5].aiType = WAITGO;
    state.enemies[5].aiState = IDLE;
    state.enemies[5].speed = 1;
    state.enemies[5].velocity = glm::vec3(0);
    state.enemies[5].textureID = ghost;
    state.enemies[5].position = glm::vec3(2.0f, -18.0f, 0);
    
    state.enemies[6].entityType = ENEMY;
    state.enemies[6].aiType = WAITGO;
    state.enemies[6].aiState = IDLE;
    state.enemies[6].speed = 1;
    state.enemies[6].velocity = glm::vec3(0);
    state.enemies[6].textureID = ghost;
    state.enemies[6].position = glm::vec3(5.0f, -23.0f, 0);
    
    
}
void Level1::Update(float deltaTime) {
    float playerRotate = 0;
    playerRotate += 50.0f * deltaTime;
    
    state.player->Update(deltaTime, state.player, state.enemies, LEVEL1_ENEMYCOUNT, state.map);
    //state.player->Update(deltaTime, state.player, state.coins, LEVEL1_COINS, state.map);
    
    for (int i = 0; i < HEART_COUNT; i++) {
        state.hearts[i].Update(deltaTime, state.player, state.enemies, LEVEL1_ENEMYCOUNT, state.map);

    }
    

    
    for(int i = 0; i < LEVEL1_ENEMYCOUNT; i++){
        state.enemies[i].Update(deltaTime, state.player, state.player, 1, state.map);
    }
    
    state.coins->Update(deltaTime, state.player, state.player, 1, state.map);
//    state.enemies[15].Update(deltaTime, state.enemies, state.player, 1, state.map);
//    state.enemies[16].Update(deltaTime, state.enemies, state.player, 1, state.map);
//    state.enemies[17].Update(deltaTime, state.enemies, state.player, 1, state.map);
//    state.enemies[17].Update(deltaTime, state.enemies, state.player, 1, state.map);

    
    
//    if((state.player->position.x >= 15.0f) && (state.player->position.y >= -4.0f)){
//        state.nextScene = 2;
//    }
}

void Level1::Render(ShaderProgram *program) {
    state.map->Render(program);
    state.player->Render(program);
    GLuint fontTextureID = Util::LoadTexture("font1.png");
    
    for (int i = 0; i < heartCount; i++) {
        state.hearts[i].Render(program);
    }
    for(int i = 0; i < LEVEL1_ENEMYCOUNT; i++){
        state.enemies[i].Render(program);
    }
    state.coins->Render(program);
//    state.enemies[15].Render(program);
//    state.enemies[16].Render(program);
//    state.enemies[17].Render(program);
//    state.enemies[18].Render(program);


    
    Util::DrawText(program, fontTextureID, "LEVEL 1" , .50f, -0.25f, glm::vec3(1.0f, -.5f, 0));
    
    if((state.player->position.x >= 8 ) && (state.player->position.y <= -27.5)){
        
        coinCount -= 1;
        state.player->position = glm::vec3(5, -3, 0);
        winner = 1;
    }
    
    if(heartCount <= 0){
        Util::DrawText(program, fontTextureID, "You Lost..." , .75f, -0.25f, glm::vec3(1.0f, -3.0f, 0));
        gameOver = 1;
    }
    
    if(coinCount <= 0){
        Util::DrawText(program, fontTextureID, "You survived the forest" , .60f, -0.25f, glm::vec3(1.0f, -3.0f, 0));
    }
    
    
    
}
