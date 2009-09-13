// simple pixel and vertex shader that simply passes through position and colour values

// used to transform vertices from local space into homogenous clipping space
uniform extern float4x4 g_matWorldViewProjection;

// position vertex shader input structure
struct VSInput_Pos
{
	float3 pos: POSITION0;
};

// position vertex shader output structure
struct VSOutput_Pos
{
	float4 pos: POSITION0;
};

// position and colour vertex shader input structure
struct VSInput_PosCol
{
	float3 pos: POSITION0;
	float4 col: COLOR0;
};

// position and colour vertex shader output structure
struct VSOutput_PosCol
{
	float4 pos: POSITION0;
	float4 col: COLOR0;
};

// position vertex shader
VSOutput_Pos VS_Pos(VSInput_Pos a_Input)
{
	VSOutput_Pos Output;

	// compute vertex transformation
	Output.pos = mul(float4(a_Input.pos.rgb, 1.0f), g_matWorldViewProjection);

	return Output;
}

// position pixel shader
float4 PS_Pos(VSOutput_Pos a_Input) : COLOR
{
	// return black as no colour has been passed through
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}

// position and colour vertex shader
VSOutput_PosCol VS_PosCol(VSInput_PosCol a_Input)
{
	VSOutput_PosCol Output;

	// compute vertex transformation
	Output.pos = mul(float4(a_Input.pos, 1.0f), g_matWorldViewProjection);

	// copy colour across
	Output.col = a_Input.col;

	return Output;
}

// position and colour pixel shader
float4 PS_PosCol(VSOutput_PosCol a_Output) : COLOR
{
	// return passed in colour
	return a_Output.col;
}

// technique used for just position
technique PosTech
{
	pass P0
	{
		vertexShader = compile vs_2_0 VS_Pos();
		pixelShader = compile ps_2_0 PS_Pos();
	}
}

// technique used for both position and colour
technique PosColTech
{
	pass P0
	{
		vertexShader = compile vs_2_0 VS_PosCol();
		pixelShader = compile ps_2_0 PS_PosCol();
	}
}