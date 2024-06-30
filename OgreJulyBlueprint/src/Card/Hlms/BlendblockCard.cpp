﻿#include "Card/Hlms/BlendblockCard.h"

#include "BKCreator.hpp"
#include "container/BKCell.h"
#include "unit/BKLabel.h"
#include "unit/BKComboBox.h"
#include "unit/BKSpacer.h"
#include "unit/BKCheckBox.h"
#include "BlueprintEditor.h"
#include <QDebug>

const QMap<QString, Ogre::SceneBlendType> name2SceneBlendType = {
    {"Transparent Alpha", Ogre::SceneBlendType::SBT_TRANSPARENT_ALPHA },
    {"Transparent Color", Ogre::SceneBlendType::SBT_TRANSPARENT_COLOUR },
    {"Add", Ogre::SceneBlendType::SBT_ADD },
    {"Modulate", Ogre::SceneBlendType::SBT_MODULATE },
    {"Replace", Ogre::SceneBlendType::SBT_REPLACE },
};

BlendblockCard::BlendblockCard()
{
    setTitle("Blendblock");

    BKCell* output = BKCreator::create(BKAnchor::AnchorType::Output);
    output->setDataType(BKAnchor::Output, QMetaTypeId<Ogre::HlmsBlendblock>::qt_metatype_id())
        ->append(BKCreator::create<BKLabel>()
            ->setAlignment(Qt::AlignVCenter | Qt::AlignRight)
            ->setText("Output")
            ->setMinWidth(140)
        , false)
        ->getAnchor(BKAnchor::AnchorType::Output)
        ->redirectToCard();

    BKCheckBox*& redChannel = mChannelMaskCheckBoies[0];
    redChannel = BKCreator::create<BKCheckBox>();
    redChannel->setChecked(mBlendblock.mBlendChannelMask & Ogre::HlmsBlendblock::BlendChannelRed);
    redChannel->setDataChangeCallback([this, redChannel, output](BKUnit* unit, const QVariant& param) -> bool {
        updateBlendChannelMask(redChannel, param.toBool());
        output->valueChanged(getCurrentCardValue());
        return true;
        });

    BKCheckBox*& greenChannel = mChannelMaskCheckBoies[1];
    greenChannel = BKCreator::create<BKCheckBox>();
    greenChannel->setChecked(mBlendblock.mBlendChannelMask & Ogre::HlmsBlendblock::BlendChannelGreen);
    greenChannel->setDataChangeCallback([this, greenChannel, output](BKUnit* unit, const QVariant& param) -> bool {
        updateBlendChannelMask(greenChannel, param.toBool());
        output->valueChanged(getCurrentCardValue());
        return true;
        });

    BKCheckBox*& blueChannel = mChannelMaskCheckBoies[2];
    blueChannel = BKCreator::create<BKCheckBox>();
    blueChannel->setChecked(mBlendblock.mBlendChannelMask & Ogre::HlmsBlendblock::BlendChannelBlue);
    blueChannel->setDataChangeCallback([this, blueChannel, output](BKUnit* unit, const QVariant& param) -> bool {
        updateBlendChannelMask(blueChannel, param.toBool());
        output->valueChanged(getCurrentCardValue());
        return true;
        });

    BKCheckBox*& alphaChannel = mChannelMaskCheckBoies[3];
    alphaChannel = BKCreator::create<BKCheckBox>();
    alphaChannel->setChecked(mBlendblock.mBlendChannelMask & Ogre::HlmsBlendblock::BlendChannelAlpha);
    alphaChannel->setDataChangeCallback([this, alphaChannel, output](BKUnit* unit, const QVariant& param) -> bool {
        updateBlendChannelMask(alphaChannel, param.toBool());
        output->valueChanged(getCurrentCardValue());
        return true;
        });

    QStringList blendTypeList = name2SceneBlendType.keys();

    _pack({
        output,

         BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>()->setText("Alpha to coverage")),
         BKCreator::create(BKAnchor::AnchorType::None)->append(
                BKCreator::create<BKCheckBox>()
                    ->setChecked(mBlendblock.mAlphaToCoverageEnabled)
                    ->setDataChangeCallback([this, output](BKUnit* unit, const QVariant& param) -> bool {
                        mBlendblock.mAlphaToCoverageEnabled = param.toBool();
                        output->valueChanged(getCurrentCardValue());
                        return true;
                    })
        ),

        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>()->setText("Channel mask")),
        BKCreator::create(BKAnchor::AnchorType::None)->append({
                BKCreator::create<BKLabel>()->setAlignment(Qt::AlignCenter)->setText("Red"),
                BKCreator::create<BKLabel>()->setAlignment(Qt::AlignCenter)->setText("Green"),
                BKCreator::create<BKLabel>()->setAlignment(Qt::AlignCenter)->setText("Blue"),
                BKCreator::create<BKLabel>()->setAlignment(Qt::AlignCenter)->setText("Alpha"),
            }),
         BKCreator::create(BKAnchor::AnchorType::None)->append({
               { redChannel },
               { greenChannel },
               { blueChannel },
               { alphaChannel },
            }),

        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>()->setText("Transparent option")),
        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKComboBox>()
            ->setItems(QStringList() << "Not" << "Automatically" << "Forced" << "A & F")
            ->setCurrentIndex(mBlendblock.mIsTransparent)
            ->setCallbackParamType(BKComboBox::CallbackParamType::Index)
            ->setDataChangeCallback([this, output](BKUnit* unit, const QVariant& param) -> bool {
                mBlendblock.mIsTransparent = param.toInt();
                output->valueChanged(getCurrentCardValue());
                return true;
                })
            ),

        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>()->setText("Blend type")),
        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKComboBox>()
            ->setItems(blendTypeList)
            ->setCurrentIndex(blendTypeList.indexOf(name2SceneBlendType.key(mBlendType)))
            ->setDataChangeCallback([this, output](BKUnit* unit, const QVariant& param) -> bool {
                mBlendType = name2SceneBlendType[param.toString()];
                updateBlendType();
                output->valueChanged(getCurrentCardValue());
                return true;
                })
            ),

        BKCreator::create(BKAnchor::AnchorType::None)->append({
                BKCreator::create<BKLabel>()->setText("Alpha blend type"),
            { BKCreator::create<BKCheckBox>()
                    ->setChecked(mbAlphaBlendTypeEnable)
                    ->setDataChangeCallback([this, output](BKUnit* unit, const QVariant& param) -> bool {
                        mbAlphaBlendTypeEnable = param.toBool();
                        updateBlendType();
                        output->valueChanged(getCurrentCardValue());
                        return true;
                    }), true}
            }),
        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKComboBox>()
            ->setItems(blendTypeList)
            ->setCurrentIndex(blendTypeList.indexOf(name2SceneBlendType.key(mAlphaBlendType)))
            ->setDataChangeCallback([this, output](BKUnit* unit, const QVariant& param) -> bool {
                mAlphaBlendType = name2SceneBlendType[param.toString()];
                updateBlendType();
                output->valueChanged(getCurrentCardValue());
                return true;
                })
            ),
        });
}

QVariant BlendblockCard::getCurrentCardValue()
{
    return QVariant::fromValue(mBlendblock);
}

void BlendblockCard::updateBlendChannelMask(BKCheckBox* ptr, bool enable)
{
    auto update_mask = [&](Ogre::HlmsBlendblock::BlendChannelMasks target) {
        if (enable) {
            mBlendblock.mBlendChannelMask |= target;
        }
        else {
            mBlendblock.mBlendChannelMask &= ~target;
        }
    };

    if (ptr == mChannelMaskCheckBoies[0]) {
        update_mask(Ogre::HlmsBlendblock::BlendChannelRed);
    }
    else if(ptr == mChannelMaskCheckBoies[1]) {
        update_mask(Ogre::HlmsBlendblock::BlendChannelGreen);
    }
    else if (ptr == mChannelMaskCheckBoies[2]) {
        update_mask(Ogre::HlmsBlendblock::BlendChannelBlue);
    }
    else if (ptr == mChannelMaskCheckBoies[3]) {
        update_mask(Ogre::HlmsBlendblock::BlendChannelAlpha);
    }
}

void BlendblockCard::updateBlendType()
{
    if (mbAlphaBlendTypeEnable) {
        mBlendblock.setBlendType(static_cast<Ogre::SceneBlendType>(mBlendType), static_cast<Ogre::SceneBlendType>(mAlphaBlendType));
    }
    else {
        mBlendblock.setBlendType(static_cast<Ogre::SceneBlendType>(mBlendType));
    }
}
