#include "Articulated.h"

using std::vector;
using std::map;

namespace SGLib
{
	/**
	*	\brief	Articulated constructor
	*	\param	LPDIRECT3DDEVICE9 a_pD3DDevice - pointer to direct3ddevice used for directx operations
	*	\param	FLOAT a_fLinkLength - dh notation link length
	*	\param	FLOAT a_fLinkDisplacement - dh notation link displacement
	*	\param	FLOAT a_fRotAngle - dh rotation angle (this is also the default rotation angle)
	*	\param	FLOAT a_fRotMin - rotation min angle
	*	\param	FLOAT a_fRotMax - rotation max angle
	*	\param	FLOAT a_fTwistAngle - dh twist angle (this is also the default twist angle)
	*	\param	FLOAT a_fTwistMin - twist min angle
	*	\param	FLOAT a_fTwistMax - twist max angle
	*	\param	LPCTSTR a_sFileName - filename of .x geometry
	*/

	Articulated::Articulated(	LPDIRECT3DDEVICE9 a_pD3DDevice, 
								FLOAT a_fLinkLength, 
								FLOAT a_fLinkDisplacement,
								FLOAT a_fRotAngle,
								FLOAT a_fRotMin, 
								FLOAT a_fRotMax,
								FLOAT a_fTwistAngle, 
								FLOAT a_fTwistMin, 
								FLOAT a_fTwistMax, 
								LPCTSTR a_sFileName) :	
								
									Node(a_pD3DDevice), 
									Geometry(a_pD3DDevice, a_sFileName), 
									Transform(a_pD3DDevice), 
									m_fLinkLength(a_fLinkLength), 
									m_fLinkDisplacement(a_fLinkDisplacement),
									m_fRotAngle(a_fRotAngle),
									m_fRotMin(a_fRotMin),
									m_fRotMax(a_fRotMax), 
									m_fTwistAngle(a_fTwistAngle),  
									m_fTwistMin(a_fTwistMin),
									m_fTwistMax(a_fTwistMax),
									m_fRotDefault(a_fRotAngle),
									m_fTwistDefault(a_fTwistAngle),
									m_fTimeOffset(0.0f),
									m_fAnimLength(0.0f),
									m_bAnimRepeat(FALSE),
									m_bAnimating(FALSE),
									m_nCurrRotFrame(0),
									m_nCurrTwistFrame(0),
									m_sCurrAnimName(NULL)
	{
		m_pReference = NULL;
		CalculateMatrix();
	}

	/**
	*	\brief	Articulated reference constructor
	*	\param	Articulated* a_pReference - pointer to articulated node that this node will mimic
	*	\pre	a_pReference != NULL
	*	\note	This constructor is used to allow many articulated nodes to reference a single geometry
	*			mesh. All other variables are copied across (including animation containers) and as such,
	*			their animation systems operate idependently of each other. 
	*/

	Articulated::Articulated(	Articulated* a_pReference) :	
									Node(a_pReference->GetDevice()), 
									Geometry(a_pReference->GetDevice(), NULL), 
									Transform(a_pReference->GetDevice()), 
									m_fLinkLength(a_pReference->m_fLinkLength), 
									m_fLinkDisplacement(a_pReference->m_fLinkDisplacement),
									m_fRotAngle(a_pReference->m_fRotAngle),
									m_fRotMin(a_pReference->m_fRotMin),
									m_fRotMax(a_pReference->m_fRotMax), 
									m_fTwistAngle(a_pReference->m_fTwistAngle),  
									m_fTwistMin(a_pReference->m_fTwistMin),
									m_fTwistMax(a_pReference->m_fTwistMax),
									m_fRotDefault(a_pReference->m_fRotAngle),
									m_fTwistDefault(a_pReference->m_fTwistAngle),
									m_fTimeOffset(0.0f),
									m_fAnimLength(0.0f),
									m_bAnimRepeat(FALSE),
									m_bAnimating(FALSE),
									m_nCurrRotFrame(0),
									m_nCurrTwistFrame(0),
									m_sCurrAnimName(NULL),
									m_mapAnimations(a_pReference->m_mapAnimations)
	{
		m_pReference = a_pReference;
		CalculateMatrix();
	}

	/**
	*	\brief	Articulated destructor
	*	\note	Child and Sibling nodes are not touched and their destruction is left up to the user
	*/

	Articulated::~Articulated(void)
	{
	}

	/**
	*	\brief	Render function called when the scene graph is initially rendering this node
	*	\pre	Device must point to a valid DIRECT3DDEVICE object
	*	\post	Both the Geometry and Transform aspects of this node are rendered
	*/

	void Articulated::Render()
	{
		Transform::Render();
		Geometry::Render();
	}

	/**
	*	\brief	Called after the scene graph has called Render() on this node and on this node's child but
	*			before it is called on this node's sibling to undo changes made by Render()
	*	\pre	Device must point to a valid DIRECT3DDEVICE object
	*	\post	Both the Geometry and Transform aspects of this node are post-rendered
	*/

	void Articulated::PostRender()
	{
		Transform::PostRender();
		Geometry::PostRender();
	}

	/**
	*	\brief	Update function called on the initial pass of the scene graph before the render call
	*			Updates transpose matrix for geometry if required
	*	\param	FLOAT a_fTimeDiff - time difference since last update call
	*/

	void Articulated::Update(FLOAT a_fTimeDiff)
	{
		HRESULT hr;

		// if animating and animation exists
		if (m_bAnimating && m_mapAnimations.find(m_sCurrAnimName) != m_mapAnimations.end())
		{
			m_fTimeOffset += a_fTimeDiff;

			// if animation time has elapsed
			if (m_fTimeOffset > m_fAnimLength)
			{
				// if animation on repeat, reset time
				if (m_bAnimRepeat)
				{
					while (m_fTimeOffset > m_fAnimLength)
						m_fTimeOffset -= m_fAnimLength;

					m_nCurrRotFrame = m_nCurrTwistFrame = 0;
				}
				else
					m_bAnimating = FALSE;
			}

			// get rotation and twist angle
			m_fRotAngle = GetAngle(m_nCurrRotFrame, m_mapAnimations[m_sCurrAnimName].GetVecRot());
			m_fTwistAngle = GetAngle(m_nCurrTwistFrame, m_mapAnimations[m_sCurrAnimName].GetVecTwist());

			ClampAngle(m_fRotAngle, m_fRotMin, m_fRotMax);
			ClampAngle(m_fTwistAngle, m_fTwistMin, m_fTwistMax);

			// calculate new matrix
			CalculateMatrix();
		}

		D3DXMATRIX matTransLength, matTemp;

		// apply current matrix to geometry matrix
		V(m_pD3DDevice->GetTransform(D3DTS_WORLD, &m_oMatrixPrevious))
		D3DXMatrixMultiply(&m_oMatrix, &m_oDHMat, &m_oMatrixPrevious);

		// apply link length and set transform for next link
		D3DXMatrixTranslation(&matTransLength, m_fLinkLength, 0.0f, 0.0f);
		D3DXMatrixMultiply(&matTemp, &matTransLength, &m_oMatrix);

		V(m_pD3DDevice->SetTransform(D3DTS_WORLD, &matTemp))
	}

	/**
	*	\brief	Called after the scene graph has called Update() on this node and on this node's child but
	*			before it is called on this node's sibling to undo changes made by Update()
	*/

	void Articulated::PostUpdate()
	{
		Transform::PostUpdate();
		Geometry::PostUpdate();
	}

	/**
	*	\brief	Accessor for object's type
	*	\return	NodeType - returns SGLib::NodeType::ARTICULATED
	*/

	NodeType Articulated::GetType() const
	{
		return ARTICULATED;
	}

	/**
	*	\brief	Called when device has been created
	*	\param	LPDIRECT3DDEVICE9 a_pD3DDevice - pointer to new DIRECT3DDEVICE
	*/

	void Articulated::OnCreateDevice(LPDIRECT3DDEVICE9 a_pD3DDevice)
	{
		// call create device to recreate mesh if it is stored in D3DPOOL_MANAGED memory
		Geometry::OnCreateDevice(a_pD3DDevice);
	}

	/**
	*	\brief	Called when device has been reset
	*	\param	LPDIRECT3DDEVICE9 a_pD3DDevice - pointer to new DIRECT3DDEVICE
	*/

	void Articulated::OnResetDevice(LPDIRECT3DDEVICE9 a_pD3DDevice)
	{
		// call reset device to recreate mesh if it is stored in D3DPOOL_DYNAMIC memory
		Geometry::OnResetDevice(a_pD3DDevice);
	}

	/**
	*	\brief	Called when device has been lost
	*/

	void Articulated::OnLostDevice()
	{
		// resolves dominance warning
		Geometry::OnLostDevice();
	}

	/**
	*	\brief	Called when device has been destroyed
	*/

	void Articulated::OnDestroyDevice()
	{
		// resolves dominance warning
		Geometry::OnDestroyDevice();
	}

	/**
	*	\brief	Calls SetAnimation() on this object and all objects of this type below it in the hierarchy
	*	\param	LPCTSTR a_sAnimName - name of animation to activate
	*	\param	BOOL a_bRepeat - specifies whether to put the animation on repeat
	*	\return	FLOAT - the largest length of the animation - returns -1 if animation not found
	*/

	FLOAT Articulated::SetAnimationAll(LPCTSTR a_sAnimName, BOOL a_bRepeat)
	{
		FLOAT fMaxAnimLength = -1.0f;
		vector<Node*>::iterator iter;

		// get all nodes of ARTICULATED type below this node in the scene graph
		vector<Node*> vecNodes = this->GetNodesOfType(ARTICULATED);

		// set animation for all nodes and store largest animation length
		for (iter = vecNodes.begin(); iter != vecNodes.end(); ++iter)
			fMaxAnimLength = max(fMaxAnimLength, dynamic_cast<Articulated*>(*iter)->SetAnimation(a_sAnimName, a_bRepeat));

		// if animation was found, set largest animation length for all nodes
		if (fMaxAnimLength != -1.0f)
		{
			for (iter = vecNodes.begin(); iter != vecNodes.end(); ++iter)
				dynamic_cast<Articulated*>(*iter)->SetAnimLength(fMaxAnimLength);
		}

		// return largest animation length
		return fMaxAnimLength;
	}

	/**
	*	\brief	Searches for animation and sets it if found
	*	\param	LPCTSTR a_sAnimName - name of animation to activate
	*	\param	BOOL a_bRepeat - specifies whether to put the animation on repeat
	*	\return	FLOAT - the length of the animation - returns -1 if animation not found
	*/

	FLOAT Articulated::SetAnimation(LPCTSTR a_sAnimName, BOOL a_bRepeat)
	{
		m_bAnimating = FALSE;
		m_sCurrAnimName = a_sAnimName;
		m_fAnimLength = -1.0f;
		m_fTimeOffset = 0.0f;
		m_bAnimRepeat = a_bRepeat;
		m_nCurrRotFrame = m_nCurrTwistFrame = 0;

		// if animation is found
		if (m_mapAnimations.find(a_sAnimName) != m_mapAnimations.end())
		{
			m_fAnimLength = max(m_mapAnimations[a_sAnimName].m_vecRot.back().m_fTime, m_mapAnimations[a_sAnimName].m_vecTwist.back().m_fTime);
			m_bAnimating = TRUE;
		}

		return m_fAnimLength;
	}

	/**
	*	\brief	Calls StopAnimation() on this object and all objects of this type below it in the hierarchy
	*	\param	BOOL a_bReset - specifies whether to reset the angles to default
	*/

	void Articulated::StopAnimationAll(BOOL a_bReset)
	{
		// get all nodes of ARTICULATED type below this node in the scene graph
		vector<Node*> vecNodes = this->GetNodesOfType(ARTICULATED);
		vector<Node*>::iterator iter;

		// call StopAnimation() on all nodes
		for (iter = vecNodes.begin(); iter != vecNodes.end(); ++iter)
			dynamic_cast<Articulated*>(*iter)->StopAnimation(a_bReset);
	}

	/**
	*	\brief	Stops the animation and if specified, resets angles to defaults
	*	\param	BOOL a_bReset - specifies whether to reset the angles to default
	*/

	void Articulated::StopAnimation(BOOL a_bReset)
	{
		m_bAnimating = FALSE;

		if (a_bReset)
			SetDefaults();
	}

	/**
	*	\brief	Calls ContinueAnimation() on this object and all objects of this type below it in the hierarchy
	*	\pre	An animation must of been previously set and StopAnimation() been called
	*/

	void Articulated::ContinueAnimationAll()
	{
		// get all nodes of ARTICULATED type below this node in the scene graph
		vector<Node*> vecNodes = this->GetNodesOfType(ARTICULATED);
		vector<Node*>::iterator iter;

		// call ContinueAnimation() on all nodes
		for (iter = vecNodes.begin(); iter != vecNodes.end(); ++iter)
			dynamic_cast<Articulated*>(*iter)->ContinueAnimation();
	}

	/**
	*	\brief	If a previous animation has been set, continue it
	*	\pre	m_sCurrAnimName != NULL;
	*/

	void Articulated::ContinueAnimation()
	{
		// if animation is found
		if (m_mapAnimations.find(m_sCurrAnimName) != m_mapAnimations.end())
			m_bAnimating = TRUE;
	}

	/**
	*	\brief	Calls SetDefault() on this object and all objects of this type below it in the hierarchy
	*/

	void Articulated::SetDefaultsAll()
	{
		// get all nodes of ARTICULATED type below this node in the scene graph
		vector<Node*> vecNodes = this->GetNodesOfType(ARTICULATED);
		vector<Node*>::iterator iter;

		// call SetDefaults() on all nodes
		for (iter = vecNodes.begin(); iter != vecNodes.end(); ++iter)
			dynamic_cast<Articulated*>(*iter)->SetDefaults();
	}

	/**
	*	\brief	Sets the rotation angles to defaults, sets time offset back to 0.0f, sets the current frame
	*			numbers to 0 and recalculates the DH world matrix
	*/

	void Articulated::SetDefaults()
	{
		// reset angles to defaults
		m_fRotAngle = m_fRotDefault;
		m_fTwistAngle = m_fTwistDefault;

		// reset time offset
		m_fTimeOffset = 0.0f;

		// set frame numbers to 0
		m_nCurrRotFrame = m_nCurrTwistFrame = 0;

		// recalculate matrix
		CalculateMatrix();
	}

	/**
	*	\brief	Mutator to set animation length
	*	\param	FLOAT a_fAnimLength - variable to set animation length with
	*/

	void Articulated::SetAnimLength(FLOAT a_fAnimLength)
	{
		m_fAnimLength = a_fAnimLength;
	}

	/**
	*	\brief	Adds the animation to the map of animations of this node if the name doesn't already exist
	*	\param	LPCTSTR a_sAnimName - name to identify this animation with
	*	\param	AnimContainer& a_rAnim - reference to animation container that holds animation angles
	*	\return	BOOL - specifies whether the animation was succesfully added
	*	\note	There is not difference between this AddAnimation() and the other in this class, except the
	*			function parameters
	*/

	BOOL Articulated::AddAnimation(LPCTSTR a_sAnimName, AnimContainer& a_rAnim)
	{
		BOOL bResult = FALSE;

		// if animation was not found
		if (m_mapAnimations.find(a_sAnimName) == m_mapAnimations.end())
		{
			// add animation
			m_mapAnimations[a_sAnimName] = a_rAnim;
			bResult = TRUE;
		}

		return bResult;
	}

	/**
	*	\brief	Adds the animation to the map of animations of this node if the name doesn't already exist
	*	\param	LPCTSTR a_sAnimName - name to identify this animation with
	*	\param	vector<TimeStep>& a_rvecRot - reference to vector of rotation TimeSteps
	*	\param	vector<TimeStep>& a_rvecTwist - reference to vector of twist TimeSteps
	*	\return	BOOL - specifies whether the animation was succesfully added
	*	\note	There is not difference between this AddAnimation() and the other in this class, except the
	*			function parameters
	*/

	BOOL Articulated::AddAnimation(LPCTSTR a_sAnimName, vector<TimeStep>& a_rvecRot, vector<TimeStep>& a_rvecTwist)
	{
		// call other AddAnimation function by combining vectors into an animation container
		return AddAnimation(a_sAnimName, AnimContainer(a_rvecRot, a_rvecTwist));
	}

	/**
	*	\brief	Calls DeleteAnimation() on this object and all objects of this type below it in the hierarchy
	*	\param	LPCTSTR a_sAnimName - Name of animation to delete
	*	\return	BOOL - specifies whether the animation was found and delete anywhere along the hierarchy
	*/

	BOOL Articulated::DeleteAnimationAll(LPCTSTR a_sAnimName)
	{
		BOOL bFound = FALSE;
		vector<Node*> vecNodes = this->GetNodesOfType(ARTICULATED);
		vector<Node*>::iterator iter;

		// call delete animation on all nodes and if it is found at least once, set bFound to TRUE
		for (iter = vecNodes.begin(); iter != vecNodes.end(); ++iter)
			if (dynamic_cast<Articulated*>(*iter)->DeleteAnimation(a_sAnimName))
				bFound = TRUE;

		return bFound;
	}

	/**
	*	\brief	Deletes the animation from the map of animation of this nodes
	*	\param	LPCTSTR a_sAnimName - Name of animation to delete
	*	\return	BOOL - specifies whether the animation was found and delete
	*/

	BOOL Articulated::DeleteAnimation(LPCTSTR a_sAnimName)
	{
		BOOL bFound = FALSE;

		map<LPCTSTR, AnimContainer>::iterator iterPos = m_mapAnimations.find(a_sAnimName);

		// if animation was found
		if (iterPos != m_mapAnimations.end())
		{
			m_mapAnimations.erase(iterPos);
			bFound = TRUE;
		}

		return bFound;
	}

	/**
	*	\brief	Accessor for name of current animation
	*	\return	LPCTSTR - returns name of animation or NULL if no animation is currently set
	*/

	LPCTSTR	Articulated::GetCurrAnimation() const
	{
		return m_sCurrAnimName;
	}

	/**
	*	\brief	Clamps the animation between a min and max value
	*	\param	FLOAT& a_pAngle - reference to angle that needs to be clamped
	*	\param	FLOAT a_pMinAngle - minimum angle used in clamp
	*	\param	FLOAT a_pMaxAngle - maximum angle used in clamp
	*/

	void Articulated::ClampAngle(FLOAT& a_pAngle, FLOAT a_pMinAngle, FLOAT a_pMaxAngle)
	{
		if (a_pAngle < a_pMinAngle)
			a_pAngle = a_pMinAngle;
		else if (a_pAngle > a_pMaxAngle)
			a_pAngle = a_pMaxAngle;		
	}

	/**
	*	\brief	Calculates transpose matrix based on the DH notation algorithm and the link displacement, 
	*			twist angle and rotation angle.
	*/

	void Articulated::CalculateMatrix()
	{
		D3DXMATRIX matRot, matTwist, matTransDis;

		// twist matrix
		D3DXMatrixRotationX(&matTwist, m_fTwistAngle);

		// displacement matrix
		D3DXMatrixTranslation(&matTransDis, 0.0f, 0.0f, m_fLinkDisplacement);

		// rotation matrix
		D3DXMatrixRotationZ(&matRot, m_fRotAngle);

		// combine them into m_oMatrix
		D3DXMatrixMultiply(&m_oDHMat, &matRot, &matTwist);
		D3DXMatrixMultiply(&m_oDHMat, &m_oDHMat, &matTransDis);
	}

	/**
	*	\brief	Gets angle from vector of TimeSteps and current time offset into animation
	*	\param	UINT& a_nCurrFrame - reference to current position within a_rvecAngles angle is being
	*								 sourced from
	*	\param	vector<TimeStep>& a_rvecAngles - vector of timesteps to resource angle from
	*	\return	FLOAT - angle calculated based on time 
	*/

	FLOAT Articulated::GetAngle(UINT& a_nCurrFrame, const vector<TimeStep>& a_rvecAngles)
	{
		FLOAT fPrevFrameTime, fNextFrameTime, fPrevFrameAngle, fNextFrameAngle;
		fPrevFrameTime = fNextFrameTime = fPrevFrameAngle = fNextFrameAngle = 0.0f;

		// if current frame is last in vector, return last timestep angle
		if (a_nCurrFrame+1 >= a_rvecAngles.size())
			return a_rvecAngles[a_nCurrFrame].m_fAngle;

		fNextFrameTime = a_rvecAngles[a_nCurrFrame+1].m_fTime;

		// if time offset is greater than next frame time
		while (m_fTimeOffset > fNextFrameTime)
		{
			// increment current frame
			a_nCurrFrame++;

			// if current frame is last in vector, return last timestep angle
			if (a_nCurrFrame+1 >= a_rvecAngles.size())
				return a_rvecAngles[a_nCurrFrame].m_fAngle;
			
			fNextFrameTime = a_rvecAngles[a_nCurrFrame+1].m_fTime;
		}

		fPrevFrameTime = a_rvecAngles[a_nCurrFrame].m_fTime;
		fPrevFrameAngle = a_rvecAngles[a_nCurrFrame].m_fAngle;
		fNextFrameAngle = a_rvecAngles[a_nCurrFrame+1].m_fAngle;

		// prevent possible divide by zero error
		if (abs(fNextFrameTime - fPrevFrameTime) > std::numeric_limits<float>::epsilon())
			// calculate angle based on current time offset
			return ((fNextFrameAngle - fPrevFrameAngle) * ((m_fTimeOffset - fPrevFrameTime) / (fNextFrameTime - fPrevFrameTime))) + fPrevFrameAngle;	
		else
			return fNextFrameAngle;
	}
}