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
#include <vector>

#include <SDL_mixer.h>


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Entity.h"
#define BAR_COUNT 9


struct GameState{
    Entity *leftPad;
    Entity *square;
    Entity *rightPad;
    Entity *bars;
};

GameState state;

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

Mix_Music *music;
Mix_Chunk *bounce;

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
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
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
    
    glClearColor(0.0f, 0.5f, 0.8f, 0.0f);
    glEnable(GL_BLEND);
    
    // Good setting for transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    //Textures!!
    state.square = new Entity();
    
    GLuint squareTextureID = LoadTexture("square.png");
    state.square->textureID = squareTextureID;
    state.square->position = glm::vec3(0.0f, 0.0f, 0.0f);
    state.square->movement = glm::vec3(0);
    state.square->speed = 2.0f;
    
    
    state.bars = new Entity[BAR_COUNT];
    GLuint barsTextureID = LoadTexture("bar.png");
   
    
    state.bars[0].textureID = barsTextureID;
    state.bars[0].position = glm::vec3(0.0f, 2.0f, 0.0f);
    state.bars[0].movement = glm::vec3(0);
    
    state.bars[1].textureID = barsTextureID;
    state.bars[1].position = glm::vec3(0.0f, 1.0f, 0.0f);
    state.bars[1].movement = glm::vec3(0);
    
    state.bars[2].textureID = barsTextureID;
    state.bars[2].position = glm::vec3(0.0f, 0.0f, 0.0f);
    state.bars[2].movement = glm::vec3(0);
    
    state.bars[3].textureID = barsTextureID;
    state.bars[3].position = glm::vec3(0.0f, 3.0f, 0.0f);
    state.bars[3].movement = glm::vec3(0);
    
    state.bars[4].textureID = barsTextureID;
    state.bars[4].position = glm::vec3(0.0f, -1.0f, 0.0f);
    state.bars[4].movement = glm::vec3(0);
    
    state.bars[5].textureID = barsTextureID;
    state.bars[5].position = glm::vec3(0.0f, -2.0f, 0.0f);
    state.bars[5].movement = glm::vec3(0);
    
    state.bars[6].textureID = barsTextureID;
    state.bars[6].position = glm::vec3(0.0f, -3.0f, 0.0f);
    state.bars[6].movement = glm::vec3(0);
    
    state.bars[7].textureID = barsTextureID;
    state.bars[7].position = glm::vec3(0.0f, -4.0f, 0.0f);
    state.bars[7].movement = glm::vec3(0);
    
    state.bars[8].textureID = barsTextureID;
    state.bars[8].position = glm::vec3(0.0f, 4.0f, 0.0f);
    state.bars[8].movement = glm::vec3(0);
    
    state.bars[9].textureID = barsTextureID;
    state.bars[9].position = glm::vec3(0.0f, 5.0f, 0.0f);
    state.bars[9].movement = glm::vec3(0);
    
    
    GLuint paddleTextureID = LoadTexture("Paddle2.png");
    
    state.leftPad = new Entity();
    state.leftPad->position = glm::vec3(-5.0f, 0.0f, 0.0f);
    state.leftPad->movement = glm::vec3(0);
    state.leftPad->speed = 3.0f;
    state.leftPad->textureID = paddleTextureID;
    
    state.rightPad = new Entity();
    state.rightPad->textureID = paddleTextureID;
    state.rightPad->position = glm::vec3(5.0f, 0.0f, 0.0f);
    state.rightPad->movement = glm::vec3(0);
    state.rightPad->speed = 3.0f;
    
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    music = Mix_LoadMUS("dooblydoo.mp3");
    Mix_PlayMusic(music, -1);
    Mix_VolumeMusic(MIX_MAX_VOLUME / 4);
    
    bounce = Mix_LoadWAV("bounce.wav");
    
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
   state.leftPad->movement = glm::vec3(0);
   state.rightPad->movement = glm::vec3(0);
    
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
                       state.square->movement = glm::vec3(myX_direc[rand_X], myY_direc[rand_Y],0.0f);
                       break;
                   case SDLK_r:
                       state.square->position = glm::vec3(0);
                       state.leftPad->position = glm::vec3(-5.0f, 0.0f, 0.0f);
                       state.rightPad->position = glm::vec3(5.0f, 0.0f, 0.0f);
                       break;
               }
               break; // SDL_KEYDOWN
       }
       
   }
    
    //Creating borders for pong to bounce off of
    if(state.square->position.y > 3.6f){
        state.square->movement.y *= -1.0f;
        Mix_PlayChannel(-1, bounce, 0);
    }
    else if(state.square->position.y < -3.6f){
        state.square->movement.y *= -1.0f;
        Mix_PlayChannel(-1, bounce, 0);
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
    
    for(int i = 0; i < BAR_COUNT; i++){
        state.bars[i].Update(deltaTime);
    }

    state.square->Update(deltaTime);
    state.leftPad->Update(deltaTime);
    state.rightPad->Update(deltaTime);
    
    
}

//Rendering for Textures!
void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    GLuint fontTextureID = LoadTexture("font2.png");
    DrawText(&program, fontTextureID, "Player 1" , 0.5f, -0.25f, glm::vec3(-4.0f, 3.3, 0));
    
    DrawText(&program, fontTextureID, "Player 2" , 0.5f, -0.25f, glm::vec3(2.5f, 3.3, 0));
    
    for(int i = 0; i < BAR_COUNT; i++){
        state.bars[i].Render(&program);
    }
    
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
