#include "Engine/Renderer/Camera.h"

namespace Hx {
    
    Camera::Camera() {
        position = Vector3(0.0f, 0.0f, 0.0f);
        rotation = Vector3(0.0f, 0.0f, 0.0f);
        projectionMatrix = Matrix4::Identity();
    }

    Camera::~Camera() {
    }

    void Camera::SetPerspective(f32 fov, f32 aspectRatio, f32 nearPlane, f32 farPlane) {
        projectionMatrix = Perspective(Radians(fov), aspectRatio, nearPlane, farPlane);
    }

    void Camera::SetOrthographic(f32 width, f32 height, f32 nearPlane, f32 farPlane) {
        projectionMatrix = Orthographic(width, height, nearPlane, farPlane);
    }

    const Matrix4& Camera::GetProjectionMatrix() const {
        return projectionMatrix;
    }

    Matrix4 Camera::GetViewMatrix() const {
        Vector3 forward = GetForwardVector();
        Vector3 worldUp = Vector3(0.0f, 1.0f, 0.0f);
        Vector3 target = position + forward;
        return LookAt(position, target, worldUp);
    }

    Vector3 Camera::GetForwardVector() const {
        f32 pitch = Radians(rotation.x);
        f32 yaw = Radians(rotation.y);

        Vector3 forward;
        forward.x = Cos(pitch) * Sin(yaw);
        forward.y = Sin(pitch);
        forward.z = -Cos(pitch) * Cos(yaw);
        return Normalize(forward);
    }

    Vector3 Camera::GetRightVector() const {
        Vector3 forward = GetForwardVector();
        Vector3 worldUp = Vector3(0.0f, 1.0f, 0.0f);
        Vector3 right = Cross(forward, worldUp);
        return Normalize(right);
    }

    Vector3 Camera::GetUpVector() const {
        Vector3 forward = GetForwardVector();
        Vector3 right = GetRightVector();
        Vector3 up = Cross(right, forward);
        return Normalize(up);
    }

}