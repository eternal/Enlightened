/**
*	\class		SGLib::Node
*	\brief		Base class used for all links within the scene graph
*	\author		QUT - Michael Samiec
*	\date		5/2/07
*	\version	1.0
*	\note		This is an abstract class
*
*	This class provides all the basic functionality required by all nodes in the scene graph. It is 
*	based in the SGLib namespace. This scene graph has also been based around the DXUT framework and
*	although it is no necessary for it to function, it relates many of its function calls to it. 
*	For example -	Render();
*					Update();
*					OnCreateDevice();
*					OnResetDevice();
*					OnLostDevice();
*					OnDestroyDevice();
*
*	The functions PostRender(); and PostUpdate(); are added functions that allow the scene graph
*	to flow properly. They undo any changes (usually to the device) made by their respective 
*	predeseccors.
*/

#ifndef SGLIB_NODE
#define SGLIB_NODE

#pragma once

#include "dxstdafx.h"

#include <d3d9.h>
#include <d3dx9.h>
#include <vector>
#include <stack>

namespace SGLib
{
	// defines different basic types used within the scene graph
	enum NodeType
	{
		ARTICULATED,
		CAMERA,
		GEOMETRY,
		PARTICLESYS,
		PROJECTION,
		SHADER,
		STATE,
		TRANSFORM
	};

	class Node
	{
	public:
		Node(LPDIRECT3DDEVICE9 a_pD3DDevice);
		virtual ~Node(void);

	protected:
		LPCTSTR					m_sDescription;	///< user defined string identifying node
		Node*					m_pChild;		///< pointer to child node
		Node*					m_pSibling;		///< pointer to sibling node
		LPDIRECT3DDEVICE9		m_pD3DDevice;	///< pointer to direct3ddevice used for directx operations

	public:
		// mutators
		Node*	SetChild		(Node* a_pChild);
		Node*	SetSibling		(Node* a_pSibling);
		void	InsertChild		(Node* a_pChild);
		void	InsertSibling	(Node* a_pSibling);
		void	InsertChildHierarchy	(Node* a_pChild);
		void	InsertSiblingHierarchy	(Node* a_pSibling);
		Node*	RemoveChild		();
		Node*	RemoveSibling	();
		void	SetDescription	(LPCTSTR a_sDescription);
		void	SetDevice		(LPDIRECT3DDEVICE9 a_pD3DDevice);

		// accessors
		Node*				GetNode		(LPCTSTR a_sDescription);
		Node*				GetSibling	() const;
		Node*				GetChild	() const;
		LPCTSTR				GetDescription	() const;
		LPDIRECT3DDEVICE9	GetDevice	() const;
		virtual NodeType	GetType		() const = 0;
		std::vector<Node*>	GetNodesOfType(NodeType a_enType);

		// functions that deal with situations regarding changes in a device's state
		virtual void		OnCreateDevice(LPDIRECT3DDEVICE9 a_pD3DDevice);	// used to create any D3DPOOL_MANAGED resources
		virtual void		OnResetDevice(LPDIRECT3DDEVICE9 a_pD3DDevice);	// used to create any D3DPOOL_DEFAULT resources
		virtual void		OnLostDevice();			// cleans up memory allocated in OnResetDevice()
		virtual void		OnDestroyDevice();		// cleans up memory allocated in OnCreateDevice()

		// primary scene graph related functions

		// implemented here with no functionality to allow derived classes to skip definition 
		// as not all derived classes need to use all four functions
		virtual void		Render		();
		virtual void		PostRender	();
		virtual void		Update		(FLOAT a_fTimeDiff);
		virtual void		PostUpdate	();

		/**
		*	\brief	Template function that recursively searches this node's hierarchy and returns a vector of
		*			Type* pointing to all nodes that are of type Type (including this). 
		*	\param	None
		*	\return	vector< Type* > containing pointers to all nodes of Type in this node's hierarchy
		*/
		template<class Type>
		std::vector<Type*>	GetNodesOfType()
		{
			Node* pTempNode = NULL;
			Type* pDerivedNode = NULL;
			std::stack<Node*> staRecursion;		// used for recursively loop
			std::vector<Type*> vecDerivedNodes;	// return structure

			staRecursion.push(this);

			while (!staRecursion.empty())
			{
				// store and remove top element from stack
				pTempNode = staRecursion.top();
				staRecursion.pop();

				if (pTempNode)
				{
					pDerivedNode = dynamic_cast<Type*>(pTempNode);

					if (pDerivedNode)
						vecDerivedNodes.push_back(pDerivedNode);
				
					if (pTempNode->GetChild())
						staRecursion.push(pTempNode->GetChild());

					if (pTempNode->GetSibling())
						staRecursion.push(pTempNode->GetSibling());
				}
			}

			return vecDerivedNodes;			
		}
	};
}
#endif