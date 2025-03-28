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

    //Shaders
    //GLSLProgram shadowProg;
    GLSLProgram skyboxProg;
    GLSLProgram objectProg;

    void DrawScene(GLSLProgram& prog);
    void ResetCorridor();

    void setMatrices(GLSLProgram& prog);
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