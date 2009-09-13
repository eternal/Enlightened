/**
*	\class		SGLib::State
*	\brief		Used to control any fixed function pipeline techniques
*	\author		QUT - Michael Samiec
*	\date		5/2/07
*	\version	1.0
*
*	The State node is used primarily to set render states within the device. This SGLib::Shader node will
*	start to take much of the functionality off this node in programmable pipeline setups.
*/

#ifndef SGLIB_STATE
#define SGLIB_STATE

#pragma once

#include "node.h"
#include <vector>

namespace SGLib
{
	class State : public Node
	{
	public:
		State(LPDIRECT3DDEVICE9 a_pD3DDevice);
		~State(void);

	private:

		// vector of pairs that details light number and light properties
		std::vector<std::pair<DWORD, D3DLIGHT9>> m_vecLights;

		// vector of pairs for render states to be set when this node is rendered
		std::vector<std::pair<D3DRENDERSTATETYPE, DWORD>>	m_vecRenderStates;

		// vector of previous render states that need to be reset when this node is finished
		std::vector<std::pair<D3DRENDERSTATETYPE, DWORD>> m_vecPreviousRenderStates;

	public:
		void		AddRenderState(D3DRENDERSTATETYPE a_enType, DWORD a_dwValue);
		void		AddLight(DWORD a_dwPos, D3DLIGHT9& a_rLight);

		NodeType	GetType		() const;
		void		Render		();
		void		PostRender	();
	};
}

#endif
