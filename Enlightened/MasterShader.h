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
#include <iostream>
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

// vertex structure
struct Vertex_PosTex
{
    D3DXVECTOR3 pos;
    D3DXVECTOR2	texture;
};

class MasterShader : public Shader
{
protected:
    std::map<std::string, LPDIRECT3DTEXTURE9>* m_normalTextures;
	float m_time;
	Light m_lights[6];
	std::vector<LPDIRECT3DTEXTURE9>* m_textureShadowMap;
	std::vector<LPDIRECT3DSURFACE9>* m_shadowMapSurface;
    std::vector<LPDIRECT3DSURFACE9>* m_pSurfaceShadowDS;
    LPDIRECT3DVERTEXDECLARATION9	 m_pVertexDec;
    LPDIRECT3DTEXTURE9               m_billboardTexture;
    LPDIRECT3DVERTEXBUFFER9			 m_pVB;

public:
	MasterShader(LPDIRECT3DDEVICE9 a_device, LPCTSTR a_fileName, std::vector<std::string>* a_meshNames, std::vector<LPDIRECT3DTEXTURE9>* a_textureShadowMap, std::vector<LPDIRECT3DSURFACE9>* a_pSurfaceShadowDS, std::vector<LPDIRECT3DSURFACE9>* a_shadowMapSurface ) : Shader(a_device, a_fileName)
	{
	    this->m_pSurfaceShadowDS = a_pSurfaceShadowDS;
	    this->m_textureShadowMap = a_textureShadowMap;
	    this->m_shadowMapSurface = a_shadowMapSurface;
	    
        // definition of square vertices
        Vertex_PosTex vertSquare[] = 
        {
            {D3DXVECTOR3(-1.0f,-1.0f, 0.0f), D3DXVECTOR2(0.0f, 1.0f)},
            {D3DXVECTOR3(-1.0f, 1.0f, 0.0f), D3DXVECTOR2(0.0f, 0.0f)},
            {D3DXVECTOR3( 1.0f,-1.0f, 0.0f), D3DXVECTOR2(1.0f, 1.0f)},
            {D3DXVECTOR3( 1.0f, 1.0f, 0.0f), D3DXVECTOR2(1.0f, 0.0f)},
        };
	    HRESULT hr;
        // create vertex buffer used to hold triangle vertices
        V(a_device->CreateVertexBuffer(sizeof(vertSquare), D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &m_pVB, 0))
	    
        // lock memory, copy data in, unlock memory
        void* pMem = NULL;
        V(m_pVB->Lock(0, 0, &pMem, 0))
            memcpy(pMem, vertSquare, sizeof(vertSquare));
        V(m_pVB->Unlock())
	    
        // define vertex structure
        D3DVERTEXELEMENT9 VertexArray[] = 
        {
            {0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},	// position
            {0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},	// texture coords
            D3DDECL_END()
        };

        V(a_device->CreateVertexDeclaration(VertexArray, &m_pVertexDec))

        D3DXCreateTextureFromFileEx(a_device, L"billboard_tree.png", 1524, 1748, 0, 0,
        D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT,
        D3DX_DEFAULT, 0x00000000, NULL, NULL, &m_billboardTexture);
    
		m_time = 0.0f;

		if (m_pEffect) {
			m_pEffect->SetTechnique("Master");
		}
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
        m_lights[0].target = D3DXVECTOR3(-200.0f, 1.0f, -313.0f);
        m_lights[0].position = D3DXVECTOR3(-500.0f, 50.25f, -313.0f);
		m_pEffect->SetValue("g_lights[0].color", m_lights[0].color, sizeof(D3DXVECTOR4));
		m_pEffect->SetValue("g_lights[0].target", m_lights[0].target, sizeof(D3DXVECTOR3));
		m_pEffect->SetBool("g_lights[0].isTargetLight", true);
		m_pEffect->SetValue("g_lights[0].position", m_lights[0].position, sizeof(D3DXVECTOR3));
		m_pEffect->SetFloat("g_lights[0].radius", 1000.0f);
		m_pEffect->SetFloat("g_lights[0].outerCone", D3DXToRadian(90.0f));
		m_pEffect->SetFloat("g_lights[0].innerCone", D3DXToRadian(30.0f));
		
        m_lights[1].color = D3DXVECTOR4(1.0f, 1.0f, 0.0f, 1.0f);
        m_lights[1].target = D3DXVECTOR3(-200.0f, 1.0f, 3.0f);
        m_lights[1].position = D3DXVECTOR3(-500.0f, 50.25f, 3.0f);
        m_pEffect->SetValue("g_lights[1].color", m_lights[1].color, sizeof(D3DXVECTOR4));
		m_pEffect->SetValue("g_lights[1].target", m_lights[1].target, sizeof(D3DXVECTOR3));
		m_pEffect->SetBool("g_lights[1].isTargetLight", true);
		m_pEffect->SetValue("g_lights[1].position", m_lights[1].position, sizeof(D3DXVECTOR3));
		m_pEffect->SetFloat("g_lights[1].radius", 1000.0f);
		m_pEffect->SetFloat("g_lights[1].outerCone", D3DXToRadian(90.0f));
		m_pEffect->SetFloat("g_lights[1].innerCone", D3DXToRadian(10.0f));
		
        m_lights[2].target = D3DXVECTOR3(-200.0f, 1.0f, -813.0f);
        m_lights[2].position = D3DXVECTOR3(1200.0f, 50.25f, -313.0f);		
		m_pEffect->SetValue("g_lights[2].color", D3DXVECTOR4(1.0f, 1.0f, 0.0f, 1.0f), sizeof(D3DXVECTOR4));
		m_pEffect->SetValue("g_lights[2].target", m_lights[2].target, sizeof(D3DXVECTOR3));
		m_pEffect->SetBool("g_lights[2].isTargetLight", true);
		m_pEffect->SetValue("g_lights[2].position", m_lights[2].position, sizeof(D3DXVECTOR3));
		m_pEffect->SetFloat("g_lights[2].radius", 1000.0f);
		m_pEffect->SetFloat("g_lights[2].outerCone", D3DXToRadian(90.0f));
		m_pEffect->SetFloat("g_lights[2].innerCone", D3DXToRadian(30.0f));
		
		/*

        m_lights[3].target = D3DXVECTOR3(900.0f, -1.0f, -813.0f);
        m_lights[3].position = D3DXVECTOR3(1200.0f, 50.25f, -813.0f);
		m_pEffect->SetValue("g_lights[3].color", D3DXVECTOR4(1.0f, 1.0f, 0.0f, 1.0f), sizeof(D3DXVECTOR4));
		m_pEffect->SetValue("g_lights[3].target", m_lights[3].target, sizeof(D3DXVECTOR3));
		m_pEffect->SetBool("g_lights[3].isTargetLight", true);
		m_pEffect->SetValue("g_lights[3].position", m_lights[3].position, sizeof(D3DXVECTOR3));
		m_pEffect->SetFloat("g_lights[3].radius", 1000.0f);
		m_pEffect->SetFloat("g_lights[3].outerCone", D3DXToRadian(90.0f));
		m_pEffect->SetFloat("g_lights[3].innerCone", D3DXToRadian(30.0f));

        m_lights[4].target = D3DXVECTOR3(900.0f, -1.0f, -313.0f);
        m_lights[4].position = D3DXVECTOR3(1200.0f, 50.25f, -313.0f);
   		m_pEffect->SetValue("g_lights[4].color", D3DXVECTOR4(1.0f, 1.0f, 0.0f, 1.0f), sizeof(D3DXVECTOR4));
		m_pEffect->SetValue("g_lights[4].target", m_lights[4].target, sizeof(D3DXVECTOR3));
		m_pEffect->SetBool("g_lights[4].isTargetLight", true);
		m_pEffect->SetValue("g_lights[4].position", m_lights[4].position, sizeof(D3DXVECTOR3));
		m_pEffect->SetFloat("g_lights[4].radius", 1000.0f);
		m_pEffect->SetFloat("g_lights[4].outerCone", D3DXToRadian(90.0f));
		m_pEffect->SetFloat("g_lights[4].innerCone", D3DXToRadian(30.0f));

        m_lights[5].target = D3DXVECTOR3(900.0f, -1.0f, 3.0f);
        m_lights[5].position = D3DXVECTOR3(1200.0f, 50.25f, 3.0f);
        m_pEffect->SetValue("g_lights[5].color", D3DXVECTOR4(1.0f, 1.0f, 0.0f, 1.0f), sizeof(D3DXVECTOR4));
		m_pEffect->SetValue("g_lights[5].target", m_lights[5].target, sizeof(D3DXVECTOR3));
		m_pEffect->SetBool("g_lights[5].isTargetLight", true);
		m_pEffect->SetValue("g_lights[5].position", m_lights[5].position, sizeof(D3DXVECTOR3));
		m_pEffect->SetFloat("g_lights[5].radius", 1000.0f);
		m_pEffect->SetFloat("g_lights[5].outerCone", D3DXToRadian(90.0f));
		m_pEffect->SetFloat("g_lights[5].innerCone", D3DXToRadian(30.0f));*/
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
    void GenerateShadowMap(UINT index, SGLib::Geometry* a_geometry, D3DXMATRIX matWorld)
    {
      
        HRESULT hr;
        UINT unPasses;
        D3DXMATRIX lightView, lightProj, lightViewProjection, lightWorldViewProjection;
        D3DXVECTOR3 lightUp(0.0f, 1.0f, 0.0f);

        // calculate light's view matrix
        D3DXMatrixLookAtLH(&lightView, &(m_lights[0].position), &(m_lights[0].target), &lightUp);

        //// calculate light's projection matrix
        D3DXMatrixPerspectiveFovLH(&lightProj, D3DX_PI*0.25f, 1.5f, 1.0f, 2000.0f);

        // calculate light's view-projection matrix
        D3DXMatrixMultiply(&lightViewProjection, &lightView, &lightProj);

        D3DXMatrixMultiply(&lightWorldViewProjection, &matWorld, &lightViewProjection);

        V(m_pEffect->SetMatrix("g_lightWorldViewProjectionMatrix[0]", &lightWorldViewProjection))   
        // calculate light's view matrix
        D3DXMatrixLookAtLH(&lightView, &(m_lights[1].position), &(m_lights[1].target), &lightUp);

        //// calculate light's projection matrix
        D3DXMatrixPerspectiveFovLH(&lightProj, D3DX_PI*0.25f, 1.5f, 1.0f, 2000.0f);

        // calculate light's view-projection matrix
        D3DXMatrixMultiply(&lightViewProjection, &lightView, &lightProj);

        D3DXMatrixMultiply(&lightWorldViewProjection, &matWorld, &lightViewProjection);

        V(m_pEffect->SetMatrix("g_lightWorldViewProjectionMatrix[1]", &lightWorldViewProjection))   

            // calculate light's view matrix
         D3DXMatrixLookAtLH(&lightView, &(m_lights[2].position), &(m_lights[2].target), &lightUp);

        //// calculate light's projection matrix
        D3DXMatrixPerspectiveFovLH(&lightProj, D3DX_PI*0.25f, 1.5f, 1.0f, 2000.0f);

        // calculate light's view-projection matrix
        D3DXMatrixMultiply(&lightViewProjection, &lightView, &lightProj);

        D3DXMatrixMultiply(&lightWorldViewProjection, &matWorld, &lightViewProjection);

        V(m_pEffect->SetMatrix("g_lightWorldViewProjectionMatrix[2]", &lightWorldViewProjection))   

        
        V(m_textureShadowMap->at(index)->GetSurfaceLevel(0, &(m_shadowMapSurface->at(index))))
        V(m_pD3DDevice->SetRenderTarget(0, m_shadowMapSurface->at(index)))
        V(m_pD3DDevice->SetDepthStencilSurface((*m_pSurfaceShadowDS)[index])) 

        V(m_pEffect->SetTechnique("MasterGenerate"))
        V( m_pEffect->CommitChanges());
        V(m_pEffect->Begin(&unPasses, NULL))

            for (UINT j = 0; j < unPasses; ++j)
            {
                V(m_pEffect->BeginPass(j))

                    a_geometry->Render();

                V(m_pEffect->EndPass())
            }

            V(m_pEffect->End())   
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

        LPDIRECT3DSURFACE9 pSurfaceOld;
        LPDIRECT3DSURFACE9 pSurfaceOldDS;
        
		V(m_pD3DDevice->GetTransform(D3DTS_WORLD, &oMatWorld))
		V(m_pD3DDevice->GetTransform(D3DTS_VIEW, &oMatView))
		V(m_pD3DDevice->GetTransform(D3DTS_PROJECTION, &oMatProj))

		D3DXMatrixMultiply(&oMatWorldViewProj, &oMatWorld, &oMatView);
		D3DXMatrixMultiply(&oMatWorldViewProj, &oMatWorldViewProj, &oMatProj);

		D3DXMatrixInverse(&oMatWorldIT, NULL, &oMatWorld);
		D3DXMatrixTranspose(&oMatWorldIT, &oMatWorldIT);
		
		
 		V(m_pEffect->SetMatrix("g_worldViewProjectionMatrix", &oMatWorldViewProj))
		V(m_pEffect->SetMatrix("g_worldMatrix", &oMatWorld))
		V(m_pEffect->SetMatrix("g_worldInverseTransposeMatrix", &oMatWorldIT))
		V(m_pEffect->SetMatrix("g_viewMatrix", &oMatView))
		V(m_pEffect->SetBool("g_isBillboard", false));
        if (a_geometry->GetDescription() == (LPCTSTR)"dwarf")
        {
            V(m_pEffect->SetTexture("g_normalTexture",(*m_normalTextures->find("dwarf")).second))
        }
        //else if (a_geometry->GetDescription() == (LPCTSTR)"tree")
        //{
        //    V(m_pEffect->SetTexture("g_normalTexture",(*m_normalTextures->find("tree")).second))
        //}
        //else if (a_geometry->GetDescription() == (LPCTSTR)"PetrolStation")
        //{
        //    V(m_pEffect->SetTexture("g_normalTexture",(*m_normalTextures->find("PetrolStation")).second))
        //}
        //else if (a_geometry->GetDescription() == (LPCTSTR)"monster")
        //{
        //    V(m_pEffect->SetTexture("g_normalTexture",(*m_normalTextures->find("monster")).second))
        //}
        else if (a_geometry->GetDescription() == (LPCTSTR)"Billboard")
        {
            V(m_pEffect->SetBool("g_isBillboard", true));
        }
        //V(m_pEffect->SetTexture("g_normalTexture",(*m_normalTextures->find("dwarf")).second))
        
        V(m_pD3DDevice->GetRenderTarget(0, &pSurfaceOld))
        V(m_pD3DDevice->GetDepthStencilSurface(&pSurfaceOldDS))
        
        GenerateShadowMap(0,a_geometry,oMatWorld);

        V(m_pD3DDevice->SetRenderTarget(0, pSurfaceOld))
        V(m_pD3DDevice->SetDepthStencilSurface(pSurfaceOldDS))
        
        V(m_pEffect->SetTechnique("Master")) 
        V(m_pEffect->SetTexture("g_shadowTexture", m_textureShadowMap->at(0)))
        
        if (a_geometry->GetDescription() == (LPCTSTR)"Billboard")
        {
            V(m_pD3DDevice->SetVertexDeclaration(m_pVertexDec))
            V(m_pD3DDevice->SetStreamSource(0, m_pVB, 0, sizeof(Vertex_PosTex)))

        }
        
        V(m_pEffect->Begin(&unPasses, NULL))

        for (UINT i = 0; i < unPasses; ++i)
        {
            V(m_pEffect->BeginPass(i))

                if (a_geometry->GetDescription() != (LPCTSTR)"Billboard")
                {
                    a_geometry->Render();
                }
                else 
                {
                    //for (int i = 0; i < 9; ++i)	// width
                    //{
                    //    for (int j = 0; j < 9; ++j)	// depth
                    //    {
                    //        
                    //        D3DXMatrixTranslation(&oMat, i * 7, 0.0f, j * 7);

                    //        // calculate world-view-projection matrix
                    //        D3DXMatrixMultiply(&matWorldViewProj, &matWorld, &matTrans);
                    //        D3DXMatrixMultiply(&matWorldViewProj, &matWorldViewProj, &matView);
                    //        D3DXMatrixMultiply(&matWorldViewProj, &matWorldViewProj, &matProj);

                
                    V(m_pEffect->SetTexture("g_billboardTexture", m_billboardTexture))                    
                    V(m_pEffect->CommitChanges())
                    V(m_pD3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2))
                }

            V(m_pEffect->EndPass())
        }

        V(m_pEffect->End())
        //V(pSurfaceOld->Release())
        //V(pSurfaceOldDS->Release())
	}

};

