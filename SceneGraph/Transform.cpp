#include "Transform.h"

namespace SGLib
{
	/**
	*	\brief	Transform constructor
	*	\param	LPDIRECT3DDEVICE9 a_pD3DDevice - pointer to direct3ddevice used for directx operations
	*	\param	D3DXMATRIX a_oMatrixTrans - reference to matrix to set transform with
	*/

	Transform::Transform(	LPDIRECT3DDEVICE9 a_pD3DDevice, 
							D3DXMATRIX& a_rMatrixTrans) : 
								Node(a_pD3DDevice)
	{
		SetMatrix(a_rMatrixTrans);
	}

	/**
	*	\brief	Transform protected constructor - only used by derived types
	*	\param	LPDIRECT3DDEVICE9 a_pD3DDevice - pointer to direct3ddevice used for directx operations
	*/

	Transform::Transform(LPDIRECT3DDEVICE9 a_pD3DDevice) : 
							Node(a_pD3DDevice)
	{
	}

	/**
	*	\brief	Transform destructor
	*	\note	Child and Sibling nodes are not touched and their destruction is left up to the user
	*/

	Transform::~Transform(void)
	{
	}

	/**
	*	\brief	Mutator for matrix transformation
	*	\param	D3DXMATRIX& a_rMatrixTrans - matrix to set transform with
	*/

	void Transform::SetMatrix(D3DXMATRIX& a_rMatrixTrans)
	{
		m_oMatrixTrans = a_rMatrixTrans;
	}

	/**
	*	\brief	Multiplies current transform matrix by a_rMatrixTrans and sets it as new transform
	*	\param	D3DXMATRIX& a_rMatrixTrans - matrix to set multiply original transform matrix with
	*/

	void Transform::MultMatrix(D3DXMATRIX& a_rMatrixTrans)
	{
		D3DXMatrixMultiply(&m_oMatrixTrans, &m_oMatrixTrans, &a_rMatrixTrans);
	}

	/**
	*	\brief	Accessor for matrix transformation
	*	\return	D3DXMATRIX - returns transform matrix
	*/

	D3DXMATRIX Transform::GetMatrix()
	{
		return m_oMatrixTrans;
	}

	/**
	*	\brief	Accessor for object's type
	*	\return	NodeType - returns SGLib::NodeType::TRANSFORM
	*/

	NodeType Transform::GetType() const
	{
		return TRANSFORM;
	}

	/**
	*	\brief	Render function called when the scene graph is initially rendering this node
	*	\pre	Device must point to a valid DIRECT3DDEVICE object
	*	\post	World matrix is updated with transform matrix
	*/

	void Transform::Render()
	{
		HRESULT hr;

		// set combined matrix
		V(m_pD3DDevice->SetTransform(D3DTS_WORLD, &m_oMatrix))
	}

	/**
	*	\brief	Render function called after the scene graph has called Render() on this object and Render()
	*			on the child object of this object but before Render() is called on this object's sibling
	*	\pre	Device must point to a valid DIRECT3DDEVICE object
	*	\post	Previous world matrix is set
	*/

	void Transform::PostRender()
	{
		HRESULT hr;

		// set old world matrix back
		V(m_pD3DDevice->SetTransform(D3DTS_WORLD, &m_oMatrixPrevious))
	}

	/**
	*	\brief	Update function called on the initial pass of the scene graph before the render call
	*	\param	FLOAT a_fTimeDiff - time difference since last update call
	*	\pre	Device must point to a valid DIRECT3DDEVICE object
	*	\post	Previous world matrix is stored and new world matrix is set
	*/

	void Transform::Update(FLOAT a_fTimeDiff)
	{
		HRESULT	hr;

		// retrieve current world matrix
		V(m_pD3DDevice->GetTransform(D3DTS_WORLD, &m_oMatrixPrevious))

		// calculate new world matrix
		D3DXMatrixMultiply(&m_oMatrix, &m_oMatrixPrevious, &m_oMatrixTrans);

		// set new world matrix
		V(m_pD3DDevice->SetTransform(D3DTS_WORLD, &m_oMatrix))
	}

	/**
	*	\brief	Sets the previous world matrix back
	*	\pre	Device must point to a valid DIRECT3DDEVICE object
	*	\post	Previous world matrix is set
	*/

	void Transform::PostUpdate()
	{
		HRESULT hr;

		// set old world matrix back
		V(m_pD3DDevice->SetTransform(D3DTS_WORLD, &m_oMatrixPrevious))
	}
}