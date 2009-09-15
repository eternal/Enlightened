#pragma once
#include "Camera.h"

namespace Feisty{

class Mouse
{
protected:
	bool m_tracking;
	int m_xPosition;
	int m_yPosition;

	Camera* m_camera;

public:

	Mouse(void) : m_tracking(false)
	{
		
	}

	virtual ~Mouse(void)
	{
	}

	void SetCamera(Camera* a_camera)
	{
		m_camera = a_camera;
	}

	bool GetTracking()
	{
		return m_tracking;
	}

	void SetTracking(bool a_tracking)
	{
		m_tracking = a_tracking;
	}

	void StopTracking()
	{
		m_tracking = false;
		m_xPosition = NULL;
		m_yPosition = NULL;
	}

	void Track(bool a_active, int a_xPosition, int a_yPosition)
	{
		if (a_active)
		{
			if (!m_tracking)
			{
				m_tracking = true;
			}
			else if (m_tracking)
			{
				float xDelta = (float)((a_xPosition - m_xPosition) * 0.01f);
				float yDelta = (float)((a_yPosition - m_yPosition) * 0.01f);

				//D3DXVECTOR3 position = m_camera->GetNode()->GetPos() + D3DXVECTOR3(xDelta, yDelta, 0.0f);
				//m_camera->GetNode()->SetPos(position);
				//D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
				//m_camera->RotateGlobal(up, xDelta);

				//D3DXVECTOR3 across(1.0f, 0.0f, 0.0f);
				//m_camera->Rotate(across, yDelta);
				m_camera->Handle(xDelta, yDelta);
			}

			m_xPosition = a_xPosition;
			m_yPosition = a_yPosition;
		}
		else if (!a_active)
		{
			if (m_tracking)
			{
				StopTracking();
			}
			else {
			}
		}
	}
};
}