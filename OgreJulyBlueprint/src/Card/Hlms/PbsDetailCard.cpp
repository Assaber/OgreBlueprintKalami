#include "Card/Hlms/PbsDetailCard.h"

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

#define PBS_TEXTURE_NAME(x) #x

// https://zhuanlan.zhihu.com/p/688689323
// 优雅
#define PBS_BLEND_MODES \
    PBM_ITEM(NORMAL_NON_PREMUL) \
    PBM_ITEM(NORMAL_PREMUL) \
    PBM_ITEM(ADD) \
    PBM_ITEM(SUBTRACT) \
    PBM_ITEM(MULTIPLY) \
    PBM_ITEM(MULTIPLY2X) \
    PBM_ITEM(SCREEN) \
    PBM_ITEM(OVERLAY) \
    PBM_ITEM(LIGHTEN) \
    PBM_ITEM(DARKEN) \
    PBM_ITEM(GRAIN_EXTRACT) \
    PBM_ITEM(GRAIN_MERGE) \
    PBM_ITEM(DIFFERENCE)

#define PBM_ITEM(x) x = Ogre::PbsBlendModes::PBSM_BLEND_##x,
enum ShadowPbsBlendModes {
    PBS_BLEND_MODES  PBSM_BLEND_COUNT
};
#undef PBM_ITEM

#define PBM_ITEM(x) #x,
static constexpr const char* ShadowPbsBlendModesName[] = { PBS_BLEND_MODES "" };
#undef  PBM_ITEM

extern const Ogre::String c_pbsBlendModes[];

PbsDetailCard::PbsDetailCard()
{
    setTitle("Pbs细节");

    QStringList blendModes;
    for (int i = 0; i <= ShadowPbsBlendModes::PBSM_BLEND_COUNT; ++i)
        blendModes << ShadowPbsBlendModesName[i];

    mpOutputCell = BKCreator::create(BKAnchor::AnchorType::Output);
    mpOutputCell->setDataType(BKAnchor::Output, QMetaTypeId<PbsDetailCard::DetailInfo>::qt_metatype_id())
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
        ->setItems(QStringList() << PBS_TEXTURE_NAME(PBSM_DETAIL0)      << PBS_TEXTURE_NAME(PBSM_DETAIL1)       << PBS_TEXTURE_NAME(PBSM_DETAIL2)       << PBS_TEXTURE_NAME(PBSM_DETAIL3)
                                 << PBS_TEXTURE_NAME(PBSM_DETAIL0_NM)   << PBS_TEXTURE_NAME(PBSM_DETAIL1_NM)    << PBS_TEXTURE_NAME(PBSM_DETAIL2_NM)    << PBS_TEXTURE_NAME(PBSM_DETAIL3_NM))
        ->setCurrentIndex(0, false)
        ->setCallbackParamType(BKComboBox::CallbackParamType::Index)
        ->setDataChangeCallback([this](const QVariant& param) -> bool {
            mDetailInfo.textureType = static_cast<Ogre::PbsTextureTypes>(param.toInt() + Ogre::PbsTextureTypes::PBSM_DETAIL0);
            mpOutputCell->valueChanged(mDetailInfo);
            return true;
            })
        ),

        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>()->setText("贴图")),
        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKPixmap>()
        ->setDataChangeCallback([this](const QVariant& param) -> bool {
            mDetailInfo.texture = param.toString().toStdString();
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
                    ->setItems(QStringList() << "UNKNOWN" << "WRAP" << "MIRROR" << "CLAMP" << "BORDER")
                    ->setCurrentIndex(0)
                    ->setDataChangeCallback([this](const QVariant& param) -> bool {
                        mDetailInfo.sampler.mU = getTAMByName(param.toString().toStdString().c_str());
                        mpOutputCell->valueChanged(mDetailInfo);
                        return true;
                        }),

                    BKCreator::create<BKComboBox>()
                    ->setMinWidth(60)
                    ->setItems(QStringList() << "UNKNOWN" << "WRAP" << "MIRROR" << "CLAMP" << "BORDER")
                    ->setCurrentIndex(0)
                    ->setDataChangeCallback([this](const QVariant& param) -> bool {
                        mDetailInfo.sampler.mV = getTAMByName(param.toString().toStdString().c_str());
                        mpOutputCell->valueChanged(mDetailInfo);
                        return true;
                        }),

                    BKCreator::create<BKComboBox>()
                    ->setMinWidth(60)
                    ->setItems(QStringList() << "UNKNOWN" << "WRAP" << "MIRROR" << "CLAMP" << "BORDER")
                    ->setCurrentIndex(0)
                    ->setDataChangeCallback([this](const QVariant& param) -> bool {
                        mDetailInfo.sampler.mW = getTAMByName(param.toString().toStdString().c_str());
                        mpOutputCell->valueChanged(mDetailInfo);
                        return true;
                        }),

                }),


        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>()->setText("环绕模式")),
        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKComboBox>()
            ->setItems(blendModes)
            ->setCurrentIndex(ShadowPbsBlendModes::PBSM_BLEND_COUNT)
            ->setCallbackParamType(BKComboBox::CallbackParamType::Index)
            ->setDataChangeCallback([this](const QVariant& param) -> bool {
                mDetailInfo.blendMode = static_cast<Ogre::PbsBlendModes>(param.toInt());
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

                        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKVectorEditor>(BKVectorEditor::Type::Int, 1)
                            ->setNames({ "绑定资源" })
                            ->setValue(QVariant::fromValue(BKVectorEditor::IntegerVec{ -1 }))
                            ->setRange(0, { -1, 7 })
                            ->setDataChangeCallback([this](const QVariant& param) -> bool {
                                BKVectorEditor::IntegerVec data = param.value<BKVectorEditor::IntegerVec>();
                                if (data.size() == 1 && mDetailInfo.offsetScaleEnable)
                                {
                                    mDetailInfo.uv = data[0];
                                    mpOutputCell->valueChanged(mDetailInfo);
                                }
                                return true;
                                })
                        ),

        });
}

QVariant PbsDetailCard::getCurrentCardValue()
{
    return QVariant();
}

Ogre::TextureAddressingMode PbsDetailCard::getTAMByName(const char* name) const
{
    if (!strcmp(name, "UNKNOWN"))
        return Ogre::TAM_UNKNOWN;
    else if (!strcmp(name, "WRAP"))
        return Ogre::TAM_WRAP;
    else if (!strcmp(name, "MIRROR"))
        return Ogre::TAM_MIRROR;
    else if (!strcmp(name, "CLAMP"))
        return Ogre::TAM_CLAMP;
    else if (!strcmp(name, "BORDER"))
        return Ogre::TAM_BORDER;

    return Ogre::TAM_UNKNOWN;
}