/**
*	\class		SGLib::SGRenderer
*	\brief		Used to navigate a node heirarchy and call the appropriate render and update functions
*	\author		QUT - Michael Samiec
*	\date		5/2/07
*	\version	1.1
*
*	This is a base version which performs basic functionality on the graph traversal. To perform more complicated
*	techniques this class will have to be extended to perform the 'behind-the-scene' connections and traversals.
*
*	Update: 22/5/07 - The functionality to define the clear options of the back buffer has been included
*/

#ifndef SGLIB_SGRENDERER
#define SGLIB_SGRENDERER

#pragma once

#include "Node.h"
#include "Shader.h"
#include "State.h"
#include "Articulated.h"

#include <stack>

namespace SGLib
{
	class SGRenderer
	{
	public:
		SGRenderer();
		~SGRenderer();

	protected:
		std::stack<Shader*>	m_stpShaders;	///< shaders used in the graph
		std::stack<State*>	m_stpStates;	///< states used within the graph

		// variables associated with clearing the back buffer
		DWORD				m_dwOptions;	///< options used to clear the buffer
		D3DCOLOR			m_colourClear;	///< colour to clear the back buffer to
		FLOAT				m_fZClear;		///< depth to clear the z buffer to
		DWORD				m_dwStencil;	///< value to set stencil plane to

	public:
		virtual void	Render(Node* a_pNodeBase);
		virtual void	Update(Node* a_pNodeBase, FLOAT a_fTimeDiff);
		void			SetClearColour(D3DCOLOR a_colour);
		void			SetClearOptions(DWORD a_dwOptions, FLOAT a_fZClear = 1.0f, DWORD a_dwStencil = 0);

	protected:
		virtual void	RenderNode(Node* a_pNode);
		virtual void	UpdateNode(Node* a_pNode, FLOAT a_fTimeDiff);
	};
}

#endif
