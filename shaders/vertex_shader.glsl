#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 vertexColor;

out vec3 FragColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float cSize;
void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f);
    FragColor = vertexColor;
    gl_PointSize = cSize;
}

