#version 460 core

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform sampler2D u_Texture_Diffuse;
uniform sampler2D u_Texture_Specular;

uniform bool u_UseDiffuseColor;
uniform vec3 u_DiffuseColor;

void main()
{
	// Store the fragment position vector in the first gbuffer texture
	gPosition = FragPos;
	// Also store the per-fragment normals into the gbuffer
	gNormal = normalize(Normal);
	// And the diffuse per-fragment color
	gAlbedoSpec.rgb = texture(u_Texture_Diffuse, TexCoords).rgb;
    if(u_UseDiffuseColor)
    {
        gAlbedoSpec.rgb = u_DiffuseColor;
    }
	// Store specular intensity in gAlbedoSpec's alpha component
	// TODO: Find Specular map (Texture) of the object
	//gAlbedoSpec.a = texture(u_Texture_Specular, TexCoords).r;
	// Temporary set alpha to 0.0f to disable specular
	gAlbedoSpec.a = 0.0f;
}