#include "Camera.h"

Camera::Camera() 
	:
	_Position(0.0f, 0.0f, 0.0f),
	_RightDirection(0.0f, 0.0f, 0.0f),
	_UpDirection(0.0f, 0.0f, 0.0f),
	_LookAt(0.0f, 0.0f, 0.0f),
	_AspectRatio(0.0f),
	_ViewAngleY(0.0f),
	_NearPlaneZ(0.0f),
	_FarPlaneZ(0.0f),
	_NearPlaneHeight(0.0f),
	_FarPlaneHeight(0.0f),
	_ViewMatrix(),
	_ProjMatrix(),
	_Viewport()
{}

Camera::~Camera() {

}

DirectX::XMFLOAT3 Camera::GetPos()const {
	return _Position;
}

DirectX::XMFLOAT3 Camera::GetRightDir()const {
	return _RightDirection;
}

DirectX::XMFLOAT3 Camera::GetUpDir()const {
	return _UpDirection;
}

DirectX::XMFLOAT3 Camera::GetLookAt()const {
	return _LookAt;
}

float Camera::GetAspectRatio()const {
	return _AspectRatio;
}

float Camera::GetViewAngleY()const {
	return _ViewAngleY;
}

float Camera::GetNearPlaneZ()const {
	return _NearPlaneZ;
}

float Camera::GetFarPlaneZ()const {
	return _FarPlaneZ;
}

float Camera::GetNearPlaneHeight()const {
	return _NearPlaneHeight;
}

float Camera::GetFarPlaneHeight()const {
	return _FarPlaneHeight;
}

DirectX::XMFLOAT4X4 Camera::GetViewMatrix()const {
	return _ViewMatrix;
}

DirectX::XMFLOAT4X4 Camera::GetProjMatrix()const {
	return _ProjMatrix;
}

D3D11_VIEWPORT Camera::GetViewport()const {
	return _Viewport;
}

void Camera::SetFrustum(float viewAngleY, float aspectRatio, float nearPlaneZ, float  farPlaneZ) {
	_ViewAngleY = viewAngleY;
	_AspectRatio = aspectRatio;
	_NearPlaneZ = nearPlaneZ;
	_FarPlaneZ = farPlaneZ;
	_NearPlaneHeight = 2.0f * tanf(viewAngleY / 2.0f) * nearPlaneZ;
	_FarPlaneHeight = 2.0f * tanf(viewAngleY / 2.0f) * farPlaneZ;
	// Set the Projection Matrix
	DirectX::XMStoreFloat4x4(&_ProjMatrix, DirectX::XMMatrixPerspectiveFovLH(_ViewAngleY, _AspectRatio, _NearPlaneZ, _FarPlaneZ));
}

void Camera::SetViewPort(float topLeftX, float topLeftY, float width, float height, float minDepth, float maxDepth) {
	_Viewport.TopLeftX = topLeftX;
	_Viewport.TopLeftY = topLeftY;
	_Viewport.Height = height;
	_Viewport.Width = width;
	_Viewport.MinDepth = minDepth;
	_Viewport.MaxDepth = maxDepth;
}

void Camera::SetViewPort(const D3D11_VIEWPORT& viewport) {
	_Viewport = viewport;
}

// First Person Camera
FirstPersonCamera::FirstPersonCamera()
	:
	Camera()
{}

FirstPersonCamera::~FirstPersonCamera() {}

void FirstPersonCamera::SetPosition(const DirectX::XMFLOAT3& pos) {
	_Position = pos;
}

void XM_CALLCONV FirstPersonCamera::SetLookAt(const DirectX::XMVECTOR& pos, const DirectX::XMVECTOR& focus, const DirectX::XMVECTOR& upDir) {
	using namespace DirectX;
	XMVECTOR l = XMVector3Normalize(focus - pos);
	XMVECTOR r = XMVector3Normalize(XMVector3Cross(upDir, l));
	XMVECTOR u = XMVector3Cross(l, r);

	XMStoreFloat3(&_Position, pos);
	XMStoreFloat3(&_LookAt, l);
	XMStoreFloat3(&_UpDirection, u);
	XMStoreFloat3(&_RightDirection, r);
}

void FirstPersonCamera::SetLookAt(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& focus, const DirectX::XMFLOAT3& upDir) {
	using namespace DirectX;
	SetLookAt(XMLoadFloat3(&pos), XMLoadFloat3(&focus), XMLoadFloat3(&upDir));
}

void FirstPersonCamera::Walk(float distance) {
	// Move alone LookAt
	using namespace DirectX;
	XMVECTOR basePos = XMLoadFloat3(&_Position);
	
	// Walk will not change z coordinates;
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR right = XMLoadFloat3(&_RightDirection);
	XMVECTOR headDirection = XMVector3Normalize(XMVector3Cross(right, up));
	XMVECTOR dis = XMVectorReplicate(distance);
	XMStoreFloat3(&_Position, XMVectorMultiplyAdd(dis, headDirection, basePos));
}

void FirstPersonCamera::Pan(float dis) {
	using namespace DirectX;
	XMVECTOR basePos = XMLoadFloat3(&_Position);
	XMVECTOR lookat = XMLoadFloat3(&_LookAt);
	XMVECTOR distant = XMVectorReplicate(dis);

	XMStoreFloat3(&_Position, XMVectorMultiplyAdd(distant, lookat, basePos));
}

void FirstPersonCamera::LeftRightRotate(float radius) {
	using namespace DirectX;
	XMMATRIX rotation = XMMatrixRotationY(radius);
	XMVECTOR up = XMVector3TransformNormal(XMLoadFloat3(&_UpDirection), rotation);
	XMVECTOR right = XMVector3TransformNormal(XMLoadFloat3(&_RightDirection), rotation);
	XMVECTOR look = XMVector3TransformNormal(XMLoadFloat3(&_LookAt), rotation);

	XMStoreFloat3(&_UpDirection, up);
	XMStoreFloat3(&_RightDirection, right);
	XMStoreFloat3(&_LookAt, look);
}

void FirstPersonCamera::UpDownRotate(float radius) {
	using namespace DirectX;
	XMMATRIX rotationMatrix = XMMatrixRotationAxis(XMLoadFloat3(&_RightDirection), radius);
	XMVECTOR upDir = XMVector3TransformNormal(XMLoadFloat3(&_UpDirection), rotationMatrix);
	XMVECTOR lookAt = XMVector3TransformNormal(XMLoadFloat3(&_LookAt), rotationMatrix);
	float yOfLook = XMVectorGetY(lookAt);
	//Restrict Up Down Rotate to + - pi / 6 = 30degree
	if (fabs(yOfLook) > cosf(XM_PI / 6))
		return;
	XMStoreFloat3(&_LookAt, lookAt);
	XMStoreFloat3(&_UpDirection, upDir);
}

void FirstPersonCamera::UpdateViewMatrix() {
	using namespace DirectX;
	XMVECTOR up = XMLoadFloat3(&_UpDirection);
	XMVECTOR right = XMLoadFloat3(&_RightDirection);
	XMVECTOR look = XMLoadFloat3(&_LookAt);
	XMVECTOR pos = XMLoadFloat3(&_Position);

	look = XMVector3Normalize(look);
	up = XMVector3Normalize(XMVector3Cross(look, right));
	right = XMVector3Cross(up, look);
	// Update vectors
	XMStoreFloat3(&_LookAt, look); 
	XMStoreFloat3(&_UpDirection, up);
	XMStoreFloat3(&_RightDirection, right);
	
	// Calculate View Matrix 英文原版书P431
	float Qx = -XMVectorGetX(XMVector3Dot(pos, right));
	float Qy = -XMVectorGetX(XMVector3Dot(pos, up));
	float Qz = -XMVectorGetX(XMVector3Dot(pos, look));

	_ViewMatrix = {
		_RightDirection.x, _UpDirection.x, _LookAt.x, 0.0,
		_RightDirection.y, _UpDirection.y, _LookAt.y, 0.0,
		_RightDirection.z, _UpDirection.z, _LookAt.z, 0.0,
		Qx,                Qy,             Qz,        1.0f
	};

}