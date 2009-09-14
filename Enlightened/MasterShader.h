#include "Shader.h"
#include <map>
#include <string>
#include <vector>

using SGLib::Shader;

class MasterShader : public Shader
{
protected:
    std::map<std::string, LPDIRECT3DTEXTURE9>* m_normalTextures;
public:
	MasterShader(LPDIRECT3DDEVICE9 a_device, LPCTSTR a_fileName, std::vector<std::string>* a_meshNames) : Shader(a_device, a_fileName)
	{
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
		m_pEffect->SetValue("g_materials[0].specular", D3DXVECTOR3(1.0f, 1.0f, 1.0f), sizeof(D3DXVECTOR3));
		m_pEffect->SetValue("g_materials[0].diffuse",   D3DXVECTOR3(1.0f, 1.0f, 1.0f), sizeof(D3DXVECTOR3));
		m_pEffect->SetValue("g_materials[0].ambient",   D3DXVECTOR3(0.0f, 0.0f, 0.0f), sizeof(D3DXVECTOR3));

		m_pEffect->SetValue("g_lights[0].color", D3DXVECTOR3(1.0f, 1.0f, 1.0f), sizeof(D3DXVECTOR3));
		m_pEffect->SetValue("g_lights[0].direction", D3DXVECTOR3(0.0f, -1.0f, 0.0f), sizeof(D3DXVECTOR3));
		m_pEffect->SetValue("g_lights[0].position", D3DXVECTOR3(0.0f, -10.0f, 0.0f), sizeof(D3DXVECTOR3));
		m_pEffect->SetFloat("g_materials[0].attenuation", 1.0f);

		m_pEffect->SetValue("g_lights[1].color", D3DXVECTOR3(1.0f, 1.0f, 1.0f), sizeof(D3DXVECTOR3));
		m_pEffect->SetValue("g_lights[1].direction", D3DXVECTOR3(0.0f, -1.0f, 0.0f), sizeof(D3DXVECTOR3));
		m_pEffect->SetValue("g_lights[1].position", D3DXVECTOR3(0.0f, 10.0f, 0.0f), sizeof(D3DXVECTOR3));
		m_pEffect->SetFloat("g_materials[0].attenuation", 1.0f);
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
		
 		V(m_pEffect->SetMatrix("g_worldViewProjectionMatrix", &oMatWorldViewProj))
		V(m_pEffect->SetMatrix("g_worldMatrix", &oMatWorld))
		V(m_pEffect->SetMatrix("g_worldInverseTransposeMatrix", &oMatWorldIT))
        
        V(m_pEffect->SetTexture("g_normalTexture",(*m_normalTextures->find("dwarf")).second))
        
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

