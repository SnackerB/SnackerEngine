#shader vertex
#version 410 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 tex;

out vec2 vs_tex;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

void main()
{
	gl_Position = u_projection * u_view * u_model * vec4(position, 1.0);
	vs_tex = tex;
};

#shader fragment
#version 410 core

in vec2 vs_tex;

out vec4 color;

uniform sampler2D u_Texture;

void main()
{
	color = texture(u_Texture, vs_tex);
}