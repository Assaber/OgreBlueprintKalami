#include "Card/Hlms/PbsDetailTextureCard.h"

#include "BKCreator.h"
#include "container/BKCell.h"
#include "unit/BKLabel.h"
#include "unit/BKComboBox.h"
#include "unit/BKCheckBox.h"
#include "unit/BKSpacer.h"
#include "unit/BKPixmap.h"
#include "unit/BKColorSelectorEx.h"
#include "unit/BKVectorEditor.h"
#include "BlueprintEditor.h"

#include <QFileInfo>

const QMap<QString, Ogre::PbsBlendModes> name2PbsBlendModes = {
    { "NormalNonPremul", Ogre::PBSM_BLEND_NORMAL_NON_PREMUL },
    { "NormalPremul"   ,  Ogre::PBSM_BLEND_NORMAL_PREMUL },
    { "Add",  Ogre::PBSM_BLEND_ADD },
    { "Subtract",  Ogre::PBSM_BLEND_SUBTRACT },
    { "Multiply",  Ogre::PBSM_BLEND_MULTIPLY },
    { "Multiply2x",  Ogre::PBSM_BLEND_MULTIPLY2X },
    { "Screen",  Ogre::PBSM_BLEND_SCREEN },
    { "Overlay",  Ogre::PBSM_BLEND_OVERLAY },
    { "Lighten",  Ogre::PBSM_BLEND_LIGHTEN },
    { "Darken",  Ogre::PBSM_BLEND_DARKEN },
    { "GrainExtract",  Ogre::PBSM_BLEND_GRAIN_EXTRACT },
    { "GrainMerge",  Ogre::PBSM_BLEND_GRAIN_MERGE },
    { "Difference",  Ogre::PBSM_BLEND_DIFFERENCE }
};

const QMap<QString, Ogre::PbsTextureTypes> name2PbsDetailTextureType = {
    { "Detail0", Ogre::PBSM_DETAIL0 },
    { "Detail1", Ogre::PBSM_DETAIL1 },
    { "Detail2", Ogre::PBSM_DETAIL2 },
    { "Detail3", Ogre::PBSM_DETAIL3 },
    { "Detail0_NM", Ogre::PBSM_DETAIL0_NM },
    { "Detail1_NM", Ogre::PBSM_DETAIL1_NM },
    { "Detail2_NM", Ogre::PBSM_DETAIL2_NM },
    { "Detail3_NM", Ogre::PBSM_DETAIL3_NM },
};

const QMap<QString, Ogre::TextureAddressingMode> name2TextureAddrMode = {
    { "Unknow", Ogre::TAM_UNKNOWN },
    { "Wrap", Ogre::TAM_WRAP },
    { "Mirror", Ogre::TAM_MIRROR },
    { "Clamp", Ogre::TAM_CLAMP },
    { "Border", Ogre::TAM_BORDER },
};

PbsDetailTextureCard::PbsDetailTextureCard()
    : ResetResourceSup("ResourceGroup.Pbs.Snow")
{
    setTitle("Pbs细节");

    QStringList blendModes = name2PbsBlendModes.keys();
    int initBlendModeIndex = blendModes.indexOf(name2PbsBlendModes.key(mDetailInfo.blendMode));

    QStringList textureTypes = name2PbsDetailTextureType.keys();
    int initTextureTypeIndex = textureTypes.indexOf(name2PbsDetailTextureType.key(mDetailInfo.type));
    mDetailInfo.indexOffset = getOffsetByTextureType(mDetailInfo.type);

    QStringList textureAddrMode = name2TextureAddrMode.keys();
    int initTextureAddrModeIndex = textureAddrMode.indexOf(name2TextureAddrMode.key(Ogre::TAM_WRAP));


    mpOutputCell = BKCreator::create(BKAnchor::AnchorType::Output);
    mpOutputCell->setDataType(BKAnchor::Output, QMetaTypeId<PbsDetailTextureCard::Info>::qt_metatype_id())
        ->append(BKCreator::create<BKLabel>()
            ->setAlignment(Qt::AlignVCenter | Qt::AlignRight)
            ->setText("输出")
            ->setMinWidth(140)
            , false)
        ->getAnchor(BKAnchor::AnchorType::Output)
        ->redirectToCard();

    _pack({
        mpOutputCell,

        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>()->setText("纹理类型")),
        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKComboBox>()
        ->setItems(textureTypes)
        ->setCurrentIndex(initTextureTypeIndex, false)
        ->setDataChangeCallback([this](const QVariant& param) -> bool {
            mDetailInfo.type = name2PbsDetailTextureType[param.toString()];
            mDetailInfo.indexOffset = getOffsetByTextureType(mDetailInfo.type);
            mpOutputCell->valueChanged(mDetailInfo);
            return true;
            })
        ),

        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>()->setText("贴图")),
        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKPixmap>()
        ->setDataChangeCallback([this](const QVariant& param) -> bool {
            Ogre::String texturePath = param.toString().toStdString();
            resetResourceDir(texturePath);

            mDetailInfo.texture = QFileInfo(param.toString()).fileName().toStdString();
            mpOutputCell->valueChanged(mDetailInfo);
            return true;
            })
        ),

        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>()->setText("纹理寻址模式")),
        BKCreator::create(BKAnchor::AnchorType::None)
                ->append({
                    BKCreator::create<BKLabel>()->setAlignment(Qt::AlignCenter)->setText("U"),
                    BKCreator::create<BKLabel>()->setAlignment(Qt::AlignCenter)->setText("V"),
                    BKCreator::create<BKLabel>()->setAlignment(Qt::AlignCenter)->setText("W"),
                }),
        BKCreator::create(BKAnchor::AnchorType::None)
                ->append({
                    BKCreator::create<BKComboBox>()
                    ->setMinWidth(60)
                    ->setItems(textureAddrMode)
                    ->setCurrentIndex(initTextureAddrModeIndex, false)
                    ->setDataChangeCallback([this](const QVariant& param) -> bool {
                        mDetailInfo.sampler.mU = name2TextureAddrMode[param.toString()];
                        mpOutputCell->valueChanged(mDetailInfo);
                        return true;
                        }),

                    BKCreator::create<BKComboBox>()
                    ->setMinWidth(60)
                    ->setItems(textureAddrMode)
                    ->setCurrentIndex(initTextureAddrModeIndex, false)
                    ->setDataChangeCallback([this](const QVariant& param) -> bool {
                        mDetailInfo.sampler.mV = name2TextureAddrMode[param.toString()];
                        mpOutputCell->valueChanged(mDetailInfo);
                        return true;
                        }),

                    BKCreator::create<BKComboBox>()
                    ->setMinWidth(60)
                    ->setItems(textureAddrMode)
                    ->setCurrentIndex(initTextureAddrModeIndex, false)
                    ->setDataChangeCallback([this](const QVariant& param) -> bool {
                        mDetailInfo.sampler.mW = name2TextureAddrMode[param.toString()];
                        mpOutputCell->valueChanged(mDetailInfo);
                        return true;
                        }),

                }),

        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>()->setText("混合模式")),
        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKComboBox>()
            ->setItems(blendModes)
            ->setCurrentIndex(initBlendModeIndex, false)
            ->setDataChangeCallback([this](const QVariant& param) -> bool {
                mDetailInfo.blendMode = name2PbsBlendModes[param.toString()];
                mpOutputCell->valueChanged(mDetailInfo);
                return true;
                })
            ),

        BKCreator::create(BKAnchor::AnchorType::None)->append({
                BKCreator::create<BKLabel>()->setText("偏移比例"),
                BKCreator::create<BKCheckBox>()->setDataChangeCallback([this](const QVariant& param) -> bool {
                    mDetailInfo.offsetScaleEnable = param.toBool();
                    mpOutputCell->valueChanged(mDetailInfo);
                    return true;
                    })
                }),
        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKVectorEditor>(BKVectorEditor::Type::Float, 4)
            ->setItemInLine(2)
            ->setNames({"U偏移", "V偏移", "U缩放", "V缩放"})
            ->setValue(QVariant::fromValue(BKVectorEditor::FloatVec{0, 0, 1.0f, 1.0f}))
            ->setDataChangeCallback([this](const QVariant& param) -> bool {
                BKVectorEditor::FloatVec data = param.value<BKVectorEditor::FloatVec>();
                if (data.size() == 4 && mDetailInfo.offsetScaleEnable)
                {
                    mDetailInfo.offsetScale = { data[0], data[1], data[2], data[3] };
                    mpOutputCell->valueChanged(mDetailInfo);
                }
                return true;
                })
            ),

        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>()->setText("UV")),
            BKCreator::create(BKAnchor::AnchorType::None)
                ->append(BKCreator::create<BKComboBox>()
                    ->setItems(QStringList() << "" << "0" << "1" << "2" << "3" << "4" << "5" << "6" << "7")
                    ->setCurrentItem(0, false)          // 对应-1
                    ->setDataChangeCallback([this](const QVariant& param) -> bool {
                        QString si = param.toString();
                        mDetailInfo.uv = si.isEmpty() ? -1 : si.toInt();
                        mpOutputCell->valueChanged(mDetailInfo);
                        return true;
                        }
                    )
         ),

    });
}

QVariant PbsDetailTextureCard::getCurrentCardValue()
{
    return mDetailInfo;
}

static_assert(Ogre::PBSM_DETAIL1 < Ogre::PBSM_DETAIL0_NM, "This comparison relates to calculating 'indexOffset'");
int8_t PbsDetailTextureCard::getOffsetByTextureType(Ogre::PbsTextureTypes type)
{
    if (mDetailInfo.type >= Ogre::PBSM_DETAIL0 && mDetailInfo.type <= Ogre::PBSM_DETAIL3)
        return mDetailInfo.type - Ogre::PBSM_DETAIL0;
    else if (mDetailInfo.type >= Ogre::PBSM_DETAIL0_NM && mDetailInfo.type <= Ogre::PBSM_DETAIL3_NM)
        return mDetailInfo.type - Ogre::PBSM_DETAIL0_NM;

    return -1;
}