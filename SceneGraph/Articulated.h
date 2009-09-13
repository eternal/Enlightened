/**
*	\class		SGLib::Articulated
*	\brief		Articulated node provides animation to the scene graph through an implementation of
*				Denavit-Hartenberg (DH) notation. As such both geometry and matrix transforms are 
*				encapsulated within this node.
*	\author		QUT - Michael Samiec
*	\date		5/2/07
*	\version	1.0
*
*	There is both geometry and transforms associated with this class and as such it is derived from
*	both SGLib::Geometry and SGLib::Transform.
*
*	DH notation is a forward kinematics solution to animate individual links. Four parameters are 
*	set to define the position and orientation of the geometry. These variables are -
*
*		link length - length along the x axis (used to calculate offset for next node)
*		displacement length - length along the z axis
*		rotation angle - rotation of the x axis around the z axis
*		twist angle - rotation of the z axis around the x axis
*
*	Parameters are also set to define the default, min and max values for each rotation. Animation
*	is then enabled through keyframes that specify times and angles for both the rotation and twist
*	angles. 
*	
*	SGLib::TimeStep is used to identify a time and angle association. 
*	SGLib::AnimContainer is used to encapsulate a collection of TimeStep
*
*	Update 1/6/07 -	Fixed problems with the SetDefaults and SetDefaultsAll functions so they work correctly. 
*/

#ifndef SGLIB_ARTICULATED
#define	SGLIB_ARTICULATED

#pragma once

#include "transform.h"
#include "geometry.h"
#include <vector>
#include <map>
#include <limits>

namespace SGLib
{
	// identifies a time and angle used for animation
	struct TimeStep
	{
		FLOAT m_fTime;
		FLOAT m_fAngle;

		TimeStep(FLOAT a_fTime, FLOAT a_fAngle) : m_fAngle(a_fAngle)
		{
			// don't allow m_fTime to be less than zero
			m_fTime = (a_fTime < 0.0f) ?  0.0f : a_fTime; 
		}
	};

	// encapsulated a map linking animation names to vectors of SGLib::TimeStep
	struct AnimContainer
	{
		std::vector<TimeStep> m_vecRot;
		std::vector<TimeStep> m_vecTwist;

		AnimContainer(){}

		AnimContainer(	std::vector<TimeStep>& a_pvecRot, 
						std::vector<TimeStep>& a_pvecTwist) :
						m_vecRot(a_pvecRot), 
						m_vecTwist(a_pvecTwist) 
		{}

		std::vector<TimeStep>& GetVecRot(){return m_vecRot;}
		std::vector<TimeStep>& GetVecTwist(){return m_vecTwist;}
	};

	class Articulated : public Transform, public Geometry
	{
	public:
		Articulated(LPDIRECT3DDEVICE9 a_pD3DDevice, 
					FLOAT a_fLinkLength, 
					FLOAT a_fLinkDisplacement,
					FLOAT a_fRotAngle,
					FLOAT a_fRotMax, 
					FLOAT a_fRotMin,
					FLOAT a_fTwistAngle, 
					FLOAT a_fTwistMax, 
					FLOAT a_fTwistMin, 
					LPCTSTR a_sFileName = NULL);

		Articulated(Articulated* a_pReference);

		~Articulated(void);

	protected:
		// dh notation
		FLOAT	m_fLinkLength;			///< link length
		FLOAT	m_fLinkDisplacement;	///< link displacement
		FLOAT	m_fRotAngle;			///< link rotation
		FLOAT	m_fTwistAngle;			///< link twist

		FLOAT	m_fRotMax;				///< max rotation angle
		FLOAT	m_fRotMin;				///< min rotation angle
		FLOAT	m_fRotDefault;			///< default rotation angle

		FLOAT	m_fTwistMax;			///< max twist angle
		FLOAT	m_fTwistMin;			///< min twist angle
		FLOAT	m_fTwistDefault;		///< default twist angle

		// current animation
		FLOAT	m_fTimeOffset;			///< current time displacement into animation loop
		FLOAT	m_fAnimLength;			///< length of animation
		BOOL	m_bAnimRepeat;			///< specifies whether to repeat animation when finished
		BOOL	m_bAnimating;			///< if link is currently animating
		UINT	m_nCurrRotFrame;		///< current position into rotation vector
		UINT	m_nCurrTwistFrame;		///< current position into twist vector
		LPCTSTR	m_sCurrAnimName;		///< name of animation
		D3DXMATRIX	m_oDHMat;			///< holds static matrix transformation that doesn't have to be updated every frame

		std::map<LPCTSTR, AnimContainer> m_mapAnimations;	///< map that links animation name to animation angles

	public:
		BOOL	AddAnimation(LPCTSTR a_sAnimName, AnimContainer& a_rAnim); 
		BOOL	AddAnimation(LPCTSTR a_sAnimName, std::vector<TimeStep>& a_rvecRot, std::vector<TimeStep>& a_rvecTwist); 

		BOOL	DeleteAnimation(LPCTSTR a_sAnimName);
		BOOL	DeleteAnimationAll(LPCTSTR a_sAnimName);

		FLOAT	SetAnimation(LPCTSTR a_sAnimName, BOOL a_bRepeat);
		FLOAT	SetAnimationAll(LPCTSTR a_sAnimName, BOOL a_bRepeat);

		void	SetDefaults();
		void	SetDefaultsAll();

		void	StopAnimation(BOOL a_bReset = FALSE);
		void	StopAnimationAll(BOOL a_bReset = FALSE);

		void	ContinueAnimation();
		void	ContinueAnimationAll();

		// scene graph related functions
		void	Render();
		void	PostRender();
		void	Update(FLOAT a_fTimeDiff);
		void	PostUpdate();

		// accessors
		NodeType	GetType() const;
		LPCTSTR		GetCurrAnimation() const;

		// device handling functions
		void	OnCreateDevice(LPDIRECT3DDEVICE9 a_pD3DDevice);
		void	OnResetDevice(LPDIRECT3DDEVICE9 a_pD3DDevice);
		void	OnLostDevice();
		void	OnDestroyDevice();

	private:
		void	CalculateMatrix();
		void	SetAnimLength	(FLOAT a_nAnimLength);
		FLOAT	GetAngle		(UINT& a_nCurrFrame, const std::vector<TimeStep>& a_rvecAngles);
		void	ClampAngle		(FLOAT& a_rfAngle, FLOAT a_fMinAngle, FLOAT a_fMaxAngle);
	};
}

#endif
