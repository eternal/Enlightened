#pragma once

namespace Feisty
{

class Camera
{
protected:
	SGLib::Camera* m_node;

public:
	Camera(void)
	{
		m_node = NULL;
	}

	Camera(SGLib::Camera* a_node)
	{
		m_node = a_node;
	}

	void Initialize(void)
	{
		Update();
	}

	void Update(void)
	{
		SGLib::Camera* camera = GetNode();
	}

	virtual ~Camera(void)
	{
	}

	SGLib::Camera* GetNode()
	{
		return m_node;
	}

	void SetNode(SGLib::Camera* a_node)
	{
		m_node = a_node;
	}
};
}