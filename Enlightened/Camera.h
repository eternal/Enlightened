//====================================================================
// Camera.h
// Camera extension to provide additional features not found in the
// base camera class
// Date 15/09/09
// Author: Nicholas Kinsey
// Version 0.5
//====================================================================
#pragma once

#include <math.h>

#define PI 3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679f

namespace Feisty
{

class Camera : public SGLib::Camera
{
protected:
	SGLib::Projection* m_projectionNode;

	float m_fieldOfView;
	float m_near;
	float m_far;
	float m_aspectRatio;

	D3DXVECTOR3 m_rotation;
	D3DXVECTOR3 m_position;
	D3DXVECTOR3 m_seekTarget;
	D3DXVECTOR3 m_seekOrigin;
	D3DXVECTOR3 m_targetPosition;
	D3DXVECTOR3 m_up;

	D3DXVECTOR3 m_offset;

	LPDIRECT3DDEVICE9 m_device;

	float m_seekTime;
	float m_elapsedSeekTime;
	float m_elapsedTime;
	bool m_seeking;

public:
	Camera(LPDIRECT3DDEVICE9 a_device) : SGLib::Camera(a_device), SGLib::Node(a_device)
	{
		m_device = a_device;

		m_projectionNode = NULL;

		// timing code
		m_elapsedTime = 0.0f;
		m_seekTime = 5.0f;
		m_elapsedSeekTime = 0.0f;
		m_seeking = false;

		m_offset = D3DXVECTOR3(0.0f, 100.0f, -100.0f);
		m_seekOrigin = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		m_seekTarget = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		m_targetPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		m_up = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		m_position = m_offset;

		//SetSimpleMovement(true);

		m_fieldOfView = D3DX_PI * 0.25f;
		m_aspectRatio = 1.5f;
		m_near = 1.0f;
		m_far = 1000.0f;

		// projection
		D3DXMATRIX projectionMatrix;
		D3DXMatrixPerspectiveFovLH(&projectionMatrix, m_fieldOfView, m_aspectRatio, m_near, m_far);
		m_projectionNode = new SGLib::Projection(a_device, projectionMatrix);
		SetChild(m_projectionNode);

		Refresh();
	}

	void Refresh()
	{
		D3DXMatrixLookAtLH(&m_oMatrix, &m_position, &m_targetPosition, &m_up);
	}

	void Update(float a_timeDelta)
	{
		if (m_seeking)
		{
			m_elapsedSeekTime += a_timeDelta;

			if (m_elapsedSeekTime >= m_seekTime)
			{
				m_elapsedSeekTime = m_seekTime;
				m_seeking = false;
			}

			float progress = m_elapsedSeekTime / m_seekTime;
			// easing: y = (-cos[x] * 0.5) + 0.5
			float bias = ((-cos(progress * PI) * 0.5f) + 0.5f);
			D3DXVec3Lerp(&m_position, &m_seekOrigin, &m_seekTarget, bias);

			Refresh();
		}

		//m_elapsedTime += a_timeDelta;
		//if (m_elapsedTime >= 1.0f && m_seeking == false)
		//{
		//	StartSeeking(D3DXVECTOR3(0.0f, 75.0f, -50.0f));
		//}
	}

	void StartSeeking(D3DXVECTOR3 a_vector)
	{
		m_seekTarget = a_vector;
		m_elapsedSeekTime = 0.0f;
		m_seekOrigin = m_position;
		m_seeking = true;
	}

	void StopSeeking()
	{
		
	}

	void Handle(float a_xDelta, float a_yDelta)
	{
		D3DXVECTOR3 targetToCamera = m_position - m_targetPosition;
		float cameraDistance = D3DXVec3Length(&targetToCamera);

		D3DXVECTOR3 targetToCameraUnitVector;
		D3DXVec3Normalize(&targetToCameraUnitVector, &targetToCamera);

		D3DXMATRIX rotationMatrix;
		D3DXMatrixRotationYawPitchRoll(&rotationMatrix, a_xDelta, a_yDelta, 0.0f);
		
		D3DXVECTOR3 targetToNewCameraUnit;
		D3DXVec3TransformCoord(&targetToNewCameraUnit, &targetToCameraUnitVector, &rotationMatrix);

		D3DXVECTOR3 targetToNewCameraPosition;
		D3DXVec3Scale(&targetToNewCameraPosition, &targetToNewCameraUnit, cameraDistance);

		m_position = targetToNewCameraPosition;

		Refresh();
	}

	//void SetOffset(D3DXVECTOR3 a_offset)
	//{
	//	m_offset = a_offset;
	//}

	//void Snap()
	//{
	//	m_position = m_targetPosition + m_offset;
	//}

	virtual ~Camera(void)
	{
	}

	void AdoptShader(SGLib::Node* g_node)
	{
		m_projectionNode->SetChild(g_node);
	}

	void SetFieldOfView(float a_fieldOfView)
	{
		m_fieldOfView = a_fieldOfView;
	}

	void SetAspectRatio(float a_aspectRatio)
	{
		m_aspectRatio = a_aspectRatio;
	}

	void SetNear(float a_near)
	{
		m_near = a_near;
	}

	void SetFar(float a_far)
	{
		m_far = a_far;
	}

	D3DXVECTOR3 GetPosition()
	{
		return m_position;
	}
};
}