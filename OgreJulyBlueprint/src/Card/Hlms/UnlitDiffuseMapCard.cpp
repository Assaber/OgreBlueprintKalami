#include "Card/Hlms/UnlitDiffuseMapCard.h"

#include "BKCreator.hpp"
#include "container/BKCell.h"
#include "unit/BKLabel.h"
#include "unit/BKComboBox.h"
#include "unit/BKSpacer.h"
#include "unit/BKPixmap.h"
#include "unit/BKColorSelectorEx.h"
#include "BlueprintEditor.h"

#include "OgreResourceManager.h"

#include <QFileInfo>

const QMap<QString, Ogre::UnlitBlendModes> name2UnlitBlendModes = {
    { "NormalNonPremul", Ogre::UNLIT_BLEND_NORMAL_NON_PREMUL },
    { "NormalPremul"   ,  Ogre::UNLIT_BLEND_NORMAL_PREMUL },
    { "Add",  Ogre::UNLIT_BLEND_ADD },
    { "Subtract",  Ogre::UNLIT_BLEND_SUBTRACT },
    { "Multiply",  Ogre::UNLIT_BLEND_MULTIPLY },
    { "Multiply2x",  Ogre::UNLIT_BLEND_MULTIPLY2X },
    { "Screen",  Ogre::UNLIT_BLEND_SCREEN },
    { "Overlay",  Ogre::UNLIT_BLEND_OVERLAY },
    { "Lighten",  Ogre::UNLIT_BLEND_LIGHTEN },
    { "Darken",  Ogre::UNLIT_BLEND_DARKEN },
    { "GrainExtract",  Ogre::UNLIT_BLEND_GRAIN_EXTRACT },
    { "GrainMerge",  Ogre::UNLIT_BLEND_GRAIN_MERGE },
    { "Difference",  Ogre::UNLIT_BLEND_DIFFERENCE }
};

UnlitDiffuseMapCard::UnlitDiffuseMapCard()
    : ResetResourceSup("ResourceGroup.Unlit.Snow")
{
    setTitle("Unlit diffuse");

    BKCell* outputCell = BKCreator::create(BKAnchor::AnchorType::Output);
    outputCell->setDataType(BKAnchor::Output, QMetaTypeId<UnlitDiffuseMapCard::Info>::qt_metatype_id())
        ->append(BKCreator::create<BKLabel>()
            ->setAlignment(Qt::AlignVCenter | Qt::AlignRight)
            ->setText("Output")
            ->setMinWidth(140)
        , false)
        ->getAnchor(BKAnchor::AnchorType::Output)
        ->redirectToCard();

    QStringList blendModes = name2UnlitBlendModes.keys();
    int initBlendModeIndex = blendModes.indexOf(name2UnlitBlendModes.key(mTextureInfo.blendMode));

    QStringList indexList;
    for (int i = 0; i < Ogre::UnlitTextureTypes::NUM_UNLIT_TEXTURE_TYPES; ++i)
        indexList << QString::number(i);

    _pack({
        outputCell,

         BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>()->setText("Index")),
         BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKComboBox>()
            ->setItems(indexList)
            ->setCurrentIndex(0, false)
            ->setDataChangeCallback([this, outputCell](BKUnit* unit, const QVariant& param) -> bool {
                mTextureInfo.index = param.toString().toInt();
                outputCell->valueChanged(mTextureInfo);
                return true;
                })
         ),

        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>()->setText("Pixmap")),
        BKCreator::create(BKAnchor::AnchorType::Input)
                    ->setDataType(BKAnchor::Input, BKAnchor::DataType::String)
                    ->append(BKCreator::create<BKPixmap>()
                ->setFixedSize({100, 100})
                ->setDataChangeCallback([this, outputCell](BKUnit* unit, const QVariant& param) -> bool {
                    if (!param.isValid() || !param.canConvert<QString>())
                    {
                        mLastTextureName = "";
                        mTextureInfo.texture = "";
                        outputCell->valueChanged(mTextureInfo);
                    }
                    else 
                    {
                        QString name = param.toString();

                        if (mLastTextureName.compare(name) == 0) {      // No changes are required
                            return true;
                        }
                        else
                        {
                            if (QFile(name).exists())
                            {
                                Ogre::String texturePath = name.toStdString();
                                resetResourceDir(texturePath);
                            }
                            else  {  
                                // In this case, it is considered as the texture is coming in from the connection line, not the resource
                                // Such as canvas, it has a inner texture, so we only need update input name
                                mLastTextureName = name;
                            }

                            mTextureInfo.texture = QFileInfo(name).fileName().toStdString();
                            outputCell->valueChanged(mTextureInfo);
                        }
                    }

                    return true;
                    })
            ),

        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>()->setText("Blend mode")),
        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKComboBox>()
            ->setItems(blendModes)
            ->setCurrentIndex(initBlendModeIndex, false)
            ->setDataChangeCallback([this, outputCell](BKUnit* unit, const QVariant& param) -> bool {
                mTextureInfo.blendMode = name2UnlitBlendModes[param.toString()];
                outputCell->valueChanged(mTextureInfo);
                return true;
                })
            ),

        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>()->setText("UV")),
        BKCreator::create(BKAnchor::AnchorType::None)
            ->append(BKCreator::create<BKComboBox>()
                ->setItems(QStringList() << "0" << "1" << "2" << "3" << "4" << "5" << "6" << "7")
                ->setCurrentItem(0, false)
                ->setDataChangeCallback([this, outputCell](BKUnit* unit, const QVariant& param) -> bool {
                    mTextureInfo.uv = param.toString().toInt();
                    outputCell->valueChanged(mTextureInfo);
                    return true;
                    })
            )
        });
}

QVariant UnlitDiffuseMapCard::getCurrentCardValue()
{
    return mTextureInfo;
}
