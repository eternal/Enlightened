// matrices
uniform extern float4x4 g_matWorldViewProjection;
uniform extern float4x4 g_matWorldInverseTranspose;
uniform extern float4x4 g_matWorld;

// material components
uniform float4 g_vecMaterialDiffuse = float4(1.0f, 0.5f, 0.0f, 1.0f);
uniform float4 g_vecMaterialAmbient = float4(0.9f, 0.2f, 0.0f, 1.0f);
uniform float4 g_vecMaterialSpecular = float4(0.2f, 0.2f, 0.2f, 1.0f);

// light components
uniform float4 g_vecLightDiffuse = float4(1.0f, 1.0f, 1.0f, 1.0f);
uniform float4 g_vecLightAmbient = float4(0.3f, 0.4f, 0.3f, 1.0f);
uniform float4 g_vecLightSpecular = float4(0.2f, 0.2f, 0.2f, 1.0f);
uniform float3 g_vecLightDirection = float3(0.5f, 0.3f, 0.6f);

uniform extern float3 g_vecLightPos;
uniform float3 g_vecSpotLightDirection = float3(0.0f,0.3f,0.6f);

// camera compoenents
uniform extern float3 g_vecCameraPos;

uniform float  g_fSpecPower = 8.0f;
uniform float  g_fSpotPower = 0.5f;
struct VSInput
{
	float3 pos: POSITION0;
	float3 norm: NORMAL0;
};

struct VSPhongOutput
{
	float4 pos: POSITION0;
	float3 norm: TEXCOORD0;
	float3 posW: TEXCOORD1;
};
VSPhongOutput VS_Phong(VSInput a_Input)
{
	VSPhongOutput Output;

	Output.pos = mul(float4(a_Input.pos, 1.0f), g_matWorldViewProjection);

	Output.posW = mul(float4(a_Input.pos, 1.0f), g_matWorld).xyz;

	float3 normal = mul(float4(a_Input.norm, 0.0f), g_matWorldInverseTranspose).xyz;
	Output.norm = normalize(normal);

	return Output;
}

float4 PS_Phong(VSPhongOutput a_Output) : COLOR
{
	a_Output.norm = normalize(a_Output.norm);

	float3 toEye = normalize(g_vecCameraPos - a_Output.posW);
	///temp spotlight shit
    float3 lightVec = normalize(g_vecLightPos - a_Output.posW);
    float spot = pow(max(dot(-lightVec, g_vecSpotLightDirection), 0.00000001f), g_fSpotPower);
    

	float3 r = reflect(-g_vecLightDirection, a_Output.norm);
    float3 ta = max(dot(r, toEye), 0.0000000001f);
	float t = pow(ta, g_fSpecPower);

	float s = max(dot(g_vecLightDirection, a_Output.norm), 0.001f);

	float3 specular = t * (g_vecLightSpecular * g_vecMaterialSpecular).rgb;
	float3 diffuse = s * (g_vecLightDiffuse * g_vecMaterialDiffuse).rgb;
	float3 ambient = g_vecLightAmbient * g_vecMaterialAmbient;

	return float4(spot * (diffuse + ambient + specular), g_vecMaterialDiffuse.a);
}


technique PhongTech
{
	pass P0
	{
		vertexShader = compile vs_2_0 VS_Phong();
		pixelShader = compile ps_2_0 PS_Phong();
	}
}