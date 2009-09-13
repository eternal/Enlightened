/**
*	\class		SGLib::Projection
*	\brief		Controls projection matrix
*	\author		QUT - Michael Samiec
*	\date		5/2/07
*	\version	1.0
*
*	The projection node specifies a projection matrix is recommended to only occur once within the scene graph
*	and reside high up the hierarchy.
*/

#ifndef SGLIB_PROJECTION
#define SGLIB_PROJECTION

#pragma once

#include "transform.h"

namespace SGLib
{
	class Projection : public Transform
	{
	public:
		Projection	(LPDIRECT3DDEVICE9 a_pD3DDevice, D3DXMATRIX& a_oMatrixProj);
		Projection	(LPDIRECT3DDEVICE9 a_pD3DDevice, FLOAT a_fFov, FLOAT a_fAspect, FLOAT a_fNear, FLOAT a_fFar);	// constructor for D3DXMatrixPerspectiveFovRH call
		~Projection	(void);

	public:
		void	SetProjMatrix	(D3DXMATRIX& a_rMatrixProj);
		void	ResetMatrix		(FLOAT a_fFov, FLOAT a_fAspect, FLOAT a_fNear, FLOAT a_fFar);

		// scene graph
		void	Render			();
		void	PostRender		();

		// only declared to ensure the transform declerations are called
		void	Update			(FLOAT a_fTimeDiff);
		void	PostUpdate		();

		NodeType	GetType			() const;
	};
}

#endif