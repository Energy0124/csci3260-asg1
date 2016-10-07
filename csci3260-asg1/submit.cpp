/*********************************************************
FILE : submit.cpp (csci3260 2016-2017 Assignment 1)
*********************************************************/
/*********************************************************
Student Information
Student ID: 1155062557
Student Name: Ling Leong
*********************************************************/


#include "Dependencies\glew\glew.h"
#include "Dependencies\freeglut\freeglut.h"
#include "Dependencies\glm\glm.hpp"
#include "Dependencies\glm\gtc\matrix_transform.hpp"
#include "Dependencies\glm\gtc\type_ptr.hpp"
#include "Dependencies\freeglut\freeglut_ext.h"
#include <iostream>
#include <fstream>
using namespace std;

// Window dimensions
 GLuint WIDTH = 800, HEIGHT = 600;

GLint programID;
glm::vec3 cubePositions[10];
GLuint VBO, VAO, EBO;
GLint xAxis = 0, yAxis = 0, zAxis = 0;
GLfloat deltaX = 0.05f, deltaY = 0.05f, deltaZ = 0.05f;
GLfloat scaleFactor = 1, deltaScale = 1.05f;
GLint rotation = 0;
GLfloat deltaAngle = 0.05f;

//GLfloat radius = 10.0f;
//GLfloat camX = 0;
//GLfloat camZ = 0;
// Camera
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

GLfloat deltaCameraTime = 0.0f;	// Time between current frame and last frame
GLfloat lastCameraFrame = 0.0f;  	// Time of last frame

GLfloat sensitivity = 0.1f;
GLfloat lastX = WIDTH / 2.0f;
GLfloat lastY = HEIGHT / 2.0f;
bool firstMouse = true;

GLfloat yaw = -90.0f;	// Yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right (due to how Eular angles work) so we initially rotate a bit to the left.
GLfloat pitch = 0.0f;

GLfloat fov = 45.0f;

bool keys[1024];

GLuint VBO_Tetrahedron, VAO_Tetrahedron, EBO_Tetrahedron;
GLuint VBO_Ground, VAO_Ground, EBO_Ground;

GLboolean wireframeMode = false;

//time for calculating FPS
int lastTime = 0;
int deltaTime = 0;
int deltaFrame = 0;




bool checkStatus(
	GLuint objectID,
	PFNGLGETSHADERIVPROC objectPropertyGetterFunc,
	PFNGLGETSHADERINFOLOGPROC getInfoLogFunc,
	GLenum statusType)
{
	GLint status;
	objectPropertyGetterFunc(objectID, statusType, &status);
	if (status != GL_TRUE)
	{
		GLint infoLogLength;
		objectPropertyGetterFunc(objectID, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar* buffer = new GLchar[infoLogLength];

		GLsizei bufferSize;
		getInfoLogFunc(objectID, infoLogLength, &bufferSize, buffer);
		cout << buffer << endl;

		delete[] buffer;
		return false;
	}
	return true;
}

bool checkShaderStatus(GLuint shaderID)
{
	return checkStatus(shaderID, glGetShaderiv, glGetShaderInfoLog, GL_COMPILE_STATUS);
}

bool checkProgramStatus(GLuint programID)
{
	return checkStatus(programID, glGetProgramiv, glGetProgramInfoLog, GL_LINK_STATUS);
}

string readShaderCode(const char* fileName)
{
	ifstream meInput(fileName);
	if (!meInput.good())
	{
		cout << "File failed to load..." << fileName;
		exit(1);
	}
	return std::string(
		std::istreambuf_iterator<char>(meInput),
		std::istreambuf_iterator<char>()
	);
}

void installShaders()
{
	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	const GLchar* adapter[1];
	//adapter[0] = vertexShaderCode;
	string temp = readShaderCode("VertexShader.glsl");
	adapter[0] = temp.c_str();
	glShaderSource(vertexShaderID, 1, adapter, 0);
	//adapter[0] = fragmentShaderCode;
	temp = readShaderCode("FragmentShader.glsl");
	adapter[0] = temp.c_str();
	glShaderSource(fragmentShaderID, 1, adapter, 0);

	glCompileShader(vertexShaderID);
	glCompileShader(fragmentShaderID);

	if (!checkShaderStatus(vertexShaderID) || !checkShaderStatus(fragmentShaderID))
		return;

	programID = glCreateProgram();
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);
	glLinkProgram(programID);

	if (!checkProgramStatus(programID))
		return;

	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);

	glUseProgram(programID);
}

void keyboard(unsigned char key, int x, int y)
{
	if (key == 'a')
	{
		xAxis -= 1;
	}
	if (key == 'd')
	{
		xAxis += 1;
	}
	if (key == 'w')
	{
		zAxis -= 1;
	}
	if (key == 's')
	{
		zAxis += 1;
	}
	if (key == 'q')
	{
		yAxis -= 1;
	}
	if (key == 'e')
	{
		yAxis += 1;
	}
	if (key == 'r')
	{
		scaleFactor *= deltaScale;
	}
	if (key == 'f')
	{
		scaleFactor /= deltaScale;
	}
	if (key == 't')
	{
		rotation += 1;
	}
	if (key == 'g')
	{
		rotation -= 1;
	}
	if (key == 'm')
	{
		wireframeMode = !wireframeMode;
	}
	if (key == 'c')
	{
		//		static int centerX = glutGet(GLUT_WINDOW_WIDTH) / 2;
		//		static int centerY = glutGet(GLUT_WINDOW_HEIGHT) / 2;
		//		lastX = centerX;
		//		lastY = centerY;
		yaw = -90.0f;
		pitch = 0.0f;
		cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
		firstMouse = true;

	}
	if (key == 'z')
	{

		sensitivity -= 0.02f;
	}
	if (key == 'x')
	{
		sensitivity += 0.02f;

	}


}

void handleSpecialKeypress(int key, int x, int y) {
	if (key >= 0 && key < 1024)
	{
		keys[key] = true;
	}


}

void handleSpecialKeyReleased(int key, int x, int y) {
	if (key >= 0 && key < 1024)
	{
		keys[key] = false;
	}
}

void bindTenColorCube()
{
	// Set up vertex data (and buffer(s)) and attribute pointers
	GLfloat vertices[] = {
		-0.5f, -0.5f, -0.5f,	0.0f, 0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,		1.0f, 0.0f, 0.0f,
		0.5f,  0.5f, -0.5f,		0.0f, 1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,		0.0f, 1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,	1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,	0.0f, 0.0f, 1.0f,

		-0.5f, -0.5f,  0.5f,	1.0f, 0.0f, 0.0f,
		0.5f, -0.5f,  0.5f,		0.0f, 1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,		0.0f, 0.0f, 1.0f,
		0.5f,  0.5f,  0.5f,		0.0f, 0.0f, 1.0f,
		-0.5f,  0.5f,  0.5f, 	0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f, 	1.0f, 0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,	0.0f, 1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,	1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,	0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,	0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,	1.0f, 0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,	0.0f, 1.0f, 0.0f,

		0.5f,  0.5f,  0.5f,		0.0f, 0.0f, 1.0f,
		0.5f,  0.5f, -0.5f,		0.0f, 1.0f, 0.0f,
		0.5f, -0.5f, -0.5f,		1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,		1.0f, 0.0f, 0.0f,
		0.5f, -0.5f,  0.5f,		0.0f, 1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,		0.0f, 0.0f, 1.0f,

		-0.5f, -0.5f, -0.5f,	0.0f, 0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,		1.0f, 0.0f, 0.0f,
		0.5f, -0.5f,  0.5f,		0.0f, 1.0f, 0.0f,
		0.5f, -0.5f,  0.5f,		0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,	1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,	0.0f, 0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,	1.0f, 0.0f, 0.0f,
		0.5f,  0.5f, -0.5f,		0.0f, 1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,		0.0f, 0.0f, 1.0f,
		0.5f,  0.5f,  0.5f,		0.0f, 0.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,	0.0f, 1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,	1.0f, 0.0f, 0.0f,
	};


	// World space positions of our cubes
	glm::vec3 tempCubePositions[10] = {
		glm::vec3(0.0f,  0.0f,  -5.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};

	int i = 0;
	for (glm::vec3 v : tempCubePositions)
	{
		cubePositions[i] = v;
		i++;
	}

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// Color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0); // Unbind VAO
}

void bindTetrahedron()
{
	GLfloat vertices_Tetrahedron[] = {
		 0.5,  0.5,  0.5 , /* index 0 */     1.0, 0.0, 0.0	 , /* red */
		 -0.5, -0.5,  0.5,  /* index 1 */	0.0,  1.0,  0.0	 , /* green */
		 -0.5,  0.5, -0.5,  /* index 2 */	0.0,  0.0,  1.0	 , /* blue */
		 0.5, -0.5, -0.5 ,					1.0,  1.0,  1.0	   /* white */
	};

	/*Explaination:
	 *As you can see tetrahedron[][] defines the four vertices needed to create a tetrahedron.
	 *The tetraindices const defines how to connect these vertices together.
	 *Since we are using a triangle strip, we only need 6 indices to create 4 triangles.
	 *Triangle strips use 3 indices to connect the first triangle
	 *and create another triangle for each additional index defined using the last two indices used.
	 *For instance this will create triangles using indices 0,1,2 then 1,2,3 then 2,3,0 and finally 3,0,1.
	 *
	 */
	GLuint tetraindices[] = { 0, 1, 2, 3, 0, 1 };


	glGenVertexArrays(1, &VAO_Tetrahedron);
	glGenBuffers(1, &VBO_Tetrahedron);
	glGenBuffers(1, &EBO_Tetrahedron);
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO_Tetrahedron);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_Tetrahedron);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_Tetrahedron), vertices_Tetrahedron, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_Tetrahedron);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(tetraindices), tetraindices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// Color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0); // Unbind VAO*/

};

void bindGround()
{
	GLfloat vertices_Ground[] = {
		1.0,  0.0,  1.0 , /* index 0 */     1.0,  0.0,  0.0	 , /* red */
		1.0,  0.0,  -1.0,  /* index 1 */	0.0,  1.0,  0.0	 , /* green */
		-1.0, 0.0,  1.0,  /* index 2 */		0.0,  0.0,  1.0	 , /* blue */
		-1.0, 0.0,  -1.0 ,					1.0,  1.0,  1.0	   /* white */
	};

	/*Explaination:
	*As you can see tetrahedron[][] defines the four vertices needed to create a tetrahedron.
	*The tetraindices const defines how to connect these vertices together.
	*Since we are using a triangle strip, we only need 6 indices to create 4 triangles.
	*Triangle strips use 3 indices to connect the first triangle
	*and create another triangle for each additional index defined using the last two indices used.
	*For instance this will create triangles using indices 0,1,2 then 1,2,3 then 2,3,0 and finally 3,0,1.
	*
	*/
	GLuint ground_indices[] = { 0, 1, 2, 3 };


	glGenVertexArrays(1, &VAO_Ground);
	glGenBuffers(1, &VBO_Ground);
	glGenBuffers(1, &EBO_Ground);
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO_Ground);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_Ground);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_Ground), vertices_Ground, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_Ground);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(ground_indices), ground_indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// Color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0); // Unbind VAO*/
};

void sendDataToOpenGL()
{
	//TODO:
	//create solid objects here and bind to VAO & VBO

	/*// Set up vertex data (and buffer(s)) and attribute pointers
	GLfloat vertices[] = {
		// Positions         // Colors
		0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  // Bottom Right
		-0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  // Bottom Left
		0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f   // Top
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// Color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0); // Unbind VAO*/

	bindTenColorCube();
	bindTetrahedron();
	bindGround();


}


void calculateFPS()
{
	int currentTime = glutGet(GLUT_ELAPSED_TIME);
	if (lastTime == 0) lastTime = currentTime;
	else if (deltaTime >= 1000)
	{
		printf("FPS: %d\n", deltaFrame);

		deltaFrame = 1;

		deltaTime = currentTime - lastTime;
		lastTime = currentTime;
	}
	else
	{
		deltaFrame++;
		deltaTime += currentTime - lastTime;
		lastTime = currentTime;
	}

};

void drawTenCubes()
{

	// Create transformations
	//glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;

	//model = glm::rotate(model, (GLfloat)glutGet(GLUT_ELAPSED_TIME) * 0.001f, glm::vec3(0.5f, 1.0f, 0.0f));
	//view = glm::translate(view, glm::vec3(-2.0f, 0.0f, -5.0f));
	//view = glm::lookAt(glm::vec3(camX, 0.0f, camZ), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	// Note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
	projection = glm::perspective(fov, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
	// Get their uniform location
	GLint modelLoc = glGetUniformLocation(programID, "model");
	GLint viewLoc = glGetUniformLocation(programID, "view");
	GLint projLoc = glGetUniformLocation(programID, "projection");
	// Pass them to the shaders

	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
	//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	// Draw container
	glBindVertexArray(VAO);
	for (GLuint i = 0; i < 10; i++)
	{
		// Calculate the model matrix for each object and pass it to shader before drawing
		glm::mat4 model;
		model = glm::translate(model, cubePositions[i]);
		//model = glm::translate(model, glm::vec3(deltaX * xAxis, deltaY * yAxis, deltaZ * zAxis));
		GLfloat angle = 1.0f * i + 1.0f;
		model = glm::rotate(model, (GLfloat)glutGet(GLUT_ELAPSED_TIME) * 0.0001f*angle, glm::vec3(1.0f, 0.3f, 0.5f));
		//model = glm::scale(model, scaleFactor*glm::vec3(1.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
	//glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}



void drawTetrahedron()
{
	// Create transformations
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;

	//model = glm::rotate(model, (GLfloat)glutGet(GLUT_ELAPSED_TIME) * 0.001f, glm::vec3(0.5f, 1.0f, 0.0f));
	//view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
	//view = glm::lookAt(glm::vec3(camX, 0.0f, camZ), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	// Note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
	projection = glm::perspective(fov, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);



	model = glm::translate(model, glm::vec3(deltaX * xAxis, deltaY * yAxis, deltaZ * zAxis));
	model = glm::rotate(model, deltaAngle*rotation - 2.3f, glm::vec3(0.5f, 1.0f, 0.0f));
	model = glm::scale(model, scaleFactor*glm::vec3(1.0f));


	// Get their uniform location
	GLint modelLoc = glGetUniformLocation(programID, "model");
	GLint viewLoc = glGetUniformLocation(programID, "view");
	GLint projLoc = glGetUniformLocation(programID, "projection");

	// Pass them to the shaders

	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	// Draw container
	glBindVertexArray(VAO_Tetrahedron);
	glDrawElements(GL_TRIANGLE_STRIP, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}


void drawGround()
{
	// Create transformations
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;



	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, -0.5f, -2.5f));
	model = glm::scale(model, 3.0f*glm::vec3(1.0f));
	//view = glm::translate(view, glm::vec3(0.0f, -0.5f, -2.5f));
	//view = glm::translate(view, glm::vec3(0.0f, 0.0f, 0.0f));
	//view = glm::lookAt(glm::vec3(camX, 0.0f, camZ), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	// Note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
	projection = glm::perspective(fov, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
	// Get their uniform location
	GLint modelLoc = glGetUniformLocation(programID, "model");
	GLint viewLoc = glGetUniformLocation(programID, "view");
	GLint projLoc = glGetUniformLocation(programID, "projection");
	// Pass them to the shaders

	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glBindVertexArray(VAO_Ground);
	glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void updateCamera()
{
	//camX = sin((GLfloat)glutGet(GLUT_ELAPSED_TIME) * 0.0001f) * radius;
	//camZ = cos((GLfloat)glutGet(GLUT_ELAPSED_TIME) * 0.0001f) * radius;

	GLfloat currentFrame = (GLfloat) glutGet(GLUT_ELAPSED_TIME);
	deltaCameraTime = currentFrame - lastCameraFrame;
	lastCameraFrame = currentFrame;
	// Camera controls
	GLfloat cameraSpeed = 0.01f*deltaCameraTime;
	/*switch (key) {
	case GLUT_KEY_LEFT:

	break;
	case GLUT_KEY_RIGHT:

	break;
	case GLUT_KEY_UP:
	cameraPos += cameraSpeed * cameraFront;
	break;
	case GLUT_KEY_DOWN:
	break;
	}*/
	if (keys[GLUT_KEY_UP])
		cameraPos += cameraSpeed * cameraFront;
	if (keys[GLUT_KEY_DOWN])
		cameraPos -= cameraSpeed * cameraFront;
	if (keys[GLUT_KEY_LEFT])
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (keys[GLUT_KEY_RIGHT])
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

};


void handlePassiveMotion(int x, int y)
{
	if (firstMouse)
	{
		lastX = (GLfloat)x;
		lastY = (GLfloat)y;
		firstMouse = false;
	}

	GLfloat xoffset = x - lastX;
	GLfloat yoffset = lastY - y;
	lastX = (GLfloat)x;
	lastY = (GLfloat)y;


	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);


	//failed mouse capture, glut sucks
//	static int centerX = glutGet(GLUT_WINDOW_WIDTH) / 2;
//	static int centerY = glutGet(GLUT_WINDOW_HEIGHT) / 2;
//	if (x >= centerX+100 || y >= centerY+100|| x <= centerX - 100 || y <= centerY - 100) glutWarpPointer(centerX, centerY);


}
void handleMouseWheel(int wheel, int direction, int x, int y)
{
	if (fov >= 1.0f && fov <= 60.0f)
		fov -= direction*0.05f;
	//cout << direction<<" "<<fov <<endl;
	if (fov <= 1.0f)
		fov = 1.0f;
	if (fov >= 60.0f)
		fov = 60.0f;
}

void updateRatio()
{
	WIDTH = glutGet(GLUT_WINDOW_WIDTH) / 2;
	HEIGHT = glutGet(GLUT_WINDOW_HEIGHT) / 2;

};

void paintGL(void)
{
	//TODO:
	//render your objects and control the transformation here
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (wireframeMode)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	/*glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(0);*/

	/*	glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);*/

	updateRatio();

	updateCamera();

	drawTetrahedron();

	drawTenCubes();

	drawGround();


	glFlush();
	glutPostRedisplay();

	

	calculateFPS();
}

void initializedGL(void) //run only once
{
	glewInit();
	sendDataToOpenGL();
	installShaders();
}


int main(int argc, char *argv[])
{
	/*Initialization of GLUT library*/
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	//Initialize the windows size
	glutInitWindowSize(WIDTH, HEIGHT);
	/*Create a window with title specified*/
	//TODO:
	glutCreateWindow("Clannad is the best anime! ACGN for the win!");


	initializedGL();
	glViewport(0, 0, WIDTH, HEIGHT);

	//enable depth test
	glEnable(GL_DEPTH_TEST);

	/*Register different CALLBACK function for GLUT to response
	with different events, e.g. window sizing, mouse click or
	keyboard stroke */

	glutDisplayFunc(paintGL);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(handleSpecialKeypress);
	glutSpecialUpFunc(handleSpecialKeyReleased);
	glutPassiveMotionFunc(handlePassiveMotion);
	glutMouseWheelFunc(handleMouseWheel);


	/*Enter the GLUT event processing loop which never returns.
	it will call different registered CALLBACK according
	to different events. */
	glutMainLoop();

	return 0;
}