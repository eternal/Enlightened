#include "State.h"

using std::pair;
using std::vector;

namespace SGLib
{
	/**
	*	\brief	State constructor
	*	\param	LPDIRECT3DDEVICE9 a_pD3DDevice - pointer to direct3ddevice used for directx operations
	*/

	State::State(LPDIRECT3DDEVICE9 a_pD3DDevice) :	
					Node(a_pD3DDevice)
	{
	}

	/**
	*	\brief	State destructor
	*	\note	Child and Sibling nodes are not touched and their destruction is left up to the user
	*/

	State::~State(void)
	{
	}

	/**
	*	\brief	Pushes this render state type and value onto the render states vector
	*	\param	D3DRENDERSTATETYPE a_enumType - type of render state object to be set
	*	\param	DWORD a_dwValue - the value desired for the specific render state type
	*	\note	these variables are used in the render call - device->SetRenderState(a_enType, a_dwValue)
	*/

	void State::AddRenderState(D3DRENDERSTATETYPE a_enType, DWORD a_dwValue)
	{
		m_vecRenderStates.push_back(pair<D3DRENDERSTATETYPE, DWORD>(a_enType, a_dwValue));
	}

	/**
	*	\brief	Adds light to the vector of lights to be set when rendered
	*	\param	DWORD a_dwPos - light position
	*	\param	D3DLIGHT9& a_rLight - reference to light properties
	*	\note	all lights are considered turned on when rendering
	*/

	void State::AddLight(DWORD a_dwPos, D3DLIGHT9& a_rLight)
	{
		m_vecLights.push_back(pair<DWORD, D3DLIGHT9>(a_dwPos, a_rLight));
	}

	/**
	*	\brief	Accessor for object's type
	*	\return	NodeType - returns SGLib::NodeType::STATE
	*/

	NodeType State::GetType() const
	{
		return STATE;
	}

	/**
	*	\brief	Render function called when the scene graph is initially rendering this node
	*	\pre	Device must point to a valid DIRECT3DDEVICE object
	*	\post	All render states are set, previous render states stored, and lights enabled
	*/

	void State::Render()
	{
		vector<pair<D3DRENDERSTATETYPE, DWORD>>::iterator iterRS;
		vector<pair<DWORD, D3DLIGHT9>>::iterator iterLight;
		DWORD dwPreSetValue = 0;
		HRESULT hr;

		for (iterRS = m_vecRenderStates.begin(); iterRS != m_vecRenderStates.end(); ++iterRS)
		{
			// get render state for desired change
			V(m_pD3DDevice->GetRenderState(iterRS->first, &dwPreSetValue))

			// if it is different, store old state, and change it
			if (iterRS->second != dwPreSetValue)
			{
				V(m_pD3DDevice->SetRenderState(iterRS->first, iterRS->second))

				m_vecPreviousRenderStates.push_back(pair<D3DRENDERSTATETYPE, DWORD>(iterRS->first, dwPreSetValue));			
			}
		}

		// set and enable all lights in vector
		for (iterLight = m_vecLights.begin(); iterLight != m_vecLights.end(); ++iterLight)
		{
			V(m_pD3DDevice->SetLight(iterLight->first, &iterLight->second))
			V(m_pD3DDevice->LightEnable(iterLight->first, TRUE))
		}
	}

	/**
	*	\brief	Called after the scene graph has called Render() on this node and on this node's child but
	*			before it is called on this node's sibling to undo changes made by Render()
	*	\pre	Device must point to a valid DIRECT3DDEVICE object
	*	\post	Previous render states are restored
	*/

	void State::PostRender()
	{
		vector<pair<D3DRENDERSTATETYPE, DWORD>>::iterator iterRS;
		vector<pair<DWORD, D3DLIGHT9>>::iterator iterLight;
		HRESULT hr;

		// set all previous render states
		for (iterRS = m_vecPreviousRenderStates.begin(); iterRS != m_vecPreviousRenderStates.end(); ++iterRS)
		{
			V(m_pD3DDevice->SetRenderState(iterRS->first, iterRS->second))
		}

		m_vecPreviousRenderStates.clear();

		// disable all lights
		for (iterLight = m_vecLights.begin(); iterLight != m_vecLights.end(); ++iterLight)
		{
			V(m_pD3DDevice->LightEnable(iterLight->first, FALSE))
		}
	}
}