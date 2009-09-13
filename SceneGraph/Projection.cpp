#include "Projection.h"

namespace SGLib
{
	/**
	*	\brief	Projection constructor - sets projection matrix with a_rMatrixProj
	*	\param	LPDIRECT3DDEVICE9 a_pD3DDevice - pointer to direct3ddevice used for directx operations
	*	\param	D3DXMATRIX& a_rMatrixProj - reference to matrix used for projection
	*/

	Projection::Projection(	LPDIRECT3DDEVICE9 a_pD3DDevice, 
							D3DXMATRIX& a_rMatrixProj) :
								Node(a_pD3DDevice), 
								Transform(a_pD3DDevice)
	{
		m_oMatrix = a_rMatrixProj;
	}

	/**
	*	\brief	Projection constructor - uses D3DXMatrixPerspectiveFovLH to create projection matrix
	*	\param	LPDIRECT3DDEVICE9 a_pD3DDevice - pointer to direct3ddevice used for directx operations
	*	\param	FLOAT a_fFov - field of view
	*	\param	FLOAT a_fAspect - width/height aspect ratio
	*	\param	FLOAT a_fNear - near clipping plane
	*	\param	FLOAT a_fFar - far clipping plane
	*/

	Projection::Projection(	LPDIRECT3DDEVICE9 a_pD3DDevice, 
							FLOAT a_fFov, 
							FLOAT a_fAspect, 
							FLOAT a_fNear, 
							FLOAT a_fFar) : 
								Node(a_pD3DDevice), 
								Transform(a_pD3DDevice)
	{
		D3DXMatrixPerspectiveFovLH(&m_oMatrix, a_fFov, a_fAspect, a_fNear, a_fFar);
	}

	/**
	*	\brief	Projection destructor
	*	\note	Child and Sibling nodes are not touched and their destruction is left up to the user
	*/

	Projection::~Projection(void)
	{
	}

	/**
	*	\brief	Accessor for object's type
	*	\return	NodeType - returns SGLib::NodeType::PROJECTION
	*/

	NodeType Projection::GetType() const
	{
		return PROJECTION;
	}

	/**
	*	\brief	Mutator for projection matrix
	*	\return	NodeType - returns SGLib::NodeType::PROJECTION
	*/

	void Projection::SetProjMatrix(D3DXMATRIX& a_rMatrixProj)
	{
		m_oMatrix = a_rMatrixProj;
	}

	/**
	*	\brief	Sets projection matrix using D3DXMatrixPerspectiveFovLH
	*	\param	FLOAT a_fFov - field of view
	*	\param	FLOAT a_fAspect - width/height aspect ratio
	*	\param	FLOAT a_fNear - near clipping plane
	*	\param	FLOAT a_fFar - far clipping plane
	*/

	void Projection::ResetMatrix(FLOAT a_fFov, FLOAT a_fAspect, FLOAT a_fNear, FLOAT a_fFar)
	{
		D3DXMatrixPerspectiveFovLH(&m_oMatrix, a_fFov, a_fAspect, a_fNear, a_fFar);
	}

	/**
	*	\brief	Render function called when the scene graph is initially rendering this node
	*	\pre	Device must point to a valid DIRECT3DDEVICE object
	*	\post	Previous projection matrix is saved and latest projection matrix is set
	*/

	void Projection::Render()
	{
		HRESULT	hr;

		V(m_pD3DDevice->GetTransform(D3DTS_PROJECTION, &m_oMatrixPrevious))

		V(m_pD3DDevice->SetTransform(D3DTS_PROJECTION, &m_oMatrix))
	}

	/**
	*	\brief	Render function called after the scene graph has called Render() on this object and Render()
	*			on the child object of this object but before it is called on this object's sibling
	*	\pre	Device must point to a valid DIRECT3DDEVICE object
	*	\post	Previous projection matrix is set
	*/

	void Projection::PostRender()
	{
		HRESULT hr;

		V(m_pD3DDevice->SetTransform(D3DTS_PROJECTION, &m_oMatrixPrevious))
	}

	/**
	*	\brief	Only declared to avoid calling Transform::Update as the projection matrix functions slightly
	*			differently than a regular transform matrix
	*/

	void Projection::Update(FLOAT a_fTimeDiff)
	{

	}

	/**
	*	\brief	Only declared to avoid calling Transform::PostUpdate as the projection matrix functions slightly
	*			differently than a regular transform matrix
	*/

	void Projection::PostUpdate()
	{

	}
}