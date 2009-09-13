#include "Shader.h"
#include "Articulated.h"
#include "Transform.h"

//using SGLib::Shader;
using namespace SGLib;

class ShaderSimple : public Shader
{
public:
	ShaderSimple(LPDIRECT3DDEVICE9 a_pD3DDevice, LPCTSTR a_sFileName) : Shader(a_pD3DDevice, a_sFileName)
	{
		if (m_pEffect)
			m_pEffect->SetTechnique("PosTech");
	}
	~ShaderSimple(){}

	void RenderGeometry(SGLib::Geometry* a_pGeoNode)
	{
		if (!a_pGeoNode && m_pEffect)
			return;

		//if (dynamic_cast<Articulated*>(a_pGeoNode))
		//	dynamic_cast<Articulated*>(a_pGeoNode)->Transform::Render();

		HRESULT hr;
		UINT unPasses;
		D3DXMATRIX oMatWorldViewProj, oMatWorld, oMatView, oMatProj;

		V(m_pD3DDevice->GetTransform(D3DTS_WORLD, &oMatWorld))
		V(m_pD3DDevice->GetTransform(D3DTS_VIEW, &oMatView))
		V(m_pD3DDevice->GetTransform(D3DTS_PROJECTION, &oMatProj))

		D3DXMatrixMultiply(&oMatWorldViewProj, &oMatWorld, &oMatView);
		D3DXMatrixMultiply(&oMatWorldViewProj, &oMatWorldViewProj, &oMatProj);

		V(m_pEffect->SetMatrix("g_matWorldViewProjection", &oMatWorldViewProj))

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

