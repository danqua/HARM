#pragma once

#include "Engine/Math/Math.h"

namespace Hx {

    class Camera {
    public:
        Vector3 position;
        Vector3 rotation;

    public:
        Camera();
        ~Camera();

        void SetPerspective(f32 fovDegrees, f32 aspectRatio, f32 nearPlane, f32 farPlane);
        void SetOrthographic(f32 width, f32 height, f32 nearPlane, f32 farPlane);

        const Matrix4& GetProjectionMatrix() const;
        Matrix4 GetViewMatrix() const;

        Vector3 GetForwardVector() const;
        Vector3 GetRightVector() const;
        Vector3 GetUpVector() const;

    private:
        Matrix4 projectionMatrix;
    };

}