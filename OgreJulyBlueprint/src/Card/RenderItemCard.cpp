#include "RenderItemCard.h"
#include "unit/BKLabel.h"
#include "unit/BKComboBox.h"
#include "OgreRoot.h"
#include "OgreWidget.h"
#include <QDebug>

const std::map<QString, QString> RenderItemCard::mRenderItemList = {
    {"Sphere", "sphere.mesh"},
    {"Cylinder", "cylinder.mesh"},
    {"Cube", "cube.mesh"},
    {"Cone", "cone.mesh"},
    {"Torus", "torus.mesh"},
    {"Plane", "plane.mesh"},
    {"Disc", "disc.mesh"},
    {"Knot", "knot.mesh"},
};

RenderItemCard::RenderItemCard()
    : BKCard()
    , mpSceneManager(Ogre::Root::getSingletonPtr()->getSceneManager(OgreWidget::strSceneMgrName))
{
    setTitle("Render item");

    QStringList list;
    for (const auto& item : mRenderItemList)
        list << item.first;

    _pack(
        {
            BKCreator::create(BKAnchor::AnchorType::None)->append({ BKCreator::create<BKLabel>()->setText("Type")}),
            BKCreator::create(BKAnchor::AnchorType::None)
                ->append({ BKCreator::create<BKComboBox>()
                                ->setItems(list)
                                ->setMinWidth(140)
                                ->setDataChangeCallback(std::bind(&RenderItemCard::renderItemChange, this, std::placeholders::_1, std::placeholders::_2))
                                ->setCurrentIndex(0)
                }),
            BKCreator::create(BKAnchor::AnchorType::None)->append({ BKCreator::create<BKLabel>()->setText("Material")}),
            BKCreator::create(BKAnchor::AnchorType::Input)
                ->setDataType(BKAnchor::Input, BKAnchor::String)
                ->append(BKCreator::create<BKComboBox>()
                                ->setItems(QStringList() << "snow")
                                ->setDataChangeCallback(std::bind(&RenderItemCard::materialChange, this, std::placeholders::_1, std::placeholders::_2))
                                ->setCurrentIndex(0)
                ),
        }
    );
}

RenderItemCard::~RenderItemCard()
{
    destroyRenderItem();
}

bool RenderItemCard::renderItemChange(BKUnit* unit, const QVariant& param)
{
    destroyRenderItem();

    QString type = param.toString();
    auto itor = mRenderItemList.find(type);             // There shouldn't be any mischievous ones
                                                        // Ok, myself... 2024.06.26
    if (itor == mRenderItemList.end())
        return false;

    mpRenderItem = mpSceneManager->createItem(itor->second.toStdString(),
        Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
        Ogre::SCENE_DYNAMIC);

    Ogre::SceneNode* rootNode = mpSceneManager->getRootSceneNode();
    rootNode->attachObject(mpRenderItem);

    updateFullCard();

    return true;
}

bool RenderItemCard::materialChange(BKUnit* unit, const QVariant& param)
{
    mLastMaterialName = param.toString();

    if (mpRenderItem) {
        mpRenderItem->setDatablockOrMaterialName(mLastMaterialName.toStdString());
    }

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
    materialChange(nullptr, mLastMaterialName);
}
