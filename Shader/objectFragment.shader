#version 330 core

out vec4 FragColor;

uniform vec3 u_Color;
uniform vec3 u_LightPos;
uniform vec3 u_Halfway;

uniform bool u_DoLight;
uniform bool u_IsTexture;
uniform sampler2D u_Texture;

// Light Component
float I = 1.0f;
float p = 8;
float ks = 0.8f;
float ka = 0.15f;
float kd = 1.0f;
vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
vec3 lightDir;

in vec3 Normal;
in vec3 FragPos;
in vec2 TextureCoord;

void main()
{
	vec3 finalColor = u_Color;

	if (u_DoLight)
	{
		lightDir = normalize(u_LightPos - FragPos);

		vec3 norm = normalize(Normal);
		vec3 halfWay = normalize(u_Halfway);
		
		float r = length(u_LightPos - FragPos);
		vec3 ambient = ka * lightColor;
		vec3 diffuse = max(0.0f, dot(norm, lightDir)) * lightColor;
		vec3 specular = ks * (I / (r * r)) * pow(max(0.0f, dot(norm, halfWay)), p) * lightColor;
		
		/*
		float ambient = ka;
		float diffuse = kd * (I / (r * r)) * max(0.0f, dot(normalVertex, u_LightVector));
		float specular = ks * (I / (r * r)) * pow(max(0.0f, dot(normalVertex, u_Halfway)), p);
		finalColor = normalVertex;
		*/

		finalColor = (ambient + diffuse + specular) * u_Color;
	}

	if (u_IsTexture)
	{
		FragColor = vec4(finalColor, 1.0f) * texture(u_Texture, TextureCoord);
	}
	else
	{
		FragColor = vec4(finalColor, 1.0f);
	}
}