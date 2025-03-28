#include "scenebasic_uniform.h"

#include <cstdio>
#include <cstdlib>

#include <string>
using std::string;

#include <sstream>
#include <iostream>
#include <random>
using std::cerr;
using std::endl;

#include "helper/glutils.h"
#include "helper/texture.h"

using glm::vec3;
using glm::vec4;
using glm::mat3;
using glm::mat4;

#include <GLFW/glfw3.h>

//For working out delta time
float lastFrameTime = 0.0f;

//Relative position within world space
vec3 cameraPosition = vec3(0.0f, 0.0f, 10.0f);
//The direction of travel
vec3 cameraFront = vec3(0.0f, 0.0f, -1.0f);
//Up position within world space
vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);
//Fixed height of camera
float fixedY = 0.0f;

//For mouse controls
float lastX = 800.0f / 2.0;
float lastY = 600.0f / 2.0;
float yaw = -90.0f;
float pitch = 0.0f;
bool firstMouse = true;

//For Window
int windowWidth, windowHeight;
bool resized = false;

//For lighting animation
float brightness = 0.0f;
bool negative = true;

//For blastdoor control
float negDoorHeight = -0.5f;
float posDoorHeight = -0.5f;

//For spaceship control
float shipHeight = -10.0f;
bool shipRising = false;

//Collisons
bool collide = true;
bool kPressed = false;

//Lights
vec4 lightPositions[] = {
    vec4(0.0f, 0.0f, -35.0f, 1.0f),
    vec4(0.0f, 0.0f, 35.0f, 1.0f),
    vec4(0.0f, 0.0f, -35.0f, 1.0f),
    vec4(0.0f, 0.0f, 35.0f, 1.0f)
};

//Posters
vec3 posterPositions[] = {
    vec3(1.99f, 0.25f, -2.5f),
    vec3(1.99f, 0.25f, 0.0f),
    vec3(1.99f, 0.25f, 2.5f)
};

//Corridor Controls
bool canUpdateCorridor = true;
int corridorVariant = 0;

SceneBasic_Uniform::SceneBasic_Uniform() : angle(0.0f), sky(100.0f)
{
    //Objects
    floor = ObjMesh::load("media/floor.obj", true);
    windowWall = ObjMesh::load("media/windowWall.obj", true);
    wall = ObjMesh::load("media/wall.obj", true);
    ceiling = ObjMesh::load("media/ceiling.obj", true);
    doorframe = ObjMesh::load("media/doorframe.obj", true);
    blastdoor = ObjMesh::load("media/blastdoor.obj", true);
    spaceship = ObjMesh::load("media/spaceship.obj", true);
    poster = ObjMesh::load("media/poster.obj", true);

    //Textures
    floorTexture = Texture::loadTexture("media/textures/floor.png");
    wallTexture = Texture::loadTexture("media/textures/wall.png");
    ceilingTexture = Texture::loadTexture("media/textures/ceiling.png");
    doorframeTexture = Texture::loadTexture("media/textures/doorframe.png");
    blastdoorTexture = Texture::loadTexture("media/textures/blastdoor.png");
    spaceshipTexture = Texture::loadTexture("media/textures/spaceship/StarSparrow_Red.png");

    //Posters
    powerPath = Texture::loadTexture("media/textures/posters/PowerPath.png");
    endureTime = Texture::loadTexture("media/textures/posters/EndureTime.png");
    endlessBeyond = Texture::loadTexture("media/textures/posters/EndlessBeyond.png");
}

void SceneBasic_Uniform::initScene()
{
    compile();
    glEnable(GL_DEPTH_TEST);

    model = mat4(1.0f);
    view = lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);
    projection = mat4(1.0f);

    //Window Settings
    GLFWwindow* window = glfwGetCurrentContext();
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, SceneBasic_Uniform::mouse_callback);

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);

    windowWidth = mode->width;
    windowHeight = mode->height;

    //Skybox
    GLuint cubeTex = Texture::loadCubeMap("media/skybox/space");
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTex);

    for (int i = 0; i < 4; i++) {
        std::stringstream position;
        position << "lights[" << i << "].Position";
        objectProg.setUniform(position.str().c_str(), view * lightPositions[i]);

        std::stringstream L;
        L << "lights[" << i << "].L";
        objectProg.setUniform(L.str().c_str(), vec3(0.8f, 0.0f, 0.0f));

        std::stringstream La;
        La << "lights[" << i << "].La";
        objectProg.setUniform(La.str().c_str(), vec3(0.8f, 0.0f, 0.0f));

        std::stringstream Intensity;
        Intensity << "lights[" << i << "].Intensity";
        objectProg.setUniform(Intensity.str().c_str(), 0.2f);
    }
}

void SceneBasic_Uniform::compile()
{
    try {
        skyboxProg.compileShader("shader/skybox.vert");
        skyboxProg.compileShader("shader/skybox.frag");
        skyboxProg.link();
        skyboxProg.use();

        objectProg.compileShader("shader/object.vert");
        objectProg.compileShader("shader/object.frag");
        objectProg.link();
        objectProg.use();
    }
    catch (GLSLProgramException& e) {
        cerr << e.what() << endl;
        exit(EXIT_FAILURE);
    }
}

void SceneBasic_Uniform::update(float t)
{
    if (!resized)
    {
        resize(windowWidth, windowHeight);
        resized = true;
    }

    GLFWwindow* window = glfwGetCurrentContext();

    float deltaTime = t - lastFrameTime;
    lastFrameTime = t;

    vec3 positionBefore = cameraPosition;

    //Movement
    const float movementSpeed = 5.0f * deltaTime;
    vec3 forwardDir = normalize(vec3(cameraFront.x, 0.0f, cameraFront.z));
    vec3 rightDir = normalize(cross(forwardDir, cameraUp));

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        cameraPosition += movementSpeed * forwardDir;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        cameraPosition -= movementSpeed * forwardDir;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        cameraPosition -= rightDir * movementSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        cameraPosition += rightDir * movementSpeed;
    }

    //Fix Height
    cameraPosition.y = fixedY;

    float x = cameraPosition.x;
    float z = cameraPosition.z;

    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS && !kPressed)
    {
        collide = !collide;
        kPressed = true;
    }

    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_RELEASE)
    {
        kPressed = false;
    }

    if (collide)
    {
        //Wall Collision Check
        if (x < -1.5f) {
            cameraPosition.x = -1.5f;
        }
        else if (x > 1.5f) {
            cameraPosition.x = 1.5f;
        }

        if (z < -16.8f) {
            cameraPosition.z = -16.8f;
        }
        else if (z > 16.8f) {
            cameraPosition.z = 16.8f;
        }

        //Doorframe Collision Check
        if (((z > 8.75f && z < 11.25f) || (z < -8.75f && z > -11.25f)))
        {
            if (!(positionBefore.x < 0.5f && positionBefore.x > -0.5f))
            {
                cameraPosition.z = positionBefore.z;
            }
            else if (!(cameraPosition.x < 0.5f && cameraPosition.x > -0.5f))
            {
                cameraPosition.x = positionBefore.x;
            }
        }
    }

    //Next Corridor Check
    if (!canUpdateCorridor)
    {
        if (cameraPosition.z < 10.0f)
        {
            canUpdateCorridor = true;
        }
    }
    else
    {
        if (cameraPosition.z >= 14.0f && positionBefore.z < 14.0f)
        {
            cameraPosition.x *= -1.0f;
            cameraPosition.z = 14.0f - (cameraPosition.z - 14.0f);

            yaw += 180.0f;
            glm::vec3 front;
            front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
            front.y = sin(glm::radians(pitch));
            front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
            cameraFront = glm::normalize(front);

            ResetCorridor();
        }
        else if (cameraPosition.z <= -14.0f && positionBefore.z > -14.0f)
        {
            cameraPosition.z += 28.0f;

            ResetCorridor();
        }
    }

    //Blastdoor Updates
    float doorSpeed = 4.0f;

    if (cameraPosition.z >= 6.0f) {
        posDoorHeight += doorSpeed * deltaTime;
        if (posDoorHeight > 2.51f) {
            posDoorHeight = 2.51f;
        }
    }
    else {
        posDoorHeight -= doorSpeed * deltaTime;
        if (posDoorHeight < -0.5f) {
            posDoorHeight = -0.5f;
        }
    }

    if (cameraPosition.z <= -6.0f) {
        negDoorHeight += doorSpeed * deltaTime;
        if (negDoorHeight > 2.51f) {
            negDoorHeight = 2.51f;
        }
    }
    else {
        negDoorHeight -= doorSpeed * deltaTime;
        if (negDoorHeight < -0.5f) {
            negDoorHeight = -0.5f;
        }
    }

    //Spaceship Updates
    float shipSpeed = 2.5f;

    if (!shipRising) {
        if (positionBefore.z > 6.0f && cameraPosition.z <= 6.0f) {
            shipRising = true;
        }
    }
    else {
        shipHeight += shipSpeed * deltaTime;
        if (shipHeight > -1.0f) {
            shipHeight = -1.0f;
        }
    }

    //std::cout << cameraPosition.x << ", " << cameraPosition.z << std::endl;
    //std::cout << pitch << ", " << yaw << std::endl;
    //Update View
    view = lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);

    //Light Positions
    for (int i = 0; i < 4; i++) {
        std::stringstream position;
        position << "lights[" << i << "].Position";
        objectProg.setUniform(position.str().c_str(), view * lightPositions[i]);
    }

    //Alarm Lighting
    if (negative) {
        brightness -= deltaTime / 10;
        if (brightness < 0.0f) {
            brightness = 0.0f;
            negative = false;
        }
    }
    else {
        brightness += deltaTime;
        if (brightness > 0.2f) {
            brightness = 0.2f;
            negative = true;
        }
    }

    objectProg.setUniform("lights[2].Intensity", brightness);
    objectProg.setUniform("lights[3].Intensity", brightness);
}

void SceneBasic_Uniform::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //
    //Skybox
    //
    skyboxProg.use();
    model = mat4(1.0f);
    setMatrices(skyboxProg);
    sky.render();

    //
    //Objects
    //
    objectProg.use();
    DrawScene(objectProg);
}

void SceneBasic_Uniform::DrawScene(GLSLProgram& prog)
{
    prog.use();
    prog.setUniform("Material.Kd", vec3(0.4f, 0.4f, 0.4f));
    prog.setUniform("Material.Ka", vec3(0.5f, 0.5f, 0.5f));
    prog.setUniform("Material.Ks", vec3(0.2f, 0.2f, 0.2f));
    prog.setUniform("Material.Shininess", 80.0f);

    //
    //Floor
    //
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, floorTexture);

    model = mat4(1.0f);
    model = glm::translate(model, vec3(0.0f, -2.0f, 0.0f));
    setMatrices(prog);
    floor->render();

    //
    //Window Wall
    //
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, wallTexture);

    model = mat4(1.0f);
    model = glm::translate(model, vec3(-2.2f, 0.0f, 0.0f));
    setMatrices(prog);
    windowWall->render();

    //
    //Wall
    //
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, wallTexture);

    model = mat4(1.0f);
    model = glm::translate(model, vec3(2.0f, 0.0f, 0.0f));
    setMatrices(prog);
    wall->render();

    //
    //Ceiling
    //
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, ceilingTexture);

    model = mat4(1.0f);
    model = glm::translate(model, vec3(0.0f, 2.0f, 0.0f));
    setMatrices(prog);
    ceiling->render();

    //
    //Doorframes
    //
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, doorframeTexture);

    //Inner Neg
    model = mat4(1.0f);
    model = glm::translate(model, vec3(0.0f, 0.0f, -10.0f));
    setMatrices(prog);
    doorframe->render();

    //Inner Pos
    model = mat4(1.0f);
    model = glm::translate(model, vec3(0.0f, 0.0f, 10.0f));
    setMatrices(prog);
    doorframe->render();

    //Outer Neg
    model = mat4(1.0f);
    model = glm::translate(model, vec3(0.0f, 0.0f, -18.0f));
    setMatrices(prog);
    doorframe->render();

    //Outer Pos
    model = mat4(1.0f);
    model = glm::translate(model, vec3(0.0f, 0.0f, 18.0f));
    setMatrices(prog);
    doorframe->render();

    //
    //Blastdoors
    //
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, blastdoorTexture);

    //Inner Neg
    model = mat4(1.0f);
    model = glm::translate(model, vec3(0.0f, negDoorHeight, -10.0f));
    setMatrices(prog);
    blastdoor->render();

    //Inner Pos
    model = mat4(1.0f);
    model = glm::translate(model, vec3(0.0f, posDoorHeight, 10.0f));
    setMatrices(prog);
    blastdoor->render();

    //Outer Neg
    model = mat4(1.0f);
    model = glm::translate(model, vec3(0.0f, -0.5f, -18.0f));
    setMatrices(prog);
    blastdoor->render();

    //Outer Pos
    model = mat4(1.0f);
    model = glm::translate(model, vec3(0.0f, -0.5f, 18.0f));
    setMatrices(prog);
    blastdoor->render();

    //
    //Spaceship
    //
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, spaceshipTexture);

    model = mat4(1.0f);
    model = glm::scale(model, vec3(0.5f, 0.5f, 0.5f));
    model = glm::rotate(model, glm::radians(90.0f), vec3(0.0f, 1.0f, 0.0f));
    model = glm::translate(model, vec3(0.0f, shipHeight, -30.0f));
    setMatrices(prog);
    spaceship->render();

    //
    //Posters
    //
    for (int i = 0; i < 3; i++)
    {
        int textureIndex = (corridorVariant == 1) ? (2 - i) : i;

        glActiveTexture(GL_TEXTURE1);
        switch (textureIndex)
        {
        case 0:
            glBindTexture(GL_TEXTURE_2D, powerPath);
            break;
        case 1:
            glBindTexture(GL_TEXTURE_2D, endureTime);
            break;
        case 2:
            glBindTexture(GL_TEXTURE_2D, endlessBeyond);
            break;
        }

        model = mat4(1.0f);
        model = glm::translate(model, posterPositions[i]);
        model = glm::scale(model, vec3(2.0f, 2.0f, 2.0f));
        setMatrices(prog);
        poster->render();
    }
}

void SceneBasic_Uniform::ResetCorridor()
{
    canUpdateCorridor = false;
    negDoorHeight = -0.5f;
    posDoorHeight = -0.5f;
    shipHeight = -10.0f;

    if (rand() % 2 == 0) {
        corridorVariant = 0;
    }
    else {
        corridorVariant = rand() % 1 + 1;
    }
}

void SceneBasic_Uniform::setMatrices(GLSLProgram& prog)
{
    mat4 mv = view * model;
    prog.setUniform("ModelViewMatrix", mv);
    prog.setUniform("NormalMatrix", mat3(mv));
    prog.setUniform("MVP", projection * mv);
}

void SceneBasic_Uniform::mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    const float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

void SceneBasic_Uniform::resize(int w, int h)
{
    width = w;
    height = h;
    glViewport(0, 0, w, h);
    projection = glm::perspective(glm::radians(70.0f), (float)w / h, 0.3f, 100.0f);
}