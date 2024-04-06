﻿#include "RenderItemCard.h"
#include "unit/BKLabel.h"
#include "unit/BKComboBox.h"
#include "OgreRoot.h"
#include "OgreWidget.h"
#include <QDebug>

const std::map<QString, QString> RenderItemCard::mRenderItemList = {
    {"球体", "sphere.mesh"},
    {"圆柱体", "cylinder.mesh"},
    {"立方体", "cube.mesh"},
    {"锥体", "cone.mesh"},
    {"甜甜圈", "torus.mesh"},
    {"平面", "plane.mesh"},
    {"圆面", "disc.mesh"},
    {"情比金坚七天锁", "knot.mesh"},
};

RenderItemCard::RenderItemCard()
    : BKCard()
    , mpSceneManager(Ogre::Root::getSingletonPtr()->getSceneManager(OgreWidget::strSceneMgrName))
{
    setTitle("渲染对象");

    QStringList list;
    for (const auto& item : mRenderItemList)
        list << item.first;

    _pack(
        {
            BKCreator::create(BKAnchor::AnchorType::None)->append({ BKCreator::create<BKLabel>()->setText("对象类别")}),
            BKCreator::create(BKAnchor::AnchorType::None)
                // ->append({ BKCreator::create<BKLabel>()->setText("对象类别")->setFixedSize({60, 20})})
                ->append({ BKCreator::create<BKComboBox>()
                                ->setItems(list)
                                ->setMinWidth(140)
                                ->setDataChangeCallback(std::bind(&RenderItemCard::renderItemChange, this, std::placeholders::_1))
                                ->setCurrentIndex(0)
                }),
            BKCreator::create(BKAnchor::AnchorType::None)->append({ BKCreator::create<BKLabel>()->setText("材质")}),
            BKCreator::create(BKAnchor::AnchorType::Input)
                ->setDataType(BKAnchor::Input, BKAnchor::String)
                // ->append({ BKCreator::create<BKLabel>()->setText("材质")->setFixedSize({60, 20})})
                ->append({ BKCreator::create<BKComboBox>()
                                ->setItems(QStringList() << "snow")
                                ->setDataChangeCallback(std::bind(&RenderItemCard::materialChange, this, std::placeholders::_1))
                                ->setCurrentIndex(0)
                }),
        }
    );
}

RenderItemCard::~RenderItemCard()
{
    destroyRenderItem();
}

bool RenderItemCard::renderItemChange(const QVariant& param)
{
    QString type = param.toString();
    auto itor = mRenderItemList.find(type);            // 应该没有调皮捣蛋的吧

    destroyRenderItem();
    mpRenderItem = mpSceneManager->createItem(itor->second.toStdString(),
        Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
        Ogre::SCENE_DYNAMIC);
    Ogre::SceneNode* rootNode = mpSceneManager->getRootSceneNode();
    rootNode->attachObject(mpRenderItem);

    updateFullCard();

    return true;
}

bool RenderItemCard::materialChange(const QVariant& param)
{
    mLastMaterialName = param.toString();

    if (mpRenderItem)
        mpRenderItem->setDatablockOrMaterialName(mLastMaterialName.toStdString());

    return true;
}

void RenderItemCard::destroyRenderItem()
{
    if (mpRenderItem)
    {
        mpSceneManager->destroyItem(mpRenderItem);
        mpRenderItem = nullptr;
    }
}

void RenderItemCard::updateFullCard()
{
    materialChange(mLastMaterialName);
}
