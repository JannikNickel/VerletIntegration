#include "graphics.h"
#include "glad/glad.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <format>

static float quadVertices[] =
{
	//POS					UV
	-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,
	0.5f, -0.5f, 0.0f,		1.0f, 0.0f,
	-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,
	0.5f, 0.5f, 0.0f,		1.0f, 1.0f,
};
static unsigned int quadIndices[] =
{
	0, 1, 2,
	1, 3, 2
};

static unsigned int quadVBO, quadVAO, quadEBO;
static unsigned int quadTex;
static unsigned int circleTex;

static unsigned int shaderProgram;
static unsigned int shaderModelLoc;
static unsigned int shaderViewLoc;
static unsigned int shaderProjectionLoc;
static unsigned int shaderMainColorLoc;

static glm::mat4 viewMatrix;
static glm::mat4 projectionMatrix;

static void ValidateShaderCompilation(unsigned int shader)
{
	int success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if(!success)
	{
		char log[512];
		glGetShaderInfoLog(shader, 512, NULL, log);
		throw std::exception(std::format("Shader compilation failed! (%s)", log).c_str());
	}
}

static void ValidateShaderLinking(unsigned int program)
{
	int success;
	glGetProgramiv(program, GL_COMPILE_STATUS, &success);
	if(!success)
	{
		char log[512];
		glGetProgramInfoLog(program, 512, NULL, log);
		throw std::exception(std::format("Shader program linking failed! (%s)", log).c_str());
	}
}

static unsigned int CompileShader(const char* path, unsigned int shaderType)
{
	std::ifstream file = std::ifstream(path);
	std::stringstream buffer;
	buffer << file.rdbuf();
	std::string str = buffer.str();
	const char* content = str.c_str();
	file.close();

	unsigned int shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &content, NULL);
	glCompileShader(shader);
	ValidateShaderCompilation(shader);
	return shader;
}

static void CreateVertexArrayObject(unsigned int* vao, unsigned int* vbo, unsigned int* ebo, const float* vertices, int verticesLength, const unsigned int* indices, int indicesLength)
{
	glGenVertexArrays(1, vao);
	glGenBuffers(1, vbo);
	glGenBuffers(1, ebo);
	glBindVertexArray(*vao);

	glBindBuffer(GL_ARRAY_BUFFER, *vbo);
	glBufferData(GL_ARRAY_BUFFER, verticesLength, quadVertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesLength, quadIndices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

static unsigned int CreateTexture(const char* path)
{
	stbi_set_flip_vertically_on_load(true);
	int width, height, channels;
	unsigned char* img = stbi_load(path, &width, &height, &channels, 0);
	if(img == nullptr)
	{
		stbi_image_free(img);
		throw std::exception(std::format("Could not load image data! (%s)", path).c_str());
	}

	unsigned int tex;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(img);

	return tex;
}

void Graphics::Init(unsigned int width, unsigned int height)
{
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	unsigned int vertexShader = CompileShader("shaders/default2d.vert", GL_VERTEX_SHADER);
	unsigned int fragmentShader = CompileShader("shaders/default2d.frag", GL_FRAGMENT_SHADER);

	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	ValidateShaderLinking(shaderProgram);

	shaderMainColorLoc = glGetUniformLocation(shaderProgram, "mainColor");
	shaderModelLoc = glGetUniformLocation(shaderProgram, "model");
	shaderViewLoc = glGetUniformLocation(shaderProgram, "view");
	shaderProjectionLoc = glGetUniformLocation(shaderProgram, "projection");

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	viewMatrix = glm::mat4(1.0f);
	projectionMatrix = glm::ortho(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height));

	glUseProgram(shaderProgram);
	glUniformMatrix4fv(shaderViewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(shaderProjectionLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	CreateVertexArrayObject(&quadVAO, &quadVBO, &quadEBO, quadVertices, sizeof(quadVertices), quadIndices, sizeof(quadIndices));

	quadTex = CreateTexture("resources/square.png");
	circleTex = CreateTexture("resources/circle.png");
}

void Graphics::Shutdown()
{
	glDeleteProgram(shaderProgram);
	glDeleteVertexArrays(1, &quadVAO);
	glDeleteBuffers(1, &quadVBO);
	glDeleteBuffers(1, &quadEBO);
	glDeleteTextures(1, &quadTex);
	glDeleteTextures(1, &circleTex);
}

static void PrepareShader(const glm::mat4& mat, unsigned int texture, const Color& color)
{
	glUseProgram(shaderProgram);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glUniform4f(shaderMainColorLoc, color.r, color.g, color.b, color.a);
	glUniformMatrix4fv(shaderModelLoc, 1, GL_FALSE, glm::value_ptr(mat));
}

void Graphics::Quad(Vector2 pos, Vector2 size, const Color& color)
{
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(pos.x, pos.y, 0.0f));
	model = glm::scale(model, glm::vec3(size.x, size.y, 1.0f));

	PrepareShader(model, quadTex, color);

	glBindVertexArray(quadVAO);
	glDrawElements(GL_TRIANGLES, sizeof(quadIndices), GL_UNSIGNED_INT, 0);
}

void Graphics::Circle(Vector2 pos, float radius, const Color& color)
{
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(pos.x, pos.y, 0.0f));
	model = glm::scale(model, glm::vec3(radius * 2.0f));

	PrepareShader(model, circleTex, color);

	glBindVertexArray(quadVAO);
	glDrawElements(GL_TRIANGLES, sizeof(quadIndices), GL_UNSIGNED_INT, 0);
}
