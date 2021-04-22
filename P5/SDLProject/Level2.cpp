#include "Level2.h"
#define LEVEL2_WIDTH 17
#define LEVEL2_HEIGHT 8
#define LEVEL2_ENEMYCOUNT 7
#define HEART_COUNT 3

unsigned int level2_data[] =
{
   59, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 59,
   59, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 75, 0, 0, 0, 0, 59,
   59, 75, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 0, 58, 0, 0, 59,
   59, 30, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 59,
   59, 0, 30, 0, 0, 0, 0, 0, 58, 0, 0, 0, 0, 0, 2, 3, 59,
   59, 0, 0, 30, 0, 0, 2, 0, 0, 0, 58, 0, 0, 2, 3, 3, 59,
   59, 2, 2, 2, 2, 2, 3, 0, 0, 0, 0, 0, 2, 3, 3, 3, 59,
   59, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 59
};

void Level2::Initialize() {
    state.nextScene = -1;
    
    GLuint mapTextureID = Util::LoadTexture("newTiles.png");
    state.map = new Map(LEVEL2_WIDTH, LEVEL2_HEIGHT, level2_data, mapTextureID, 1.0f, 14, 7);
    // Move over all of the player and enemy code from initialization.
    
    // Initialize Player
    state.player = new Entity();
    state.player->position = glm::vec3(1.0f, -2.0f, 0);
    
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

    state.player->animIndices = state.player->animRight;
    state.player->animFrames = 4;
    state.player->animIndex = 0;
    state.player->animTime = 0;
    state.player->animRows = 4;
    state.player->animCols = 4;
    
    state.enemies = new Entity[LEVEL2_ENEMYCOUNT];
    GLuint enemyTextureID = Util::LoadTexture("ctg.png");
    
    state.enemies[0].entityType = ENEMY;
    state.enemies[0].textureID = enemyTextureID;
    state.enemies[0].position = glm::vec3(4, -4, 0);
    state.enemies[0].speed = 1;
    state.enemies[0].aiType = WAITGO ;
    state.enemies[0].aiState = IDLE;
    state.enemies[0].acceleration = glm::vec3(0, -9.81f, 0);
    state.enemies[0].height = 1.0f;
    state.enemies[0].width = .6f;
    
 
    GLuint iceText = Util::LoadTexture("ice.png");
    state.enemies[1].entityType = ENEMY;
    state.enemies[1].textureID = iceText;
    state.enemies[1].position = glm::vec3(7, -6.0f, 0);
    state.enemies[1].height = 1.0f;
    state.enemies[1].width = .6f;
    

    
    state.enemies[2].entityType = ENEMY;
    state.enemies[2].textureID = iceText;
    state.enemies[2].position = glm::vec3(8.0f, -6.0f, 0);
    state.enemies[2].height = 1.0f;
    state.enemies[2].width = .6f;
    
    state.enemies[3].entityType = ENEMY;
    state.enemies[3].textureID = iceText;
    state.enemies[3].position = glm::vec3(9.0f, -6.0f, 0);
    state.enemies[3].height = 1.0f;
    state.enemies[3].width = .6f;
    
    state.enemies[4].entityType = ENEMY;
    state.enemies[4].textureID = iceText;
    state.enemies[4].position = glm::vec3(10.0f, -6.0f, 0);
    state.enemies[4].height = 1.0f;
    state.enemies[4].width = .6f;

    
    state.enemies[5].entityType = ENEMY;
    state.enemies[5].textureID = iceText;
    state.enemies[5].position = glm::vec3(11.0f, -6.0f, 0);
    state.enemies[5].height = 1.0f;
    state.enemies[5].width = .6f;

    
    GLuint sawText = Util::LoadTexture("saw.png");
    
    state.enemies[6].entityType = ENEMY;
    state.enemies[6].textureID = sawText;
    state.enemies[6].position = glm::vec3(5, -2, 0);
    state.enemies[6].speed = 1;
    state.enemies[6].aiType =PATROLLER ;
    state.enemies[6].aiState = PATROLLING;
    state.enemies[6].height = 1.0f;
    state.enemies[6].width = .6f;
    
    
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
    
}
void Level2::Update(float deltaTime) {
    state.player->Update(deltaTime, state.player, state.enemies, LEVEL2_ENEMYCOUNT, state.map);

    for (int i = 0; i < HEART_COUNT; i++) {
        state.hearts[i].Update(deltaTime, state.player, state.enemies, LEVEL2_ENEMYCOUNT, state.map);

    }
    
    for(int i = 0; i < LEVEL2_ENEMYCOUNT; i++){
        state.enemies[i].Update(deltaTime, state.player, state.player, 1, state.map);
    }
    
    if((state.player->position.x >= 11) && (state.player->position.x <= 12) && (state.player->position.y >= -2.0f) && (state.player->position.y <= -1.0f)){
        state.nextScene = 3;
    }
}

void Level2::Render(ShaderProgram *program) {
    state.map->Render(program);
    state.player->Render(program);

    GLuint fontTextureID = Util::LoadTexture("font1.png");
    
    for(int i = 0; i < LEVEL2_ENEMYCOUNT; i++){
        state.enemies[i].Render(program);
    }
    
    for (int i = 0; i < heartCount; i++) {
        state.hearts[i].Render(program);
    }
    
    Util::DrawText(program, fontTextureID, "LEVEL 2" , .50f, -0.25f, glm::vec3(1.0f, -.5f, 0));

    if(heartCount <= 0){
        Util::DrawText(program, fontTextureID, "You Lost..." , .75f, -0.25f, glm::vec3(6.0f, -1.0f, 0));
        gameOver = 1;
    }
}

