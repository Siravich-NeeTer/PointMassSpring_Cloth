#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec3 aTexCoord;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

out vec3 FragPos;
out vec3 Normal;
out vec2 TextureCoord;

void main()
{
	Normal = normalize(aNormal);
	gl_Position = u_Projection * u_View * u_Model * vec4(aPos, 1.0f);
	FragPos = vec3(u_Model * vec4(aPos, 1.0f));
	TextureCoord = aTexCoord.xy;
}