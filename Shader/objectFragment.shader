#version 330 core

#define N_LIGHTS 3

out vec4 FragColor;

uniform vec3 u_Color;
uniform vec3 u_LightPos[N_LIGHTS];
uniform vec3 u_CameraPos;

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
vec3 halfWay;

in vec3 Normal;
in vec3 FragPos;
in vec2 TextureCoord;

vec3 CalcPointLight(vec3 lightPos)
{
	lightDir = normalize(lightPos - FragPos);
	halfWay = normalize(lightPos - u_CameraPos);

	float r = length(lightPos - FragPos);
	vec3 ambient = ka * lightColor;
	vec3 diffuse = max(0.0f, dot(Normal, lightDir)) * lightColor;
	vec3 specular = ks * (I / (r * r)) * pow(max(0.0f, dot(Normal, halfWay)), p) * lightColor;

	return (ambient + diffuse + specular);
}

void main()
{
	vec3 finalColor = u_Color;

	if (u_DoLight)
	{
		finalColor = vec3(0.0f);
		for (int i = 0; i < N_LIGHTS; i++)
		{
			finalColor += CalcPointLight(u_LightPos[i]) * u_Color;
		}
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