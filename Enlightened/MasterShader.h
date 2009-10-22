//====================================================================
// MasterShader.h
// This class provides all the paramentalisation and callbacks for the 
// shader
// Version 0.92
// Authors: Douglas Cook & Nicholas Kinesey
// Date 15/09/09
//====================================================================
#pragma once

#include "Shader.h"

#include <map>
#include <string>
#include <vector>
#include <math.h>

#define PI 3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679f

using SGLib::Shader;

struct Light
{
    bool isTargetLight;
    D3DXVECTOR4 color;
    D3DXVECTOR3 target;
    D3DXVECTOR3 position;
    D3DXVECTOR3 direction;
    float radius;
    float innerCone;
    float outerCone;
};

class MasterShader : public Shader
{
protected:
    std::map<std::string, LPDIRECT3DTEXTURE9>* m_normalTextures;
	float m_time;
	Light m_lights[6];
	LPDIRECT3DTEXTURE9 m_textureShadowMap;
    LPDIRECT3DSURFACE9 m_pSurfaceShadowMap;
    LPDIRECT3DSURFACE9 m_pSurfaceShadowDS;
    LPDIRECT3DSURFACE9 m_pSurfaceOld;
    LPDIRECT3DSURFACE9 m_pSurfaceTexture;
    LPDIRECT3DSURFACE9 m_pSurfaceOldDS;

public:
	MasterShader(LPDIRECT3DDEVICE9 a_device, LPCTSTR a_fileName, std::vector<std::string>* a_meshNames, LPDIRECT3DTEXTURE9 a_textureShadowMap, LPDIRECT3DSURFACE9 a_pSurfaceShadowDS ) : Shader(a_device, a_fileName)
	{
	    this->m_pSurfaceShadowDS = a_pSurfaceShadowDS;
	    this->m_textureShadowMap = a_textureShadowMap;
	    
		m_time = 0.0f;

		if (m_pEffect) {
			m_pEffect->SetTechnique("Master");
		}
	    HRESULT hr;
	    m_normalTextures = new std::map<std::string, LPDIRECT3DTEXTURE9>();
	    
	    for( std::vector<std::string>::iterator iter=a_meshNames->begin(); iter != a_meshNames->end(); ++iter ) {
            //theres a much cleaner way to append in a oneliner i'm sure but running out of time
            std::string filename = (*iter);
            filename += "_norm.png";
            LPDIRECT3DTEXTURE9 texture = NULL;
            
            std::wstring wideFilename;
            wideFilename.assign(filename.begin(), filename.end());
            
            V(D3DXCreateTextureFromFile(a_device, wideFilename.c_str(), &texture));
            m_normalTextures->insert( std::pair<std::string, LPDIRECT3DTEXTURE9>( (*iter), texture));
        }
        
        
	    
		m_pEffect->SetFloat("g_materials[0].specularAttenuation", 1.0f);
		m_pEffect->SetValue("g_materials[0].specular", D3DXVECTOR4(0.0f, 0.0f, 0.0f, 1.0f), sizeof(D3DXVECTOR4));
		m_pEffect->SetValue("g_materials[0].diffuse",   D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), sizeof(D3DXVECTOR4));
		m_pEffect->SetValue("g_materials[0].ambient",   D3DXVECTOR4(0.1f, 0.1f, 0.1f, 1.0f), sizeof(D3DXVECTOR4));
        //-25.792118, -6.239999, 112.264069
        
        m_lights[0].color = D3DXVECTOR4(1.0f, 1.0f, 0.0f, 1.0f);
        m_lights[0].target = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
        m_lights[0].position = D3DXVECTOR3(-500.0f, 70.25f, -313.0f);
		m_pEffect->SetValue("g_lights[0].color", m_lights[0].color, sizeof(D3DXVECTOR4));
		m_pEffect->SetValue("g_lights[0].target", m_lights[0].target, sizeof(D3DXVECTOR3));
		m_pEffect->SetBool("g_lights[0].isTargetLight", true);
		m_pEffect->SetValue("g_lights[0].position", m_lights[0].position, sizeof(D3DXVECTOR3));
		m_pEffect->SetFloat("g_lights[0].radius", 1000.0f);
		m_pEffect->SetFloat("g_lights[0].outerCone", D3DXToRadian(90.0f));
		m_pEffect->SetFloat("g_lights[0].innerCone", D3DXToRadian(30.0f));

        m_pEffect->SetValue("g_lights[1].color", D3DXVECTOR4(1.0f, 1.0f, 0.0f, 1.0f), sizeof(D3DXVECTOR4));
		m_pEffect->SetValue("g_lights[1].direction", D3DXVECTOR3(0.0f, -1.0f, 0.0f), sizeof(D3DXVECTOR3));
		m_pEffect->SetBool("g_lights[1].isTargetLight", false);
		m_pEffect->SetValue("g_lights[1].position", D3DXVECTOR3(-500.0f, 70.25f, 3.0f), sizeof(D3DXVECTOR3));
		m_pEffect->SetFloat("g_lights[1].radius", 1000.0f);
		m_pEffect->SetFloat("g_lights[1].outerCone", D3DXToRadian(90.0f));
		m_pEffect->SetFloat("g_lights[1].innerCone", D3DXToRadian(10.0f));
		
		m_pEffect->SetValue("g_lights[2].color", D3DXVECTOR4(1.0f, 1.0f, 0.0f, 1.0f), sizeof(D3DXVECTOR4));
		m_pEffect->SetValue("g_lights[2].direction", D3DXVECTOR3(0.0f, -1.0f, 0.0f), sizeof(D3DXVECTOR3));
		m_pEffect->SetBool("g_lights[2].isTargetLight", false);
		m_pEffect->SetValue("g_lights[2].position", D3DXVECTOR3(-500.0f, 70.25f, -813.0f), sizeof(D3DXVECTOR3));
		m_pEffect->SetFloat("g_lights[2].radius", 1000.0f);
		m_pEffect->SetFloat("g_lights[2].outerCone", D3DXToRadian(90.0f));
		m_pEffect->SetFloat("g_lights[2].innerCone", D3DXToRadian(30.0f));
		
		m_pEffect->SetValue("g_lights[3].color", D3DXVECTOR4(1.0f, 1.0f, 0.0f, 1.0f), sizeof(D3DXVECTOR4));
		m_pEffect->SetValue("g_lights[3].direction", D3DXVECTOR3(0.0f, -1.0f, 0.0f), sizeof(D3DXVECTOR3));
		m_pEffect->SetBool("g_lights[3].isTargetLight", false);
		m_pEffect->SetValue("g_lights[3].position", D3DXVECTOR3(1200.0f, 70.25f, -813.0f), sizeof(D3DXVECTOR3));
		m_pEffect->SetFloat("g_lights[3].radius", 1000.0f);
		m_pEffect->SetFloat("g_lights[3].outerCone", D3DXToRadian(90.0f));
		m_pEffect->SetFloat("g_lights[3].innerCone", D3DXToRadian(30.0f));

   		m_pEffect->SetValue("g_lights[4].color", D3DXVECTOR4(1.0f, 1.0f, 0.0f, 1.0f), sizeof(D3DXVECTOR4));
		m_pEffect->SetValue("g_lights[4].direction", D3DXVECTOR3(0.0f, -1.0f, 0.0f), sizeof(D3DXVECTOR3));
		m_pEffect->SetBool("g_lights[4].isTargetLight", false);
		m_pEffect->SetValue("g_lights[4].position", D3DXVECTOR3(1200.0f, 70.25f, -313.0f), sizeof(D3DXVECTOR3));
		m_pEffect->SetFloat("g_lights[4].radius", 1000.0f);
		m_pEffect->SetFloat("g_lights[4].outerCone", D3DXToRadian(90.0f));
		m_pEffect->SetFloat("g_lights[4].innerCone", D3DXToRadian(30.0f));

        m_pEffect->SetValue("g_lights[5].color", D3DXVECTOR4(1.0f, 1.0f, 0.0f, 1.0f), sizeof(D3DXVECTOR4));
		m_pEffect->SetValue("g_lights[5].direction", D3DXVECTOR3(0.0f, -1.0f, 0.0f), sizeof(D3DXVECTOR3));
		m_pEffect->SetBool("g_lights[5].isTargetLight", false);
		m_pEffect->SetValue("g_lights[5].position", D3DXVECTOR3(1200.0f, 70.25f, 3.0f), sizeof(D3DXVECTOR3));
		m_pEffect->SetFloat("g_lights[5].radius", 1000.0f);
		m_pEffect->SetFloat("g_lights[5].outerCone", D3DXToRadian(90.0f));
		m_pEffect->SetFloat("g_lights[5].innerCone", D3DXToRadian(30.0f));

		//m_pEffect->SetValue("g_lights[1].color", D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), sizeof(D3DXVECTOR4));
		//m_pEffect->SetValue("g_lights[1].direction", D3DXVECTOR3(0.0f, -1.0f, 0.0f), sizeof(D3DXVECTOR3));
		//m_pEffect->SetBool("g_lights[1].isTargetCamera", false);
		//m_pEffect->SetValue("g_lights[1].position", D3DXVECTOR3(0.0f, 0.0f, -0.0f), sizeof(D3DXVECTOR3));
		//m_pEffect->SetFloat("g_lights[1].radius", 100.0f);
		//m_pEffect->SetFloat("g_lights[1].outerCone", 50.0f);
		//m_pEffect->SetFloat("g_lights[1].innerCone", 10.0f);
	}

	void Update(float a_timeDelta)
	{
		m_time += a_timeDelta;

		if (m_time > 2*PI)
		{
			m_time -= 2*PI;
		}

		float bias = (sin(m_time) * 1.0f / 3.0f) + 0.5f;
		bias = max(min(bias, 0.75f), 0.25f);
		//m_pEffect->SetValue("g_lights[0].position", D3DXVECTOR3((200.0f* bias)/3, 100.0f, 0.0f), sizeof(D3DXVECTOR3));
		/*m_pEffect->SetValue("g_lights[1].position", D3DXVECTOR3(200.0f* bias, 0.0f, -50.0f), sizeof(D3DXVECTOR3));*/
		m_pEffect->SetValue("g_sunlight", D3DXVECTOR4(0.75f * bias, 0.75f * bias, 0.0f, 1.0f), sizeof(D3DXVECTOR4));
		m_pEffect->SetFloat("g_time", bias);
	}

	~MasterShader() {}

	void SetCameraPosition(D3DXVECTOR3 a_cameraPosition)
	{
		if (m_pEffect) {
			m_pEffect->SetValue("g_camera.position", a_cameraPosition, sizeof(D3DXVECTOR3));
		}
	}

	void RenderGeometry(SGLib::Geometry* a_geometry)
	{
		if (!a_geometry && m_pEffect)
		{
			return;
		}
        
		HRESULT hr;
		UINT unPasses;
		D3DXMATRIX oMatWorldViewProj, oMatWorld, oMatView, oMatProj, oMatWorldIT;

		V(m_pD3DDevice->GetTransform(D3DTS_WORLD, &oMatWorld))
		V(m_pD3DDevice->GetTransform(D3DTS_VIEW, &oMatView))
		V(m_pD3DDevice->GetTransform(D3DTS_PROJECTION, &oMatProj))

		D3DXMatrixMultiply(&oMatWorldViewProj, &oMatWorld, &oMatView);
		D3DXMatrixMultiply(&oMatWorldViewProj, &oMatWorldViewProj, &oMatProj);

		D3DXMatrixInverse(&oMatWorldIT, NULL, &oMatWorld);
		D3DXMatrixTranspose(&oMatWorldIT, &oMatWorldIT);
		
		
        D3DXMATRIX lightView, lightProj, lightViewProjection, lightWorldViewProjection;

        D3DXVECTOR3 lightLook(0.0f, 0.0f, 0.0f);
        D3DXVECTOR3 lightUp(0.0f, 1.0f, 0.0f);
        D3DXVECTOR3 lightPos(1200.0f, 70.25f, 3.0f);

        //D3DXVECTOR3 lightLookAt = lightLook - lightPos;

        // calculate light's view matrix
        D3DXMatrixLookAtLH(&lightView, &(m_lights[0].position), &(m_lights[0].target), &lightUp);

        //// calculate light's projection matrix
        D3DXMatrixPerspectiveFovLH(&lightProj, D3DX_PI*0.25f, 1.5f, 1.0f, 2000.0f);

        // calculate light's view-projection matrix
        D3DXMatrixMultiply(&lightViewProjection, &lightView, &lightProj);

        D3DXMatrixMultiply(&lightWorldViewProjection, &oMatWorld, &lightViewProjection);
		
 		V(m_pEffect->SetMatrix("g_worldViewProjectionMatrix", &oMatWorldViewProj))
		V(m_pEffect->SetMatrix("g_worldMatrix", &oMatWorld))
		V(m_pEffect->SetMatrix("g_worldInverseTransposeMatrix", &oMatWorldIT))
        V(m_pEffect->SetMatrix("g_lightWorldViewProjectionMatrix", &lightWorldViewProjection))
        
        V(m_pEffect->SetTexture("g_normalTexture",(*m_normalTextures->find("dwarf")).second))
        
        V(m_pD3DDevice->GetRenderTarget(0, &m_pSurfaceOld))
        if (SUCCEEDED(m_textureShadowMap->GetSurfaceLevel(0, &m_pSurfaceTexture)))
        {
            if (SUCCEEDED(m_pD3DDevice->SetRenderTarget(0, m_pSurfaceTexture)))
            {
                if (SUCCEEDED(m_pD3DDevice->GetDepthStencilSurface(&m_pSurfaceOldDS)))
                {
                    m_pD3DDevice->SetDepthStencilSurface(m_pSurfaceShadowDS);
                }
            }
        }
        
        
        V(m_pEffect->SetTechnique("MasterGenerate"))
        V( m_pEffect->CommitChanges() );
		V(m_pEffect->Begin(&unPasses, NULL))

		for (UINT i = 0; i < unPasses; ++i)
		{
			V(m_pEffect->BeginPass(i))

			a_geometry->Render();

			V(m_pEffect->EndPass())
		}

		V(m_pEffect->End())
		
        V(m_pD3DDevice->SetRenderTarget(0, m_pSurfaceOld))
        V(m_pD3DDevice->SetDepthStencilSurface(m_pSurfaceOldDS))
        
        V(m_pEffect->SetTechnique("Master")) 
        V(m_pEffect->SetTexture("g_shadowTexture", m_textureShadowMap))
        
        V(m_pEffect->Begin(&unPasses, NULL))

        for (UINT i = 0; i < unPasses; ++i)
        {
            V(m_pEffect->BeginPass(i))

                a_geometry->Render();

            V(m_pEffect->EndPass())
        }

        V(m_pEffect->End())
	}

};

