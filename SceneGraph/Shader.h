/**
*	\class		SGLib::Shader
*	\brief		Used to control an effect within the scene graph
*	\author		QUT - Michael Samiec
*	\date		5/2/07
*	\version	1.0
*
*	The Shader node uses the D3DXEFFECT object to allow shaders into the scene graph. Every geometry node 
*	below this node in the scene graph with be rendered with this effect as long as no other shader is subsequently 
*	set. The rendering for each geometry mesh is performed using the RenderGeometry function. In this function the 
*	effect's variables are set up according to the specific geometry object (eg. world matrix, textures etc.) and 
*	then calls the geometry objects render function.
*/

#ifndef SGLIB_SHADER
#define	SGLIB_SHADER

#pragma once

#include "node.h"
#include "Geometry.h"

namespace SGLib
{
	class Shader : public Node
	{
	public:
		Shader(LPDIRECT3DDEVICE9 a_pD3DDevice, LPCTSTR a_sFileName);
		Shader(Shader* a_pReference);
		~Shader(void);

		virtual void RenderGeometry	(Geometry* a_pGeometryNode);

		void		OnCreateDevice(LPDIRECT3DDEVICE9 a_pD3DDevice);
		void		OnResetDevice(LPDIRECT3DDEVICE9 a_pD3DDevice);
		void		OnLostDevice();
		void		OnDestroyDevice();
		NodeType	GetType		() const;

	protected:
		LPCTSTR			m_sFileName;	///< filename of effect
		LPD3DXEFFECT	m_pEffect;		///< direct 3d effect object that can encapsulate many techniques
		Shader*			m_pReference;	///< points to shader object that contins effect used by this node

		void		CreateEffect();
	};
}

#endif