﻿#pragma once
#include "BlueprintLoader.h"
#include "OgreColourValue.h"

class BlueprintEditor : public BlueprintLoader
{
    using super = BlueprintLoader;

public:
    BlueprintEditor(QWidget* parent = nullptr);

    static Ogre::ColourValue toColor(const QColor& color);
    static Ogre::Vector3 toVec3f(const QColor& color);

protected:
    virtual void keyPressEvent(QKeyEvent* event) override;
};