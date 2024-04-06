#pragma once
#include "BlueprintLoader.h"

class BlueprintEditor : public BlueprintLoader
{
    using super = BlueprintLoader;
public:
    BlueprintEditor(QWidget* parent = nullptr);

protected:
    virtual void keyPressEvent(QKeyEvent* event) override;
};