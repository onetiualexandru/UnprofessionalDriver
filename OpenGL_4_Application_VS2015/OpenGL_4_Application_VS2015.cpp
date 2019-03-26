//
//  main.cpp
//  OpenGL Advances Lighting
//
//  Created by CGIS on 28/11/16.
//  Copyright � 2016 CGIS. All rights reserved.
//

#define GLEW_STATIC

#include <iostream>
#include "glm/glm.hpp"//core glm functionality
#include "glm/gtc/matrix_transform.hpp"//glm extension for generating common transformation matrices
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "GLEW/glew.h"
#include "GLFW/glfw3.h"
#include <string>
#include <stdio.h>
#include "Shader.hpp"
#include "Camera.hpp"
#include "SkyBox.hpp"
#define TINYOBJLOADER_IMPLEMENTATION
#define SHADOW_WIDTH 2048
#define SHADOW_HEIGHT 2048
#include "Model3D.hpp"
#include "Mesh.hpp"

float move = 10.0f;
float move1 = 10.0f;
float move3 = -0.8f;
int ok1 = 0;
int ok2 = 0;
int coliziune = 0;
float i = 0;
int glWindowWidth = 640;
int glWindowHeight = 480;
double lastX = glWindowWidth / 2;
double lastY = glWindowHeight / 2;
int retina_width, retina_height;
bool firstMouse = true;
float	yaw = -90.0f;
float pitch = 0.0f;
GLFWwindow* glWindow = NULL;
float lightAngle;
glm::mat4 model;
GLuint modelLoc;
glm::mat4 view;
GLuint viewLoc;
glm::mat4 projection;
GLuint projectionLoc;
glm::mat3 normalMatrix;
GLuint normalMatrixLoc;
glm::mat4 viewLight;
GLuint shadowMapFBO;
GLuint depthMapTexture;
const GLfloat near_plane = 1.0f, far_plane = 30.0f;
glm::vec3 lightDir;
GLuint lightDirLoc;
glm::vec3 lightColor;
GLuint lightColorLoc;
glm::mat4 lightProjection;
gps::Camera myCamera(glm::vec3(0.0f, 1.0f, 2.5f), glm::vec3(0.0f, 0.0f, 0.0f));
float cameraSpeed = 0.03f;
glm::mat4 lightSpaceTrMatrix;
bool pressedKeys[1024];
float angle = 0.0f;
bool ceata = false;
float axaZ = -2.0f;
float axaX = 0.0f;

GLuint lightDirMatrixLoc;
GLuint textureID;
glm::mat3 lightDirMatrix;
gps::Model3D myModel;
gps::Model3D myGround;
gps::Model3D myLamp;
gps::Model3D myCat;
gps::Model3D myBallon;
gps::Model3D myIsland;
gps::Model3D myBridge;
gps::Model3D myRiver;
gps::Model3D myGate;
gps::SkyBox mySkyBox;
gps::Shader myCustomShader;
gps::Shader depthMapShader;
gps::Shader skyBoxShader;
std::vector<const GLchar*> faces;
GLenum glCheckError_(const char *file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
		case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height)
{
	fprintf(stdout, "window resized to width: %d , and height: %d\n", width, height);
	//TODO
	//for RETINA display
	glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

	//set projection matrix
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	//send matrix data to shader
	GLint projLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	//set Viewport transform
	glViewport(0, 0, retina_width, retina_height);
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			pressedKeys[key] = true;
		else if (action == GLFW_RELEASE)
			pressedKeys[key] = false;
	}
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	if (state != GLFW_PRESS)
	{
		firstMouse = true;
		return;
	}
	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	float xOffset = xpos - lastX;
	float yOffset = ypos - lastY;
	lastX = xpos;
	lastY = ypos;
	yaw += xOffset;
	pitch -= yOffset;
	myCamera.rotate(pitch, yaw);
}

void processMovement()
{

	if (pressedKeys[GLFW_KEY_Q]) {
		angle += 0.1f;
		if (angle > 360.0f)
			angle -= 360.0f;
	}

	if (pressedKeys[GLFW_KEY_E]) {
		angle -= 0.1f;
		if (angle < 0.0f)
			angle += 360.0f;
	}

	if (pressedKeys[GLFW_KEY_W]) {
		myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_S]) {
		myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_A]) {
		myCamera.move(gps::MOVE_LEFT, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_D]) {
		myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_UP]) {
		axaZ -= 0.03f;
		myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_DOWN]) {
		axaZ += 0.03f;
		myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_LEFT]) {
		axaX -= 0.03f;
		myCamera.move(gps::MOVE_LEFT, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_RIGHT]) {
		axaX += 0.03f;
		myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_J]) {
		lightAngle -= 0.5f;
		if (lightAngle < 0.0f)
			lightAngle = 360.0f;
		glm::vec3 lightDirTr = glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f))*glm::vec4(lightDir, 1.0f));
		myCustomShader.useShaderProgram();
		glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDirTr));
	}
	if (pressedKeys[GLFW_KEY_K]) {
		lightAngle += 0.5f;
		if (lightAngle > 360.f)
			lightAngle = 0.0f;
		glm::vec3 lightDirTr = glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f))*glm::vec4(lightDir, 1.0f));
		myCustomShader.useShaderProgram();
		glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDirTr));
	}

	if (pressedKeys[GLFW_KEY_C]) {
		myCustomShader.useShaderProgram();
		glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "ceata"), 1);
	}
	if (pressedKeys[GLFW_KEY_V]) {
		myCustomShader.useShaderProgram();
		glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "ceata"), 0);
	}

	if (pressedKeys[GLFW_KEY_M]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //wireframe
	}
	if (pressedKeys[GLFW_KEY_N]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); //flat
	}
	if (pressedKeys[GLFW_KEY_B]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	}

	if (axaZ <= -16.3f && axaZ >= -24.0f && axaX <= move + 1.0f && axaX >= move - 1.0f)
		coliziune = 1;
	/*else
		coliziune = 0;*/

}

bool initOpenGLWindow()
{
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return false;
	}

	//for Mac OS X
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glWindow = glfwCreateWindow(glWindowWidth, glWindowHeight, "OpenGL Shader Example", NULL, NULL);
	if (!glWindow) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return false;
	}

	glfwSetWindowSizeCallback(glWindow, windowResizeCallback);
	glfwMakeContextCurrent(glWindow);

	glfwWindowHint(GLFW_SAMPLES, 4);

	// start GLEW extension handler
	glewExperimental = GL_TRUE;
	glewInit();

	// get version info
	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	//for RETINA display
	glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

	glfwSetKeyCallback(glWindow, keyboardCallback);
	glfwSetCursorPosCallback(glWindow, mouseCallback);
	//glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	return true;
}

void initOpenGLState()
{
	glClearColor(0.3, 0.3, 0.3, 1.0);
	glViewport(0, 0, retina_width, retina_height);

	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise
}

void initModels()
{
	myModel = gps::Model3D("objects/car/10604_slot_car_red_SG_v1_iterations-2.obj", "objects/car/");
	myGround = gps::Model3D("objects/env/Street environment_V01.obj", "objects/env/");
	myLamp = gps::Model3D("objects/streetlamp/streetlamp.obj", "objects/streetlamp/");
	myCat = gps::Model3D("objects/cat/cat.obj", "objects/cat/");
	myBallon = gps::Model3D("objects/mil/mil.obj", "objects/mil/");
	myIsland = gps::Model3D("objects/castle/castle.obj", "objects/castle/");
	myBridge = gps::Model3D("objects/ponte/ponte.obj", "objects/ponte/");
	myRiver = gps::Model3D("objects/pamant/pamant.obj", "objects/pamant/");
	myGate = gps::Model3D("objects/gate/BrandenburgGate.obj", "objects/gate/");


}

void initShaders()
{
	myCustomShader.loadShader("shaders/shaderStart.vert", "shaders/shaderStart.frag");
	myCustomShader.useShaderProgram();
	depthMapShader.loadShader("shaders/shaderShadow.vert", "shaders/shaderShadow.frag");
	depthMapShader.useShaderProgram();
	skyBoxShader.loadShader("shaders/skyBoxShader.vert", "shaders/skyBoxShader.frag");
	skyBoxShader.useShaderProgram();

}

void initSkyBox() {
	faces.push_back("skybox/right.tga");
	faces.push_back("skybox/left.tga");
	faces.push_back("skybox/top.tga");
	faces.push_back("skybox/bottom.tga");
	faces.push_back("skybox/back.tga");
	faces.push_back("skybox/front.tga");
	mySkyBox.Load(faces);
}
void initUniforms()
{
	myCustomShader.useShaderProgram();
	model = glm::mat4(1.0f);
	modelLoc = glGetUniformLocation(myCustomShader.shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	view = myCamera.getViewMatrix();
	viewLoc = glGetUniformLocation(myCustomShader.shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	normalMatrixLoc = glGetUniformLocation(myCustomShader.shaderProgram, "normalMatrix");
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	projectionLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	//set the light direction (direction towards the light)
	lightDir = glm::vec3(0.0f, 1.0f, 2.0f);
	lightDirLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDir");
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));

	//set light color
	lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
	lightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor");
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

}

void initFBO() {
	glGenFramebuffers(1, &shadowMapFBO);
	glGenTextures(1, &depthMapTexture);
	glBindTexture(GL_TEXTURE_2D, depthMapTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}
glm::mat4 computeLightSpaceMatrix() {
	const GLfloat near_plane = 1.0f, far_plane = 10.0f;
	glm::mat4 lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, near_plane, far_plane);
	glm::vec3 lightDirTr = glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(lightDir, 1.0f));
	glm::mat4 lightView = glm::lookAt(lightDirTr, myCamera.getCameraTarget(), glm::vec3(0.0f, 1.0f, 0.0f));

	return lightProjection * lightView;
}

void renderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	processMovement();
	//render the scene to the depth buffer (first pass)

	depthMapShader.useShaderProgram();
	viewLight = glm::lookAt(lightDir, myCamera.getCameraTarget(), glm::vec3(0.0f, 1.0f, 0.0f));
	lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, near_plane, far_plane);
	lightSpaceTrMatrix = lightProjection * viewLight;


	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix"), 1, GL_FALSE, glm::value_ptr(computeLightSpaceMatrix()));

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);

	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	//initialize the view matrix
	view = myCamera.getViewMatrix();
	//send view matrix data to shader	


	//initialize the model matrix
	model = glm::mat4(1.0f);
	//model 
	model = glm::translate(model, glm::vec3(axaX, -0.90f, axaZ));
	model = glm::rotate(model, glm::radians(270.0f), glm::vec3(1, 0, 0));
	model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 0, 1));
	//model = glm::scale(model, glm::vec3(0.025, 0.025, 0.025));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	myModel.Draw(depthMapShader);


	model = glm::translate(glm::mat4(1.0f), glm::vec3(-3.0f, -0.6f, 45.0f));
	model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 1, 0));
	model = glm::scale(model, glm::vec3(8.0, 13.0, 8.0));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	myGate.Draw(depthMapShader);

	//baloon
	model = glm::translate(glm::mat4(1.0f), glm::vec3(30.0f, move3, 0.0f));
	model = glm::rotate(model, glm::radians(270.0f), glm::vec3(1, 0, 0));
	model = glm::scale(model, glm::vec3(0.005, 0.005, 0.005));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	myBallon.Draw(depthMapShader);

	//cat
	model = glm::rotate(glm::mat4(1.0f), glm::radians(270.0f), glm::vec3(1, 0, 0));
	model = glm::translate(model, glm::vec3(move, 20.0f, -0.90f));
	model = glm::scale(model, glm::vec3(0.03, 0.03, 0.03));

	if (ok2 == 0) {
		if (move1 >= -14.0f)
			move1 -= 0.006f;
		else
			ok2 = 1;
	}
	else
	{
		if (move1 <= 10.0f)
			move1 += 0.006f;
		else
			ok2 = 0;
	}
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	myCat.Draw(depthMapShader);


	model = glm::translate(glm::mat4(1.0f), glm::vec3(0, -1, 0));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	myGround.Draw(depthMapShader);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//send model matrix data to shader	
	
	//lumina punctiforma
	glViewport(0, 0, retina_width, retina_height);
	glm::vec3 positionLumina = glm::vec3(11.20f, 0.0f, 11.20f);
	glm::vec3 colorLumina = glm::vec3(1.0f, 0.0f, 0.0f);
	myCustomShader.useShaderProgram();
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "lumina.position"), positionLumina.x, positionLumina.y, positionLumina.z);
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "lumina.ambient"), colorLumina.x*0.1f, colorLumina.y*0.1f, colorLumina.z*0.1f);
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "lumina.diffuse"), colorLumina.x, colorLumina.y, colorLumina.z);
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "lumina.specular"), 1.0f, 1.0f, 1.0f);
	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "lumina.constant"), 1.0f);
	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "lumina.linear"), 0.09f);
	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "lumina.quadratic"), 0.032f);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, depthMapTexture);
	glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "shadowMap"), 3);
	glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "lightSpaceTrMatrix"), 1, GL_FALSE, glm::value_ptr(computeLightSpaceMatrix()));
	model = glm::mat4(1.0f);
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	//initialize the view matrix
	view = myCamera.getViewMatrix();
	//send view matrix data to shader	

	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	//create normal matrix
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	lightDirMatrix = glm::mat3(glm::inverseTranspose(view));
	glUniformMatrix3fv(glGetUniformLocation(myCustomShader.shaderProgram, "lightDirMatrix"), 1, GL_FALSE, glm::value_ptr(lightDirMatrix));
	

	//gate
	model = glm::translate(glm::mat4(1.0f), glm::vec3(-3.0f, -0.9f, 45.0f));
	model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 1, 0));
	model = glm::scale(model, glm::vec3(8.0, 13.0, 8.0));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	myGate.Draw(myCustomShader);

	
	//pod
	model = glm::translate(glm::mat4(1.0f), glm::vec3(-60.0f, -3.5f, 0.0f));
	//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 0, 0));
	//model = glm::scale(model, glm::vec3(0.05, 0.05, 0.05));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	myBridge.Draw(myCustomShader);

	//castel
	model = glm::translate(glm::mat4(1.0f), glm::vec3(-80.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
	model = glm::scale(model, glm::vec3(0.05, 0.05, 0.05));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	myIsland.Draw(myCustomShader);


	//apa
	model = glm::translate(glm::mat4(1.0f), glm::vec3(-60.0f, -0.9f, 0.0f));
	//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 0, 0));
	model = glm::scale(model, glm::vec3(1.18, 1.6, 3.1));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	myRiver.Draw(myCustomShader);

	//balon
	model = glm::translate(glm::mat4(1.0f), glm::vec3(30.0f, move3, 0.0f));
	model = glm::rotate(model, glm::radians(270.0f), glm::vec3(1, 0, 0));
	model = glm::scale(model, glm::vec3(0.005, 0.005, 0.005));

	if (ok1 == 0) {
		if (move3 >= -0.9f)
			move3 += 0.005f;
		else
			ok1 = 1;
	}
	else
	{
		if (move3 <= 20.0f)
			move3 -= 0.005f;
		else
			ok1 = 0;
	}

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	myBallon.Draw(myCustomShader);


	//model = glm::translate(model, glm::vec3(axaZ, -1.10f, axaX));
	model = glm::translate(glm::mat4(1.0f), glm::vec3(axaX, -0.90f, axaZ));
	model = glm::rotate(model, glm::radians(270.0f), glm::vec3(1, 0, 0));
	model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 0, 1));
	//model = glm::scale(model, glm::vec3(0.025, 0.025, 0.025));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	myModel.Draw(myCustomShader);



	//ground
	model = glm::translate(glm::mat4(1.0f), glm::vec3(0, -1, 0));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	myGround.Draw(myCustomShader);


	//trans lampa
	model = glm::translate(glm::mat4(1.0f), glm::vec3(11.5f, -1.0f, 11.35f));
	model = glm::scale(model, glm::vec3(0.25, 0.25, 0.25));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	myLamp.Draw(myCustomShader);

	//cat
	model = glm::rotate(glm::mat4(1.0f), glm::radians(270.0f), glm::vec3(1, 0, 0));
	model = glm::translate(model, glm::vec3(move, 20.0f, -0.90f));
	model = glm::scale(model, glm::vec3(0.03, 0.03, 0.03));

	if (ok1 == 0) {
		if (move >= -14.0f) 
			move -= 0.006f;
		else
			ok1 = 1;
	}
	else
	{
		if (move <= 10.0f)
			move += 0.006f;
		else
			ok1 = 0;
	}
		
	if (coliziune == 1) {
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, i += 1.2f));
		model = glm::rotate(model, glm::radians(i-0.6f), glm::vec3(1, 0, 1));
		coliziune = 2;
	}
	else
		if (coliziune == 2)
		{
			if (i >= 0.0f)
			{
				model = glm::translate(model, glm::vec3(0.0f, 0.0f, i -= 1.0f));
				model = glm::rotate(model, glm::radians(i-0.4f), glm::vec3(1, 1, 0));
			}
		}


	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	myCat.Draw(myCustomShader);


	skyBoxShader.useShaderProgram();
	view = myCamera.getViewMatrix();
	glUniformMatrix4fv(glGetUniformLocation(skyBoxShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
	projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	glUniformMatrix4fv(glGetUniformLocation(skyBoxShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	mySkyBox.Draw(skyBoxShader, view, projection);
}

int main(int argc, const char * argv[]) {

	initOpenGLWindow();
	initOpenGLState();
	initFBO();
	initModels();
	initSkyBox();
	initShaders();
	initUniforms();

	while (!glfwWindowShouldClose(glWindow)) {


		renderScene();

		glfwPollEvents();
		glfwSwapBuffers(glWindow);
	}

	//close GL context and any other GLFW resources
	glfwTerminate();

	return 0;
}
