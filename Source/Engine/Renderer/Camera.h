#pragma once

#include "Engine/Math/Math.h"

namespace Engine::Renderer {

    class Camera {
    public:
        Math::Vector3 Position;
        Math::Vector3 Rotation;

    public:
        Camera();
        ~Camera();

        void SetPerspective(f32 FOVDegrees, f32 AspectRatio, f32 NearPlane, f32 FarPlane);
        void SetOrthographic(f32 Width, f32 Height, f32 NearPlane, f32 FarPlane);

        const Math::Matrix4& GetProjectionMatrix() const;
        Math::Matrix4 GetViewMatrix() const;

        Math::Vector3 GetForwardVector() const;
        Math::Vector3 GetRightVector() const;
        Math::Vector3 GetUpVector() const;

    private:
        Math::Matrix4 ProjectionMatrix;
    };

}