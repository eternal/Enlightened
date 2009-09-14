#include "Shader.h"

using SGLib::Shader;

class ShaderTextureMulti : public Shader
{
public:
ShaderTextureMulti(LPDIRECT3DDEVICE9 a_pD3DDevice, LPCTSTR a_sFileName, LPDIRECT3DTEXTURE9 g_pTexture) : Shader(a_pD3DDevice, a_sFileName)
	{
		if (m_pEffect)
{
			m_pEffect->SetTechnique("Texture1");
            m_pEffect->SetTexture("m_Texture", g_pTexture);
		}
	}

	~ ShaderTextureMulti(){}

	void SetCamPos(D3DXVECTOR3 a_vecCamPos)
	{
		if (m_pEffect)
            m_pEffect->SetValue("g_vecCameraPos", a_vecCamPos, sizeof(D3DXVECTOR3));
	}

	void RenderGeometry(SGLib::Geometry* a_pGeoNode)
	{
		if (!a_pGeoNode && m_pEffect)
			return;

		HRESULT hr;
		UINT unPasses;
D3DXMATRIX oMatWorldViewProj, oMatWorld, oMatView, oMatProj, oMatWorldIT;

		V(m_pD3DDevice->GetTransform(D3DTS_WORLD, &oMatWorld))
		V(m_pD3DDevice->GetTransform(D3DTS_VIEW, &oMatView))
V(m_pD3DDevice->GetTransform(D3DTS_PROJECTION, &oMatProj))

D3DXMatrixMultiply(&oMatWorldViewProj, &oMatWorld, &oMatView);
D3DXMatrixMultiply(&oMatWorldViewProj,&oMatWorldViewProj, &oMatProj);

		D3DXMatrixInverse(&oMatWorldIT, NULL, &oMatWorld);
		D3DXMatrixTranspose(&oMatWorldIT, &oMatWorldIT);

		V(m_pEffect->SetMatrix("g_matWorldViewProjection", &oMatWorldViewProj))
		//V(m_pEffect->SetMatrix("g_matW", &oMatWorld))
		//V(m_pEffect->SetMatrix("g_matWIT", &oMatWorldIT))

		V(m_pEffect->Begin(&unPasses, NULL))

		for (UINT i = 0; i < unPasses; ++i)
		{
			V(m_pEffect->BeginPass(i))

			a_pGeoNode->Render();

			V(m_pEffect->EndPass())
		}

		V(m_pEffect->End())
	}

};
