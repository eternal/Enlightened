//====================================================================
// shader.fx
// Version 0.9
// Authors: Doug Cook & Nick Kinsey
// Master Shader for lighting, shading and texture/normal mapping
// Date 15/09/09
//====================================================================

#define LIGHT_COUNT 3
#define MATERIAL_COUNT 1

// sunlight /  night time
uniform extern float4 g_sunlight;
uniform extern float g_time;

//billboard switch
uniform extern bool g_isBillboard;

// Textures
uniform extern texture g_normalTexture;
uniform extern texture g_shadowTexture;
uniform extern texture g_billboardTexture;

// Samplers
sampler diffuseSampler : register(s0) = sampler_state
{
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
};

sampler normalSampler = sampler_state
{
	Texture = <g_normalTexture>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU  = WRAP;
    AddressV  = WRAP;
    MaxAnisotropy = 16;
};

sampler shadowSampler = sampler_state
{
    texture = <g_shadowTexture>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = LINEAR;
    AddressU  = CLAMP; 
    AddressV  = CLAMP;
};

sampler billboardSampler = sampler_state
{
    texture = <g_billboardTexture>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = LINEAR;
    AddressU  = WRAP;
    AddressV  = WRAP;
};

// matrices
uniform extern float4x4 g_worldMatrix;
uniform extern float4x4 g_viewMatrix;
uniform extern float4x4 g_worldViewProjectionMatrix;
uniform extern float4x4 g_worldInverseTransposeMatrix;
uniform extern float4x4 g_lightWorldViewProjectionMatrix[3];

struct Material
{
	float4 diffuse;
	float4 emissive;
	float4 ambient;
	float4 specular;
	float specularAttenuation;
};

struct Light
{
	bool isTargetLight;
	float4 color;
	float3 target;
	float3 position;
	float3 direction;
	float radius;
	float innerCone;
	float outerCone;
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
	float3 position : POSITION;
	float2 textureCoordinates : TEXCOORD0;
	float3 normal : NORMAL;
	float4 tangent : TANGENT;
	float3 binormal : BINORMAL;
};

struct VSOutput
{
	float4 position : POSITION;
	float2 textureCoordinates : TEXCOORD0;
	float3 worldPosition : TEXCOORD1;

	float3 normal : TEXCOORD2;
	float3 binormal : TEXCOORD3;
	float3 tangent : TEXCOORD4;
	
	float4 projectedTexture : TEXCOORD5;

	//float3x3 tangentToWorld : TEXCOORD5;
	//float3 view : TEXCOORD6;
};

struct VSShadowOutput
{
    float4 position: POSITION0;
    float2 depth: TEXCOORD0;
};

VSOutput VS_Lumos(VSInput a_input, uniform int index)
{
	VSOutput output;

    output.position = mul(float4(a_input.position, 1.0f), g_worldViewProjectionMatrix);    
	 
	output.worldPosition = mul(float4(a_input.position, 1.0f), g_worldMatrix).xyz;
	output.textureCoordinates = a_input.textureCoordinates;	

	output.normal = a_input.normal;
	output.tangent = a_input.tangent;
	output.binormal = cross(output.normal, output.tangent) * a_input.tangent.w;

    output.projectedTexture = mul(float4(a_input.position, 1.0f), g_lightWorldViewProjectionMatrix[index]);

	return output;
}

float4 PS_Lumos(VSOutput a_input, uniform int index) : COLOR0
{
    a_input.projectedTexture.xy /= a_input.projectedTexture.w;            
    //a_input.projectedTexture.xy /= 1.0f;
    float2 projectedTextureCoords;
    projectedTextureCoords[0] =  0.5f*a_input.projectedTexture.x + 0.5f; 
    projectedTextureCoords[1] = -0.5f*a_input.projectedTexture.y + 0.5f;
    //float4 color2 = (float4)0;
    //color2[index] = tex2D(shadowSampler, a_input.projectedTexture);
    //return color2;
    
    // Compute pixel depth for shadowing.
    float realDepth = a_input.projectedTexture.z / 2000.0f; //denominator set to roughly depth buffer zmax (2000) but can be clipped manually to modify shadows
    float4 color = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    if ((saturate(projectedTextureCoords.x) == projectedTextureCoords.x) && (saturate(projectedTextureCoords.y) == projectedTextureCoords.y))
    {

         float storedDepth = tex2D(shadowSampler, projectedTextureCoords)[index];
         float offset = 1.0f/100.0f;

         if ((realDepth - offset) <= storedDepth)   
         {
             Material material = g_materials[0];

             float3 normal   = normalize(a_input.normal);
             float3 tangent  = normalize(a_input.tangent);
             float3 binormal = normalize(a_input.binormal);

             float3x3 tangentSpaceMatrix = float3x3(
                 tangent.x, binormal.x, normal.x,
                 tangent.y, binormal.y, normal.y,
                 tangent.z, binormal.z, normal.z
                 );
             //float3x3 tangentSpaceMatrix = transpose(float3x3(tangent, binormal, normal));
             float3 view = normalize(mul(g_camera.position - a_input.worldPosition, tangentSpaceMatrix));

             float3 light   = float3(0.0f, 0.0f, 0.0f);
             float3 halfway = float3(0.0f, 0.0f, 0.0f);

             float attenuation = 0.0f;
             float normalDotLight = 0.0f;
             float normalDotHalfway = 0.0f;
             float specularAttenuation = 0.0f;

             

             normal = normalize(tex2D(normalSampler, a_input.textureCoordinates).rgb * 2.0f - 1.0f);

             Light lightModel = g_lights[index];

             light = mul((lightModel.position - a_input.worldPosition) / lightModel.radius, tangentSpaceMatrix);
             //light = (lightModel.position - a_input.worldPosition) / lightModel.radius;
             //float3 lightDirection = mul(normalize(lightModel.direction), tangentSpaceMatrix);
             attenuation = saturate(1.0f - dot(light, light));
            
             light = normalize(light);
             halfway = normalize(light + view);

             if (lightModel.isTargetLight) {
                 lightModel.direction = lightModel.target - lightModel.position;        
             }

             float2 cosSpotlightCones = cos(float2(lightModel.outerCone, lightModel.innerCone) * 0.5f);
             float spotlightDot = dot(-light, normalize(lightModel.direction));
             //float spotlightDot = dot(-light, lightDirection);
             float spotlightEffect = smoothstep(cosSpotlightCones[0], cosSpotlightCones[1], spotlightDot);

             attenuation *= spotlightEffect;

             
             if (g_isBillboard)
             {
                normalDotLight = light;
                normalDotHalfway = halfway;
             }
             else
             {
                 normalDotLight = saturate(dot(normal, light));
                 normalDotHalfway = saturate(dot(normal, halfway));
             }
             specularAttenuation = (normalDotLight == 0.0f) ? 0.0f : pow(normalDotHalfway, material.specularAttenuation);

             color +=
                 (((material.ambient * (attenuation * lightModel.color))  + g_sunlight) * g_time)        // ambient
                 + (material.diffuse * lightModel.color * normalDotLight * attenuation)        // diffuse
                 + (material.specular * lightModel.color * specularAttenuation * attenuation); // specular
        }
         
    }
    if (g_isBillboard)
    {
        return color * tex2D(billboardSampler, a_input.textureCoordinates);
    }
    return color * tex2D(diffuseSampler, a_input.textureCoordinates);
    
}

VSShadowOutput VS_Shadow(VSInput a_input, uniform int index)
{
    VSShadowOutput output;
    
    output.position = mul(float4(a_input.position, 1.0f), g_lightWorldViewProjectionMatrix[index]);
    output.depth = output.position.zw;
    
    return output;
}

float4 PS_Shadow(VSShadowOutput a_input, uniform int index) : COLOR
{
    // output the depth of the pixel from the light source, normalized into the view space
    //return a_input.depth.x / a_input.depth.y;
    float4 color = (float4)0;
    color[index] = a_input.depth.x / 2000.0f;
    return color;
    //return a_Input.depth.x / 60.0f;
}

technique Master
{
	pass P0
	{
		vertexShader = compile vs_3_0 VS_Lumos(0);
		pixelShader = compile ps_3_0 PS_Lumos(0);
	}
    pass P1
    {
        SRCBLEND = ONE;
        DESTBLEND = ONE;
        ALPHABLENDENABLE = true;
        vertexShader = compile vs_3_0 VS_Lumos(1);
        pixelShader = compile ps_3_0 PS_Lumos(1);
    }
    pass P2
    {
        vertexShader = compile vs_3_0 VS_Lumos(2);
        pixelShader = compile ps_3_0 PS_Lumos(2);
    }
}

technique MasterBillboardAlpha
{
    pass P0
    {
        AlphaTestEnable				= true;
        AlphaRef					= 0x000000AA;
        AlphaFunc					= GreaterEqual;
        vertexShader = compile vs_3_0 VS_Lumos(0);
        pixelShader = compile ps_3_0 PS_Lumos(0);
    }
    pass P1
    {
        vertexShader = compile vs_3_0 VS_Lumos(1);
        pixelShader = compile ps_3_0 PS_Lumos(1);
    }
    pass P2
    {
        vertexShader = compile vs_3_0 VS_Lumos(2);
        pixelShader = compile ps_3_0 PS_Lumos(2);
    }
}

technique MasterGenerate
{
    pass P0
    {
        colorwriteenable = red;    
        vertexShader = compile vs_3_0 VS_Shadow(0);
        pixelShader = compile ps_3_0 PS_Shadow(0);
    }
    pass P1
    {
        colorwriteenable = green;    
        vertexShader = compile vs_3_0 VS_Shadow(1);
        pixelShader = compile ps_3_0 PS_Shadow(1);
    }
    pass P2
    {
        colorwriteenable = blue;    
        vertexShader = compile vs_3_0 VS_Shadow(2);
        pixelShader = compile ps_3_0 PS_Shadow(2);
    }
}
