#shader vertex
#version 410 core

layout(location = 0) in vec2 position;

out vec2 vs_tex;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

void main()
{
	gl_Position = u_projection * u_view * u_model * vec4(vec3(position.xy, 0.0), 1.0);
	vs_tex = position;
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