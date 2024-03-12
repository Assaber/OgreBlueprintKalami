#pragma once
#include "BlueprintLoader.h"
#include "container/BKCard.h"

class OgreBlueprint : public BlueprintLoader
{
public:
    OgreBlueprint(QWidget* parent = nullptr);

protected:
    virtual void keyPressEvent(QKeyEvent* event) override;

};