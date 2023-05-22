#version 330 core

layout(location = 0) in vec3 aPos;

uniform mat4 u_View;
uniform mat4 u_Projection;

out vec3 TextureCoord;

void main()
{
	vec4 position = u_Projection * u_View * vec4(aPos, 1.0f);

	gl_Position = vec4(position.x, position.y, position.w, position.w);

	TextureCoord = vec3(aPos.x, aPos.y, -aPos.z);
}