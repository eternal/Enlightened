#include "Camera.h"

namespace SGLib
{
	/**
	*	\brief	Camera constructor
	*	\param	LPDIRECT3DDEVICE9 a_pD3DDevice - pointer to direct3ddevice used for directx operations
	*/

	Camera::Camera(LPDIRECT3DDEVICE9 a_pD3DDevice) :	Node(a_pD3DDevice), 
														Transform(a_pD3DDevice), 
														m_bSimpleMovement(FALSE)
	{
		// init vectors to some default values
		m_vecPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		m_vecUp = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		m_vecLook = D3DXVECTOR3(0.0f, 0.0f, 1.0f);

		// update view matrix
		UpdateMatrix(); 
	}

	/**
	*	\brief	Camera constructor
	*	\param	LPDIRECT3DDEVICE9 a_pD3DDevice - pointer to direct3ddevice used for directx operations
	*	\param	D3DXVECTOR3& a_rvecPos - specifies camera position vector
	*	\param	D3DXVECTOR3& a_rvecUp - specifies camera up vector
	*	\param	D3DXVECTOR3& a_rvecLook - specifies camera look vector
	*/

	Camera::Camera(	LPDIRECT3DDEVICE9 a_pD3DDevice, 
					D3DXVECTOR3& a_rvecPos, 
					D3DXVECTOR3& a_rvecUp, 
					D3DXVECTOR3& a_rvecLook) :	Node(a_pD3DDevice), 
												Transform(a_pD3DDevice), 
												m_vecPos(a_rvecPos), 
												m_vecUp(a_rvecUp), 
												m_vecLook(a_rvecLook), 
												m_bSimpleMovement(FALSE)
	{
		UpdateMatrix();
	}

	/**
	*	\brief	Camera destructor
	*	\note	Child and Sibling nodes are not touched and their destruction is left up to the user
	*/

	Camera::~Camera(void)
	{
	}

	/**
	*	\brief	Mutator for boolean specifying whether to use basic keyboard input to adjust camera position
	*	\param	BOOL a_bState - bool to set member with
	*/

	void Camera::SetSimpleMovement(BOOL a_bState)
	{
		m_bSimpleMovement = a_bState;
	}

	/**
	*	\brief	Mutator for all camera vectors
	*	\param	D3DXVECTOR3& a_rvecPos - new camera position vector
	*	\param	D3DXVECTOR3& a_rvecUp - new camera up vector
	*	\param	D3DXVECTOR3& a_rvecLook - new camera look vector
	*/

	void Camera::SetCamera(D3DXVECTOR3& a_rvecPos, D3DXVECTOR3& a_rvecUp, D3DXVECTOR3& a_rvecLook)
	{
		m_vecPos = a_rvecPos;
		m_vecUp	= a_rvecUp;
		m_vecLook = a_rvecLook;

		UpdateMatrix();
	}

	/**
	*	\brief	Mutator for camera position vector
	*	\param	D3DXVECTOR3& a_vecrPos - new camera position vector
	*/

	void Camera::SetPos(D3DXVECTOR3& a_vecrPos)
	{
		m_vecPos = a_vecrPos;
		UpdateMatrix();
	}

	/**
	*	\brief	Mutator for camera up vector
	*	\param	D3DXVECTOR3& a_vecrUp - new camera up vector
	*/

	void Camera::SetUp(D3DXVECTOR3& a_vecrUp)
	{
		m_vecUp = a_vecrUp;
		UpdateMatrix();
	}

	/**
	*	\brief	Mutator for camera look vector
	*	\param	D3DXVECTOR3& a_vecrLook - new camera look vector
	*/

	void Camera::SetLook(D3DXVECTOR3& a_vecrLook)
	{
		m_vecLook = a_vecrLook;
		UpdateMatrix();
	}

	/**
	*	\brief	Accessor for camera position vector
	*	\return	D3DXVECTOR3 - returns camera position
	*/

	D3DXVECTOR3 Camera::GetPos() const
	{
		return m_vecPos;
	}

	/**
	*	\brief	Accessor for camera up vector
	*	\return	D3DXVECTOR3 - returns camera up vector
	*/

	D3DXVECTOR3 Camera::GetUp() const
	{
		return m_vecUp;
	}

	/**
	*	\brief	Accessor for camera look vector
	*	\return	D3DXVECTOR3 - returns camera look vector
	*/

	D3DXVECTOR3 Camera::GetLook() const
	{
		return m_vecLook;
	}

	/**
	*	\brief	Accessor for view matrix
	*	\return	D3DXMATRIX - returns view matrix
	*/

	D3DXMATRIX Camera::GetViewMatrix() const
	{
		return m_oMatrix;
	}

	/**
	*	\brief	Accessor for object's type
	*	\return	NodeType - returns SGLib::NodeType::CAMERA
	*/

	NodeType Camera::GetType() const
	{
		return CAMERA;
	}

	/**
	*	\brief	Render function called when the scene graph is initially rendering this node
	*	\pre	Device must point to a valid DIRECT3DDEVICE object
	*	\post	Previous view matrix is stored and new view matrix is set
	*/

	void Camera::Render()
	{
		HRESULT	hr;

		// get current view matrix
		V(m_pD3DDevice->GetTransform(D3DTS_VIEW, &m_oMatrixPrevious))

		// set this nodes view matrix
		V(m_pD3DDevice->SetTransform(D3DTS_VIEW, &m_oMatrix))
	}

	/**
	*	\brief	Render function called after the scene graph has called Render() on this object and Render()
	*			on the child object of this object
	*	\pre	Device must point to a valid DIRECT3DDEVICE object
	*	\post	Previous view matrix is set
	*/

	void Camera::PostRender()
	{
		HRESULT hr;

		// set the old view matrix back
		V(m_pD3DDevice->SetTransform(D3DTS_VIEW, &m_oMatrixPrevious))	
	}

	/**
	*	\brief	Update function called on the initial pass of the scene graph before the render call
	*	\param	FLOAT a_fTimeDiff - time difference since last update call
	*	\post	if simple movement has been activated, matrix will be adjusted based on keyboard input
	*/

	void Camera::Update(FLOAT a_fTimeDiff)
	{
		if (m_bSimpleMovement)
		{
			FLOAT fMovementFactor = a_fTimeDiff * 100.0f;

			unsigned char cKeyStatus[256];

			// get each key's status
			if(!GetKeyboardState(cKeyStatus))
				return;

			// evaluate movement directions
			D3DXVECTOR3 vFacing = m_vecLook - m_vecPos;
			D3DXVec3Normalize(&vFacing, &vFacing);
			vFacing *= fMovementFactor;

			D3DXVECTOR3 vStrafe;
			D3DXVec3Cross(&vStrafe, &vFacing, &m_vecUp);
			D3DXVec3Normalize(&vStrafe, &vStrafe);
			vStrafe *= fMovementFactor;

			FLOAT fTwist = 0.05f * fMovementFactor;

			// move forward
			if (cKeyStatus[VK_UP]& 0x80 || cKeyStatus[VK_NUMPAD8]& 0x80)
			{
				m_vecPos += vFacing;
				m_vecLook += vFacing;
			}

			// move backward
			if (cKeyStatus[VK_DOWN]& 0x80 || cKeyStatus[VK_NUMPAD2]& 0x80 || cKeyStatus[VK_NUMPAD5]& 0x80)
			{
				m_vecPos -= vFacing;
				m_vecLook -= vFacing;
			}

			// strafe left
			if (cKeyStatus[VK_NUMPAD7]& 0x80)
			{
				m_vecPos += vStrafe;
				m_vecLook += vStrafe;		
			}

			// strafe right
			if (cKeyStatus[VK_NUMPAD9]& 0x80)
			{
				m_vecPos -= vStrafe;
				m_vecLook -= vStrafe;		
			} 

			// turn right
			if (cKeyStatus[VK_RIGHT]& 0x80 || cKeyStatus[VK_NUMPAD6]& 0x80)
			{
				D3DXVECTOR3 pos = m_vecPos + (cos(-fTwist) * vFacing);
				D3DXVECTOR3 crossFacing;
				D3DXVec3Cross(&crossFacing, &vFacing, &m_vecUp);

				m_vecLook = pos + (sin(-fTwist) * crossFacing);
			}

			// turn left
			if (cKeyStatus[VK_LEFT]& 0x80 || cKeyStatus[VK_NUMPAD4]& 0x80)
			{
				D3DXVECTOR3 pos = m_vecPos + (cos(fTwist) * vFacing);
				D3DXVECTOR3 crossFacing;
				D3DXVec3Cross(&crossFacing, &vFacing, &m_vecUp);

				m_vecLook = pos + (sin(fTwist) * crossFacing);;
			}

			UpdateMatrix();
		}
	}

	/**
	*	\brief	Updates view matrix with current camera vectors
	*/

	void Camera::UpdateMatrix()
	{
		D3DXMatrixLookAtLH(&m_oMatrix, &m_vecPos, &m_vecLook, &m_vecUp); 
	}
}