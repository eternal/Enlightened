#include "ParticleSystem.h"

using std::vector;

namespace SGLib
{
	/**
	*	\brief	ParticleSystem constructor
	*	\param	LPDIRECT3DDEVICE9 a_pD3DDevice - pointer to direct3ddevice used for directx operations
	*	\param	LPCTSTR a_sFileName - effect file name
	*	\param	std::string a_fTechName - technique to use within effect
	*	\param	std::string a_fTexName - name of texture object
	*	\param	D3DXVECTOR3& a_rvecAccel - acceleration set for each particle
	*	\param	INT a_nMaxParticles - max number of particles permitted at any one time
	*	\param	FLOAT a_fParticleTime - time delay on particle creation
	*	\post	Creates and does a basic initialization of the particles within the system
	*	\note	This method initalizes the particle vector, creates the texture, creates the
	*			vertex decleration associated with the Particle struct and creates the vertex
	*			buffer.
	*/

	ParticleSystem::ParticleSystem(	LPDIRECT3DDEVICE9 a_pD3DDevice, 
									LPCTSTR a_sFileName, 
									LPCSTR a_sTechName, 
									LPCTSTR a_sTexName,
									D3DXVECTOR3& a_rvecAccel, 
									INT a_nMaxParticles, 
									FLOAT a_fParticleTime) :
										Shader(a_pD3DDevice, a_sFileName),
										m_pTexture(NULL),
										m_sTechName(a_sTechName),
										m_sTexName(a_sTexName),
										m_pVB(NULL),
										m_pParticleDecl(NULL),
										m_vecAccel(a_rvecAccel),
										m_nMaxParticles(a_nMaxParticles),
										m_fParticleTime(a_fParticleTime),
										m_fTime(0.0f)
	{
		m_vecParticles.resize(m_nMaxParticles);
		m_vecAliveParticles.reserve(m_nMaxParticles);
		m_vecDeadParticles.reserve(m_nMaxParticles);

		for (int i = 0; i < m_nMaxParticles; ++i)
		{
			m_vecParticles[i].fLifeTime = -1.0f;
			m_vecParticles[i].fInitTime = 0.0f;
		}

		// create texture
		D3DXCreateTextureFromFile(m_pD3DDevice, m_sTexName, &m_pTexture);

		// define particle vertex structure
		D3DVERTEXELEMENT9 VertexArray[] =
		{
			{0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},	// position
			{0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},	// velocity
			{0, 24, D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1},	// size
			{0, 28, D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 2},	// init time
			{0, 32, D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 3},	// life time
			{0, 36, D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 4},	// mass
			{0, 40, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},	// colour
			D3DDECL_END()
		};

		m_pD3DDevice->CreateVertexDeclaration(VertexArray, &m_pParticleDecl);

		// create vertex buffer and declaration
		m_pD3DDevice->CreateVertexBuffer(	m_nMaxParticles*sizeof(SGLib::Particle), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY | D3DUSAGE_POINTS,
											0, D3DPOOL_DEFAULT, &m_pVB, 0);
	}

	/**
	*	\brief	ParticleSystem destructor
	*	\note	Child and Sibling nodes are not touched and their destruction is left up to the user
	*/

	ParticleSystem::~ParticleSystem(void)
	{
		SAFE_RELEASE(m_pTexture)
		SAFE_RELEASE(m_pVB)
		SAFE_RELEASE(m_pParticleDecl)
	}

	/**
	*	\brief	Called when DIRECT3DDEVICE object has been created
	*	\param	LPDIRECT3DDEVICE9 a_pD3DDevice - pointer to new DIRECT3DDEVICE
	*	\note	Recreates texture, vertex decleration and 
	*/

	void ParticleSystem::OnCreateDevice(LPDIRECT3DDEVICE9 a_pD3DDevice)
	{
		Shader::OnCreateDevice(a_pD3DDevice);

		// create texture
		D3DXCreateTextureFromFile(m_pD3DDevice, m_sTexName, &m_pTexture);

		// define particle vertex structure
		D3DVERTEXELEMENT9 VertexArray[] =
		{
			{0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},	// position
			{0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},	// velocity
			{0, 24, D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1},	// size
			{0, 28, D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 2},	// init time
			{0, 32, D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 3},	// life time
			{0, 36, D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 4},	// mass
			{0, 40, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},	// colour
			D3DDECL_END()
		};

		m_pD3DDevice->CreateVertexDeclaration(VertexArray, &m_pParticleDecl);
	}

	/**
	*	\brief	Called when DIRECT3DDEVICE object has been destroyed
	*	\post	Cleans up memory associated with mesh	
	*/

	void ParticleSystem::OnDestroyDevice()
	{
		Shader::OnDestroyDevice();

		SAFE_RELEASE(m_pTexture)
		SAFE_RELEASE(m_pParticleDecl)
	}

	/**
	*	\brief	Called when the device has been lost
	*	\param	Performs appropriate actions on the vertex buffer and the effect object
	*/

	void ParticleSystem::OnLostDevice()
	{
		Shader::OnLostDevice();

		SAFE_RELEASE(m_pVB);
	}

	/**
	*	\brief	Called when the device has been reset - recreates anything destroyed in lost
	*/

	void ParticleSystem::OnResetDevice(LPDIRECT3DDEVICE9 a_pD3DDevice)
	{
		HRESULT hr;

		Shader::OnResetDevice(a_pD3DDevice);

		if (m_pVB == NULL)
			V(m_pD3DDevice->CreateVertexBuffer(	m_nMaxParticles*sizeof(Particle), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY | D3DUSAGE_POINTS,
												0, D3DPOOL_DEFAULT, &m_pVB, 0)) 
	}

	/**
	*	\brief	[] operator - used to reference into the alive particles within the system
	*	\param	UINT a_nPos - position of particle returned
	*	\return	Particle* - returns a Particle pointer at position a_nPos in the alive vector
	*/

	Particle* ParticleSystem::operator [](UINT a_nPos)
	{
		if (a_nPos < m_vecAliveParticles.size())
			return m_vecAliveParticles[a_nPos];
		else
			return NULL;
	}

	/**
	*	\brief	Accessor for object's type
	*	\return	NodeType - returns SGLib::NodeType::PARTICLESYS
	*/

	NodeType ParticleSystem::GetType() const
	{
		return PARTICLESYS;
	}

	/**
	*	\brief	Mutator for particle system's time
	*	\param	FLOAT a_fTime - used to set time with
	*/

	void ParticleSystem::SetTime(FLOAT a_fTime)
	{
		m_fTime = a_fTime;
	}

	/**
	*	\brief	Set a particle to alive if there exists a dead one and initializes it
	*	\note	This method has been directly referenced from Frank D. Luna's book
	*/

	void ParticleSystem::AddParticle()
	{
		if (m_vecDeadParticles.size() > 0)
		{
			Particle* pPart = m_vecDeadParticles.back();
			InitParticle(pPart);

			m_vecDeadParticles.pop_back();
			m_vecAliveParticles.push_back(pPart);
		}
	}

	/**
	*	\brief	Updates the position of each particle based on their individual variables and the time difference
	*	\param	FLOAT a_fTimeDiff - time difference between update calls
	*	\note	This method has been directly referenced from Frank D. Luna's book
	*/

	void ParticleSystem::Update(FLOAT a_fTimeDiff)
	{
		m_fTime += a_fTimeDiff;

		m_vecDeadParticles.resize(0);
		m_vecAliveParticles.resize(0);

		for (int i = 0; i < m_nMaxParticles; ++i)
		{
			if ((m_fTime - m_vecParticles[i].fInitTime) > m_vecParticles[i].fLifeTime)
				m_vecDeadParticles.push_back(&m_vecParticles[i]);
			else
				m_vecAliveParticles.push_back(&m_vecParticles[i]);
		}

		if (m_fParticleTime > 0.0f)
		{
			static float fTimeAccum = 0.0f;
			fTimeAccum += a_fTimeDiff;

			while (fTimeAccum >= m_fParticleTime)
			{
				AddParticle();
				fTimeAccum -= m_fParticleTime;
			}
		}
	}
}