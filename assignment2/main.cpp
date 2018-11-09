/*********************************************************
FILE : main.cpp (csci3260 2018-2019 Assignment 2)
*********************************************************/
/*********************************************************
Student Information
Student ID: 1155123906
Student Name: Zuowen Wang
*********************************************************/


#define _CRT_SECURE_NO_DEPRECATE
#include "C:\Users\cprj2748\Downloads\Project2\Dependencies\glew\glew.h"
#include "C:\Users\cprj2748\Downloads\Project2\Dependencies\freeglut\freeglut.h"
#include "C:\Users\cprj2748\Downloads\Project2\Dependencies\glm\glm.hpp"
#include "C:\Users\cprj2748\Downloads\Project2\Dependencies\glm\gtc\matrix_transform.hpp"
#include "C:\Users\cprj2748\Downloads\Project2\Dependencies\glm\gtc\type_ptr.hpp"
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;
using glm::vec3;
using glm::mat4;
using namespace glm;


int WIDTH = 1920;
int HEIGHT = 1080;

GLint programID;
// Could define the Vao&Vbo and interaction parameter here
const int numObj = 3;
GLuint vao[numObj];

GLuint vbo[numObj];
GLuint uvbo[numObj];
GLuint nvbo[numObj];
GLuint texture[numObj];
GLuint light[numObj];
int drawSize[numObj];



glm::vec3 cameraPosition = glm::vec3(1.5f, 9.945f, 30.5f);
glm::vec3 cameraGaze = glm::vec3(0.0f, 0.01f, -1.0f);

float hor = 3.14f;
float ver = 0.0f;
float iniFov = 45.0f;



float scale1 = 1.0f;

float x_delta = 0.1f;
float i_press_num = 0;
float k_press_num = 0;
float j_press_num = 0;
float l_press_num = 0;
int angle_press_num = 0;

double lookX = 0;
double lookY = 0;
double lookZ = 0;
double rot = 0;

float scale = 4.5;

GLfloat pitch1 = 22.0f;
GLfloat yaw1 = -90.0f;







//a series utilities for setting shader parameters
void setMat4(const std::string &name, glm::mat4& value)
{
	unsigned int transformLoc = glGetUniformLocation(programID, name.c_str());
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(value));
}

void setVec4(const std::string &name, glm::vec4 value)
{
	glUniform4fv(glGetUniformLocation(programID, name.c_str()), 1, &value[0]);
}
void setVec3(const std::string &name, glm::vec3 value)
{
	glUniform3fv(glGetUniformLocation(programID, name.c_str()), 1, &value[0]);
}
void setFloat(const std::string &name, float value)
{
	glUniform1f(glGetUniformLocation(programID, name.c_str()), value);
}
void setInt(const std::string &name, int value)
{
	glUniform1i(glGetUniformLocation(programID, name.c_str()), value);
}

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
	string temp = readShaderCode("VertexShaderCode.glsl");
	adapter[0] = temp.c_str();
	glShaderSource(vertexShaderID, 1, adapter, 0);
	temp = readShaderCode("FragmentShaderCode.glsl");
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
	// //Define 6 camera position operations here:
	//X axel
	// w -> forwards
	// s -> backwards
	if (key == 'a')
	{
		cameraPosition -= 0.5f *glm::normalize(glm::cross(cameraGaze, glm::vec3(0, 1, 0)));
	}
	if (key == 'd')
	{
		cameraPosition += 0.5f *glm::normalize(glm::cross(cameraGaze, glm::vec3(0, 1, 0)));
	}

	//Y axel
	// a -> leftwards
	// d -> rightwards
	if (key == 's')
	{
		cameraPosition -= 0.5f * cameraGaze;
	}
	if (key == 'w')
	{
		cameraPosition += 0.5f *cameraGaze;
	}
	//Z axel
	// q -> upwards
	// e -> downwards
	if (key == 'e')
	{
		cameraPosition -= 0.5f *glm::vec3(0.0f, 0.0f, 1.0f);
	}
	if (key == 'q')
	{
		cameraPosition += 0.5f *glm::vec3(0.0f, 0.0f, 1.0f);
	}

	// // and 2 angle operations:
	// z-> camera angle up 
	// x -> camera angle down
	if (key == '+')
	{
		scale1 += 0.5;
	}
	if (key == '-')
	{
		scale1 -= 0.5;
	}

	if (key == 'r') {
		rot = rot + 3;
	}
	if (key == 'i') {
		i_press_num = i_press_num + 0.5;
	}
	if (key == 'k') {
		k_press_num = k_press_num - 0.5;
	}
	if (key == 'j') {
		j_press_num = j_press_num - 0.5;
	}
	if (key == 'l') {
		l_press_num = l_press_num + 0.5;
	}
}




void move(int key, int x, int y)
{
	//TODO: Use arrow keys to do interactive events and animation
}

void PassiveMouse(int x, int y)
{
	//TODO: Use Mouse to do interactive events and animation
	int mouseX;
	int mouseY;

	mouseX = x;
	mouseY = y;

	



}

bool loadOBJ(
	const char * path,
	std::vector<glm::vec3> & out_vertices,
	std::vector<glm::vec2> & out_uvs,
	std::vector<glm::vec3> & out_normals
) {

	printf("Loading OBJ file %s...\n", path);

	std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	std::vector<glm::vec3> temp_vertices;
	std::vector<glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;


	FILE * file = fopen(path, "r");
	if (file == NULL) {
		printf("Impossible to open the file ! Are you in the right path ? See Tutorial 6 for details\n");
		getchar();
		return false;
	}

	while (1) {

		char lineHeader[128];
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

				   // else : parse lineHeader

		if (strcmp(lineHeader, "v") == 0) {
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0) {
			glm::vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y);
			uv.y = -uv.y;
			temp_uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0) {
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			temp_normals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0) {
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
			if (matches != 9) {
				printf("File can't be read by our simple parser :-( Try exporting with other options\n");
				return false;
			}
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			uvIndices.push_back(uvIndex[0]);
			uvIndices.push_back(uvIndex[1]);
			uvIndices.push_back(uvIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
		}
		else {
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}

	}

	// For each vertex of each triangle
	//this is for data handling
	for (unsigned int i = 0; i<vertexIndices.size(); i++) {

		// Get the indices of its attributes
		unsigned int vertexIndex = vertexIndices[i];
		unsigned int uvIndex = uvIndices[i];
		unsigned int normalIndex = normalIndices[i];

		// Get the attributes thanks to the index
		glm::vec3 vertex = temp_vertices[vertexIndex - 1];
		glm::vec2 uv = temp_uvs[uvIndex - 1];
		glm::vec3 normal = temp_normals[normalIndex - 1];

		// Put the attributes in buffers
		out_vertices.push_back(vertex);
		out_uvs.push_back(uv);
		out_normals.push_back(normal);

	}

	return true;
}

GLuint loadBMP_custom(const char * imagepath) {

	printf("Reading image %s\n", imagepath);

	unsigned char header[54];
	unsigned int dataPos;
	unsigned int imageSize;
	unsigned int width, height;
	unsigned char * data;

	FILE * file = fopen(imagepath, "rb");
	if (!file) { printf("%s could not be opened. Are you in the right directory ? Don't forget to read the FAQ !\n", imagepath); getchar(); return 0; }

	if (fread(header, 1, 54, file) != 54) {
		printf("Not a correct BMP file\n");
		return 0;
	}
	if (header[0] != 'B' || header[1] != 'M') {
		printf("Not a correct BMP file\n");
		return 0;
	}
	if (*(int*)&(header[0x1E]) != 0) { printf("Not a correct BMP file\n");    return 0; }
	if (*(int*)&(header[0x1C]) != 24) { printf("Not a correct BMP file\n");    return 0; }

	dataPos = *(int*)&(header[0x0A]);
	imageSize = *(int*)&(header[0x22]);
	width = *(int*)&(header[0x12]);
	height = *(int*)&(header[0x16]);
	if (imageSize == 0)    imageSize = width*height * 3;
	if (dataPos == 0)      dataPos = 54;

	data = new unsigned char[imageSize];
	fread(data, 1, imageSize, file);
	fclose(file);


	GLuint textureID;
	//TODO: Create one OpenGL texture and set the texture parameter
	//clarification: for this part I simply copie from the tutorial notes

	glGenTextures(1, &textureID);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);
	// Give the image to OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);


	//filter modes
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//TODO this part don' understand yet, on some online openGL tutorials it simply use GL_NEAREST,
	//let me test the difference later

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glGenerateMipmap(GL_TEXTURE_2D);

	// OpenGL has now copied the data. Free our own version
	delete[] data;
	return textureID;
}

void sendDataToOpenGL()
{

	//TODO load texture

	//To create vao buffers and 3 vbo buffers for each object. 3 indicates we have 3 objects for now.
	glGenVertexArrays(numObj, vao);
	glGenBuffers(numObj, vbo);
	glGenBuffers(numObj, uvbo);
	glGenBuffers(numObj, nvbo);

	//these are template to pass information from loadObj to those buffers, should NOT be reused among diferent objects

	//@@@@@@@@@@@@@@@@@@@@FIRST OBJECT : PLANE@@@@@@@@@@@@@@@@@@@@@@@	
	std::vector<glm::vec3> vertices0;
	std::vector<glm::vec2> uvs0;
	std::vector<glm::vec3> normals0;
	bool obj1 = loadOBJ("C:\\Users\\cprj2748\\Downloads\\Project2\\sources\\plane.obj", vertices0, uvs0, normals0);
	texture[0] = loadBMP_custom("C:\\Users\\cprj2748\\Downloads\\Project2\\sources\\theme1.bmp");
	glBindVertexArray(vao[0]);
	//send vao of obj0 (plane) to openGL
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, vertices0.size() * sizeof(glm::vec3), &vertices0[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, uvbo[0]);
	glBufferData(GL_ARRAY_BUFFER, uvs0.size() * sizeof(glm::vec2), &uvs0[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, nvbo[0]);
	glBufferData(GL_ARRAY_BUFFER, normals0.size() * sizeof(glm::vec3), &normals0[0], GL_STATIC_DRAW);


	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(
		0, // attribute
		3, // size
		GL_FLOAT, // type
		GL_FALSE, // normalized?
		0, // stride
		(void*)0 // array buffer offset
	);
	drawSize[0] = (int)vertices0.size();
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, uvbo[0]);
	glVertexAttribPointer(
		1, // attribute
		2, // size
		GL_FLOAT, // type
		GL_FALSE, // normalized?
		0, // stride
		(void*)0 // array buffer offset
	);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, nvbo[0]);
	glVertexAttribPointer(
		2, // attribute
		3, // size
		GL_FLOAT, // type
		GL_FALSE, // normalized?
		0, // stride
		(void*)0 // array buffer offset
	);
	glEnableVertexAttribArray(2);

	glBindVertexArray(-1); 
	//@@@@@@@@@@@@@@@@@@@@FIRST OBJECT : PLANE@@@@@@@@@@@@@@@@@@@@@@@


	//@@@@@@@@@@@@@@@@@@@@SECOND OBJECT : JEEP@@@@@@@@@@@@@@@@@@@@@@@
	std::vector<glm::vec3> vertices1;
	std::vector<glm::vec2> uvs1;
	std::vector<glm::vec3> normals1;
	bool obj2 = loadOBJ("C:\\Users\\cprj2748\\Downloads\\Project2\\sources\\jeep.obj", vertices1, uvs1, normals1);
	glBindVertexArray(vao[1]);

	//send vao of obj0 (plane) to openGL
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, vertices1.size() * sizeof(glm::vec3), &vertices1[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, uvbo[1]);
	glBufferData(GL_ARRAY_BUFFER, uvs1.size() * sizeof(glm::vec2), &uvs1[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, nvbo[1]);
	glBufferData(GL_ARRAY_BUFFER, normals1.size() * sizeof(glm::vec3), &normals1[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glVertexAttribPointer(
		0, // attribute
		3, // size
		GL_FLOAT, // type
		GL_FALSE, // normalized?
		0, // stride
		(void*)0 // array buffer offset
	);
	drawSize[1] = (int)vertices1.size();
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, uvbo[1]);
	glVertexAttribPointer(
		1, // attribute
		2, // size
		GL_FLOAT, // type
		GL_FALSE, // normalized?
		0, // stride
		(void*)0 // array buffer offset
	);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, nvbo[1]);
	glVertexAttribPointer(
		2, // attribute
		3, // size
		GL_FLOAT, // type
		GL_FALSE, // normalized?
		0, // stride
		(void*)0 // array buffer offset
	);
	glEnableVertexAttribArray(2);

	glBindVertexArray(-1);
	//@@@@@@@@@@@@@@@@@@@@SECOND OBJECT : JEEP@@@@@@@@@@@@@@@@@@@@@@@

	//@@@@@@@@@@@@@@@@@@@@THIRD OBJECT : BLOCK@@@@@@@@@@@@@@@@@@@@@@@
	std::vector<glm::vec3> vertices2;
	std::vector<glm::vec2> uvs2;
	std::vector<glm::vec3> normals2;
	bool obj3 = loadOBJ("C:\\Users\\cprj2748\\Downloads\\Project2\\sources\\block.obj", vertices2, uvs2, normals2);
	glBindVertexArray(vao[2]);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, vertices2.size() * sizeof(glm::vec3), &vertices2[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, uvbo[2]);
	glBufferData(GL_ARRAY_BUFFER, uvs2.size() * sizeof(glm::vec2), &uvs2[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, nvbo[2]);
	glBufferData(GL_ARRAY_BUFFER, normals2.size() * sizeof(glm::vec3), &normals2[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glVertexAttribPointer(
		0, // attribute
		3, // size
		GL_FLOAT, // type
		GL_FALSE, // normalized?
		0, // stride
		(void*)0 // array buffer offset
	);
	drawSize[2] = (int)vertices2.size();
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, uvbo[2]);
	glVertexAttribPointer(
		1, // attribute
		2, // size
		GL_FLOAT, // type
		GL_FALSE, // normalized?
		0, // stride
		(void*)0 // array buffer offset
	);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, nvbo[2]);
	glVertexAttribPointer(
		2, // attribute
		3, // size
		GL_FLOAT, // type
		GL_FALSE, // normalized?
		0, // stride
		(void*)0 // array buffer offset
	);
	glEnableVertexAttribArray(2);

	glBindVertexArray(-1);
	//@@@@@@@@@@@@@@@@@@@@HIRD OBJECT : BLOCK@@@@@@@@@@@@@@@@@@@@@@@

}

void paintGL(void)
{
	//*****************TRANSFORMATION MATRIX SECTION**********************
	glm::mat4 scaleMatrix;
	scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.5f));  // the last is scallin coefficience

	glm::mat4 worldView;
	worldView = glm::lookAt(
		cameraPosition, // position of camera in world space
						//glm::vec3(0+x_press_num+lookX,1+y_press_num +lookY,1+z_press_num+lookZ), // gaze direction
						//glm::vec3(1,1,1),
		cameraGaze + cameraPosition,

		glm::vec3(0, 1, 0));

	glm::mat4 projection;
	projection = glm::perspective(iniFov, 800.0f / 800.0f, 0.1f, 100.0f);
	//******************COMMON MATRIX SECTION********************************

	cout << "camera position: " << cameraPosition.x << "," << cameraPosition.y << "," << cameraPosition.z << endl;
	cout << "gaze: " << cameraGaze.x << "," << cameraGaze.y << "," << cameraGaze.z << endl;

	glm::mat4 mvp = projection * worldView  * scaleMatrix;
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");

	//**************************************************************************


	glClearColor(0.94902f, 0.92941f, 0.82353f, 0.7f); // set the background color

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	//TODO:
	//Set lighting information, such as position and color of lighting source
	//Set transformation matrix
	//Bind different textures
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glViewport(0, 0, WIDTH, HEIGHT);


	//// ambientLight
	//GLint ambientLightUniformLocation = glGetUniformLocation(programID, "ambientLight");
	//vec3 ambientLight(0.1f, 0.1f, 0.1f);  // RGB light of ambient light
	//glUniform3fv(ambientLightUniformLocation, 1, &ambientLight[0]);

	////eyePosition
	//GLint eyePositionUniformLocation = glGetUniformLocation(programID, "eyePositionWorld");
	//vec3 eyePosition(0.0f, 0.0f, 0.0f);
	//glUniform3fv(eyePositionUniformLocation, 1, &eyePosition[0]);

	////light position world   ... for now it's lightPositionWorld, slide 26
	//GLint lightPositionUniformLocation = glGetUniformLocation(programID, "lightPositionWorld");
	//vec3 lightPositionWorld(2.0f, 15.0f, -10.0f);
	//glUniform3fv(lightPositionUniformLocation, 1, &lightPositionWorld[0]);

	////diffuse
	//vec3 lightVectorWorld = normalize(lightPositionWorld - vertexPositionWorld);
	//float brightness = dot(lightVectorWorld, normalize(normalWorld));
	//vec4 diffuseLight = vec4(birghtness, brightness, brightness, 1.0);


	//****************PAINT FIRST OBJECT PLANE*************
	glm::mat4 modelTransformMatrix0 = glm::mat4(1.0f);
	modelTransformMatrix0 = glm::translate(glm::mat4(), glm::vec3(0.0f, -1.0f, -5.0f));

	glm::mat4 mvp0 = projection * worldView * scaleMatrix * modelTransformMatrix0;

	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp0[0][0]);
	glBindVertexArray(vao[0]);
		//load and bind texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture[0]);
		glUniform1i(glGetUniformLocation(programID, "texture0plane"), 0);

	glDrawArrays(GL_TRIANGLES, 0, drawSize[0]);

	//disable all buffers
	glBindVertexArray(-1);
	glBindTexture(GL_TEXTURE_2D, -1);
	//******************************************************


	//****************PAINT SECOND OBJECT JEEP*************
	//glBindVertexArray(vao[1]);
	//glm::mat4 modelTransformMatrix1 = glm::mat4(1.0f);
	//modelTransformMatrix1 = glm::translate(glm::mat4(), glm::vec3(0.0f, -1.0f, -4.0f));

	//glm::mat4 scaleMatrix1;
	//scaleMatrix1 = glm::scale(glm::mat4(1.0f), glm::vec3(0.3f));  // the last is scallin coefficience


	//glm::mat4 mvp1 = projection * worldView * scaleMatrix1 * modelTransformMatrix1;

	//glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp1[0][0]);


	////glColor3f(1, 1, 0);

	//glDrawArrays(GL_TRIANGLES, 0, drawSize[1]);
	//glBindVertexArray(-1);
	//////******************************************************


	//////****************PAINT THIRD OBJECT BLOCK*************
	//glBindVertexArray(vao[2]);
	//glm::mat4 modelTransformMatrix2 = glm::mat4(1.0f);
	//modelTransformMatrix2 = glm::translate(glm::mat4(), glm::vec3(0.0f, 5.0f, -4.0f));

	//glm::mat4 scaleMatrix2;
	//scaleMatrix2 = glm::scale(glm::mat4(1.0f), glm::vec3(0.4f));  // the last is scallin coefficience


	//glm::mat4 mvp2 = projection * worldView * scaleMatrix2 * modelTransformMatrix2;

	//glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp2[0][0]);


	////glColor3f(1, 1, 0);

	//glDrawArrays(GL_TRIANGLES, 0, drawSize[2]);
	//glBindVertexArray(-1);
	//******************************************************



	glFlush();
	glutPostRedisplay();
}

void initializedGL(void) //run only once
{
	glewInit();
	installShaders();
	sendDataToOpenGL();
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutCreateWindow("Assignment 2");


	//TODO:
	/*Register different CALLBACK function for GLUT to response
	with different events, e.g. window sizing, mouse click or
	keyboard stroke */
	initializedGL();

	glutDisplayFunc(paintGL);

	glutKeyboardFunc(keyboard);
	glutSpecialFunc(move);
	glutPassiveMotionFunc(PassiveMouse);


	glutMainLoop();

	return 0;
}
