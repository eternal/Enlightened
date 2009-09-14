/*
	Multipass texture shader
	This effect demonstrates a simple multi-pass technique in the form of texture blending

	This HLSL shader provides three techniques that render	- the first texture
															- the second texture
															- combined textures

	The blend factor determines how much of either texture is used in the combined calculation
*/


// used to transform vertices from local space into homogenous clipping space
uniform extern float4x4 g_matWorldViewProjection;

uniform extern texture g_texture1;		// first texture

sampler TextureOne = sampler_state
{
	Texture = <g_texture1>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};

// vertex shader input structure
struct VSInput_PosTex
{
	float3 pos: POSITION0;
	float2 tex: TEXCOORD0;
};

// vertex shader output structure
struct VSOutput_PosTex
{
	float4 pos: POSITION0;
	float2 tex: TEXCOORD0;
};

// position and texture vertex shader
VSOutput_PosTex VS_Texture(VSInput_PosTex a_Input)
{
	VSOutput_PosTex Output;

	// compute vertex transformation
	Output.pos = mul(float4(a_Input.pos, 1.0f), g_matWorldViewProjection);

	// copy texture across
	Output.tex = a_Input.tex;

	return Output;
}

// pixel shader that uses the first texture
float4 PS_Texture1(VSOutput_PosTex a_Input) : COLOR
{
	return float4(tex2D(TextureOne, a_Input.tex));
}

// technique used for first texture
technique Texture1
{
	pass P0
	{
		vertexShader = compile vs_2_0 VS_Texture();
		pixelShader = compile ps_2_0 PS_Texture1();
	}
}