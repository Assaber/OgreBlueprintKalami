#include "CanvasCard.h"
#include "BKCreator.hpp"
#include "container/BKCell.h"
#include "BKCreator.hpp"
#include "unit/BKLabel.h"
#include "unit/BKLineEdit.h"
#include "unit/BKSpacer.h"
#include "unit/BKCanvas.h"
#include <QUuid>
#include <QDebug>

#include "OgreRoot.h"
#include "OgreRenderSystem.h"
#include "OgreTextureGpuManager.h"
#include "OgreTextureGpu.h"
#include "OgreStagingTexture.h"

CanvasCard::CanvasCard()
    : BKCard()
    , mpTextureManager(Ogre::Root::getSingletonPtr()->getRenderSystem()->getTextureGpuManager())
{
    setTitle("画板");
    mstrTextureUuid = QUuid::createUuid().toString(QUuid::WithoutBraces).toStdString();
    const QSize canvasSize = { 200, 200 };

    BKCell* textureNameLabelCell = BKCreator::create(BKAnchor::AnchorType::None);
    BKLabel* textureNameLabel = BKCreator::create<BKLabel>();
    textureNameLabel->setText("名称");
    textureNameLabelCell->append(textureNameLabel, false);

    BKCell* textureNameEditCell = BKCreator::create(BKAnchor::AnchorType::Output);
    textureNameEditCell->setDataType(BKAnchor::Output, BKAnchor::String);
    BKLineEdit* textureNameEdit = BKCreator::create<BKLineEdit>();
    textureNameEdit->setEnable(false);
    textureNameEdit->setText(mstrTextureUuid.c_str());
    textureNameEditCell->append(textureNameEdit);

    BKCell* canvasCell = BKCreator::create(BKAnchor::AnchorType::None);
    BKCanvas* canvas = BKCreator::create<BKCanvas>();
    canvas->setFixedSize(canvasSize);
    canvas->setDataChangeCallback(std::bind(&CanvasCard::slotTextureUpdate, this, std::placeholders::_1, std::placeholders::_2));
    canvasCell->append(canvas);

    // 创建纹理
    mpTexture = mpTextureManager->createOrRetrieveTexture(
        mstrTextureUuid,
        Ogre::GpuPageOutStrategy::Discard,
        Ogre::TextureFlags::ManualTexture,
        Ogre::TextureTypes::Type2D
    );

    mpTexture->setPixelFormat(Ogre::PFG_RGBA8_UNORM);
    mpTexture->setNumMipmaps(1u);
    mpTexture->setResolution(canvasSize.width(), canvasSize.height());


    size_t dataSize = 4 * canvasSize.width() * canvasSize.height();
    uint8_t* data = reinterpret_cast<uint8_t*>(OGRE_MALLOC_SIMD(dataSize, Ogre::MEMCATEGORY_RENDERSYS));
    memset(data, 0, dataSize);
    mpTexture->_transitionTo(Ogre::GpuResidency::Resident, reinterpret_cast<uint8_t*>(data));
    mpTexture->_setNextResidencyStatus(Ogre::GpuResidency::Resident);

    uploadTextureData(canvasSize.width(), canvasSize.height(), data);

    OGRE_FREE_SIMD(data, Ogre::MEMCATEGORY_RENDERSYS);
    // mpTexture->notifyDataIsReady();


    _pack({
        textureNameLabelCell,
        textureNameEditCell,
        canvasCell
        });
}

CanvasCard::~CanvasCard()
{
    mpTextureManager->destroyTexture(mpTexture);
    mpTexture = nullptr;
}

bool CanvasCard::slotTextureUpdate(BKUnit* unit, const QVariant& image)
{
    // update texture
    do 
    {
        if (!image.canConvert<QImage*>())
            break;

        QImage* img = image.value<QImage*>();
        if (!img)
            break;

        uploadTextureData(img->width(), img->height(), img->bits());

    } while (false);

    return true;
}

void CanvasCard::uploadTextureData(int width, int height, const uint8_t* data)
{
    Ogre::StagingTexture* stagingTexture = mpTextureManager->getStagingTexture(width
        , height
        , 1
        , 1
        , Ogre::PFG_RGBA8_UNORM);

    stagingTexture->startMapRegion();
    Ogre::TextureBox texBox = stagingTexture->mapRegion(width, height, 1, 1, Ogre::PFG_RGBA8_UNORM);
    texBox.copyFrom(data, width, height, 4 * width);
    stagingTexture->stopMapRegion();

    stagingTexture->upload(texBox, mpTexture, 0, 0, 0, true);
    mpTextureManager->removeStagingTexture(stagingTexture);
}
