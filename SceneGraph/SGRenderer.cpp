#include "SGRenderer.h"

using std::stack;

namespace SGLib
{
	/**
	*	\brief	SGRenderer constructor
	*	\note	Sets the clear colour to black and the clear options to D3DCLEAR_TARGET and 
	*			D3DCLEAR_ZBUFFER. Also sets the z buffer clear value to 1.0f and the stencil
	*			buffer's to 0.
	*/

	SGRenderer::SGRenderer() :	m_dwOptions(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER),
								m_colourClear(D3DCOLOR_XRGB(0, 0, 0)),
								m_fZClear(1.0f),
								m_dwStencil(0)
	{
	}

	/**
	*	\brief	SGRenderer destructor
	*/

	SGRenderer::~SGRenderer()
	{
	}

	/**
	*	\brief	Mutator for back buffer colour
	*	\param	D3DCOLOR a_colour - value used to set back buffer colour
	*/

	void SGRenderer::SetClearColour(D3DCOLOR a_colour)
	{
		m_colourClear = a_colour;
	}

	/**
	*	\brief	Mutator for back buffer clear options
	*	\param	DWORD a_dwOptions - options used to clear the back buffer with
	*	\param	FLOAT a_fZClear - depth buffer clear value - defaults to 1.0f
	*	\param	DWORD a_dwStencil - stencil buffer clear value - defaults to 0
	*/

	void SGRenderer::SetClearOptions(DWORD a_dwOptions, FLOAT a_fZClear, DWORD a_dwStencil)
	{
		m_dwOptions = a_dwOptions;
		m_fZClear = a_fZClear;
		m_dwStencil = a_dwStencil;
	}

	/**
	*	\brief	Public entry point for rendering of a_pNodeBase and its hierarchy
	*	\param	Node* a_pNodeBase - base node in the node structure being rendered
	*/

	void SGRenderer::Render(Node* a_pNodeBase)
	{
		HRESULT hr;

		if (!a_pNodeBase)
			return;
		
		// clear back buffer
		V(a_pNodeBase->GetDevice()->Clear(0, NULL, m_dwOptions,
									m_colourClear, m_fZClear, m_dwStencil))
		
		V(a_pNodeBase->GetDevice()->BeginScene())

		// call general render function for base node
		RenderNode(a_pNodeBase);

		V(a_pNodeBase->GetDevice()->EndScene())
	}

	/**
	*	\brief	Public entry point for updating of a_pNodeBase and its hierarchy prior to rendering
	*	\param	Node* a_pNodeBase - base node in the node structure being updated
	*	\param	FLOAT a_fTimeDiff - time difference between update calls
	*/

	void SGRenderer::Update(Node* a_pNodeBase, FLOAT a_fTimeDiff)
	{
		if (!a_pNodeBase)
			return;

		// call general update function for base node
		UpdateNode(a_pNodeBase, a_fTimeDiff);
	}

	/**
	*	\brief	Renders a_pNode in relation to its node type and calls render on its child and sibling
	*	\param	Node* a_pNode - node being rendered
	*	\pre	a_pNode != NULL
	*	\note	This function calls both Render() and PostRender() on a_pNode
	*/

	void SGRenderer::RenderNode(Node* a_pNode)
	{
		// obtain child and sibling nodes (if they exist)
		Node* pNodeChild = a_pNode->GetChild();
		Node* pNodeSibling = a_pNode->GetSibling();

		// obtain type of node to determine appropriate action
		NodeType enCurrentNode = a_pNode->GetType();

		switch (enCurrentNode)
		{
		// if a piece of geometry is being rendered
		case GEOMETRY:
		case ARTICULATED:

			// if a shader has been set
			if (!m_stpShaders.empty())
			{
				// if its an articulated node, apply transform before rendering geometry
				if (enCurrentNode == ARTICULATED)
					dynamic_cast<Articulated*>(a_pNode)->Transform::Render();

				// call shader node to render object
				m_stpShaders.top()->RenderGeometry(dynamic_cast<Geometry*>(a_pNode));
			}
			else
			{
				// otherwise perform simple fixed pipeline rendering
				a_pNode->Render();
			}

			// if child exists, render it
			if (pNodeChild)
				RenderNode(pNodeChild);

			// call postrender on node to clean up render call
			a_pNode->PostRender();

			// if sibling exists, render it
			if (pNodeSibling)
				RenderNode(pNodeSibling);

			break;

		default:
			// if node is a shader
			if (enCurrentNode == SHADER)
			{
				// add shader node to stack
				m_stpShaders.push(dynamic_cast<Shader*>(a_pNode));
			}
			// if node is a state
			else if (enCurrentNode == STATE)
			{
				// add state node to stack
				m_stpStates.push(dynamic_cast<State*>(a_pNode));
			}

			a_pNode->Render();

			// if child exists, render it
			if (pNodeChild)
				RenderNode(pNodeChild);

			// perform post render operations on this node
			a_pNode->PostRender();

			// if sibling exists, render it
			if (pNodeSibling)
				RenderNode(pNodeSibling);

			if (enCurrentNode == SHADER)
			{
				// remove shader node
				m_stpShaders.pop();
			}
			else if (enCurrentNode == STATE)
			{
				// remove state node
				m_stpStates.pop();
			}

			break;
		}
	}

	/**
	*	\brief	Updates a_pNode and calls this function on its child and sibling if they exist
	*	\param	Node* a_pNode - node being updated
	*	\note	This function calls both Update() and PostUpdate() on a_pNode
	*/

	void SGRenderer::UpdateNode(Node* a_pNode, FLOAT a_fTimeDiff)
	{
		Node* pNodeChild = NULL;
		Node* pNodeSibling = NULL;

		// update this node
		a_pNode->Update(a_fTimeDiff);

		// if child node exists, update it
		pNodeChild = a_pNode->GetChild();
		if (pNodeChild)
			UpdateNode(pNodeChild, a_fTimeDiff);

		// perform post update operations on this node
		a_pNode->PostUpdate();

		// if sibling node exists, update it
		pNodeSibling = a_pNode->GetSibling();
		if (pNodeSibling)
			UpdateNode(pNodeSibling, a_fTimeDiff);
	}
}