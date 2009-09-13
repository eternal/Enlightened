/**
*	\class		SGLib::Transform
*	\brief		Transform encapsulates the basic matrix operations used in the scene graph
*	\author		QUT - Michael Samiec
*	\date		5/2/07
*	\version	1.0
*
*	The Transform node provides the basis for simple world transforms as well as the camera and projection
*	matrices.
*/

#ifndef SGLIB_TRANSFORM
#define SGLIB_TRANSFORM

#pragma once

#include "node.h"

namespace SGLib
{
	class Transform : public virtual Node
	{
	public:
		Transform	(LPDIRECT3DDEVICE9 a_pD3DDevice, D3DXMATRIX& a_rMatrixTrans);
		~Transform	(void);

	protected:
		Transform	(LPDIRECT3DDEVICE9 a_pD3DDevice);

	protected:
		D3DXMATRIX	m_oMatrixTrans;		///< transform to apply
		D3DXMATRIX	m_oMatrix;			///< combined matrix state
		D3DXMATRIX	m_oMatrixPrevious;	///< previous matrix state

	public:
		// matrix operation functions
		void		SetMatrix(D3DXMATRIX& a_rMatrixTrans);
		void		MultMatrix(D3DXMATRIX& a_rMatrixTrans);
		D3DXMATRIX	GetMatrix();

		// scene graph related functions
		virtual NodeType	GetType() const;
		virtual void		Render();
		virtual void		PostRender();
		virtual void		Update(FLOAT a_fTimeDiff);
		virtual void		PostUpdate();
	};
}

#endif