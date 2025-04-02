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
    floor = ObjMesh::loadWithAdjacency("media/floor.obj", true);
    windowWall = ObjMesh::loadWithAdjacency("media/windowWall.obj", true);
    wall = ObjMesh::loadWithAdjacency("media/wall.obj", true);
    ceiling = ObjMesh::loadWithAdjacency("media/ceiling.obj", true);
    doorframe = ObjMesh::loadWithAdjacency("media/doorframe.obj", true);
    blastdoor = ObjMesh::loadWithAdjacency("media/blastdoor.obj", true);
    spaceship = ObjMesh::loadWithAdjacency("media/spaceship.obj", true);
    poster = ObjMesh::loadWithAdjacency("media/poster.obj", true);
}

void SceneBasic_Uniform::initScene()
{
    compile();
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClearStencil(0);
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

    resize(windowWidth, windowHeight);

    //Skybox
    GLuint cubeTex = Texture::loadCubeMap("media/skybox/space");
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTex);

    //Shadows
    setupFBO();

    renderProg.use();
    renderProg.setUniform("LightIntensity", vec3(1.0f));

    GLfloat verts[] = { -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f };
    GLuint bufHandle;
    glGenBuffers(1, &bufHandle);
    glBindBuffer(GL_ARRAY_BUFFER, bufHandle);
    glBufferData(GL_ARRAY_BUFFER, 4 * 3 * sizeof(GLfloat), verts, GL_STATIC_DRAW);

    glGenVertexArrays(1, &fsQuad);
    glBindVertexArray(fsQuad);
    glBindBuffer(GL_ARRAY_BUFFER, bufHandle);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    glActiveTexture(GL_TEXTURE2);
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

    updateLight();
    renderProg.use();
    renderProg.setUniform("Tex", 2);
    compProg.use();
    compProg.setUniform("DiffSpecTex", 0);
}

void SceneBasic_Uniform::updateLight() 
{
    lightPos = vec4(-20.0f, 0.0f, 0.0f, 1.0f);
}

void SceneBasic_Uniform::compile()
{
    try {
        skyboxProg.compileShader("shader/skybox.vert");
        skyboxProg.compileShader("shader/skybox.frag");
        skyboxProg.link();
        skyboxProg.use();

        volumeProg.compileShader("shader/shadowvolume.vert");
        volumeProg.compileShader("shader/shadowvolume.frag");
        volumeProg.compileShader("shader/shadowvolume.gs");
        volumeProg.link();
        volumeProg.use();

        renderProg.compileShader("shader/shadowvolume-render.vs");
        renderProg.compileShader("shader/shadowvolume-render.fs");
        renderProg.link();
        renderProg.use();

        compProg.compileShader("shader/shadowvolume-comp.vs");
        compProg.compileShader("shader/shadowvolume-comp.fs");
        compProg.link();
        compProg.use();

        /*objectProg.compileShader("shader/object.vert");
        objectProg.compileShader("shader/object.frag");
        objectProg.link();
        objectProg.use();*/
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
    /*for (int i = 0; i < 4; i++) {
        std::stringstream position;
        position << "lights[" << i << "].Position";
        objectProg.setUniform(position.str().c_str(), view * lightPositions[i]);
    }*/

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

    //objectProg.setUniform("lights[2].Intensity", brightness);
    //objectProg.setUniform("lights[3].Intensity", brightness);
}

void SceneBasic_Uniform::render()
{
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //
    //Main
    //

    pass1();
    glFlush();
    pass2();
    glFlush();
    pass3();

    //
    //Skybox
    //
    /*skyboxProg.use();
    model = mat4(1.0f);
    model = glm::translate(model, cameraPosition);
    setMatrices(skyboxProg);
    sky.render();*/
}

void SceneBasic_Uniform::drawScene(GLSLProgram& prog, bool onlyShadowCasters)
{
    vec3 color;
    if (!onlyShadowCasters) {
        color = vec3(1.0f);
        prog.setUniform("Ka", color * 0.1f);
        prog.setUniform("Kd", color);
        prog.setUniform("Ks", vec3(0.9f));
        prog.setUniform("Shininess", 150.0f);
    }

    //
    //Floor
    //
    if (!onlyShadowCasters) {
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, floorTexture);
    }

    model = mat4(1.0f);
    model = glm::translate(model, vec3(0.0f, -2.0f, 0.0f));
    setMatrices(prog);
    floor->render();

    //
    //Window Wall
    //
    if (!onlyShadowCasters) {
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, wallTexture);
    }

    model = mat4(1.0f);
    model = glm::translate(model, vec3(-2.2f, 0.0f, 0.0f));
    setMatrices(prog);
    windowWall->render();

    //
    //Wall
    //
    if (!onlyShadowCasters) {
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, wallTexture);
    }

    model = mat4(1.0f);
    model = glm::translate(model, vec3(2.0f, 0.0f, 0.0f));
    setMatrices(prog);
    wall->render();

    //
    //Ceiling
    //
    if (!onlyShadowCasters) {
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, ceilingTexture);
    }

    model = mat4(1.0f);
    model = glm::translate(model, vec3(0.0f, 2.0f, 0.0f));
    setMatrices(prog);
    ceiling->render();

    //
    //Doorframes
    //
    if (!onlyShadowCasters) {
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, doorframeTexture);
    }

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
    if (!onlyShadowCasters) {
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, blastdoorTexture);
    }

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
    if (!onlyShadowCasters) {
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, spaceshipTexture);
    }

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

        if (!onlyShadowCasters) {
            glActiveTexture(GL_TEXTURE2);
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
    prog.setUniform("ProjMatrix", projection);
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

void SceneBasic_Uniform::setupFBO() 
{
    GLuint depthBuf;
    glGenRenderbuffers(1, &depthBuf);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuf);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    GLuint ambBuf;
    glGenRenderbuffers(1, &ambBuf);
    glBindRenderbuffer(GL_RENDERBUFFER, ambBuf);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, width, height);

    glActiveTexture(GL_TEXTURE0);
    GLuint diffSpecTex;
    glGenTextures(1, &diffSpecTex);
    glBindTexture(GL_TEXTURE_2D, diffSpecTex);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glGenFramebuffers(1, &colorDepthFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, colorDepthFBO);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuf);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, ambBuf);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, diffSpecTex, 0);

    GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, drawBuffers);
    GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (result == GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Framebuffer is complete" << endl;
    }
    else {
        std::cout << "Framebuffer error:" << result << endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SceneBasic_Uniform::pass1() 
{
    glDepthMask(GL_TRUE);
    glDisable(GL_STENCIL_TEST);
    glEnable(GL_DEPTH_TEST);

    projection = glm::infinitePerspective(glm::radians(70.0f), (float)width / height, 0.5f);
    renderProg.use();
    renderProg.setUniform("LightPosition", view * lightPos);
    glBindFramebuffer(GL_FRAMEBUFFER, colorDepthFBO);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    drawScene(renderProg, false);
}

void SceneBasic_Uniform::pass2()
{
    volumeProg.use();
    volumeProg.setUniform("LightPosition", view * lightPos);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, colorDepthFBO);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, width - 1, height - 1, 0, 0, width - 1, height - 1, GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT, GL_NEAREST);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glDepthMask(GL_FALSE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_STENCIL_BUFFER_BIT);
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, 0, 0xffff);
    glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_KEEP, GL_INCR_WRAP);
    glStencilOpSeparate(GL_BACK, GL_KEEP, GL_KEEP, GL_DECR_WRAP);
    drawScene(volumeProg, true);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
}

void SceneBasic_Uniform::pass3()
{
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glStencilFunc(GL_EQUAL, 0, 0xffff);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    compProg.use();
    model = mat4(1.0f);
    projection = model;
    view = model;
    setMatrices(compProg);
    glBindVertexArray(fsQuad);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}