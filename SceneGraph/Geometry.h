/**
*	\class		SGLib::Geometry
*	\brief		Node used to hold geometry information
*	\author		QUT - Michael Samiec
*	\date		5/2/07
*	\version	1.0
*
*	This class provides default support for loading simple .x meshes. This node does not support
*	meshes not loaded through the .x interface. All meshes created are loaded into managed memory
*	so they don't have to be re-obtained when the device is reset, only when it is lost.
*/

#ifndef SGLIB_GEOMETRY
#define SGLIB_GEOMETRY

#pragma once

#include "node.h"

namespace SGLib
{
	class Geometry : public virtual Node
	{
	public:
		Geometry(LPDIRECT3DDEVICE9 a_pD3DDevice, LPCTSTR a_sFileName);
		Geometry(Geometry* a_Reference);
		~Geometry(void);

	protected:
		//Geometry(LPDIRECT3DDEVICE9 a_pD3DDevice);

		LPD3DXMESH			m_pMesh;		///< mesh used to hold geometry
		DWORD				m_dwNumMat;		///< number of materials associated with the mesh
		D3DMATERIAL9*		m_pMaterials;	///< array of materials used in the mesh
		LPDIRECT3DTEXTURE9*	m_pTextures;	///< array of textures used in the mesh
		BOOL				m_bVisible;		///< indicates visibility
		LPCTSTR				m_sFileName;	///< filename used to load .x mesh
		Geometry*			m_pReference;	///< points to the geometry reference node

	public:
		void		SetVisible(BOOL a_bVisible);
		NodeType	GetType() const;

		// geometry only requires operations to be carried out in the render function (not the PostRender, Update etc.)
		void		Render();

		// the mesh is created in managed memory so it only needs to concern the device create and destroy functions
		void		OnCreateDevice(LPDIRECT3DDEVICE9 a_pD3DDevice);
		void		OnDestroyDevice();

	protected:
		void		LoadMesh();
	};
}

#endif