#shader vertex
#version 410 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 tex;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;
uniform ivec2 u_msdfDims;

out vec2 vs_texCoords;

void main()
{
    gl_Position = u_projection * u_view * u_model * vec4(position, 1.0);
    vs_texCoords = tex / u_msdfDims;
};

#shader fragment
#version 410 core

in vec2 vs_texCoords;
out vec4 color;

uniform sampler2D u_msdf;

uniform vec4 u_color;
uniform float u_pxRange; // set to distance field's pixel range

float screenPxRange() {
    vec2 unitRange = vec2(u_pxRange) / vec2(textureSize(u_msdf, 0));
    vec2 screenTexSize = vec2(1.0) / fwidth(vs_texCoords);
    return max(0.5 * dot(unitRange, screenTexSize), 1.0);
}

float median(float r, float g, float b) {
    return max(min(r, g), min(max(r, g), b));
}

void main() {
    vec3 msd = texture(u_msdf, vs_texCoords).rgb;
    float sd = median(msd.r, msd.g, msd.b);
    float screenPxDistance = screenPxRange() * (sd - 0.5);
    float opacity = clamp(screenPxDistance + 0.5, 0.0, 1.0);
    color = vec4(u_color.rgb, opacity*u_color.w);
}