#include "Geometry.h"

namespace SGLib
{
	/**
	*	\brief	Geometry constructor - automatically loads .x file mesh
	*	\param	LPDIRECT3DDEVICE9 a_pD3DDevice - pointer to direct3ddevice used for directx operations
	*	\param	LPCTSTR a_sFileName - name of .x file holding mesh
	*	\param	DWORD a_dwOptions - memory options for loading mesh - the default is D3DXMESH_SYSTEMMEM
	*/

	Geometry::Geometry(	LPDIRECT3DDEVICE9 a_pD3DDevice, 
						LPCTSTR a_sFileName) : 
							Node(a_pD3DDevice), 
							m_bVisible(TRUE), 
							m_pMesh(NULL), 
							m_pTextures(NULL), 
							m_pMaterials(NULL), 
							m_dwNumMat(0),
							m_sFileName(a_sFileName),
							m_pReference(NULL)
	{
		LoadMesh();
	}

	/**
	*	\brief	Geometry reference constructor
	*	\param	Geometry* a_pReference - pointer to geometry node that this node will reference
	*	\pre	a_pReference != NULL
	*	\note	This constructor is used to allow a geometry node to reference the mesh of another geometry
	*			node.
	*/

	Geometry::Geometry(	Geometry* a_pReference) : 
							Node(a_pReference->GetDevice()), 
							m_bVisible(TRUE), 
							m_pMesh(NULL), 
							m_pTextures(NULL), 
							m_pMaterials(NULL), 
							m_dwNumMat(0),
							m_sFileName(NULL),
							m_pReference(a_pReference)
	{
	}
	

	///**
	//*	\brief	Protected Geometry constructor - only used to initialize Geometry from derived classes
	//*	\param	LPDIRECT3DDEVICE9 a_pD3DDevice - pointer to direct3ddevice used for directx operations
	//*/

	//Geometry::Geometry(LPDIRECT3DDEVICE9 a_pD3DDevice) : Node(a_pD3DDevice)
	//{
	//}

	/**
	*	\brief	Geometry destructor
	*	\note	Child and Sibling nodes are not touched and their destruction is left up to the user
	*/

	Geometry::~Geometry(void)
	{
		if (m_pTextures)
			for (DWORD i = 0; i < m_dwNumMat; ++i)
				if (m_pTextures[i])
					SAFE_RELEASE(m_pTextures[i]);

		SAFE_DELETE(m_pTextures);
		SAFE_DELETE_ARRAY(m_pMaterials);	
		SAFE_RELEASE(m_pMesh);
	}

	/**
	*	\brief	Called when DIRECT3DDEVICE object has been created
	*	\param	LPDIRECT3DDEVICE9 a_pD3DDevice - pointer to new DIRECT3DDEVICE
	*	\note	Recreates the mesh from the .x file
	*/

	void Geometry::OnCreateDevice(LPDIRECT3DDEVICE9 a_pD3DDevice)
	{
		Node::OnCreateDevice(a_pD3DDevice);

		LoadMesh();
	}

	/**
	*	\brief	Called when DIRECT3DDEVICE object has been destroyed
	*	\post	Cleans up memory associated with mesh	
	*/

	void Geometry::OnDestroyDevice()
	{
		Node::OnDestroyDevice();

		if (m_pTextures)
			for (DWORD i = 0; i < m_dwNumMat; ++i)
				if (m_pTextures[i])
					SAFE_RELEASE(m_pTextures[i]);

		SAFE_DELETE(m_pTextures);
		SAFE_DELETE_ARRAY(m_pMaterials);	
		SAFE_RELEASE(m_pMesh);
	}

	/**
	*	\brief	Accessor for object's type
	*	\return	NodeType - returns SGLib::NodeType::GEOMETRY
	*/

	NodeType Geometry::GetType() const
	{
		return GEOMETRY;
	}

	/**
	*	\brief	Mutator for visibility boolean
	*	\param	BOOL a_bVisible - value to update visibility boolean with
	*/

	void Geometry::SetVisible(BOOL a_bVisible)
	{
		m_bVisible = a_bVisible;
	}

	/**
	*	\brief	Render function called when the scene graph is initially rendering this node. Renders 
	*			the mesh associated with this object.
	*	\pre	Device must point to a valid DIRECT3DDEVICE object
	*	\note	If this is a reference node, it will call render on the geometry of the reference node
	*/

	void Geometry::Render()
	{
		// if reference exists use its mesh
		if (m_pReference)
		{
			m_pReference->Geometry::Render();
			return;
		}

		// if mesh is bad pointer or not visible specified, return
		if (!m_pMesh || !m_bVisible)
			return;

		HRESULT hr;
		D3DMATERIAL9 PrevMat;
		LPDIRECT3DBASETEXTURE9 pPrevTex = NULL;

		for (DWORD i = 0; i < m_dwNumMat; ++i)
		{
			// if material exists for this subset
			if (&m_pMaterials[i])
			{
				// store current material and set new
				V(m_pD3DDevice->GetMaterial(&PrevMat))
				V(m_pD3DDevice->SetMaterial(&m_pMaterials[i]))
			}

			// if texture exists for this subset
			if (m_pTextures && m_pTextures[i])
			{
				// store current texture and set new
				V(m_pD3DDevice->GetTexture(0, &pPrevTex))
				V(m_pD3DDevice->SetTexture(0, m_pTextures[i]))
			}

			// draw mesh subset
			V(m_pMesh->DrawSubset(i))

			// set previous material
			if (&m_pMaterials[i])
			{
				V(m_pD3DDevice->SetMaterial(&PrevMat))
			}

			// either set previous texture
			if (pPrevTex)
			{
				V(m_pD3DDevice->SetTexture(0, pPrevTex))
				SAFE_RELEASE(pPrevTex);
			}
			// or set to NULL
			else
			{
				V(m_pD3DDevice->SetTexture(0, NULL))
			}
		}
	}

	/**
	*	\brief	Loads the effect object out of the .fx file
	*	\pre	m_sFileName must point to a valid .fx file
	*/

	void Geometry::LoadMesh()
	{
		if (m_sFileName == NULL)
			return;

		HRESULT hr;

		LPD3DXBUFFER pBufferMat = NULL;
		D3DXMATERIAL* pMaterials = NULL;
		
		// load mesh
		V(D3DXLoadMeshFromX(m_sFileName, D3DXMESH_MANAGED, m_pD3DDevice, NULL,
							&pBufferMat, NULL, &m_dwNumMat, &m_pMesh))

		// convert LPD3DXBUFFER to D3DXMATERIAL*
		pMaterials = (D3DXMATERIAL*)pBufferMat->GetBufferPointer();

		// create appropriate number of materials and textures
		m_pMaterials = new D3DMATERIAL9[m_dwNumMat];
		m_pTextures = new LPDIRECT3DTEXTURE9[m_dwNumMat];

		for (DWORD i = 0; i < m_dwNumMat; ++i)
		{
			// load material properties
			m_pMaterials[i] = pMaterials[i].MatD3D;
			m_pMaterials[i].Ambient = m_pMaterials[i].Diffuse;

			// if texture exists, load it, otherwise set NULL
			if (pMaterials[i].pTextureFilename)
			{
				V(D3DXCreateTextureFromFileA(m_pD3DDevice, pMaterials[i].pTextureFilename, &m_pTextures[i]))
			}
			else if (m_pTextures)
			{
				m_pTextures[i] = NULL;
			}
		}

		// release the data that pBufferMat and pMaterials point to
		SAFE_RELEASE(pBufferMat);
	}
}