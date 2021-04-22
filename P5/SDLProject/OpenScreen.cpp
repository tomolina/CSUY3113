#include "OpenScreen.h"
#define LEVEL2_WIDTH 14
#define LEVEL2_HEIGHT 8
#define LEVEL2_ENEMYCOUNT 1
float playerRotate = 0;

unsigned int OScreen_data[] =
{
   45, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   45, 0, 0, 0, 46, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   45, 0, 0, 46, 0, 46, 0, 0, 0, 0, 0, 0, 0, 0,
   45, 0, 0, 0, 58, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   45, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   45, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   45, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 3,
   45, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3
};

void OScreen::Initialize() {
    state.nextScene = -1;

    GLuint mapTextureID = Util::LoadTexture("newTiles.png");
    state.map = new Map(LEVEL2_WIDTH, LEVEL2_HEIGHT, OScreen_data, mapTextureID, 1.0f, 14, 7);
    // Move over all of the player and enemy code from initialization.
    
    // Initialize Player
    state.player = new Entity();
    state.player->position = glm::vec3(4.0f, -1.5f, 0);
    
    state.player->entityType = PLAYER;
    state.player->velocity = glm::vec3(0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, -9.81f, 0);
    
    state.player->speed = 3.0f;
    state.player->textureID = Util::LoadTexture("george_0.png");
    
    state.player->height = .8f;
    state.player->width = .6f;
    state.player->jumpPower = 6.0f;
    
    
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
    
    state.enemies = new Entity[LEVEL2_ENEMYCOUNT];
    GLuint enemyTextureID = Util::LoadTexture("ctg.png");
    
    state.enemies[0].entityType = ENEMY;
    state.enemies[0].textureID = enemyTextureID;
    state.enemies[0].position = glm::vec3(1.0f, -2.0f, 0);
    state.enemies[0].speed = 1;
    state.enemies[0].aiType = WAITGO ;
    state.enemies[0].aiState = IDLE;
    state.enemies[0].acceleration = glm::vec3(0,-9.8f, 0);
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
    Util::DrawText(program, fontTextureID, "Avoid the Unusual.." , .45f, -0.25f, glm::vec3(6.0f, -2.0f, 0));

    Util::DrawText(program, fontTextureID, "Plataforma" , .75f, -0.25f, glm::vec3(2.0f, -4.0f, 0));
    
}
