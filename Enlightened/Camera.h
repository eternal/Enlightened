#pragma once

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
	D3DXVECTOR3 m_target;
	D3DXVECTOR3 m_up;

	LPDIRECT3DDEVICE9 m_device;

	//D3DXMATRIX m_matrix;
	//D3DXMATRIX m_matrixStore;

public:
	Camera(LPDIRECT3DDEVICE9 a_device) : SGLib::Camera(a_device), SGLib::Node(a_device)
	{
		m_device = a_device;

		m_projectionNode = NULL;

		D3DXQuaternionIdentity(&m_rotation);

		m_target = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		m_up = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		m_position = D3DXVECTOR3(0.0f, 100.0f, -20.0f);

		//SetSimpleMovement(true);

		m_fieldOfView = D3DX_PI * 0.25f;
		m_aspectRatio = 1.5f;
		m_near = 1.0f;
		m_far = 1000.0f;

		D3DXMATRIX projectionMatrix;
		D3DXMatrixPerspectiveFovLH(&projectionMatrix, m_fieldOfView, m_aspectRatio, m_near, m_far);
		m_projectionNode = new SGLib::Projection(a_device, projectionMatrix);
		SetChild(m_projectionNode);

		D3DXMATRIX lookAtMatrix;
		D3DXMatrixLookAtLH(&lookAtMatrix, &m_position, &m_target, &m_up);

		D3DXMATRIX inverseLookAt;
		D3DXMatrixInverse(&inverseLookAt, NULL, &lookAtMatrix);

		D3DXQUATERNION lookAtQuaternion;
		D3DXQuaternionRotationMatrix(&lookAtQuaternion, &inverseLookAt);

		D3DXQuaternionNormalize(&m_rotation, &lookAtQuaternion);

		Refresh();
	}

	void Refresh()
	{
		D3DXMATRIX rotationMatrix;
		D3DXMATRIX translationMatrix;

		D3DXMatrixRotationQuaternion(&rotationMatrix, &m_rotation);
		D3DXMatrixTranslation(&translationMatrix, m_position.x, m_position.y, m_position.z);

		D3DXMATRIX productMatrix;
		D3DXMatrixMultiply(&productMatrix, &rotationMatrix, &translationMatrix);

		D3DXMatrixInverse(&m_oMatrix, NULL, &productMatrix);
	}

	//D3DXMATRIX GetViewMatrix()
	//{
	//	return m_matrix;
	//}

	//void Render()
	//{
	//	HRESULT	hr;
	//	V(m_device->GetTransform(D3DTS_VIEW, &m_matrixStore))
	//	V(m_device->SetTransform(D3DTS_VIEW, &m_matrix))
	//}

	//void PostRender()
	//{
	//	HRESULT hr;
	//	V(m_device->SetTransform(D3DTS_VIEW, &m_matrixStore))	
	//}

	//void Update(float a_timeDelta)
	//{
	//	//UpdateMatrix();
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

	void Rotate(D3DXVECTOR3 a_axis, float a_angle) {
		D3DXMATRIX rotationMatrix;
		D3DXMatrixRotationQuaternion(&rotationMatrix, &m_rotation);

		D3DXVECTOR4 axis;
		D3DXVec3Transform(&axis, &a_axis, &rotationMatrix);

		D3DXVECTOR3 axis3(axis);

		D3DXQUATERNION rotatationQuaternion;
		D3DXQuaternionRotationAxis(&rotatationQuaternion, &axis3, a_angle);

		D3DXQUATERNION productQuaternion;
		D3DXQuaternionMultiply(&productQuaternion, &rotatationQuaternion, &m_rotation);

		D3DXQuaternionNormalize(&m_rotation, &productQuaternion);

		Refresh();
	}
	
	void Translate(D3DXVECTOR3 a_translationVector) {
		//Position += Vector3.Transform(distance, Matrix.CreateFromQuaternion(Rotation));
		//UpdateMatrices();
	}
	
	void Revolve(D3DXVECTOR3 a_axis, float a_angle) {
		//Vector3 revolveAxis = Vector3.Transform(axis, Matrix.CreateFromQuaternion(Rotation));
		//Quaternion rotate = Quaternion.CreateFromAxisAngle(revolveAxis, angle);
		//Position = Vector3.Transform(Position - Target, Matrix.CreateFromQuaternion(rotate)) + Target;
		//Rotate(axis, angle);
	}

	void RotateGlobal(D3DXVECTOR3 a_axis, float a_angle) {
		D3DXQUATERNION rotatationQuaternion;
		D3DXQuaternionRotationAxis(&rotatationQuaternion, &a_axis, a_angle);

		D3DXQUATERNION normalizedRotatationQuaternion;
		D3DXQuaternionNormalize();

		D3DXQUATERNION productQuaternion;
		D3DXQuaternionMultiply(&productQuaternion, &rotatationQuaternion, &m_rotation);

		D3DXQuaternionNormalize(&m_rotation, &productQuaternion);

		Refresh();
	}
};
}