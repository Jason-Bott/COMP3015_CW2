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

//Timer
float timer = 0.0f;

//For Menus
unsigned int quadVAO, quadVBO;

float quadVertices[] = {
    -1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
     1.0f, -1.0f,  1.0f, 0.0f,

    -1.0f,  1.0f,  0.0f, 1.0f,
     1.0f, -1.0f,  1.0f, 0.0f,
     1.0f,  1.0f,  1.0f, 1.0f
};

float timeInMenu = 0.0f;

//Relative position within world space
vec3 cameraPosition = vec3(0.0f, 0.0f, 14.0f);
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
int shipNumber = 0;
vec3 shipPosition = vec3(0.0f, 0.0f, 0.0f);
float shipScale = 0.5f;
float shipSpeed = 10.0f;

vec3 shipStartPositions[] = {
    vec3(-10.0f, -10.0f, 0.0f),
    vec3(-10.0f, -10.0f, -10.0f),
    vec3(-10.0f, -10.0f, 10.0f)
};

vec3 shipEndPositions[] = {
    vec3(-10.0f, 10.0f, 0.0f),
    vec3(-10.0f, 10.0f, 10.0f),
    vec3(-10.0f, 10.0f, -10.0f)
};

//Collisons
bool collide = true;
bool kPressed = false;

float windowWallPosition = -3;
bool windowNegative = false;

//Posters
vec3 posterPositions[] = {
    vec3(1.99f, 0.25f, -2.5f),
    vec3(1.99f, 0.25f, 0.0f),
    vec3(1.99f, 0.25f, 2.5f)
};

//Toys
vec3 toyPositions[] = {
    vec3(-2.4f, -0.42f, 7.3f),
    vec3(-2.0f, -0.42f, -0.4f),
    vec3(-2.0f, -0.42f, -1.0f),
    vec3(-2.4f, -0.42f, -5.4f),
    vec3(-2.4f, -0.42f, -4.0f)
};

float toyRotations[] = {
    135.0f,
    -135.0f,
    -135.0f,
    135.0f,
    135.0f
};

//Corridor Controls
bool canUpdateCorridor = true;
int corridorVariant = 0;
int currentCorridor = 8;
float whiteness = 0.0f;

SceneBasic_Uniform::SceneBasic_Uniform() : angle(0.0f), sky(100.0f), shadowMapWidth(30000), shadowMapHeight(30000), 
drawBuf(1), time(0.0f), deltaT(0), particleLifetime(3.0f), nParticles(4000), emitterPos(1, 0, 0), emitterDir(0, 1, 0)
{
    //Objects
    floor = ObjMesh::load("media/floor.obj", true);
    windowWall = ObjMesh::load("media/windowWall.obj", true);
    wall = ObjMesh::load("media/wall.obj", true);
    ceiling = ObjMesh::load("media/ceiling.obj", true);
    doorframe = ObjMesh::load("media/doorframe.obj", true);
    blastdoor = ObjMesh::load("media/blastdoor.obj", true);
    alarm = ObjMesh::load("media/alarm.obj", true);
    spaceship = ObjMesh::load("media/spaceship.obj", true);
    poster = ObjMesh::load("media/poster.obj", true);

    //Textures
    floorTexture = Texture::loadTexture("media/textures/floor.png");
    wallTexture = Texture::loadTexture("media/textures/wall.png");
    ceilingTexture = Texture::loadTexture("media/textures/ceiling.png");
    doorframeTexture = Texture::loadTexture("media/textures/doorframe.png");
    blastdoorTexture = Texture::loadTexture("media/textures/blastdoor.png");
    alarmTexture = Texture::loadTexture("media/textures/alarm.png");
    spaceshipTexture = Texture::loadTexture("media/textures/spaceship/StarSparrow_Red.png");

    //Corridor Numbers
    wall1Texture = Texture::loadTexture("media/textures/wall1.png");
    wall2Texture = Texture::loadTexture("media/textures/wall2.png");
    wall3Texture = Texture::loadTexture("media/textures/wall3.png");
    wall4Texture = Texture::loadTexture("media/textures/wall4.png");
    wall5Texture = Texture::loadTexture("media/textures/wall5.png");
    wall6Texture = Texture::loadTexture("media/textures/wall6.png");
    wall7Texture = Texture::loadTexture("media/textures/wall7.png");
    wall8Texture = Texture::loadTexture("media/textures/wall8.png");

    //Posters
    powerPath = Texture::loadTexture("media/textures/posters/PowerPath.png");
    endureTime = Texture::loadTexture("media/textures/posters/EndureTime.png");
    endlessBeyond = Texture::loadTexture("media/textures/posters/EndlessBeyond.png");
    flippedEndlessBeyond = Texture::loadTexture("media/textures/posters/FlippedEndlessBeyond.png");

    //Menus
    congratsMenu = Texture::loadTexture("media/textures/menus/congrats.png");
    madeItMenu = Texture::loadTexture("media/textures/menus/madeIt.png");
    digitTextures[0] = Texture::loadTexture("media/textures/menus/digit0.png");
    digitTextures[1] = Texture::loadTexture("media/textures/menus/digit1.png");
    digitTextures[2] = Texture::loadTexture("media/textures/menus/digit2.png");
    digitTextures[3] = Texture::loadTexture("media/textures/menus/digit3.png");
    digitTextures[4] = Texture::loadTexture("media/textures/menus/digit4.png");
    digitTextures[5] = Texture::loadTexture("media/textures/menus/digit5.png");
    digitTextures[6] = Texture::loadTexture("media/textures/menus/digit6.png");
    digitTextures[7] = Texture::loadTexture("media/textures/menus/digit7.png");
    digitTextures[8] = Texture::loadTexture("media/textures/menus/digit8.png");
    digitTextures[9] = Texture::loadTexture("media/textures/menus/digit9.png");
    colonTexture = Texture::loadTexture("media/textures/menus/colon.png");
    pointTexture = Texture::loadTexture("media/textures/menus/point.png");
}

void SceneBasic_Uniform::initScene()
{
    compile();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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

    //Menu Setup
    menuProg.use();
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glBindVertexArray(0);

    //Shadow Setup
    setupFBO();

    GLuint programHandle = mainProg.getHandle();
    pass1Index = glGetSubroutineIndex(programHandle, GL_FRAGMENT_SHADER, "recordDepth");
    pass2Index = glGetSubroutineIndex(programHandle, GL_FRAGMENT_SHADER, "shadeWithShadow");
    shadowBias = mat4(
        vec4(0.5f, 0.0f, 0.0f, 0.0f),
        vec4(0.0f, 0.5f, 0.0f, 0.0f),
        vec4(0.0f, 0.0f, 0.5f, 0.0f),
        vec4(0.5f, 0.5f, 0.5f, 1.0f)
    );

    mainProg.use();
    vec3 lightPos = vec3(-20.0f, 0.0f, 0.0f);
    lightFrustum.orient(lightPos, vec3(0.0f), vec3(0.0f, 1.0f, 0.0f));
    lightFrustum.setPerspective(90.0f, 1.0f, 1.0f, 25.0f);
    lightPV = shadowBias * lightFrustum.getProjectionMatrix() * lightFrustum.getViewMatrix();
    mainProg.setUniform("lights[0].Position", view * vec4(lightPos, 1.0f));
    mainProg.setUniform("lights[0].L", vec3(45.0f));
    mainProg.setUniform("ShadowMap", 0);
    mainProg.setUniform("OffsetTex", 1);
    mainProg.setUniform("Whiteness", whiteness);

    mainProg.setUniform("lights[1].Position", view * vec4(vec3(0.0f, 0.0f, 0.0f), 1.0));
    mainProg.setUniform("lights[1].L", vec3(0.0f, 0.0f, 0.0f));
    mainProg.setUniform("lights[2].Position", view * vec4(vec3(0.0f, 0.0f, 0.0f), 1.0));
    mainProg.setUniform("lights[2].L", vec3(0.0f, 0.0f, 0.0f));
    mainProg.setUniform("lights[3].Position", view * vec4(vec3(0.0f, 0.0f, 0.0f), 1.0));
    mainProg.setUniform("lights[3].L", vec3(0.0f, 0.0f, 0.0f));
    mainProg.setUniform("lights[4].Position", view * vec4(vec3(0.0f, 0.0f, 0.0f), 1.0));
    mainProg.setUniform("lights[4].L", vec3(0.0f, 0.0f, 0.0f));

    //Particle Setup
    particleProg.use();

    glActiveTexture(GL_TEXTURE2);
    Texture::loadTexture("media/textures/fire.png");
    glActiveTexture(GL_TEXTURE3);
    ParticleUtils::createRandomTex1D(nParticles * 3);

    initBuffers();

    particleProg.setUniform("RandomTex", 3);
    particleProg.setUniform("ParticleTex", 2);
    particleProg.setUniform("ParticleLifetime", particleLifetime);
    particleProg.setUniform("ParticleSize", 0.5f);
    particleProg.setUniform("Accel", vec3(0.0f, 0.1f, 0.0f));
    particleProg.setUniform("EmitterPos", emitterPos);
    particleProg.setUniform("EmitterBasis", ParticleUtils::makeArbitraryBasis(emitterDir));

    //Set Randomness Seed
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

void SceneBasic_Uniform::compile()
{
    try {
        menuProg.compileShader("shader/menu.vert");
        menuProg.compileShader("shader/menu.frag");
        menuProg.link();
        menuProg.use();

        skyboxProg.compileShader("shader/skybox.vert");
        skyboxProg.compileShader("shader/skybox.frag");
        skyboxProg.link();
        skyboxProg.use();

        particleProg.compileShader("shader/particle.vert");
        particleProg.compileShader("shader/particle.frag");
        GLuint progHandle = particleProg.getHandle();
        const char* outputNames[] = { "Position", "Velocity", "Age" };
        glTransformFeedbackVaryings(progHandle, 3, outputNames, GL_SEPARATE_ATTRIBS);
        particleProg.link();
        particleProg.use();

        mainProg.compileShader("shader/basic_uniform.vert");
        mainProg.compileShader("shader/basic_uniform.frag");
        mainProg.link();
        mainProg.use();
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

    if (currentCorridor == 0) {
        whiteness += deltaTime / 2.5;
        if (whiteness > 1.0f) {
            whiteness = 1.0f;
            timeInMenu += deltaTime;
        }

        mainProg.use();
        mainProg.setUniform("Whiteness", whiteness);

        return;
    }

    //Update Timer
    timer += deltaTime;

    //Corridor Variant Changes
    if (corridorVariant == 2) {
        shipScale += deltaTime / 10;
        if (shipScale > 1.5f) {
            shipScale = 1.5f;
        }
    }
    else {
        shipScale = 0.5f;
    }

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

            if (corridorVariant == 0) {
                ResetCorridor(false);
            }
            else {
                ResetCorridor(true);
            }
        }
        else if (cameraPosition.z <= -14.0f && positionBefore.z > -14.0f)
        {
            cameraPosition.z += 28.0f;

            if (corridorVariant == 0) {
                ResetCorridor(true);
            }
            else {
                ResetCorridor(false);
            }
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
        if (corridorVariant == 9) {
            negDoorHeight += (doorSpeed / 2) * deltaTime;
        }
        else {
            negDoorHeight += doorSpeed * deltaTime;
        }

        if (negDoorHeight > 2.51f) {
            negDoorHeight = 2.51f;
        }
    }
    else {
        if (corridorVariant == 9) {
            negDoorHeight -= (doorSpeed / 2) * deltaTime;
        }
        else {
            negDoorHeight -= doorSpeed * deltaTime;
        }

        if (negDoorHeight < -0.5f) {
            negDoorHeight = -0.5f;
        }
    }

    //Spaceship Updates
    if (length(shipPosition - shipEndPositions[shipNumber]) < 0.1) {
        if (shipNumber == 2) {
            shipNumber = 0;
        }
        else {
            shipNumber++;
        }
        shipPosition = shipStartPositions[shipNumber];
    }

    vec3 target = shipEndPositions[shipNumber];
    vec3 direction = glm::normalize(target - shipPosition);
    shipPosition += direction * shipSpeed * deltaTime; 

    //std::cout << cameraPosition.x << ", " << cameraPosition.z << std::endl;
    //std::cout << pitch << ", " << yaw << std::endl;
    //Update View
    view = lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);

    //Alarm Lighting
    if (corridorVariant != 3) 
    {
        if (negative) {
            brightness -= deltaTime * 10;
            if (brightness < 0.0f) {
                brightness = 0.0f;
                negative = false;
            }
        }
        else {
            brightness += deltaTime * 10;
            if (brightness > 10.0f) {
                brightness = 10.0f;
                negative = true;
            }
        }
    }

    mainProg.use();
    mainProg.setUniform("lights[0].Position", view * vec4(lightPos, 1.0f));
    mainProg.setUniform("lights[1].Position", view * vec4(vec3(0.0f, 1.0f, -14.0f), 1.0));
    mainProg.setUniform("lights[2].Position", view * vec4(vec3(0.0f, 1.0f, -5.0f), 1.0));
    mainProg.setUniform("lights[3].Position", view * vec4(vec3(0.0f, 1.0f, 5.0f), 1.0));
    mainProg.setUniform("lights[4].Position", view * vec4(vec3(0.0f, 1.0f, 14.0f), 1.0));

    mainProg.setUniform("lights[1].L", vec3(brightness, 0.0f, 0.0f));
    mainProg.setUniform("lights[2].L", vec3(brightness, 0.0f, 0.0f));
    mainProg.setUniform("lights[3].L", vec3(brightness, 0.0f, 0.0f));
    mainProg.setUniform("lights[4].L", vec3(brightness, 0.0f, 0.0f));

    //Particles
    deltaT = t - time;
    time = t;
}

void SceneBasic_Uniform::render()
{
    //Completion Screen
    if (whiteness >= 1.0f) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        menuProg.use();
        glBindVertexArray(quadVAO);

        //Timer
        int minutes = timer / 60;
        int seconds = int(timer) % 60;
        int milliseconds = (timer - int(timer)) * 100;

        if (timeInMenu > 3.0f) {
            int tens = minutes / 10;
            int ones = minutes % 10;

            drawDigit(digitTextures[tens], -0.7f, -0.5f, 0.1f, 0.2f);
            drawDigit(digitTextures[ones], -0.5f, -0.5f, 0.1f, 0.2f);
        }
        if (timeInMenu > 4.0f) {
            int tens = seconds / 10;
            int ones = seconds % 10;

            drawDigit(colonTexture, -0.3f, -0.5f, 0.1f, 0.2f);
            drawDigit(digitTextures[tens], -0.1f, -0.5f, 0.1f, 0.2f);
            drawDigit(digitTextures[ones], 0.1f, -0.5f, 0.1f, 0.2f);
        }
        if (timeInMenu > 5.0f) {
            int tens = milliseconds / 10;
            int ones = milliseconds % 10;

            drawDigit(pointTexture, 0.3f, -0.5f, 0.1f, 0.2f);
            drawDigit(digitTextures[tens], 0.5f, -0.5f, 0.1f, 0.2f);
            drawDigit(digitTextures[ones], 0.7f, -0.5f, 0.1f, 0.2f);
        }

        //Main Menu Section
        glActiveTexture(GL_TEXTURE5);
        if (timeInMenu < 2.0f) {
            glBindTexture(GL_TEXTURE_2D, congratsMenu);
        }
        else {
            glBindTexture(GL_TEXTURE_2D, madeItMenu);
        }
        menuProg.setUniform("position", glm::vec2(0.0f, 0.0f));
        menuProg.setUniform("scale", glm::vec2(1.0f, 1.0f));
        glDrawArrays(GL_TRIANGLES, 0, 6);

        return;
    }

    
    //
    //Main
    //
    mainProg.use();

    //Pass 1 Shadow Map Gen
    view = lightFrustum.getViewMatrix();
    projection = lightFrustum.getProjectionMatrix();
    glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, shadowMapWidth, shadowMapHeight);
    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &pass1Index);
    glEnable(GL_CULL_FACE);
    //glCullFace(GL_FRONT);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(2.5f, 10.0f);
    drawScene(mainProg);
    glCullFace(GL_BACK);
    glFlush();

    //Pass 2 Render Pass
    view = lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);
    projection = glm::perspective(glm::radians(70.0f), (float)windowWidth / windowHeight, 0.2f, 100.0f);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, windowWidth, windowHeight);
    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &pass2Index);
    glDisable(GL_CULL_FACE);
    drawScene(mainProg);

    //
    //Skybox
    //
    skyboxProg.use();
    model = mat4(1.0f);
    model = glm::translate(model, cameraPosition);
    setMatrices(skyboxProg);
    sky.render();

    //
    //Particles
    //
    particleProg.use();

    if (corridorVariant == 1) {
        glActiveTexture(GL_TEXTURE2);
        Texture::loadTexture("media/textures/purpleFire.png");
    }
    else {
        glActiveTexture(GL_TEXTURE2);
        Texture::loadTexture("media/textures/fire.png");
    }

    model = mat4(1.0f);
    model = glm::translate(model, vec3(-1.8f, -2.0f, 0.0f));
    model = glm::rotate(model, glm::radians(90.0f), vec3(0.0f, 1.0f, 0.0f));

    particleProg.setUniform("Time", time);
    particleProg.setUniform("DeltaT", deltaT);
    particleProg.setUniform("Pass", 1);

    glEnable(GL_RASTERIZER_DISCARD);
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedback[drawBuf]);
    glBeginTransformFeedback(GL_POINTS);
    glBindVertexArray(particleArray[1 - drawBuf]);
    glVertexAttribDivisor(0, 0);
    glVertexAttribDivisor(1, 0);
    glVertexAttribDivisor(2, 0);
    glDrawArrays(GL_POINTS, 0, nParticles);
    glBindVertexArray(0);
    glEndTransformFeedback();
    glDisable(GL_RASTERIZER_DISCARD);

    setMatrices(particleProg);
    particleProg.setUniform("Pass", 2);

    glDepthMask(GL_FALSE);
    glBindVertexArray(particleArray[drawBuf]);
    glVertexAttribDivisor(0, 1);
    glVertexAttribDivisor(1, 1);
    glVertexAttribDivisor(2, 1);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, nParticles);
    glBindVertexArray(0);
    glDepthMask(GL_TRUE);
    drawBuf = 1 - drawBuf;
}

void SceneBasic_Uniform::drawScene(GLSLProgram& prog)
{
    prog.setUniform("Material.Rough", 0.9f);
    prog.setUniform("Material.Metal", 0);
    prog.setUniform("Material.Color", vec3(0.1f));

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
    switch (currentCorridor)
    {
        case 1:
            glBindTexture(GL_TEXTURE_2D, wall1Texture);
            break;
        case 2:
            glBindTexture(GL_TEXTURE_2D, wall2Texture);
            break;
        case 3:
            glBindTexture(GL_TEXTURE_2D, wall3Texture);
            break;
        case 4:
            glBindTexture(GL_TEXTURE_2D, wall4Texture);
            break;
        case 5:
            glBindTexture(GL_TEXTURE_2D, wall5Texture);
            break;
        case 6:
            glBindTexture(GL_TEXTURE_2D, wall6Texture);
            break;
        case 7:
            glBindTexture(GL_TEXTURE_2D, wall7Texture);
            break;
        case 8:
            glBindTexture(GL_TEXTURE_2D, wall8Texture);
            break;
        default:
            glBindTexture(GL_TEXTURE_2D, wallTexture);
            break;
    }

    model = mat4(1.0f);
    model = glm::translate(model, vec3(-2.2f, 0.0f, 0.0f));
    model = glm::scale(model, vec3(1.0f, 1.01f, 1.0f));
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
    //Alarms
    //
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, alarmTexture);

    //Neg Room
    model = mat4(1.0f);
    model = glm::translate(model, vec3(0.0f, 2.0f, -14.0f));
    setMatrices(prog);
    alarm->render();

    //Neg Corridor
    model = mat4(1.0f);
    model = glm::translate(model, vec3(0.0f, 2.0f, -5.0f));
    setMatrices(prog);
    alarm->render();

    //Pos Corridor
    model = mat4(1.0f);
    model = glm::translate(model, vec3(0.0f, 2.0f, 5.0f));
    setMatrices(prog);
    alarm->render();

    //Pos Room
    model = mat4(1.0f);
    model = glm::translate(model, vec3(0.0f, 2.0f, 14.0f));
    setMatrices(prog);
    alarm->render();

    //
    //Spaceship Outside
    //
    prog.setUniform("Material.Rough", 0.3f);
    prog.setUniform("Material.Metal", 1);
    prog.setUniform("Material.Color", vec3(0.5f));

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, spaceshipTexture);

    model = mat4(1.0f);
    model = glm::translate(model, shipPosition);
    model = glm::scale(model, vec3(shipScale, shipScale, shipScale));
    switch (shipNumber) {
        case 0:
            model = glm::rotate(model, glm::radians(90.0f), vec3(0.0f, -1.0f, 0.0f));
            model = glm::rotate(model, glm::radians(90.0f), vec3(-1.0f, 0.0f, 0.0f));
            break;
        case 1:
            model = glm::rotate(model, glm::radians(45.0f), vec3(-1.0f, 0.0f, 0.0f));
            break;
        case 2:
            model = glm::rotate(model, glm::radians(180.0f), vec3(0.0f, 1.0f, 0.0f));
            model = glm::rotate(model, glm::radians(45.0f), vec3(-1.0f, 0.0f, 0.0f));
            break;
    }
    if (corridorVariant == 6) {
        model = glm::rotate(model, glm::radians(180.0f), vec3(0.0f, 1.0f, 0.0f));
    }
    setMatrices(prog);
    spaceship->render();

    //
    //Toy Spaceships
    //
    prog.setUniform("Material.Rough", 0.7f);
    prog.setUniform("Material.Metal", 1);
    prog.setUniform("Material.Color", vec3(0.2f));

    for (int i = 0; i < 4; i++)
    {
        if (corridorVariant == 7 && cameraPosition.z < toyPositions[i].z) {
            continue;
        }

        model = mat4(1.0f);
        model = glm::translate(model, toyPositions[i]);
        model = glm::scale(model, vec3(0.05f, 0.05f, 0.05f));
        model = glm::rotate(model, glm::radians(toyRotations[i]), vec3(0.0f, 1.0f, 0.0f));
        setMatrices(prog);
        spaceship->render();
    }

    if (corridorVariant == 8) {
        model = mat4(1.0f);
        model = glm::translate(model, toyPositions[4]);
        model = glm::scale(model, vec3(0.05f, 0.05f, 0.05f));
        model = glm::rotate(model, glm::radians(toyRotations[4]), vec3(0.0f, 1.0f, 0.0f));
        setMatrices(prog);
        spaceship->render();
    }

    //
    //Posters
    //
    prog.setUniform("Material.Rough", 0.9f);
    prog.setUniform("Material.Metal", 0);
    prog.setUniform("Material.Color", vec3(0.1f));

    for (int i = 0; i < 3; i++)
    {
        int textureIndex = (corridorVariant == 10) ? (2 - i) : i;

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
                if (corridorVariant == 5) {
                    glBindTexture(GL_TEXTURE_2D, flippedEndlessBeyond);
                }
                else {
                    glBindTexture(GL_TEXTURE_2D, endlessBeyond);
                }
                break;
        }

        model = mat4(1.0f);
        model = glm::translate(model, posterPositions[i]);
        if (corridorVariant == 4) {
            model = glm::scale(model, vec3(2.2f, 2.2f, 2.2f));
        }
        else {
            model = glm::scale(model, vec3(2.0f, 2.0f, 2.0f));
        }

        setMatrices(prog);
        poster->render();
    }
}

void SceneBasic_Uniform::ResetCorridor(bool correct)
{
    if (correct) {
        currentCorridor--;
    }
    else {
        currentCorridor = 8;
    }

    canUpdateCorridor = false;
    negDoorHeight = -0.5f;
    posDoorHeight = -0.5f;
    shipNumber = 0;
    shipPosition = vec3(0.0f, 0.0f, 0.0f);

    //50% chance to be normal
    if (std::rand() % 2 == 0) {
        corridorVariant = 0;
    }
    //10% (20 of 50) chance to be obvious variant
    else if (std::rand() % 5 == 0) {
        corridorVariant = std::rand() % 3 + 1;
    }
    //10% (25 of 40) chance to be obscure variant
    else if (std::rand() % 4 == 0) {
        corridorVariant = std::rand() % 2 + 4;
    }
    //30% chance to be basic variant
    else {
        corridorVariant = std::rand() % 5 + 6;
    }

    std::cout << corridorVariant << std::endl;
    //corridorVariant = std::rand() % 3 + 1;
    //3 represents how many different variants in that section
    //1 represents total number of variants before
}

void SceneBasic_Uniform::setMatrices(GLSLProgram& prog)
{
    mat4 mv = view * model;
    prog.setUniform("ModelViewMatrix", mv);
    prog.setUniform("NormalMatrix", mat3(mv));
    prog.setUniform("MVP", projection * mv);
    prog.setUniform("ShadowMatrix", lightPV * model);
    prog.setUniform("ProjectionMatrix", projection);
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
    projection = glm::perspective(glm::radians(70.0f), (float)w / h, 0.2f, 100.0f);
}

void SceneBasic_Uniform::drawDigit(GLuint texture, float x, float y, float w, float h) {
    menuProg.use();
    glBindVertexArray(quadVAO);

    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, texture);

    menuProg.setUniform("position", glm::vec2(x, y));
    menuProg.setUniform("scale", glm::vec2(w, h));

    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void SceneBasic_Uniform::setupFBO() 
{
    GLfloat border[] = { 1.0f, 0.0f, 0.0f, 0.0f };
    GLuint depthTex;

    glGenTextures(1, &depthTex);
    glBindTexture(GL_TEXTURE_2D, depthTex);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT24, shadowMapWidth, shadowMapHeight);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, depthTex);

    glGenFramebuffers(1, &shadowFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTex, 0);

    GLenum drawBuffers[] = { GL_NONE };
    glDrawBuffers(1, drawBuffers);
    GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (result == GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Framebuffer is complete" << endl;
    }
    else {
        std::cout << "Framebuffer error: " << result << endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SceneBasic_Uniform::initBuffers() 
{
    glGenBuffers(2, posBuf);
    glGenBuffers(2, velBuf);
    glGenBuffers(2, age);

    int size = nParticles * 3 * sizeof(GLfloat);
    glBindBuffer(GL_ARRAY_BUFFER, posBuf[0]);
    glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_COPY);
    glBindBuffer(GL_ARRAY_BUFFER, posBuf[1]);
    glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_COPY);
    glBindBuffer(GL_ARRAY_BUFFER, velBuf[0]);
    glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_COPY);
    glBindBuffer(GL_ARRAY_BUFFER, velBuf[1]);
    glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_COPY);
    glBindBuffer(GL_ARRAY_BUFFER, age[0]);
    glBufferData(GL_ARRAY_BUFFER, nParticles * sizeof(float), 0, GL_DYNAMIC_COPY);
    glBindBuffer(GL_ARRAY_BUFFER, age[1]);
    glBufferData(GL_ARRAY_BUFFER, nParticles * sizeof(float), 0, GL_DYNAMIC_COPY);

    std::vector<GLfloat> tempData(nParticles);
    float rate = particleLifetime / nParticles;
    for (int i = 0; i < nParticles; i++) {
        tempData[i] = rate * (i - nParticles);
    }
    Random::shuffle(tempData);
    glBindBuffer(GL_ARRAY_BUFFER, age[0]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, nParticles * sizeof(float), tempData.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenVertexArrays(2, particleArray);
    
    //Array 0
    glBindVertexArray(particleArray[0]);
    glBindBuffer(GL_ARRAY_BUFFER, posBuf[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, velBuf[0]);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, age[0]);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

    //Array 1
    glBindVertexArray(particleArray[1]);
    glBindBuffer(GL_ARRAY_BUFFER, posBuf[1]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, velBuf[1]);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, age[1]);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    glGenTransformFeedbacks(2, feedback);
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedback[0]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, posBuf[0]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, velBuf[0]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, age[0]);
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedback[1]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, posBuf[1]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, velBuf[1]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, age[1]);
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
}