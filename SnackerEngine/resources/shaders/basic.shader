#shader vertex
#version 410 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 tex;
layout(location = 2) in vec3 normal;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

out vec3 vs_normal;

void main()
{
	gl_Position = u_projection * u_view * u_model * vec4(position, 1.0);
	vs_normal = normal;
};

#shader fragment
#version 410 core

in vec3 vs_normal;

out vec4 color;

void main()
{
	color = vec4(vs_normal.xyz, 1.0);
}