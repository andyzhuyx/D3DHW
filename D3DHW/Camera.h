#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
class Camera {
protected:
	DirectX::XMFLOAT3 _Position;
	DirectX::XMFLOAT3 _RightDirection;
	DirectX::XMFLOAT3 _UpDirection;
	DirectX::XMFLOAT3 _LookAt; // It's a vector

	// spectrum
	float _AspectRatio;
	float _ViewAngleY;
	float _NearPlaneZ;
	float _FarPlaneZ;
	float _NearPlaneHeight;
	float _FarPlaneHeight;

	DirectX::XMFLOAT4X4 _ViewMatrix;
	DirectX::XMFLOAT4X4 _ProjMatrix;
	D3D11_VIEWPORT _Viewport;

public:
	Camera();
	virtual ~Camera();
	DirectX::XMFLOAT3 GetPos()const;
	DirectX::XMFLOAT3 GetRightDir()const;
	DirectX::XMFLOAT3 GetUpDir()const;
	DirectX::XMFLOAT3 GetLookAt()const;

	float GetAspectRatio()const;
	float GetViewAngleY()const;
	float GetNearPlaneZ()const;
	float GetFarPlaneZ()const;
	float GetNearPlaneHeight()const;
	float GetFarPlaneHeight()const;

	DirectX::XMFLOAT4X4 GetViewMatrix()const;
	DirectX::XMFLOAT4X4 GetProjMatrix()const;
	D3D11_VIEWPORT GetViewport()const;

	void SetFrustum(float viewAngleY, float aspectRatio, float nearPlaneZ, float  farPlaneZ);
	void SetViewPort(const D3D11_VIEWPORT& viewport);
	void SetViewPort(float topLeftX, float topLeftY, float width, float height, float minDepth = 0.0f, float maxDepth = 1.0f);

	virtual void UpdateViewMatrix() = 0;
};

class FirstPersonCamera : public Camera {
public:
	FirstPersonCamera();
	~FirstPersonCamera() override;
	void SetPosition(const DirectX::XMFLOAT3& pos);
	void XM_CALLCONV SetLookAt(const DirectX::XMVECTOR& pos, const DirectX::XMVECTOR& focus, const DirectX::XMVECTOR& upDir);

	void Walk(float dis);
	void Pan(float dis);
	void LeftRightRotate(float radius);
	void UpDownRotate(float radius);

	void UpdateViewMatrix() override;
};