#include "Card/Hlms/PbsDataBlockCard.h"

#include "BKCreator.h"
#include "container/BKCell.h"
#include "unit/BKLabel.h"
#include "unit/BKComboBox.h"
#include "unit/BKPushButton.h"
#include "unit/BKLineEdit.h"
#include "unit/BKSliderBar.h"
#include "unit/BKCheckBox.h"
#include "unit/BKSpacer.h"
#include "unit/BKColorSelectorEx.h"
#include "BlueprintEditor.h"

#include "OgreRoot.h"
#include "OgreHlmsManager.h"
#include "OgreHlmsPbs.h"

#include <QMessageBox>
#include <QUuid>

PbsDatablockCard::PbsDatablockCard()
    : mpPbs(reinterpret_cast<Ogre::HlmsPbs*>(Ogre::Root::getSingletonPtr()->getHlmsManager()->getHlms(Ogre::HLMS_PBS)))
    , mstrName("snow")
{
    setTitle("Pbs数据块");

    mpNameLineEdit = BKCreator::create<BKLineEdit>();
    mpNameLineEdit->setText(mstrName.c_str())
        ->setDataChangeCallback([this](const QVariant& param) -> bool {
        std::string name = param.toString().toStdString();
        if (name != mstrOldName)
        {
            mstrName = name;
            createHlms(true);
        }
        return false;
        });

    mpOutputCell = BKCreator::create(BKAnchor::AnchorType::Output);
    mpOutputCell->setDataType(BKAnchor::Output, BKAnchor::String)
        ->append(BKCreator::create<BKLabel>()
            ->setAlignment(Qt::AlignVCenter | Qt::AlignRight)
            ->setText("输出"), false)
        ->getAnchor(BKAnchor::AnchorType::Output)           // 皮一下 XD
        ->redirectToCard();

    auto texMapInputCell = BKCreator::create(BKAnchor::Input | BKAnchor::MultiConn, BKAnchor::None);

    texMapInputCell->append(BKCreator::create<BKLabel>()
        ->setText("材质组", true)
        ->setDataChangeCallback([this](const QVariant& param) -> bool {
            std::vector<QVariant> items;
            if (mpTextureMapInputAnchor->getBindOutputData(items) < 0)
                return true;

                mTextureInfoVec.clear();
                for (const auto& item : items)
                    mTextureInfoVec.push_back(item.value<PbsMapCard::TexInfo>());

                // createHlms(true);
                return true;
            })
        );
    mpTextureMapInputAnchor = texMapInputCell->getAnchor(BKAnchor::Input);
    mpTextureMapInputAnchor->setDateType(QMetaTypeId<PbsMapCard::TexInfo>::qt_metatype_id());


    _pack({
        mpOutputCell,

        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>()->setText("名称")),
        BKCreator::create(BKAnchor::AnchorType::None)->append(mpNameLineEdit),

        BKCreator::create(BKAnchor::AnchorType::None)->append({ BKCreator::create<BKLabel>()->setText("漫反射背景色")}),
        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKColorSelectorEx>(BKColorSelectorEx::Type::Vector4)
            ->setColor(QColor())
            ->setDataChangeCallback([this](const QVariant& param) -> bool {
                mBackgroundColor = param.value<QColor>();
                createHlms();
                return false;
                })),

        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>()->setText("漫反射")),
        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKColorSelectorEx>()
            ->setColor(Qt::white)
            ->setDataChangeCallback([this](const QVariant& param) -> bool {
                mDiffuse = param.value<QColor>();
                createHlms();
                return false;
                })),

        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>()->setText("镜面反射")),
        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKColorSelectorEx>()
            ->setColor(Qt::white)
            ->setDataChangeCallback([this](const QVariant& param) -> bool {
                mSpecular = param.value<QColor>();
                createHlms();
                return false;
                })),

        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>()->setText("粗糙度")),
        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKSliderBar>(BKSliderBar::DateType::Double)
            ->setMinimum(0)
            ->setMaximum(1.0f)
            ->setCurrentValue(mRoughness)
            ->setDataChangeCallback([this](const QVariant& param) -> bool {
                mRoughness = param.toFloat();
                createHlms();
                return false;
                })),

        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>()->setText("自发光")),
        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKColorSelectorEx>()
            ->setColor(Qt::white)
            ->setDataChangeCallback([this](const QVariant& param) -> bool {
                mEmissive = param.value<QColor>();
                createHlms();
                return false;
                })),

        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>()->setText("菲涅尔")),
        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKColorSelectorEx>()
            ->setColor(Qt::white)
            ->setDataChangeCallback([this](const QVariant& param) -> bool {
                mFresnel = param.value<QColor>();
                createHlms();
                return false;
                })),

        texMapInputCell

        });

    createHlms(true);
}

PbsDatablockCard::~PbsDatablockCard()
{
    // if (mpDatablock && mpDatablock != mpPbs->getDefaultDatablock())
    // {
    //     mpPbs->destroyDatablock(mpDatablock->getName());
    //     mpDatablock = nullptr;
    // }
}

QVariant PbsDatablockCard::getCurrentCardValue()
{
    return QString(mstrName.c_str());
}

void PbsDatablockCard::createHlms(bool recreate/* = false*/)
{
    bool notify = false;

    // 我无法销毁一个datablock，因为别的渲染对象可能正在使用这个材质
    // 策略变更：只要名字发生变化，则重新创建datablock，当然也包括HlmsMacroblock和HlmsBlendblock的更新

    if (recreate)
    {
        mpDatablock = reinterpret_cast<Ogre::HlmsPbsDatablock*>(mpPbs->getDatablock(mstrName));

        Ogre::String suffix = "";
        while (mpDatablock)
        {
            suffix = QUuid::createUuid().toString(QUuid::WithoutBraces).toStdString();
            mpDatablock = reinterpret_cast<Ogre::HlmsPbsDatablock*>(mpPbs->getDatablock(mstrName + suffix));
        }

        if (!suffix.empty())
        {
            Ogre::String name = mstrName + suffix;
            int ret = QMessageBox::warning(nullptr, "警告", QString("存在重名数据块，名称存在更新：\n%1->%2\n确定将完成此更换，否则将推出数据块创建")
                .arg(mstrName.c_str())
                .arg(name.c_str()), QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Ok);
            if (ret == QMessageBox::Cancel)
            {
                mstrName = mstrOldName;
                mpNameLineEdit->setText(mstrName.c_str());
                return;
            }
            else
            {
                mstrName += suffix;
                mpNameLineEdit->setText(mstrName.c_str());
            }
        }

        mstrOldName = mstrName;
    }

    if (!mpDatablock)
    {
        mpDatablock = reinterpret_cast<Ogre::HlmsPbsDatablock*>(mpPbs->createDatablock(mstrName, 
            mstrName, 
            mMacroblock,
            mBlendblock, 
            Ogre::HlmsParamVec()
        ));
        notify = true;
    }
        

    mpDatablock->setBackgroundDiffuse(mBackgroundColor.isValid() ? BlueprintEditor::toColor(mBackgroundColor) : Ogre::ColourValue());
    mpDatablock->setDiffuse(mDiffuse.isValid() ? BlueprintEditor::toVec3f(mDiffuse) : Ogre::Vector3(1.0f, 1.0f, 1.0f));
    {
        // 这里选择不冒险
        mpDatablock->setSpecular(mSpecular.isValid() ? BlueprintEditor::toVec3f(mSpecular) : Ogre::Vector3(1.0f, 1.0f, 1.0f));
        // 跟参数传入保持一致
        // todo...
    }
    mpDatablock->setRoughness(mRoughness);
    mpDatablock->setEmissive(mEmissive.isValid() ? BlueprintEditor::toVec3f(mEmissive) : Ogre::Vector3(1.0f, 1.0f, 1.0f));

    mpDatablock->setFresnel(mFresnel.isValid() ? BlueprintEditor::toVec3f(mFresnel) : Ogre::Vector3(1.0f, 1.0f, 1.0f), true);



    if (notify)
        mpOutputCell->valueChanged(QString(mstrName.c_str()));
}
