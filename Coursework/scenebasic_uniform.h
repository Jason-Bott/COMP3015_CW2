#ifndef SCENEBASIC_UNIFORM_H
#define SCENEBASIC_UNIFORM_H

#include "helper/scene.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "helper/glslprogram.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "helper/objmesh.h"
#include "helper/skybox.h"
#include "helper/frustum.h"
#include "helper/random.h"
#include "helper/grid.h"
#include "helper/particleutils.h"

class SceneBasic_Uniform : public Scene
{
private:
    float angle;
    SkyBox sky;

    //Objects
    std::unique_ptr<ObjMesh> floor;
    std::unique_ptr<ObjMesh> windowWall;
    std::unique_ptr<ObjMesh> wall;
    std::unique_ptr<ObjMesh> ceiling;
    std::unique_ptr<ObjMesh> doorframe;
    std::unique_ptr<ObjMesh> blastdoor;
    std::unique_ptr<ObjMesh> alarm;
    std::unique_ptr<ObjMesh> spaceship;
    std::unique_ptr<ObjMesh> poster;

    //Textures
    GLuint floorTexture;
    GLuint wallTexture;
    GLuint ceilingTexture;
    GLuint doorframeTexture;
    GLuint blastdoorTexture;
    GLuint alarmTexture;
    GLuint spaceshipTexture;

    //Corridor Numbers
    GLuint wall1Texture;
    GLuint wall2Texture;
    GLuint wall3Texture;
    GLuint wall4Texture;
    GLuint wall5Texture;
    GLuint wall6Texture;
    GLuint wall7Texture;
    GLuint wall8Texture;

    //Posters
    GLuint powerPath;
    GLuint endureTime;
    GLuint endlessBeyond;
    GLuint flippedEndlessBeyond;

    //Menus
    GLuint congratsMenu;
    GLuint madeItMenu;
    GLuint digitTextures[10];
    GLuint colonTexture;
    GLuint pointTexture;

    //Shaders
    GLSLProgram menuProg;
    GLSLProgram skyboxProg;
    GLSLProgram particleProg;
    GLSLProgram mainProg;

    //Shadows
    void setupFBO();
    GLuint shadowFBO, pass1Index, pass2Index;
    int shadowMapWidth, shadowMapHeight;
    glm::mat4 lightPV, shadowBias;
    glm::vec3 lightPos;
    glm::mat4 shadowScale;
    Frustum lightFrustum;

    //Particles
    float tPrev;
    float time, deltaT;
    float particleLifetime;
    int nParticles;
    Random rand;

    //Particle Buffer
    GLuint posBuf[2], velBuf[2], age[2];

    //VAO
    GLuint particleArray[2];

    //Transform Feedbacks
    GLuint feedback[2];

    GLuint drawBuf;
    glm::vec3 emitterPos, emitterDir;

    void initBuffers();
    //float randFloat();

    //Gameplay Loop
    void ResetCorridor(bool correct);

    void drawScene(GLSLProgram& prog);
    void setMatrices(GLSLProgram& prog);
    void drawDigit(GLuint texture, float x, float y, float w, float h);
    void compile();
    static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
public:
    SceneBasic_Uniform();

    void initScene();
    void update( float t );
    void render();
    void resize(int, int);
};

#endif // SCENEBASIC_UNIFORM_H