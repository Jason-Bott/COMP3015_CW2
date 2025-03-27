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
    std::unique_ptr<ObjMesh> spaceship;
    std::unique_ptr<ObjMesh> poster;

    //Textures
    GLuint floorTexture;
    GLuint wallTexture;
    GLuint ceilingTexture;
    GLuint doorframeTexture;
    GLuint blastdoorTexture;
    GLuint spaceshipTexture;

    //Posters
    GLuint powerPath;
    GLuint endureTime;
    GLuint endlessBeyond;

    //Normal Maps
    GLuint defaultNormal;
    GLuint spaceshipNormal;

    //Shadows
    void setupFBO();
    GLuint shadowMap, shadowFBO, pass1Index, pass2Index;
    int shadowMapWidth, shadowMapHeight;
    glm::mat4 lightPV, shadowBias;
    glm::vec3 lightPos;
    glm::mat4 shadowScale;
    Frustum lightFrustum;

    //Jitter
    float jitter();
    void buildJitterTex();
    int samplesU, samplesV;
    int jitterMapSize;
    float radius;

    //Shaders
    GLSLProgram shadowProg;
    GLSLProgram skyboxProg;
    GLSLProgram objectProg;

    void renderShadow();
    void renderSkybox();
    void renderObjects();


    void DrawScene();
    void ResetCorridor();

    void setMatrices();
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