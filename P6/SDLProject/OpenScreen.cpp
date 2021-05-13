#include "OpenScreen.h"
#define LEVEL2_WIDTH 14
#define LEVEL2_HEIGHT 8
#define LEVEL2_ENEMYCOUNT 1
float playerRotate = 0;

unsigned int OScreen_data[] =
{
   92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 0, 0, 0,
   92, 64, 64, 64, 64, 64, 64, 64, 64, 64, 92, 0, 0, 0,
    92, 64, 64, 64, 64, 64, 64, 64, 64, 64, 92, 0, 0, 0,
    92, 64, 64, 64, 64, 64, 64, 64, 64, 64, 92, 0, 0, 0,
    92, 64, 64, 64, 64, 64, 64, 64, 64, 64, 92, 0, 0, 0,
    92, 64, 64, 64, 64, 64, 64, 64, 64, 64, 92, 0, 0, 0,
    92, 64, 64, 64, 64, 64, 64, 64, 64, 64, 92, 3, 3, 3,
    92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 3, 3, 3
};

void OScreen::Initialize() {
    state.nextScene = -1;

    GLuint mapTextureID = Util::LoadTexture("basictiles.png");
    state.map = new Map(LEVEL2_WIDTH, LEVEL2_HEIGHT, OScreen_data, mapTextureID, 1.0f, 8, 15);
    // Move over all of the player and enemy code from initialization.
    
    // Initialize Player
    state.player = new Entity();
    state.player->position = glm::vec3(4.0f, -1.5f, 0);
    
    state.player->entityType = PLAYER;
    state.player->velocity = glm::vec3(0);
    state.player->movement = glm::vec3(0);
    //state.player->acceleration = glm::vec3(0, -9.81f, 0);
    
    state.player->speed = 3.0f;
    state.player->textureID = Util::LoadTexture("log.png");
    
    state.player->height = .8f;
    state.player->width = .6f;
    state.player->jumpPower = 6.0f;
    
    
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
    
    state.enemies = new Entity[LEVEL2_ENEMYCOUNT];
    GLuint enemyTextureID = Util::LoadTexture("ctg.png");
    
    state.enemies[0].entityType = ENEMY;
    state.enemies[0].textureID = enemyTextureID;
    state.enemies[0].position = glm::vec3(1.0f, -2.0f, 0);
    state.enemies[0].speed = 1;
    state.enemies[0].aiType = WAITGO ;
    state.enemies[0].aiState = IDLE;
    //state.enemies[0].acceleration = glm::vec3(0,-9.8f, 0);
    state.enemies[0].height = 1.0f;
    state.enemies[0].width = .6f;
    

    
}
void OScreen::Update(float deltaTime) {
    playerRotate = playerRotate + (50.0f * deltaTime);
    
    state.player->Update(deltaTime, state.player, state.enemies, LEVEL2_ENEMYCOUNT, state.map);
    state.hearts->Update(deltaTime, state.player, state.enemies, LEVEL2_ENEMYCOUNT, state.map);

    state.enemies[0].Update(deltaTime, state.player, NULL, NULL, state.map);
    
    if(state.player->position.x >= 15){
        state.nextScene = 1;
    }
    
}

void OScreen::Render(ShaderProgram *program) {
    state.map->Render(program);
    state.player->Render(program);
    GLuint fontTextureID = Util::LoadTexture("font1.png");
    
    Util::DrawText(program, fontTextureID, "Press RETURN to begin" , .50f, -0.25f, glm::vec3(2.0f, -5.0f, 0));
    Util::DrawText(program, fontTextureID, "They say this forest is haunted..." , .45f, -0.25f, glm::vec3(2.0f, -6.0f, 0));

    Util::DrawText(program, fontTextureID, "The Coin" , .75f, -0.25f, glm::vec3(2.0f, -4.0f, 0));
    
}
