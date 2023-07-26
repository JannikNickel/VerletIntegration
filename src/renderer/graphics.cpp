#include "graphics.h"
#include "structs/matrix4.h"
#include "glad/glad.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <format>

const static float quadVertices[] =
{
	//POS					UV
	-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,
	0.5f, -0.5f, 0.0f,		1.0f, 0.0f,
	-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,
	0.5f, 0.5f, 0.0f,		1.0f, 1.0f,
};
const static unsigned int quadIndices[] =
{
	0, 1, 2,
	1, 3, 2
};

static Window* window;

static unsigned int quadVBO, quadVAO, quadEBO;

static unsigned int quadInstancedVBO, quadInstancedVAO, quadInstancedEBO;
static unsigned int quadInstanceTransformBuffer;
static unsigned int quadInstanceColorBuffer;

static unsigned int shaderProgram;
static unsigned int shaderModelLoc;
static unsigned int shaderMainColorLoc;

static unsigned int instancedShaderProgram;

static unsigned int quadTex;
static unsigned int circleTex;

static Matrix4 instanceTransforms[Graphics::instancingLimit];
static Color instanceColors[Graphics::instancingLimit];

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

static unsigned int CompileShaderProgram(const char* vShaderPath, const char* fShaderPath)
{
	unsigned int vertexShader = CompileShader(vShaderPath, GL_VERTEX_SHADER);
	unsigned int fragmentShader = CompileShader(fShaderPath, GL_FRAGMENT_SHADER);

	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	//ValidateShaderLinking(shaderProgram);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	return shaderProgram;
}

static void SetViewProjMat(unsigned int shader, const Matrix4& view, const Matrix4& proj)
{
	glUseProgram(shader);
	glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, reinterpret_cast<const GLfloat*>(&view));
	glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, reinterpret_cast<const GLfloat*>(&proj));
}

static void CreateVertexArrayObject(unsigned int* vao, unsigned int* vbo, unsigned int* ebo, const float* vertices, int verticesLength, const unsigned int* indices, int indicesLength, unsigned int* instanceTransformBuffer = nullptr, unsigned int* instanceColorBuffer = nullptr)
{
	glGenVertexArrays(1, vao);
	glGenBuffers(1, vbo);
	glGenBuffers(1, ebo);
	glBindVertexArray(*vao);

	glBindBuffer(GL_ARRAY_BUFFER, *vbo);
	glBufferData(GL_ARRAY_BUFFER, verticesLength, quadVertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesLength, quadIndices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	if(instanceTransformBuffer != nullptr)
	{
		glBindVertexArray(*vao);

		glGenBuffers(1, instanceTransformBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, *instanceTransformBuffer);
		glBufferData(GL_ARRAY_BUFFER, Graphics::instancingLimit * sizeof(Matrix4), instanceTransforms, GL_DYNAMIC_DRAW);

		std::size_t vec4Size = sizeof(Matrix4) / 4;
		for(int i = 0; i < 4; i++)
		{
			glEnableVertexAttribArray(2 + i);
			glVertexAttribPointer(2 + i, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(i * vec4Size));
			glVertexAttribDivisor(2 + i, 1);
		}

		glGenBuffers(1, instanceColorBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, *instanceColorBuffer);
		glBufferData(GL_ARRAY_BUFFER, Graphics::instancingLimit * sizeof(Color), instanceColors, GL_DYNAMIC_DRAW);

		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, vec4Size, (void*)0);
		glVertexAttribDivisor(6, 1);
	}

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

void Graphics::Init(Window* window, unsigned int width, unsigned int height)
{
	::window = window;

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	shaderProgram = CompileShaderProgram("shaders/default2d.vert", "shaders/default2d.frag");
	shaderMainColorLoc = glGetUniformLocation(shaderProgram, "mainColor");
	shaderModelLoc = glGetUniformLocation(shaderProgram, "model");

	instancedShaderProgram = CompileShaderProgram("shaders/default2d_instanced.vert", "shaders/default2d_instanced.frag");

	Matrix4 viewMatrix = Matrix4::identity;
	Matrix4 projectionMatrix = Matrix4::Ortho(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height));
	SetViewProjMat(shaderProgram, viewMatrix, projectionMatrix);
	SetViewProjMat(instancedShaderProgram, viewMatrix, projectionMatrix);

	CreateVertexArrayObject(&quadVAO, &quadVBO, &quadEBO, quadVertices, sizeof(quadVertices), quadIndices, sizeof(quadIndices));
	CreateVertexArrayObject(&quadInstancedVAO, &quadInstancedVBO, &quadInstancedEBO, quadVertices, sizeof(quadVertices), quadIndices, sizeof(quadIndices), &quadInstanceTransformBuffer, &quadInstanceColorBuffer);

	quadTex = CreateTexture("resources/square.png");
	circleTex = CreateTexture("resources/circle.png");
}

void Graphics::Shutdown()
{
	glDeleteProgram(shaderProgram);
	glDeleteProgram(instancedShaderProgram);

	glDeleteVertexArrays(1, &quadVAO);
	glDeleteBuffers(1, &quadVBO);
	glDeleteBuffers(1, &quadEBO);

	glDeleteVertexArrays(1, &quadInstancedVAO);
	glDeleteBuffers(1, &quadInstancedVBO);
	glDeleteBuffers(1, &quadInstancedEBO);
	glDeleteBuffers(1, &quadInstanceTransformBuffer);
	glDeleteBuffers(1, &quadInstanceColorBuffer);

	glDeleteTextures(1, &quadTex);
	glDeleteTextures(1, &circleTex);
}

void Graphics::SetClearColor(Color color)
{
	window->clearColor = color;
}

static void PrepareShader(const Matrix4& mat, const Color& color, unsigned int texture)
{
	glUseProgram(shaderProgram);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glUniform4f(shaderMainColorLoc, color.r, color.g, color.b, color.a);
	glUniformMatrix4fv(shaderModelLoc, 1, GL_FALSE, reinterpret_cast<const GLfloat*>(&mat));
}

void Graphics::Quad(Vector2 pos, Vector2 size, const Color& color)
{
	PrepareShader(Matrix4::PositionScale2d(pos, size), color, quadTex);

	glBindVertexArray(quadVAO);
	glDrawElements(GL_TRIANGLES, sizeof(quadIndices), GL_UNSIGNED_INT, 0);
}

void Graphics::Circle(Vector2 pos, float radius, const Color& color)
{
	PrepareShader(Matrix4::PositionScale2d(pos, radius * 2.0f), color, circleTex);

	glBindVertexArray(quadVAO);
	glDrawElements(GL_TRIANGLES, sizeof(quadIndices), GL_UNSIGNED_INT, 0);
}

static void PrepareInstancedRendering(Matrix4* matrices, Color* colors, int instanceCount, unsigned int texture)
{
	glBindBuffer(GL_ARRAY_BUFFER, quadInstanceTransformBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, instanceCount * sizeof(Matrix4), matrices);

	glBindBuffer(GL_ARRAY_BUFFER, quadInstanceColorBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, instanceCount * sizeof(Color), colors);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glUseProgram(instancedShaderProgram);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
}

void Graphics::QuadsInstanced(Vector2* positions, Vector2* sizes, Color* colors, int instanceCount)
{
	for(int i = 0; i < instanceCount; i++)
	{
		instanceTransforms[i] = Matrix4::PositionScale2d(positions[i], sizes[i] * 2.0f);
	}
	PrepareInstancedRendering(instanceTransforms, colors, instanceCount, quadTex);

	glBindVertexArray(quadInstancedVAO);
	glDrawElementsInstanced(GL_TRIANGLES, sizeof(quadIndices), GL_UNSIGNED_INT, 0, instanceCount);
}

void Graphics::CirclesInstanced(Vector2* positions, float* radii, Color* colors, int instanceCount)
{
	for(int i = 0; i < instanceCount; i++)
	{
		instanceTransforms[i] = Matrix4::PositionScale2d(positions[i], radii[i] * 2.0f);
	}
	PrepareInstancedRendering(instanceTransforms, colors, instanceCount, circleTex);

	glBindVertexArray(quadInstancedVAO);
	glDrawElementsInstanced(GL_TRIANGLES, sizeof(quadIndices), GL_UNSIGNED_INT, 0, instanceCount);
}
