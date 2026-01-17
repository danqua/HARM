#include "Engine/Renderer/Camera.h"

namespace Hx {
    
    Camera::Camera() {
        position = Hx::Vector3(0.0f, 0.0f, 0.0f);
        rotation = Hx::Vector3(0.0f, 0.0f, 0.0f);
        projectionMatrix = Hx::Matrix4::Identity();
    }

    Camera::~Camera() {
    }

    void Camera::SetPerspective(f32 Fov, f32 AspectRatio, f32 NearPlane, f32 FarPlane) {
        projectionMatrix = Hx::Perspective(Hx::Radians(Fov), AspectRatio, NearPlane, FarPlane);
    }

    void Camera::SetOrthographic(f32 Width, f32 Height, f32 NearPlane, f32 FarPlane) {
        projectionMatrix = Hx::Orthographic(Width, Height, NearPlane, FarPlane);
    }

    const Hx::Matrix4& Camera::GetProjectionMatrix() const {
        return projectionMatrix;
    }

    Hx::Matrix4 Camera::GetViewMatrix() const {
        Hx::Vector3 forward = GetForwardVector();
        Hx::Vector3 worldUp = Hx::Vector3(0.0f, 1.0f, 0.0f);
        Hx::Vector3 target = position + forward;
        return Hx::LookAt(position, target, worldUp);
    }

    Hx::Vector3 Camera::GetForwardVector() const {
        f32 pitch = Hx::Radians(rotation.x);
        f32 yaw = Hx::Radians(rotation.y);

        Hx::Vector3 forward;
        forward.x = Hx::Cos(pitch) * Hx::Sin(yaw);
        forward.y = Hx::Sin(pitch);
        forward.z = -Hx::Cos(pitch) * Hx::Cos(yaw);
        return Hx::Normalize(forward);
    }

    Hx::Vector3 Camera::GetRightVector() const {
        Hx::Vector3 forward = GetForwardVector();
        Hx::Vector3 worldUp = Hx::Vector3(0.0f, 1.0f, 0.0f);
        Hx::Vector3 right = Hx::Cross(forward, worldUp);
        return Hx::Normalize(right);
    }

    Hx::Vector3 Camera::GetUpVector() const {
        Hx::Vector3 forward = GetForwardVector();
        Hx::Vector3 right = GetRightVector();
        Hx::Vector3 up = Hx::Cross(right, forward);
        return Hx::Normalize(up);
    }

}