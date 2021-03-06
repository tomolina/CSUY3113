#include "Entity.h"
#include "glm/gtc/matrix_transform.hpp"

Entity::Entity()
{
    position = glm::vec3(0);
    movement = glm::vec3(0);
    //acceleration = glm::vec3(0);
    velocity = glm::vec3(0);
    
    speed = 0;
    
    modelMatrix = glm::mat4(1.0f);
}


bool Entity::CheckCollision(Entity *other){
    
    if(isActive == false || other->isActive == false) return false;
    
    float xdist = fabs(position.x - other->position.x ) - ((width + other->width) / 2.0f);
    float ydist = fabs(position.y - other->position.y ) - ((height + other->height) / 2.0f);
    
    if(xdist < 0 && ydist < 0) return true;
    
    return false;
}




void Entity::CheckCollisionsY(Entity *objects, int objectCount){
   
    for (int i = 0; i < objectCount; i++){
        
       Entity *object = &objects[i];
       if (CheckCollision(object)){
           
           float ydist = fabs(position.y - object->position.y);
           float penetrationY = fabs(ydist - (height / 2.0f) - (object->height / 2.0f));
           if (velocity.y > 0) {
               position.y -= penetrationY;
               velocity.y = 0;
               collidedTop = true;
               if(objects->entityType == ENEMY){
                   if(objects->aiType == COIN){
                       upCoin = true;
                   }
                   else{
                       collidedTop_Enemy = true;
                   }
               }
               
           }
           
           else if (velocity.y < 0) {
               position.y += penetrationY;
               velocity.y = 0;
               collidedBottom = true;
               if(objects->entityType == ENEMY){
                   if(objects->aiType == COIN){
                       bottomCoin = true;
                   }
                   else{
                       collidedTop_Enemy = true;
                   }               }
           }
           
       }
    }
}

void Entity::CheckCollisionsX(Entity *objects, int objectCount){
    
   for (int i = 0; i < objectCount; i++){
       
       Entity *object = &objects[i];
       if (CheckCollision(object)){
           
           float xdist = fabs(position.x - object->position.x);
           float penetrationX = fabs(xdist - (width / 2.0f) - (object->width / 2.0f));
           if (velocity.x > 0) {
               position.x -= penetrationX;
               velocity.x = 0;
               collidedRight = true;
               if(objects->entityType == ENEMY){
                   if(object->aiType == COIN){
                       rightCoin = true;
                   }
                   else{
                       collidedRight_Enemy = true;
                   }
               }
               
           }
           else if (velocity.x < 0) {
               position.x += penetrationX;
               velocity.x = 0;
               collidedLeft = true;
               if(objects->entityType == ENEMY){
                   if(objects->aiType == COIN){
                       leftCoin = true;
                   }
                   else{
                       collidedLeft_Enemy = true;
                   }
               }
               
           }
       }
   }
}

void Entity::CheckCollisionsY(Map *map)
{
   // Probes for tiles
   glm::vec3 top = glm::vec3(position.x, position.y + (height / 2), position.z);
    
   glm::vec3 top_left = glm::vec3(position.x - (width / 2), position.y + (height / 2), position.z);
   glm::vec3 top_right = glm::vec3(position.x + (width / 2), position.y + (height / 2), position.z);
   glm::vec3 bottom = glm::vec3(position.x, position.y - (height / 2), position.z);
    
   glm::vec3 bottom_left = glm::vec3(position.x - (width / 2), position.y - (height / 2), position.z);
   glm::vec3 bottom_right = glm::vec3(position.x + (width / 2), position.y - (height / 2), position.z);
    
   float penetration_x = 0;
   float penetration_y = 0;
    
   if (map->IsSolid(top, &penetration_x, &penetration_y) && velocity.y > 0) {
       position.y -= penetration_y;
       velocity.y = 0;
       collidedTop = true;
   }
    
   else if (map->IsSolid(top_left, &penetration_x, &penetration_y) && velocity.y > 0) {
       position.y -= penetration_y;
       velocity.y = 0;
       collidedTop = true;
   }
    
   else if (map->IsSolid(top_right, &penetration_x, &penetration_y) && velocity.y > 0) {
       position.y -= penetration_y;
       velocity.y = 0;
       collidedTop = true;
   }
    if (map->IsSolid(bottom, &penetration_x, &penetration_y) && velocity.y < 0) {
        position.y += penetration_y;
        velocity.y = 0;
        collidedBottom = true;
    }
    else if (map->IsSolid(bottom_left, &penetration_x, &penetration_y) && velocity.y < 0) {
        position.y += penetration_y;
        velocity.y = 0;
        collidedBottom = true;
    }
    
    else if (map->IsSolid(bottom_right, &penetration_x, &penetration_y) && velocity.y < 0) {
        position.y += penetration_y;
        velocity.y = 0;
        collidedBottom = true;
    }
}

void Entity::CheckCollisionsX(Map *map)
{
    // Probes for tiles
    glm::vec3 left = glm::vec3(position.x - (width / 2), position.y, position.z);
    glm::vec3 right = glm::vec3(position.x + (width / 2), position.y, position.z);
    float penetration_x = 0;
    float penetration_y = 0;
    if (map->IsSolid(left, &penetration_x, &penetration_y) && velocity.x < 0) {
        position.x += penetration_x;
        velocity.x = 0;
        collidedLeft = true;
    }
    if (map->IsSolid(right, &penetration_x, &penetration_y) && velocity.x > 0) {
        position.x -= penetration_x;
        velocity.x = 0;
        collidedRight = true;
    }
}
    
    
    
    
void Entity::AIWalker(){
    movement = glm::vec3(-1, 0, 0);
}

void Entity::AIJumper(Entity *player){
    
    //movement = glm::vec3(0, 1, 0);
    
    
    if(position.y <= 2.5f && position.y >= 1.9f){
        jump = true;

    }
}


void Entity::AIPatrol(Entity *player){
    switch(aiState){
        case IDLE:
            break;
        case WALKING:
            break;
        case JUMPING:
            break;
        case PATROLLING:
            if(position.x >= 15){
                movement = glm::vec3(-1, 0, 0);
            }
            if(position.x <= 13){
                movement = glm::vec3(1, 0, 0);
                
        }

    }
}




void Entity::AIRotate(glm::vec3 sensor){
    //glm::mat4 matrix = glm::mat4(1.0f);
    switch (aiState) {
        case IDLE:
            break;
        case WALKING:
            break;
        case JUMPING:
            break;
        case PATROLLING:
            break;
        case ROTATE:
            //movement.x = .15f;
            
            //glPushMatrix();
            glRotatef(90.0, 5, 1, 0);
            break;

    }
    
    
}

void Entity::AIWaitGo(Entity *player){
    switch(aiState){
        case IDLE:
            if(glm::distance(position, player->position) < 3.5f){
                aiState = WALKING;
            }
            break;
            
        case WALKING:
            if(player->position.x < position.x){
                movement = glm::vec3(-1, 0, 0);
            }
//            else if(player->position.y < position.y){
//                movement = glm::vec3(0, -1, 0);
//            }
//            if(player->position.x > position.x){
//                movement = glm::vec3(0, 1, 0);
//            }
            else{
                movement = glm::vec3( 1, 0, 0);
            }
            
 //       case PATROLLING:
//            if(position.x == -1){
//                movement = glm::vec3(-1, 0, 0);
//            }
//            else if (position.x == -2.9f){
//                movement = glm::vec3(1, 0, 0);
//            }
 //           break;
            
//        case ROTATE:
//            movement.y += -.15f;
//            break;
    }
}

void Entity::AIcoin(Entity *player){
    movement = glm::vec3(0);
}

void Entity::AI(Entity *player, glm::vec3 sensor){
    switch (aiType) {
        case WALKER:
            AIWalker();
            break;
            
        case WAITGO:
            AIWaitGo(player);
            break;
        case JUMPER:
            AIJumper(player);
            break;
            
        case PATROLLER:
            AIPatrol(player);
            break;
        
        case SAW:
            AIRotate(sensor);
            break;
        case COIN:
            AIcoin(player);
            break;
    }
}

void Entity::Update(float deltaTime, Entity *player, Entity *objects, int objectCount, Map *map){
    
    if(isActive == false) return;
    
    collidedTop = false;
    collidedTop_Enemy = false;
    collidedBottom = false;
    collidedLeft = false;
    collidedRight = false;
    collidedRight_Enemy = false;
    collidedLeft_Enemy = false;
    
    rightCoin = false;
    leftCoin = false;
    bottomCoin = false;
    upCoin = false;

    glm::vec3 sensorRight = glm::vec3(position.x + .25f, position.y - .25f, 0);
    
    if(entityType == ENEMY){
//        if(aiType == SAW){
//            glm::rotate(this, .45f, glm::vec3(0, 0, 1.0f));
//        }
        
        AI(player, sensorRight);
    }
    
    
    
    if (animIndices != NULL) {
        if (glm::length(movement) != 0) {
            animTime += deltaTime;

            if (animTime >= 0.25f)
            {
                animTime = 0.0f;
                animIndex++;
                if (animIndex >= animFrames)
                {
                    animIndex = 0;
                }
            }
        }
        else {
            animIndex = 0;
        }
    }
    
    if(jump){
        jump = false;
        velocity.y += jumpPower;
    }
    
    
    velocity.x = movement.x * speed;
    //velocity += acceleration * deltaTime;
    velocity.y = movement.y * speed;
    
    position.y += velocity.y * deltaTime;        // Move on Y
    CheckCollisionsY(map);
    CheckCollisionsY(objects, objectCount);      // Fix if needed
    
    position.x += velocity.x * deltaTime;        // Move on X
    CheckCollisionsX(map);
    CheckCollisionsX(objects, objectCount);      // Fix if needed
    
    
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
}



void Entity::DrawSpriteFromTextureAtlas(ShaderProgram *program, GLuint textureID, int index)
{
    float u = (float)(index % animCols) / (float)animCols;
    float v = (float)(index / animCols) / (float)animRows;
    
    float width = 1.0f / (float)animCols;
    float height = 1.0f / (float)animRows;
    
    float texCoords[] = { u, v + height, u + width, v + height, u + width, v,
        u, v + height, u + width, v, u, v};
    
    float vertices[]  = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);
    
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

void Entity::Render(ShaderProgram *program) {
    
    if(isActive == false) return;
    
    program->SetModelMatrix(modelMatrix);
    
    if (animIndices != NULL) {
        DrawSpriteFromTextureAtlas(program, textureID, animIndices[animIndex]);
        return;
    }
    
    float vertices[]  = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
    
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);
    
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}
