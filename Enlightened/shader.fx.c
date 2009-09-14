#define LIGHT_COUNT 2
#define MATERIAL_COUNT 1

// textures
//uniform extern texture g_texture;

sampler TextureSampler : register(s0) = sampler_state
{
	//Texture = <g_texture>;
	//MinFilter = LINEAR;
	//MagFilter = LINEAR;
	//MipFilter = LINEAR;
	//AddressU = WRAP;
	//AddressV = WRAP;
};

// matrices
uniform extern float4x4 g_worldViewProjectionMatrix;
uniform extern float4x4 g_worldInverseTransposeMatrix;
uniform extern float4x4 g_worldMatrix;

// samplers

struct Material
{
	float4 diffuse;
	float3 ambient;
	float3 specular;
	float specularAttenuation;
};

struct Light
{
	float3 color;
	float3 direction;
	float3 position;
	float attenuation;
};

struct Camera
{
	float3 position;
};

// material components
uniform extern Material g_materials[MATERIAL_COUNT];
uniform extern Light g_lights[LIGHT_COUNT];

// camera compoenents
uniform extern Camera g_camera;

struct VSInput
{
	float4 position: POSITION;
	float3 normal : NORMAL;
	float2 textureCoordinates : TEXCOORD0;
};

struct VSOutput
{
	float4 position : POSITION;
	float3 normal : NORMAL;
	float2 textureCoordinates : TEXCOORD0;
	
	float3 view : TEXCOORD1;

	float3 worldPosition : TEXCOORD2;
};

VSOutput VS_Lumos(VSInput a_input)
{
	VSOutput output;

	output.position = mul(a_input.position, g_worldViewProjectionMatrix);

	output.textureCoordinates = a_input.textureCoordinates;

	output.worldPosition = mul(a_input.position, g_worldMatrix);

	output.normal = mul(g_worldInverseTransposeMatrix, a_input.normal);

	output.view = mul(g_camera.position, mul(a_input.position, g_worldMatrix));

	return output;
}

float4 PS_Lumos(VSOutput a_output) : COLOR
{
	Material material = g_materials[0];

	float3 normal = normalize(a_output.normal);
	float3 view = normalize(a_output.view);

	float4 textureColor = tex2D(TextureSampler, a_output.textureCoordinates);

	float3 lightInfluenceSummation = float3(0.0f, 0.0f, 0.0f);
	for(uint index = 0; index < LIGHT_COUNT; ++index)
	{
		Light light = g_lights[index];
		float3 lightVector = normalize(-(light.position - a_output.worldPosition));
		float3 halfway = normalize(lightVector + view);

		float3 diffuse = saturate(dot(normal, lightVector)) * material.diffuse.rgb;
		float3 specular = pow(saturate(dot(normal, halfway)), material.specularAttenuation) * material.specular;
		float3 ambient = material.ambient;

		float3 color = (saturate(ambient + diffuse) * textureColor + specular) * light.color;
		lightInfluenceSummation += color;
	}

	float alpha = material.diffuse.a * textureColor.a;
	
	return textureColor;
	return float4(lightInfluenceSummation, alpha);
}

technique Master
{
	pass Master
	{
		vertexShader = compile vs_3_0 VS_Lumos();
		pixelShader = compile ps_3_0 PS_Lumos();
	}
}