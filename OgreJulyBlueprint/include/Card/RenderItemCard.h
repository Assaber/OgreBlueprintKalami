#pragma once
#include "container/BKCard.h"
#include "OgreItem.h"
#include "OgreSceneManager.h"
#include <QStringList>

class RenderItemCard : public BKCard
{
    CREATE_CARD_FACTORY(RenderItemCard)
public:
    RenderItemCard();
    ~RenderItemCard();

private:
    bool renderItemChange(const QVariant& param);
    bool materialChange(const QVariant& param);
    void destroyRenderItem();
    void updateFullCard();

private:
    static const std::map<QString, QString> mRenderItemList;
    Ogre::Item* mpRenderItem = nullptr;
    Ogre::SceneManager* mpSceneManager = nullptr;
    QString mLastMaterialName = "";
};