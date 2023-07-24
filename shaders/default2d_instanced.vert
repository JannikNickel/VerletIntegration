#version 330 core

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec2 uv;
layout (location = 2) in mat4 instanceTransform;
layout (location = 6) in vec4 instanceColor;

uniform mat4 view;
uniform mat4 projection;

out vec2 texCoord;
out vec4 color;

void main()
{
	gl_Position = projection * view * instanceTransform * vec4(vPos, 1.0);
	texCoord = uv;
	color = instanceColor;
}