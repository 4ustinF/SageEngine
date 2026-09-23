#include "Precompiled.h"
#include "Camera.h"

#include "GraphicsSystem.h"

using namespace SAGE;
using namespace SAGE::Math;
using namespace SAGE::Graphics;

void Camera::SetMode(ProjectionMode mode)
{
	mProjectionMode = mode;
}

void Camera::SetPosition(const Vector3& position)
{
	mPosition = position;
}

void Camera::SetDirection(const Vector3& direction)
{
	if (MagnitudeSqr(direction) <= 0.0f) 
	{
		return; // reject only a genuinely zero-length input
	}
	const Vector3 newDir = Normalize(direction);

	Vector3 right;
	const Vector3 worldCross = Cross(Vector3::YAxis, newDir);
	constexpr float kNearPoleThresholdSqr = 0.0025f; // matches the instability band the old 0.995f guard was protecting against

	if (MagnitudeSqr(worldCross) > kNearPoleThresholdSqr)
	{
		// Away from the poles: always rebuild from world-up.
		// This is what re-levels the camera and erases any roll
		// that crept in from an earlier pole crossing.
		right = Normalize(worldCross);
	}
	else
	{
		// Near/at a pole: Cross(YAxis, newDir) is unstable — fall back
		// to the previous right vector, projected to stay perpendicular
		// to newDir, purely to preserve continuity through the pole.
		const Vector3 prevRight = GetRight();
		Vector3 projected = prevRight - newDir * Dot(prevRight, newDir);
		if (MagnitudeSqr(projected) < 0.0001f) 
		{
			projected = Cross(Vector3::ZAxis, newDir); // only hit on a fresh camera whose direction is already vertical
		}
		right = Normalize(projected);
	}

	const Vector3 up = Normalize(Cross(newDir, right));

	const Matrix4 basis = {
		right.x,  right.y,  right.z,  0.0f,
		up.x,     up.y,     up.z,     0.0f,
		newDir.x, newDir.y, newDir.z, 0.0f,
		0.0f,     0.0f,     0.0f,     1.0f
	};

	mOrientation = Normalize(Quaternion::RotationMatrix(basis));
	UpdateDirection();
}

void Camera::SetLookAt(const Vector3& target)
{
	SetDirection(target - mPosition);
}

void Camera::SetOrientation(const Quaternion& orientation)
{
	mOrientation = orientation;
	UpdateDirection();
}

void Camera::SetFov(float fov)
{
	mFov = Clamp(fov, mMinFov, mMaxFov);
}

void Camera::SetFovInDegrees(float fov)
{
	SetFov(fov * Math::Constants::DegToRad);
}

void Camera::SetAspectRatio(float ratio)
{
	mAspectRatio = ratio;
}

void Camera::SetSize(float width, float height)
{
	mWidth = width;
	mHeight = height;
}

void Camera::SetNearPlane(float nearPlane)
{
	mNearPlane = nearPlane;
}

void Camera::SetFarPlane(float farPlane)
{
	mFarPlane = farPlane;
}

void Camera::Walk(float distance)
{
	mPosition += mDirection * distance;
}

void Camera::Strafe(float distance)
{
	const Vector3 right = Normalize(Cross(Vector3::YAxis, mDirection));
	mPosition += right * distance;
}

void Camera::Rise(float distance)
{
	mPosition += Vector3::YAxis * distance;
}

void Camera::Yaw(float radian)
{
	mOrientation = Normalize(Quaternion::RotationAxis(Vector3::YAxis, radian) * mOrientation);
	UpdateDirection();
}

void Camera::Pitch(float radian)
{
	mOrientation = Normalize(Quaternion::RotationAxis(GetRight(), radian) * mOrientation);
	UpdateDirection();
}

void Camera::Zoom(float amount)
{
	SetFov(mFov - amount);
}

const Vector3& Camera::GetPosition() const
{
	return mPosition;
}

const Vector3& Camera::GetDirection() const
{
	return mDirection;
}

const Vector3 Camera::GetDirectionWithoutPitch() const
{
	Vector3 dir = mDirection;
	dir.y = 0.0f;
	return Normalize(dir);
}

const Quaternion& Camera::GetOrientation() const
{
	return mOrientation;
}

Vector3 Camera::GetRight() const 
{
	return mOrientation.Rotate(Vector3::XAxis); 
}

Vector3 Camera::GetUp() const 
{
	return mOrientation.Rotate(Vector3::YAxis); 
}

Matrix4 Camera::GetWorldMatrix() const
{
	const Vector3 l = mDirection;
	const Vector3 r = GetRight();
	const Vector3 u = GetUp();
	return {
		r.x, r.y, r.z, 0.0f,
		u.x, u.y, u.z, 0.0f,
		l.x, l.y, l.z, 0.0f,
		mPosition.x, mPosition.y, mPosition.z, 1.0f
	};
}

Matrix4 Camera::GetViewMatrix() const
{
	const Vector3 l = mDirection;
	const Vector3 r = GetRight();
	const Vector3 u = GetUp();
	const float x = -Dot(r, mPosition);
	const float y = -Dot(u, mPosition);
	const float z = -Dot(l, mPosition);
	return {
		r.x, u.x, l.x, 0.0f,
		r.y, u.y, l.y, 0.0f,
		r.z, u.z, l.z, 0.0f,
		x,   y,   z,   1.0f
	};
}

Matrix4 Camera::GetProjectionMatrix() const
{
	return (mProjectionMode == ProjectionMode::Perspective) ? GetPerspectiveMatrix() : GetOrthographicMatrix();
}

Matrix4 Camera::GetPerspectiveMatrix() const
{
	const float a = (mAspectRatio == 0.0f) ? GraphicsSystem::Get()->GetBackBufferAspectRatio() : mAspectRatio;
	const float h = 1.0f / tan(mFov * 0.5f);
	const float w = h / a;
	const float zf = mFarPlane;
	const float zn = mNearPlane;
	const float q = zf / (zf - zn);
	return {
		w,    0.0f, 0.0f,    0.0f,
		0.0f, h,    0.0f,    0.0f,
		0.0f, 0.0f, q,       1.0f,
		0.0f, 0.0f, -zn * q, 0.0f
	};
}

Matrix4 Camera::GetOrthographicMatrix() const
{
	const float w = (mWidth == 0.0f) ? GraphicsSystem::Get()->GetBackBufferWidth() : mWidth;
	const float h = (mHeight == 0.0f) ? GraphicsSystem::Get()->GetBackBufferHeight() : mHeight;
	const float f = mFarPlane;
	const float n = mNearPlane;
	return 	{
		2 / w, 0.0f,  0.0f,        0.0f,
		0.0f,  2 / h, 0.0f,        0.0f,
		0.0f,  0.0f,  1 / (f - n), 0.0f,
		0.0f,  0.0f,  n / (n - f), 1.0f
	};
}

void Camera::UpdateDirection()
{
	mDirection = mOrientation.Rotate(Vector3::ZAxis);
}