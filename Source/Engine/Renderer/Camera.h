#pragma once

#include "Engine/Math/Math.h"

namespace Hx {

    class Camera {
    public:
        Hx::Vector3 position;
        Hx::Vector3 rotation;

    public:
        Camera();
        ~Camera();

        void SetPerspective(f32 FOVDegrees, f32 AspectRatio, f32 NearPlane, f32 FarPlane);
        void SetOrthographic(f32 Width, f32 Height, f32 NearPlane, f32 FarPlane);

        const Hx::Matrix4& GetProjectionMatrix() const;
        Hx::Matrix4 GetViewMatrix() const;

        Hx::Vector3 GetForwardVector() const;
        Hx::Vector3 GetRightVector() const;
        Hx::Vector3 GetUpVector() const;

    private:
        Hx::Matrix4 projectionMatrix;
    };

}