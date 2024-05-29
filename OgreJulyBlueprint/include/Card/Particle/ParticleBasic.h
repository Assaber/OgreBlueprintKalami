#pragma once
#ifdef OBK_INCLUDE_PARTICLE_PLUGIN
#include "common_obk.h"
#include <QObject>
#include "OgreString.h"
#include "OgreBillboardSet.h"
#include "OgreLinearForceAffector.h"

namespace particle
{
    struct ParticleAffector
    {
        enum Type {
            Default,
            ColourFader,
            ColourFader2,
            ColourImage,
            ColourInterpolator,
            DeflctorPlane,
            DirectionRandomiser,
            LinearForce,
            Rotator,
            Scaler
        };

        Type type = Default;
        void* pObj = nullptr;
    };
    Q_DECLARE_METATYPE(particle::ParticleAffector)

    enum PEmitterType
    {
        Point,
        Box,
        Cylinder,
        Ellipsoid,
        HollowEllipsoid,
        Ring,
    };

    static const QMap<QString, Ogre::v1::BillboardType> billboardName2Type = {
        { "Point", Ogre::v1::BBT_POINT },
        { "Oriented common", Ogre::v1::BBT_ORIENTED_COMMON },
        { "Oriented self", Ogre::v1::BBT_ORIENTED_SELF },
        { "Perpendicular common", Ogre::v1::BBT_PERPENDICULAR_COMMON },
        { "Perpendicular self", Ogre::v1::BBT_PERPENDICULAR_SELF },
    };

    static const QMap<QString, Ogre::v1::BillboardOrigin> billboardName2Origin = {
        { "TopLeft", Ogre::v1::BBO_TOP_LEFT },
        { "TopCenter", Ogre::v1::BBO_TOP_CENTER },
        { "TopRight", Ogre::v1::BBO_TOP_RIGHT },
        { "CenterLeft", Ogre::v1::BBO_CENTER_LEFT },
        { "Center", Ogre::v1::BBO_CENTER },
        { "CenterRight", Ogre::v1::BBO_CENTER_RIGHT },
        { "BottomLeft", Ogre::v1::BBO_BOTTOM_LEFT },
        { "BottomCenter", Ogre::v1::BBO_BOTTOM_CENTER },
        { "BottomRight", Ogre::v1::BBO_BOTTOM_RIGHT },
    };

    static const QMap<QString, Ogre::v1::BillboardRotationType> billboardName2Rotation = {
        { "Texcoord", Ogre::v1::BBR_TEXCOORD },
        { "Vertex", Ogre::v1::BBR_VERTEX },
    };

    static const QMap<QString, PEmitterType> emitterName2Type = {
        { "Point", PEmitterType::Point },
        { "Box", PEmitterType::Box },
        { "Cylinder", PEmitterType::Cylinder },
        { "Ellipsoid", PEmitterType::Ellipsoid },
        { "HollowEllipsoid", PEmitterType::HollowEllipsoid },
        { "Ring", PEmitterType::Ring },
    };

    static const QMap<QString, Ogre::LinearForceAffector::ForceApplication> linearAffectorName2Type = {
        { "Average", Ogre::LinearForceAffector::FA_AVERAGE },
        { "Add", Ogre::LinearForceAffector::FA_ADD },
    };

    inline Ogre::ColourValue qColor2ColourValue(const QColor& color) {
        return Ogre::ColourValue(static_cast<float>(color.red()) / 255,
           static_cast<float>(color.green()) / 255,
           static_cast<float>(color.blue()) / 255,
           static_cast<float>(color.alpha()) / 255 );
    }
}
#endif