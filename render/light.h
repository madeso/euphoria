#ifndef EUPHORIA_LIGHT_H
#define EUPHORIA_LIGHT_H

#include "core/vec3.h"
#include "core/rgb.h"
#include "core/angle.h"

namespace euphoria::render
{
    struct LightAttenuation
    {
    public:
        LightAttenuation();

        float
        GetConstant() const;

        float
        GetLinear() const;

        float
        GetQuadratic() const;

    private:
        float constant_;
        float linear_;
        float quadratic_;
    };

    struct Light
    {
    public:
        Light();

        // todo: move this to a light def file and let it be specified in a editor

        enum class Type : int
        {
            Directional,
            Point
        };

        void
        SetType(Type type);


        Type
        GetType() const;


        Type*
        GetTypeMod();


        // todo: replace with public variable instead
        void
        SetPosition(const core::vec3f& pos);


        const core::vec3f&
        GetPosition() const;


        void
        SetDirection(const core::unit3f& direction);


        const core::unit3f&
        GetDirection() const;


        void
        SetAmbient(const core::Rgb& c);


        const core::Rgb&
        GetAmbient() const;


        core::Rgb*
        ModifyAmbient();


        void
        SetDiffuse(const core::Rgb& c);


        const core::Rgb&
        GetDiffuse() const;


        core::Rgb*
        ModifyDiffuse();


        void
        SetSpecular(const core::Rgb& c);


        const core::Rgb&
        GetSpecular() const;


        core::Rgb*
        ModifySpecular();


        const LightAttenuation&
        GetAttenuation() const;


        const core::Angle&
        GetCutoffAngleOuter() const;


        void
        SetCutoffAngleOuter(const core::Angle& angle);


        core::Angle*
        GetCutoffAngleOuterMod();


        const core::Angle&
        GetCutoffAngleInner() const;


        void
        SetCutoffAngleInner(const core::Angle& angle);


        core::Angle*
        GetCutoffAngleInnerMod();

    private:
        Type         type_;
        core::vec3f  position_;
        core::unit3f direction_;
        core::Rgb    ambient_;
        core::Rgb    diffuse_;
        core::Rgb    specular_;
        core::Angle  cutoffAngleOuter_;
        core::Angle  cutoffAngleInner_;

        LightAttenuation attenuation_;
    };

}  // namespace euphoria::render

#endif  // EUPHORIA_LIGHT_H
