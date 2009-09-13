#include "Shader.h"

namespace SGLib
{
	/**
	*	\brief	Shader constructor
	*	\param	LPDIRECT3DDEVICE9 a_pD3DDevice - pointer to direct3ddevice used for directx operations
	*	\param	LPCTSTR a_sFileName - effect file name (.fx file)
	*	\note	Calls the CreateEffect() function to create the LPD3DXEFFECT object
	*/

	Shader::Shader(	LPDIRECT3DDEVICE9 a_pD3DDevice, 
					LPCTSTR a_sFileName) : 
						Node(a_pD3DDevice),
						m_pEffect(NULL),
						m_pReference(NULL),
						m_sFileName(a_sFileName)
	{
		CreateEffect();
	}

	/**
	*	\brief	Shader reference constructor
	*	\param	Shader* a_pReference - pointer to shader node that this node will reference
	*	\pre	a_pReference != NULL
	*	\note	This constructor is used to allow a shader node to reference another so when this node is 
	*			rendering geometry, it calls the reference's RenderGeometry() function, allowing two nodes 
	*			too share one effect object
	*/

	Shader::Shader(	Shader* a_pReference) :
						Node(a_pReference->GetDevice()),
						m_pEffect(NULL),
						m_pReference(a_pReference),
						m_sFileName(NULL)
	{
	}

	/**
	*	\brief	Shader destructor
	*	\note	Child and Sibling nodes are not touched and their destruction is left up to the user
	*/

	Shader::~Shader(void)
	{
		SAFE_RELEASE(m_pEffect);
	}

	/**
	*	\brief	Called when device has been reset
	*	\param	LPDIRECT3DDEVICE9 a_pD3DDevice - pointer to new DIRECT3DDEVICE
	*/

	void Shader::OnResetDevice(LPDIRECT3DDEVICE9 a_pD3DDevice)
	{
		Node::OnResetDevice(a_pD3DDevice);

		if (m_pEffect)
			m_pEffect->OnResetDevice();
	}

	/**
	*	\brief	Called when device has been lost
	*/

	void Shader::OnLostDevice()
	{
		Node::OnLostDevice();

		if (m_pEffect)
			m_pEffect->OnLostDevice();
	}

	/**
	*	\brief	Called when device has been created
	*	\param	LPDIRECT3DDEVICE9 a_pD3DDevice - pointer to new DIRECT3DDEVICE
	*/

	void Shader::OnCreateDevice(LPDIRECT3DDEVICE9 a_pD3DDevice)
	{
		Node::OnCreateDevice(a_pD3DDevice);

		CreateEffect();
	}

	/**
	*	\brief	Called when device has been destroyed
	*/

	void Shader::OnDestroyDevice()
	{
		Node::OnDestroyDevice();

		SAFE_RELEASE(m_pEffect);
	}

	/**
	*	\brief	Accessor for object's type
	*	\return	NodeType - returns SGLib::NodeType::SHADER
	*/

	NodeType Shader::GetType() const
	{
		return SHADER;
	}

	/**
	*	\brief	Primary function for shader objects. Is used to set the appropriate effect variables before
	*			calling render on the geometry. If a reference node has been set, the reference::RenderGeometry
	*			should be called instead.
	*	\param	Geometry* a_pGeometryNode - node that contains mesh that is to be rendered
	*	\note	If Articulated nodes are being rendered, than their Transform::Render() functions need to be
	*			called prior to this function (or at the beginning) to ensure a correct world matrix. At the 
	*			moment this is called through the SGLib::SGRenderer::RenderNode() function if it detects an
	*			Articulated node. 
	*/

	void Shader::RenderGeometry(Geometry* a_pGeometryNode)
	{
		if (m_pReference)
			m_pReference->RenderGeometry(a_pGeometryNode);
	}

	/**
	*	\brief	Creates the effect from the m_sFileName
	*/

	void Shader::CreateEffect()
	{
		if (m_sFileName == NULL)
			return;

		// if its been perviously created and not released, do so now
		if (m_pEffect)
			SAFE_RELEASE(m_pEffect);

		HRESULT hr;
		LPD3DXBUFFER pErrorMsgs = NULL;
		
		// create effect
		hr = D3DXCreateEffectFromFile(m_pD3DDevice, m_sFileName, NULL, NULL, D3DXFX_NOT_CLONEABLE, NULL, &m_pEffect, &pErrorMsgs);

		// if effect creation failed and there were error messages, output details
		if (FAILED(hr) && !pErrorMsgs && !pErrorMsgs->GetBufferPointer())
		{
			OutputDebugString((LPCWSTR)pErrorMsgs->GetBufferPointer());
		}

		SAFE_RELEASE(pErrorMsgs);
	}
}