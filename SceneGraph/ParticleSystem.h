/**
*	\class		SGLib::ParticleSystem
*	\brief		Encapsulates a particle effect into a single node used in conjunction with SGLib::Particle.
*	\author		Frank D. Luna (Introduction to 3D game programming with directx 9.0c A Shader Approach)
*	\author		QUT - Michael Samiec
*	\date		5/2/07
*	\version	1.0
*	\note		This is an abstract class
*
*	This class has been based of the template class defined in Frank Luna's book described above. There are many 
*	code sections sourced directly from the book. Implementing a particle effect is then as simple as extending 
*	from this class and defining its own InitParticle and Render methods. See the textbook for samples.
*
*	Update 23/5/07 - This class has been upgraded to improve usability. There is now no transformation
*						matrix associated with this node as it functionalitiy doubled up on the of the 
*						SGLIB::Transform. 
*/

#ifndef SGLIB_PARTICLESYSTEM
#define SGLIB_PARTICLESYSTEM

#pragma once

#include "Shader.h"
#include <vector>
#include <string>

namespace SGLib
{
	// particle variables avaliable to the shader
	struct Particle
	{
		D3DXVECTOR3	vecInitPos;	///< initial particle position
		D3DXVECTOR3	vecInitVec;	///< initial particle velocity
		FLOAT		fInitSize;	///< initial pixel size
		FLOAT		fInitTime;	///< time created (with relation to particle system time)
		FLOAT		fLifeTime;	///< life time
		FLOAT		fMass;		///< particle mass
		D3DCOLOR	colInitial;	///< initial particle colour
	};

	class ParticleSystem : public Shader
	{
	public:
		ParticleSystem(	LPDIRECT3DDEVICE9 a_pD3DDevice, LPCTSTR a_sFileName, LPCSTR a_sTechName, LPCTSTR a_sTexName,
						D3DXVECTOR3& a_rvecAccel, INT a_nMaxParticles, FLOAT a_fParticleTime);

		virtual ~ParticleSystem(void);

	protected:
		LPDIRECT3DTEXTURE9				m_pTexture;			///< texture used for rendering the sprites
		LPCSTR							m_sTechName;		///< technique used in the effect
		LPCTSTR							m_sTexName;			///< filename for texture
		LPDIRECT3DVERTEXBUFFER9			m_pVB;				///< vertex buffer holding particle positions
		LPDIRECT3DVERTEXDECLARATION9	m_pParticleDecl;	///< particle vertex decleration
		FLOAT							m_fTime;			///< time that the effect has been running
		D3DXVECTOR3						m_vecAccel;			///< acceleration applied to all particles
		INT								m_nMaxParticles;	///< max no of particles at any one time
		FLOAT							m_fParticleTime;	///< time between particle creation

		std::vector<Particle>	m_vecParticles;				///< particles associated with this node
		std::vector<Particle*>	m_vecAliveParticles;		///< particles currently active
		std::vector<Particle*>	m_vecDeadParticles;			///< particles not active

	public:
		// operator used to index into alive particles
		Particle*	operator[] (UINT a_nPos);

		FLOAT		GetTime();
		void		SetTime(FLOAT a_fTime);
		void		AddParticle();

		virtual	void	Update(FLOAT a_fTimeDiff);

		// pure virtual functions that must be instantiated
		virtual void	InitParticle(Particle* a_pPart) = 0;
		virtual void	Render() = 0;

		// device handling functions
		void	OnCreateDevice(LPDIRECT3DDEVICE9 a_pD3DDevice);
		void	OnResetDevice(LPDIRECT3DDEVICE9 a_pD3DDevice);
		void	OnLostDevice();
		void	OnDestroyDevice();

		NodeType	GetType() const;
	};
}

#endif