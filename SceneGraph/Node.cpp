#include "Node.h"

namespace SGLib
{
	/**
	*	\brief	Node constructor
	*	\param	LPDIRECT3DDEVICE9 a_pD3DDevice - pointer to direct3ddevice used for directx operations
	*/

	Node::Node(LPDIRECT3DDEVICE9 a_pD3DDevice) :	m_pD3DDevice(a_pD3DDevice), 
													m_pSibling(NULL), 
													m_pChild(NULL), 
													m_sDescription(NULL)
	{
	}

	/**
	*	\brief	Node destructor
	*	\note	Child and Sibling nodes are not touched and their destruction is left up to the user
	*/

	Node::~Node(void)
	{
	}

	/**
	*	\brief	Sets the child pointer of this object
	*	\param	Node* a_pChild - pointer to new child node
	*	\return	Node* - previous child or NULL if none exists 
	*/

	Node* Node::SetChild(Node* a_pChild)
	{
		// output string to console if potential loop detected within scene graph
		if (a_pChild == this)
			OutputDebugString(L"Warning: Loop in node hierarchy detected -> infinite recursion");

		Node* pTempNode = m_pChild;

		m_pChild = a_pChild;

		return pTempNode;
	}

	/**
	*	\brief	Sets the sibling pointer of this object
	*	\param	Node* a_pSibling - pointer to new sibling node
	*	\return	Node* - previous sibling or NULL if none exists 
	*/

	Node* Node::SetSibling(Node* a_pSibling)
	{
		// output string to console if potential loop detected within scene graph
		if (a_pSibling == this)
			OutputDebugString(L"Warning: Loop in node hierarchy detected -> infinite recursion");

		Node* pTempNode = m_pSibling;

		m_pSibling = a_pSibling;

		return pTempNode;
	}

	/**
	*	\brief	Sets new child pointer and if current child exists, set it as new child's child
	*	\param	Node* a_pChild - pointer to new child node
	*/

	void Node::InsertChild(Node* a_pChild)
	{
		if (m_pChild == a_pChild)
			return;

		Node* pTempNode = SetChild(a_pChild);

		// if previous child exists, set it as new child's child
		if (pTempNode)
			a_pChild->SetChild(pTempNode);
	}

	/**
	*	\brief	Sets new sibling pointer and if current sibling exists, set it as new sibling's sibling
	*	\param	Node* a_pSibling - pointer to new sibling node
	*/

	void Node::InsertSibling(Node* a_pSibling)
	{
		if (m_pSibling == a_pSibling)
			return;

		Node* pTempNode = SetSibling(a_pSibling);

		// if previous sibling exists, set is as new sibling's sibling
		if (pTempNode)
			a_pSibling->SetSibling(pTempNode);
	}

	/**
	*	\brief	Sets new child pointer and if current child exists, set it as child of first avaliable child
	*			member in new child's hierarchy
	*	\param	Node* a_pChild - pointer to new child node
	*/

	void Node::InsertChildHierarchy(Node* a_pChild)
	{
		if (m_pChild == a_pChild || !a_pChild)
			return;

		// if no current child, just set child and return
		if (!m_pChild)
		{
			m_pChild = a_pChild;
			return;
		}

		Node* pTempNode = a_pChild;
		Node* pTempNodeParent = NULL;

		// while node exists, get child node
		while (pTempNode)
		{
			pTempNodeParent = pTempNode;
			pTempNode = pTempNode->GetChild();
		}

		// set current child as derived child's child
		pTempNodeParent->SetChild(m_pChild);

		// set new child
		m_pChild = a_pChild;
	}

	/**
	*	\brief	Sets new sibling pointer and if current sibling exists, set it as sibling of first avaliable sibling
	*			member in new sibling's hierarchy
	*	\param	Node* a_pSibling - pointer to new sibling node
	*/

	void Node::InsertSiblingHierarchy(Node* a_pSibling)
	{
		if (m_pSibling == a_pSibling || !a_pSibling)
			return;

		// if no current sibling, just set sibling and return
		if (!m_pSibling)
		{
			m_pSibling = a_pSibling;
			return;
		}

		Node* pTempNode = a_pSibling;
		Node* pTempNodeSibling = NULL;

		// while node exists, get sibling node
		while (a_pSibling)
		{
			pTempNodeSibling = pTempNode;
			pTempNode = pTempNode->GetSibling();
		}

		// set current sibling as derived sibling's sibling
		pTempNodeSibling->SetSibling(m_pSibling);

		// set new sibling
		m_pSibling = a_pSibling;
	}

	/**
	*	\brief	Removes and returns current child and if it had a child, set it as new child
	*	\return	Node* - pointer to old child node
	*	\note	Returned node will maintain hierarchy it had previously, its just removed from scene graph
	*/

	Node* Node::RemoveChild()
	{
		if (m_pChild == NULL)
			return NULL;

		Node* pNodeChildChild = m_pChild->GetChild();

		// temp pointer to current child so it can be returned
		Node* pTempChild = m_pChild;
		
		// set new child
		m_pChild = pNodeChildChild;
		
		return pTempChild;
	}

	/**
	*	\brief	Removes and returns current sibling and if it had a sibling, set it as new sibling
	*	\return	Node* - pointer to old sibling node
	*	\note	Returned node will maintain hierarchy it had previously, its just removed from scene graph
	*/

	Node* Node::RemoveSibling()
	{
		if (m_pSibling == NULL)
			return NULL;

		Node* pNodeSiblingSibling = m_pSibling->GetSibling();

		// temp pointer to current sibling so it can be returned
		Node* pTempSibling = m_pSibling;

		// set new sibling
		m_pSibling = pNodeSiblingSibling;

		return pTempSibling;
	}

	/**
	*	\brief	Recursively searches this node's hierarchy and returns a vector of Node* pointing 
				to all nodes that are of type a_enType (including this). 
	*	\param	NodeType a_enType - enum specifying type of node 
	*	\return	vector< Node* > containing pointers to all nodes who's type matches a_enType
	*	\note	This function should only be used for retrieving nodes of basic types, if self derived 
	*			objects are required, use the template function of the same number
	*/

	std::vector<Node*> Node::GetNodesOfType(NodeType a_enType)
	{
		Node* pTempNode = NULL;
		std::stack<Node*> staRecursion;	// used for recursive loop
		std::vector<Node*> vecNodes;	// return structure

		staRecursion.push(this);

		while (!staRecursion.empty())
		{
			// store and remove top element from stack
			pTempNode = staRecursion.top();
			staRecursion.pop();

			if (pTempNode)
			{
				if (pTempNode->GetType() == a_enType)
					vecNodes.push_back(pTempNode);
			
				if (pTempNode->GetChild())
					staRecursion.push(pTempNode->GetChild());

				if (pTempNode->GetSibling())
					staRecursion.push(pTempNode->GetSibling());
			}
		}

		return vecNodes;
	}

	/**
	*	\brief	Recursively searches this node's hierarchy and returns pointer to node who's description matches
	*			a_sDescription. If no match is found NULL is returned.
	*	\param	LPCTSTR a_sDescription - description of node to search for
	*	\return	Node* - pointer to node that matches the a_sDescription
	*/

	Node* Node::GetNode(LPCTSTR a_sDescription)
	{
		if (m_sDescription == a_sDescription)
			return this;

		Node* pTempNode = NULL;

		// search child's structure
		if (m_pChild)
		{
			pTempNode = m_pChild->GetNode(a_sDescription);
			if (pTempNode)
				return pTempNode;
		}

		// search sibling's structure
		if (m_pSibling)
		{
			pTempNode = m_pSibling->GetNode(a_sDescription);
			if (pTempNode)
				return pTempNode;
		}

		// if nothing was found, return NULL to indicate failure
		return NULL;
	}

	/**
	*	\brief	Mutator for description of node
	*	\param	LPCTSTR a_sDescription - description to set for node
	*/

	void Node::SetDescription(LPCTSTR a_sDescription)
	{
		m_sDescription = a_sDescription;
	}

	/**
	*	\brief	Mutator for pointer to LPDIRECT3DDEVICE9 pointer
	*	\param	LPDIRECT3DDEVICE9 a_pD3DDevice - pointer to set member with
	*/

	void Node::SetDevice(LPDIRECT3DDEVICE9 a_pD3DDevice)
	{
		m_pD3DDevice = a_pD3DDevice;
	}

	/**
	*	\brief	Accessor for child pointer
	*	\return	Node* - if sibling exists, returns pointer to it, otherwise returns NULL
	*/

	Node* Node::GetChild() const
	{
		return m_pChild;
	}

	/**
	*	\brief	Accessor for sibling pointer
	*	\return	Node* - if sibling exists, returns pointer to it, otherwise returns NULL
	*/

	Node* Node::GetSibling() const
	{
		return m_pSibling;
	}

	/**
	*	\brief	Accessor for node's description
	*	\return	LPCTSTR - node's description
	*/

	LPCTSTR Node::GetDescription() const
	{
		return m_sDescription;
	}

	/**
	*	\brief	Accessor for node's LPDIRECT3DDEVICE9 pointer
	*	\return	LPDIRECT3DDEVICE9 - pointer to DIRECT3DDEVICE used in this node's directx operations
	*/

	LPDIRECT3DDEVICE9 Node::GetDevice() const
	{
		return m_pD3DDevice;
	}

	/**
	*	\brief	Called when the DIRECT3DDEVICE has been created to allocated D3DPOOL_MANAGED resources
	*	\param	LPDIRECT3DDEVICE9 - pointer to new DIRECT3DDEVICE
	*	\note	This is a recursive function and is called on both child and sibling nodes if they exist
	*/

	void Node::OnCreateDevice(LPDIRECT3DDEVICE9 a_pD3DDevice)
	{
		m_pD3DDevice = a_pD3DDevice;

		if (m_pChild)
			m_pChild->OnCreateDevice(a_pD3DDevice);
		if (m_pSibling)
			m_pSibling->OnCreateDevice(a_pD3DDevice);
	}

	/**
	*	\brief	Called when the DIRECT3DDEVICE has been reset to allocated D3DPOOL_DEFAULT resources
	*	\param	LPDIRECT3DDEVICE9 - pointer to new DIRECT3DDEVICE
	*	\note	This is a recursive function and is called on both child and sibling nodes if they exist
	*/

	void Node::OnResetDevice(LPDIRECT3DDEVICE9 a_pD3DDevice)
	{
		m_pD3DDevice = a_pD3DDevice;

		if (m_pChild)
			m_pChild->OnResetDevice(a_pD3DDevice);
		if (m_pSibling)
			m_pSibling->OnResetDevice(a_pD3DDevice);
	}

	/**
	*	\brief	Called when the DIRECT3DDEVICE has been lost - cleans up memort set in OnResetDevice()
	*	\note	This is a recursive function and is called on both child and sibling nodes if they exist
	*/

	void Node::OnLostDevice()
	{
		if (m_pChild)
			m_pChild->OnLostDevice();
		if (m_pSibling)
			m_pSibling->OnLostDevice();
	}

	/**
	*	\brief	Called when the DIRECT3DDEVICE has been destroyed - cleans up memort set in OnCreateDevice()
	*	\note	This is a recursive function and is called on both child and sibling nodes if they exist
	*/

	void Node::OnDestroyDevice()
	{
		if (m_pChild)
			m_pChild->OnDestroyDevice();
		if (m_pSibling)
			m_pSibling->OnDestroyDevice();
	}

	/**
	*	\brief	Renders this object
	*/

	void Node::Render()
	{
	}

	/**
	*	\brief	Cleans up anything set in the Render() function prior to the rendering of this node's sibling
	*/

	void Node::PostRender()
	{
	}

	/**
	*	\brief	Updates object based on elapsed time. Updates scene graph in preperation of render call
	*	\param	FLOAT a_fTimeDiff - elapsed time since last update call
	*/

	void Node::Update(FLOAT a_fTimeDiff)
	{
	}

	/**
	*	\brief	Cleans up anything set in the Update() function prior to the updating of this node's sibling
	*/

	void Node::PostUpdate()
	{
	}
}