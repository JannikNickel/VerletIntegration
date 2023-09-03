#include "graphics.h"
#include "structs/matrix4.h"
#include "glad/glad.h"
#include "texture.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <format>

static const float quadVertices[] =
{
	//POS					UV
	-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,
	0.5f, -0.5f, 0.0f,		1.0f, 0.0f,
	-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,
	0.5f, 0.5f, 0.0f,		1.0f, 1.0f
};
static const unsigned int quadIndices[] =
{
	0, 1, 2,
	1, 3, 2
};
static const float lineVertices[] =
{
	//POS					UV
	-0.5f, 0.0f, 0.0f,		0.0f, 0.0f,
	0.5f, 0.0f, 0.0f,		1.0f, 1.0f
};

static Window* window;
static unsigned int windowWidth;
static unsigned int windowHeight;

static unsigned int quadVAO, quadVBO, quadEBO;
static unsigned int lineVAO, lineVBO;

static unsigned int quadInstancedVAO, quadInstancedVBO, quadInstancedEBO;
static unsigned int quadInstanceTransformBuffer;
static unsigned int quadInstanceColorBuffer;

static unsigned int lineInstancedVAO, lineInstancedVBO;
static unsigned int lineInstanceTransformBuffer;
static unsigned int lineInstanceColorBuffer;

static unsigned int shaderProgram;
static unsigned int shaderModelLoc;
static unsigned int shaderMainColorLoc;

static unsigned int instancedShaderProgram;

static Texture quadTex;
static Texture circleTex;

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

static void SetViewMat(unsigned int shader, const Matrix4& view)
{
	glUseProgram(shader);
	glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, reinterpret_cast<const GLfloat*>(&view));
}

static void SetProjMat(unsigned int shader, const Matrix4& proj)
{
	glUseProgram(shader);
	glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, reinterpret_cast<const GLfloat*>(&proj));
}

static void SetViewProjMat(unsigned int shader, const Matrix4& view, const Matrix4& proj)
{
	SetViewMat(shader, view);
	SetProjMat(shader, proj);
}

static void CreateVertexArrayObject(unsigned int* vao, unsigned int* vbo, unsigned int* ebo, const float* vertices, int verticesLength, const unsigned int* indices, int indicesLength, unsigned int* instanceTransformBuffer = nullptr, unsigned int* instanceColorBuffer = nullptr)
{
	glGenVertexArrays(1, vao);
	glGenBuffers(1, vbo);
	if(indices != nullptr)
	{
		glGenBuffers(1, ebo);
	}
	glBindVertexArray(*vao);

	glBindBuffer(GL_ARRAY_BUFFER, *vbo);
	glBufferData(GL_ARRAY_BUFFER, verticesLength, vertices, GL_STATIC_DRAW);

	if(indices != nullptr)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesLength, indices, GL_STATIC_DRAW);
	}

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

void Graphics::Init(Window* window, unsigned int width, unsigned int height)
{
	::window = window;
	windowWidth = width;
	windowHeight = height;

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
	CreateVertexArrayObject(&lineVAO, &lineVBO, nullptr, lineVertices, sizeof(lineVertices), nullptr, 0);
	CreateVertexArrayObject(&lineInstancedVAO, &lineInstancedVBO, nullptr, lineVertices, sizeof(lineVertices), nullptr, 0, &lineInstanceTransformBuffer, &lineInstanceColorBuffer);

	quadTex = Texture("resources/square.png");
	circleTex = Texture("resources/circle.png");
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

	glDeleteVertexArrays(1, &lineVAO);
	glDeleteBuffers(1, &lineVBO);

	glDeleteVertexArrays(1, &lineInstancedVAO);
	glDeleteBuffers(1, &lineInstancedVBO);
	glDeleteBuffers(1, &lineInstanceTransformBuffer);
	glDeleteBuffers(1, &lineInstanceColorBuffer);
}

void Graphics::SetProjection(unsigned int width, unsigned int height)
{
	Matrix4 proj = Matrix4::Ortho(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height));
	SetProjMat(shaderProgram, proj);
	SetProjMat(instancedShaderProgram, proj);
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

void Graphics::Quad(Vector2 pos, Vector2 size, const Color& color, const optional_ref<Texture>& texture)
{
	PrepareShader(Matrix4::PositionScale2d(pos, size), color, texture.value_or(quadTex).get().GlId());

	glBindVertexArray(quadVAO);
	glDrawElements(GL_TRIANGLES, sizeof(quadIndices), GL_UNSIGNED_INT, 0);
}

void Graphics::Circle(Vector2 pos, float radius, const Color& color)
{
	PrepareShader(Matrix4::PositionScale2d(pos, radius * 2.0f), color, circleTex.GlId());

	glBindVertexArray(quadVAO);
	glDrawElements(GL_TRIANGLES, sizeof(quadIndices), GL_UNSIGNED_INT, 0);
}

void Graphics::Line(Vector2 from, Vector2 to, const Color& color)
{
	PrepareShader(Matrix4::Line(from, to), color, quadTex.GlId());

	glBindVertexArray(lineVAO);
	glDrawArrays(GL_LINES, 0, 2);
}

static void PrepareInstancedRendering(unsigned int transformBuffer, const Matrix4* matrices, unsigned int colorBuffer, const Color* colors, int instanceCount, unsigned int texture)
{
	glBindBuffer(GL_ARRAY_BUFFER, transformBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, instanceCount * sizeof(Matrix4), matrices);

	glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, instanceCount * sizeof(Color), colors);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glUseProgram(instancedShaderProgram);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
}

void Graphics::QuadsInstanced(const Vector2* positions, const Vector2* sizes, const Color* colors, int instanceCount)
{
	for(int i = 0; i < instanceCount; i++)
	{
		instanceTransforms[i] = Matrix4::PositionScale2d(positions[i], sizes[i] * 2.0f);
	}
	PrepareInstancedRendering(quadInstanceTransformBuffer, instanceTransforms, quadInstanceColorBuffer, colors, instanceCount, quadTex.GlId());

	glBindVertexArray(quadInstancedVAO);
	glDrawElementsInstanced(GL_TRIANGLES, sizeof(quadIndices), GL_UNSIGNED_INT, 0, instanceCount);
}

void Graphics::CirclesInstanced(const Vector2* positions, const float* radii, const Color* colors, int instanceCount)
{
	for(int i = 0; i < instanceCount; i++)
	{
		instanceTransforms[i] = Matrix4::PositionScale2d(positions[i], radii[i] * 2.0f);
	}
	CirclesInstanced(instanceTransforms, colors, instanceCount);
}

void Graphics::CirclesInstanced(const Matrix4* matrices, const Color* colors, int instanceCount)
{
	PrepareInstancedRendering(quadInstanceTransformBuffer, matrices, quadInstanceColorBuffer, colors, instanceCount, circleTex.GlId());

	glBindVertexArray(quadInstancedVAO);
	glDrawElementsInstanced(GL_TRIANGLES, sizeof(quadIndices), GL_UNSIGNED_INT, 0, instanceCount);
}

void Graphics::LinesInstanced(const Matrix4* matrices, const Color* colors, int instanceCount)
{
	PrepareInstancedRendering(lineInstanceTransformBuffer, matrices, lineInstanceColorBuffer, colors, instanceCount, quadTex.GlId());

	glBindVertexArray(lineInstancedVAO);
	glDrawArraysInstanced(GL_LINES, 0, 2, instanceCount);
}
