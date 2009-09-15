//====================================================================
// Mouse.h
// Mouse class to provide camera data
// Version 0.4
// Author: Nicholas Kinsey
// Date 15/09/09
//====================================================================

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

	void Track(bool a_leftButton, bool a_rightButton, int a_mouseWheelData, int a_xPosition, int a_yPosition)
	{
		if (a_leftButton || a_rightButton)
		{
			if (!m_tracking)
			{
				m_tracking = true;
			}
			else if (m_tracking)
			{
				float xDelta = (float)((a_xPosition - m_xPosition) * 0.01f);
				float yDelta = (float)((a_yPosition - m_yPosition) * 0.01f);

				if (a_leftButton && a_rightButton)
				{
					m_camera->SetWalking(true);
				}
				else
				{
					m_camera->SetWalking(false);
				}

				if (a_leftButton)
				{
					m_camera->Handle(xDelta, yDelta);
				}
				if (a_rightButton)
				{
					m_camera->Turn(xDelta, yDelta);
				}
			}

			m_xPosition = a_xPosition;
			m_yPosition = a_yPosition;
		}
		else if (m_tracking)
		{
			StopTracking();
		}
	}
};
}