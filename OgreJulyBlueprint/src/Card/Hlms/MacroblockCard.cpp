#include "Card/Hlms/MacroblockCard.h"

#include "BKCreator.hpp"
#include "container/BKCell.h"
#include "unit/BKLabel.h"
#include "unit/BKComboBox.h"
#include "unit/BKSpacer.h"
#include "unit/BKCheckBox.h"
#include "BlueprintEditor.h"
#include <QDebug>

static const QMap<QString, Ogre::PolygonMode> name2polygonMode = {
    { "point", Ogre::PolygonMode::PM_POINTS },
    { "wireframe", Ogre::PolygonMode::PM_WIREFRAME },
    { "solid", Ogre::PolygonMode::PM_SOLID },
};

static const QMap<QString, Ogre::CompareFunction> name2compareFunc = {
    { "always fail", Ogre::CompareFunction::CMPF_ALWAYS_FAIL },
    { "always pass", Ogre::CompareFunction::CMPF_ALWAYS_PASS },
    { "less", Ogre::CompareFunction::CMPF_LESS },
    { "less equal", Ogre::CompareFunction::CMPF_LESS_EQUAL },
    { "equal", Ogre::CompareFunction::CMPF_EQUAL },
    { "not equal", Ogre::CompareFunction::CMPF_NOT_EQUAL },
    { "greater equal", Ogre::CompareFunction::CMPF_GREATER_EQUAL },
    { "greater", Ogre::CompareFunction::CMPF_GREATER }
};

static const QMap<QString, Ogre::CullingMode> name2cullingMode = {
    { "none", Ogre::CullingMode::CULL_NONE },
    { "clockwise", Ogre::CullingMode::CULL_CLOCKWISE },
    { "anticlockwise", Ogre::CullingMode::CULL_ANTICLOCKWISE }
};

#define CreateFixedBoolItem(name, bind, root) \
BKCreator::create(BKAnchor::AnchorType::None) \
    ->append( \
        BKCreator::create<BKLabel>() \
        ->setText(name) \
        ->setFixedSize({ 120.0f, BKUnit::minUnitHeight }) \
    ) \
    ->append( \
        BKCreator::create<BKCheckBox>() \
        ->setChecked(bind) \
        ->setDataChangeCallback([this, root](BKUnit* unit, const QVariant& param) -> bool { \
            bind = param.toBool(); \
            root->valueChanged(getCurrentCardValue()); \
            return true; \
            }))

MacroblockCard::MacroblockCard()
{
    setTitle("宏块");

    BKCell* output = BKCreator::create(BKAnchor::AnchorType::Output);
    output->setDataType(BKAnchor::Output, QMetaTypeId<Ogre::HlmsMacroblock>::qt_metatype_id())
        ->append(BKCreator::create<BKLabel>()
            ->setAlignment(Qt::AlignVCenter | Qt::AlignRight)
            ->setText("输出")
            ->setMinWidth(140)
            , false)
        ->getAnchor(BKAnchor::AnchorType::Output)
        ->redirectToCard();

    QStringList compareFuncList = name2compareFunc.keys();
    int initCompareFuncIndex = compareFuncList.indexOf(name2compareFunc.key(mMacroblock.mDepthFunc));

    QStringList cullingModeList = name2cullingMode.keys();
    int initCullingModeIndex = cullingModeList.indexOf(name2cullingMode.key(mMacroblock.mCullMode));

    QStringList polygonModeList = name2polygonMode.keys();
    int initPolygonModeIndex = polygonModeList.indexOf(name2polygonMode.key(mMacroblock.mPolygonMode));


    _pack({
        output,

        CreateFixedBoolItem("裁剪测试",mMacroblock.mScissorTestEnabled, output),
        CreateFixedBoolItem("深度限制",mMacroblock.mDepthClamp, output),
        CreateFixedBoolItem("深度检测",mMacroblock.mDepthCheck, output),
        CreateFixedBoolItem("深度写入",mMacroblock.mDepthWrite, output),

        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>()->setText("比较函数")),
        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKComboBox>()
            ->setItems(compareFuncList)
            ->setCurrentIndex(initCompareFuncIndex)
            ->setCallbackParamType(BKComboBox::CallbackParamType::Data)
            ->setDataChangeCallback([this, output](BKUnit* unit, const QVariant& param) -> bool {
                mMacroblock.mDepthFunc = name2compareFunc[param.toString()];
                output->valueChanged(getCurrentCardValue());
                return true;
                })
            ),

        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>()->setText("裁剪模式")),
        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKComboBox>()
            ->setItems(cullingModeList)
            ->setCurrentIndex(initCullingModeIndex)
            ->setCallbackParamType(BKComboBox::CallbackParamType::Data)
            ->setDataChangeCallback([this, output](BKUnit* unit, const QVariant& param) -> bool {
                mMacroblock.mCullMode = name2cullingMode[param.toString()];
                output->valueChanged(getCurrentCardValue());
                return true;
                })
            ),

        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>()->setText("多边形模式")),
        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKComboBox>()
            ->setItems(polygonModeList)
            ->setCurrentIndex(initPolygonModeIndex)
            ->setCallbackParamType(BKComboBox::CallbackParamType::Data)
            ->setDataChangeCallback([this, output](BKUnit* unit, const QVariant& param) -> bool {
                mMacroblock.mPolygonMode = name2polygonMode[param.toString()];
                output->valueChanged(getCurrentCardValue());
                return true;
                })
            ),
        });
}

QVariant MacroblockCard::getCurrentCardValue()
{
    return QVariant::fromValue(mMacroblock);
}