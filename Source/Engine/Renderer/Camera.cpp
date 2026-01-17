#include "Engine/Renderer/Camera.h"

namespace Engine::Renderer {
    
    Camera::Camera() {
        Position = Math::Vector3(0.0f, 0.0f, 0.0f);
        Rotation = Math::Vector3(0.0f, 0.0f, 0.0f);
        ProjectionMatrix = Math::Matrix4::Identity();
    }

    Camera::~Camera() {
    }

    void Camera::SetPerspective(f32 Fov, f32 AspectRatio, f32 NearPlane, f32 FarPlane) {
        ProjectionMatrix = Math::Perspective(Math::Radians(Fov), AspectRatio, NearPlane, FarPlane);
    }

    void Camera::SetOrthographic(f32 Width, f32 Height, f32 NearPlane, f32 FarPlane) {
        ProjectionMatrix = Math::Orthographic(Width, Height, NearPlane, FarPlane);
    }

    const Math::Matrix4& Camera::GetProjectionMatrix() const {
        return ProjectionMatrix;
    }

    Math::Matrix4 Camera::GetViewMatrix() const {
        Math::Vector3 Forward = GetForwardVector();
        Math::Vector3 WorldUp = Math::Vector3(0.0f, 1.0f, 0.0f);
        Math::Vector3 Target = Position + Forward;
        return Math::LookAt(Position, Target, WorldUp);
    }

    Math::Vector3 Camera::GetForwardVector() const {
        f32 Pitch = Math::Radians(Rotation.x);
        f32 Yaw = Math::Radians(Rotation.y);

        Math::Vector3 Forward;
        Forward.x = Math::Cos(Pitch) * Math::Sin(Yaw);
        Forward.y = Math::Sin(Pitch);
        Forward.z = -Math::Cos(Pitch) * Math::Cos(Yaw);
        return Math::Normalize(Forward);
    }

    Math::Vector3 Camera::GetRightVector() const {
        Math::Vector3 Forward = GetForwardVector();
        Math::Vector3 WorldUp = Math::Vector3(0.0f, 1.0f, 0.0f);
        Math::Vector3 Right = Math::Cross(Forward, WorldUp);
        return Math::Normalize(Right);
    }

    Math::Vector3 Camera::GetUpVector() const {
        Math::Vector3 Forward = GetForwardVector();
        Math::Vector3 Right = GetRightVector();
        Math::Vector3 Up = Math::Cross(Right, Forward);
        return Math::Normalize(Up);
    }

}