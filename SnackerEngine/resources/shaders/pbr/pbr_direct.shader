#shader vertex
#version 410 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 tex;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;

out vec2 TexCoords;
out vec3 WorldPos;
out vec3 Normal;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

void main()
{
	gl_Position = u_projection * u_view * u_model * vec4(position, 1.0);
	TexCoords = tex;
	WorldPos = (u_model * vec4(position, 1.0)).xyz;
	Normal = (u_model * vec4(normal, 0.0)).xyz;
};

#shader fragment
#version 410 core

out vec4 FragColor;
in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normal;

uniform vec3 camPos;

// material parameters
uniform vec3 albedo;
uniform float metallic;
uniform float roughness;
uniform float ao;

// lights
uniform vec3 lightPositions[4];
uniform vec3 lightColors[4];

const float PI = 3.14159265359;

// Approximation for the Fresnel equation F
// cosTheta = dot(H, V)
// F0 = base reflictivity, computed through index of refraction (IOR) "surface reflection at zero incidence"
// "how much the surface reflects if looking directly at the surface"
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
	// F = F0 + (1 - F0) * (1 - (H*V))^5
	return F0 + (1.0 - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}

// Normal Distribution Function D: statistically approximates relative surface area of micro fascets aligned exactly to halfway vector h
// N = normal vector
// H = halfway vector
// Approximation: Trowbeidge-Reitz GGX
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
	float a = roughness * roughness; // Based on observations by Disney and adopted by Epic Games, the lighting looks more correct squaring the roughness 
	float a2 = a * a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH * NdotH;

	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;
	// a^2 / (pi * ((n * h)^2 * (a^2 - 1) + 1)^2)
	return a2 / denom;
}

// Geometry function G: statistically approximate the relative surface area where its micro surface-details overshadow each other causing
// light rays to be occluded.
float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r * r) / 8.0; // k_direct (needs to be different for IBL)

	float denom = NdotV * (1.0 - k) + k;
	// n*v / ((n * v) * (1 - k) + k)
	// where k = (a + 1)^2 / 8
	return NdotV / denom;
}

// Schmitts method: take in account both view direction (geometry obstruction) and the light direction (geometry shadowing)!
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);

	return ggx1 * ggx2;
}


void main()
{
	vec3 N = normalize(Normal);
	vec3 V = normalize(camPos - WorldPos); // Normal vector pointing from WorldPos of fragment -> camPos

	vec3 F0 = vec3(0.04); // (Simplifying) Assumption: most dielectric surfaces look visually correct with a constant F0 of 0.04
	F0 = mix(F0, albedo, metallic); // For metallic surfaces, we vary F0 by linearly interpolating between the original F0 and the albedo value given the metallic property.

	vec3 Lo = vec3(0.0);
	for (int i = 0; i < 1; ++i)
	{
		vec3 L = normalize(lightPositions[i] - WorldPos); // Normal vector pointing from WorldPos -> lightPositions[i]
		vec3 H = normalize(V + L); // Halfway vector

		float distance = length(lightPositions[i] - WorldPos);
		float attenuation = 1.0 / (distance * distance); // (physically based) inverse square law for light falloff

		vec3 radiance = lightColors[i] * attenuation;
		
		// Compute DFG
		vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);
		float D = DistributionGGX(N, H, roughness);
		float G = GeometrySmith(N, V, L, roughness);

		// Fresnel value directly corresponds to k_S
		vec3 kS = F;
		vec3 kD = vec3(1.0) - kS;

		kD *= (1.0 - metallic); // metallic surfaces don't refract light and thus have no diffuse reflections

		// Compute Cook-Torrance BRDF
		vec3 numerator = D * G * F;
		float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
		// D*F*G / (4 * (w_0 * v) * (w_i * n))
		vec3 specular = numerator / max(denominator, 0.001); // we constrain the denominator to 0.001 to prevent a divide by zero in case any dot product ends up 0.0

		// Compute outgoing radiance
		float NdotL = max(dot(N, L), 0.0);
		// and add to overall radiance
		Lo += (kD * albedo / PI + specular) * radiance * NdotL;
	}

	// Add ambient term (TODO: Change this)
	vec3 ambient = vec3(0.03) * albedo * ao;
	vec3 color = ambient + Lo;

	// gamma correction
	color = color / (color + vec3(1.0));
	color = pow(color, vec3(1.0 / 2.2));
	
	FragColor = vec4(color, 1.0);
}