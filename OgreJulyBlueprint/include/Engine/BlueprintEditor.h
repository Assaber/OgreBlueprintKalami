#pragma once
#include "BlueprintLoader.h"
#include "OgreVector3.h"
#include "OgreColourValue.h"

class BlueprintEditor : public BlueprintLoader
{
    using super = BlueprintLoader;

public:
    BlueprintEditor(QWidget* parent = nullptr);

    static Ogre::ColourValue toColor(const QColor& color);
    static Ogre::Vector3 toVec3f(const QColor& color);

public:
    void importScene();
    void exportScene();

protected:
    virtual void keyPressEvent(QKeyEvent* event) override;
};