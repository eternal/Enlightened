/**
*	\class		SGLib::Camera
*	\brief		Camera node used to set camera settings (view matrix) within the scene graph
*	\author		QUT - Michael Samiec
*	\date		5/2/07
*	\version	1.0
*
*	The camera node specifies a view matrix and is often the first node in the scene graph.
*/

#ifndef SGLIB_CAMERA
#define SGLIB_CAMERA

#pragma once

#include "transform.h"

namespace SGLib
{
	class Camera : public Transform
	{
	public:
		Camera(LPDIRECT3DDEVICE9 a_pD3DDevice);
		Camera(LPDIRECT3DDEVICE9 a_pD3DDevice, D3DXVECTOR3& a_rvecPos, D3DXVECTOR3& a_rvecUp, D3DXVECTOR3& a_rvecLook);
		~Camera(void);

	protected:
		D3DXVECTOR3	m_vecPos;			///< camera's position vector
		D3DXVECTOR3	m_vecUp;			///< camera's up vector
		D3DXVECTOR3	m_vecLook;			///< camera's look vector
		BOOL		m_bSimpleMovement;	///< specifies whether simple camera movement is enabled

	public:
		// mutators
		void	SetCamera	(D3DXVECTOR3& a_rvecPos, D3DXVECTOR3& a_rvecUp, D3DXVECTOR3& a_rvecLook);
		void	SetPos		(D3DXVECTOR3& a_rvecPos);
		void	SetUp		(D3DXVECTOR3& a_rvecUp);
		void	SetLook		(D3DXVECTOR3& a_rvecLook);

		void	SetSimpleMovement	(BOOL a_bState);

		// accessors
		D3DXVECTOR3	GetPos() const;
		D3DXVECTOR3	GetUp() const;
		D3DXVECTOR3	GetLook() const;
		D3DXMATRIX	GetViewMatrix() const;
		NodeType	GetType() const;

		// scene graph related functions
		void	Render		();
		void	PostRender	();
		void	Update		(FLOAT a_fTimeDiff);

	private:
		void	UpdateMatrix();
	};
}

#endif
