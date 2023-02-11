#shader vertex
#version 410 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 tex;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

void main()
{
	gl_Position = u_projection * u_view * u_model * vec4(position, 1.0);
};

#shader fragment
#version 410 core

uniform vec4 u_color;

out vec4 color;

void main()
{
	color = u_color;
}