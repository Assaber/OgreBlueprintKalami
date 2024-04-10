#include "Card/Hlms/PbsDataBlock.h"

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
            ->setText("输出"), false);

    BKAnchor* oa = mpOutputCell->getAnchor(BKAnchor::AnchorType::Output);
    oa->appendRegist(mpNameLineEdit);
    mpNameLineEdit->registOutputAnchor(oa);

    _pack({
        mpOutputCell,

        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKLabel>()->setText("名称")),
        BKCreator::create(BKAnchor::AnchorType::None)->append(mpNameLineEdit),

        BKCreator::create(BKAnchor::AnchorType::None)->append({ BKCreator::create<BKLabel>()->setText("漫反射背景")}),
        BKCreator::create(BKAnchor::AnchorType::None)->append(BKCreator::create<BKColorSelectorEx>()
            ->setColor(QColor())
            ->setDataChangeCallback([this](const QVariant& param) -> bool {
                mBackgroundColor = param.value<QColor>();
                createHlms();
                return false;
                })),

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
        
    if (mBackgroundColor.isValid())
        mpDatablock->setBackgroundDiffuse(BlueprintEditor::toColor(mBackgroundColor));

    if (notify)
        mpOutputCell->valueChanged(QString(mstrName.c_str()));
}
