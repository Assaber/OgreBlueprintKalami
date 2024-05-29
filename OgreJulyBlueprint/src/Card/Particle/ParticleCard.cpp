#ifdef OBK_INCLUDE_PARTICLE_PLUGIN
#include "Particle/ParticleCard.h"
#include "Particle/ParticleEmitterCard.h"
#include "Particle/ParticleBasic.h"
#include "Particle/ParticleBillboardSettingCard.h"

#include "unit/BKLabel.h"
#include "unit/BKLineEdit.h"
#include "unit/BKSliderBar.h"
#include "unit/BKCheckBox.h"

#include <QUuid>
#include "OgreParticleSystemManager.h"
#include "OgreParticleSystem.h"
#include "OgreBillboardParticleRenderer.h"
#include "OgreAreaEmitter.h"
#include "OgreHollowEllipsoidEmitter.h"
#include "OgreRingEmitter.h"


#include "Card/Particle/Affector/ParticleColourFader2Card.h"
#include "Card/Particle/Affector/ParticleColourFaderCard.h"
#include "Card/Particle/Affector/ParticleColourImageCard.h"
#include "Card/Particle/Affector/ParticleColourInterpolatorCard.h"
#include "Card/Particle/Affector/ParticleDeflectorPlaneCard.h"
#include "Card/Particle/Affector/ParticleDirectionRandomiserCard.h"
#include "Card/Particle/Affector/ParticleLinearForceCard.h"
#include "Card/Particle/Affector/ParticleRotatorCard.h"
#include "Card/Particle/Affector/ParticleScalerCard.h"

#include "OgreWidget.h"
#include "OgreSceneNode.h"
#include "OgreSceneManager.h"
#include "OgreRoot.h"

#define UpdateFunction(x) [this](const QVariant& data) -> bool { \
x; \
refreshParam();\
return true;\
}

ParticleCard::ParticleCard()
{
	setTitle("粒子卡片");
	mData.id = QUuid::createUuid().toString(QUuid::WithoutBraces).toStdString();

    mpSceneManager = Ogre::Root::getSingletonPtr()->getSceneManager(OgreWidget::strSceneMgrName);
    mpBindNodeParent = mpSceneManager->getRootSceneNode();
    mpBindNode = mpBindNodeParent->createChildSceneNode();

	mpParticleSystemTemplate = Ogre::ParticleSystemManager::getSingleton().createTemplate(mData.id, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	mpParticleSystemTemplate->setRenderer("billboard");

    BKLabel* outputLabel = BKCreator::create<BKLabel>()
		->setAlignment(Qt::AlignVCenter | Qt::AlignRight)
		->setText("输出");
    BKCell* outputCell = BKCreator::create(BKAnchor::AnchorType::Output)
		->setDataType(BKAnchor::Output, BKAnchor::String)
		->append(outputLabel, false);
    mpOutputAnchor = outputCell->getAnchor(BKAnchor::AnchorType::Output);
    mpOutputAnchor->redirectToCard();


	BKCell* emitterCell = BKCreator::create(BKAnchor::Input | BKAnchor::MultiConn, BKAnchor::None)
		->setDataType(BKAnchor::Input, GET_QT_METATYPE_ID(ParticleEmitterCard::Data));
	mpEmitterAnchor = emitterCell->getAnchor(BKAnchor::Input);
	emitterCell->append(BKCreator::create<BKLabel>()
            ->setText("发射器")
            ->setTitleNeverChanges(true)
            ->setDataChangeCallback(std::bind(&ParticleCard::updateEmitter, this, std::placeholders::_1))
        );

    BKCell* affectorCell = BKCreator::create(BKAnchor::Input | BKAnchor::MultiConn, BKAnchor::None)
        ->setDataType(BKAnchor::Input, GET_QT_METATYPE_ID(particle::ParticleAffector));
	mpAffectorAnchor = affectorCell->getAnchor(BKAnchor::Input);
	affectorCell->append(BKCreator::create<BKLabel>()
        ->setText("控制器")
        ->setTitleNeverChanges(true)
        ->setDataChangeCallback(std::bind(&ParticleCard::updateAffector, this, std::placeholders::_1))
    );

    _pack({
        outputCell,

        BKCreator::create(BKAnchor::AnchorType::None)->append(
            BKCreator::create<BKLabel>()->setText("材质名称")
        ),
        BKCreator::create(BKAnchor::AnchorType::None)->append(
            BKCreator::create<BKLineEdit>()
                ->setText(mData.material.c_str())
                ->setDataChangeCallback([this](const QVariant& data) -> bool {
                    QString material = data.toString();
                    mData.material = material.toStdString();
                    refreshParam();
                    return true;
                    })
        ),

        BKCreator::create(BKAnchor::AnchorType::None)->append(
			BKCreator::create<BKLabel>()->setText("存在数量")
        ),
        BKCreator::create(BKAnchor::AnchorType::None)->append(
            BKCreator::create<BKSliderBar>()->setMinimum(0)->setMaximum(2'0000)
				->setCurrentValue(mData.quota)
                ->setDataChangeCallback(UpdateFunction(mData.quota = data.toInt()))
		),

        BKCreator::create(BKAnchor::AnchorType::None)->append(
            BKCreator::create<BKLabel>()->setText("范围宽度")
        ),
        BKCreator::create(BKAnchor::AnchorType::None)->append(
            BKCreator::create<BKSliderBar>()->setMinimum(0)->setMaximum(100)
				->setCurrentValue(mData.itemWidth)
				->setDataChangeCallback(UpdateFunction(mData.itemWidth = data.toInt()))
        ),

        BKCreator::create(BKAnchor::AnchorType::None)->append(
            BKCreator::create<BKLabel>()->setText("范围高度")
        ),
        BKCreator::create(BKAnchor::AnchorType::None)->append(
            BKCreator::create<BKSliderBar>()->setMinimum(0)->setMaximum(100)
				->setCurrentValue(mData.itemHeight)
                ->setDataChangeCallback(UpdateFunction(mData.itemHeight = data.toInt()))
        ),

        BKCreator::create(BKAnchor::AnchorType::Input)
		->setDataType(BKAnchor::Input, GET_QT_METATYPE_ID(ParticleBillboardSettingCard::Data))
			->append(
				BKCreator::create<BKLabel>()->setText("渲染类型")
					->setDataChangeCallback(std::bind(&ParticleCard::updateRenderType, this, std::placeholders::_1))
					->setTitleNeverChanges(true)
			),

        BKCreator::create(BKAnchor::AnchorType::None)
			->append(BKCreator::create<BKLabel>()->setText("点渲染"))
			->append(BKCreator::create<BKCheckBox>()->setEnable(mData.isPointRendering)->setDataChangeCallback(UpdateFunction(mData.isPointRendering = data.toBool()))),

        BKCreator::create(BKAnchor::AnchorType::None)
            ->append(BKCreator::create<BKLabel>()->setText("精确面"))
            ->append(BKCreator::create<BKCheckBox>()->setEnable(mData.isAccurateFacing)->setDataChangeCallback(UpdateFunction(mData.isAccurateFacing = data.toBool()))),

        BKCreator::create(BKAnchor::AnchorType::None)
            ->append(BKCreator::create<BKLabel>()->setText("裁剪"))
            ->append(BKCreator::create<BKCheckBox>()->setEnable(mData.isCullEach)->setDataChangeCallback(UpdateFunction(mData.isCullEach = data.toBool()))),

        BKCreator::create(BKAnchor::AnchorType::None)
            ->append(BKCreator::create<BKLabel>()->setText("排序"))
            ->append(BKCreator::create<BKCheckBox>()->setEnable(mData.isSorted)->setDataChangeCallback(UpdateFunction(mData.isSorted = data.toBool()))),

        BKCreator::create(BKAnchor::AnchorType::None)
            ->append(BKCreator::create<BKLabel>()->setText("局部空间"))
            ->append(BKCreator::create<BKCheckBox>()->setEnable(mData.isLocalSpace)->setDataChangeCallback(UpdateFunction(mData.isLocalSpace = data.toBool()))),

        BKCreator::create(BKAnchor::AnchorType::None)->append(
            BKCreator::create<BKLabel>()->setText("粒子更新间隔")
        ),
        BKCreator::create(BKAnchor::AnchorType::None)->append(
            BKCreator::create<BKSliderBar>(BKSliderBar::DataType::Double)->setMinimum(0)->setMaximum(3.f)
                ->setCurrentValue(mData.updateInterval)
                ->setDataChangeCallback(UpdateFunction(mData.updateInterval = data.toFloat()))
        ),

        BKCreator::create(BKAnchor::AnchorType::None)->append(
            BKCreator::create<BKLabel>()->setText("停止更新超时")
        ),
        BKCreator::create(BKAnchor::AnchorType::None)->append(
            BKCreator::create<BKSliderBar>()->setMinimum(0)->setMaximum(60)
                ->setCurrentValue(mData.updateTimeout)
                ->setDataChangeCallback(UpdateFunction(mData.updateTimeout = data.toInt()))
        ),

		emitterCell,
		affectorCell,
	});

    refreshParam();
}

ParticleCard::~ParticleCard()
{
    if (mpSceneParticleSystem)
    {
        mpBindNode->detachObject(mpSceneParticleSystem);
        mpSceneManager->destroyParticleSystem(mpSceneParticleSystem);

        mpSceneParticleSystem = nullptr;
    }

    mpBindNodeParent->removeAndDestroyChild(mpBindNode);
    mpBindNode = nullptr;


    Ogre::ParticleSystemManager::getSingletonPtr()->removeTemplate(mData.id);
}

void ParticleCard::refreshParam()
{
    mpParticleSystemTemplate->setParticleQuota(mData.quota);
    mpParticleSystemTemplate->setMaterialName(mData.material);
    mpParticleSystemTemplate->setDefaultWidth(mData.itemWidth);
    mpParticleSystemTemplate->setDefaultHeight(mData.itemHeight);
    mpParticleSystemTemplate->setCullIndividually(mData.isCullEach);
    mpParticleSystemTemplate->setSortingEnabled(mData.isSorted);
    mpParticleSystemTemplate->setKeepParticlesInLocalSpace(mData.isLocalSpace);
    mpParticleSystemTemplate->setIterationInterval(mData.updateInterval);
    mpParticleSystemTemplate->setDefaultNonVisibleUpdateTimeout(mData.updateTimeout);

    reloadParticleToScene();
}

bool ParticleCard::updateRenderType(const QVariant& var)
{
	Ogre::ParticleSystemRenderer* renderer = mpParticleSystemTemplate->getRenderer();
	Ogre::v1::BillboardParticleRenderer* billboardRenderer = dynamic_cast<Ogre::v1::BillboardParticleRenderer*>(renderer);
	assert(billboardRenderer && "billboard is not supported");

	ParticleBillboardSettingCard::Data data = var.value<ParticleBillboardSettingCard::Data>();
    billboardRenderer->setBillboardType(particle::billboardName2Type[data.type]);
    billboardRenderer->setBillboardRotationType(particle::billboardName2Rotation[data.rotationType]);
    billboardRenderer->setBillboardOrigin(particle::billboardName2Origin[data.origin]);
    billboardRenderer->setCommonUpVector({ data.commonUp[0], data.commonUp[1], data.commonUp[2] });
    billboardRenderer->setCommonDirection({ data.commonDirection[0], data.commonDirection[1], data.commonDirection[2] });

    reloadParticleToScene();
	return true;
}

bool ParticleCard::updateEmitter(const QVariant& var)
{
    Q_UNUSED(var);
    std::vector<QVariant> items;
    if (mpEmitterAnchor->getBindOutputData(items) < 0)
        return true;

    mpParticleSystemTemplate->removeAllEmitters();

    for (const QVariant& item : items)
    {
        ParticleEmitterCard::Data emitterInfo = item.value<ParticleEmitterCard::Data>();

        const ParticleEmitterTypeCard::Data& typeInfo = emitterInfo.type;
        // 更新Type
        Ogre::ParticleEmitter* emitter = mpParticleSystemTemplate->addEmitter(typeInfo.type.toStdString());
        particle::PEmitterType type = particle::emitterName2Type[typeInfo.type];
        switch (type)
        {
        case particle::Box:
        case particle::Cylinder:
        case particle::Ellipsoid:
        {
            Ogre::AreaEmitter* ae = dynamic_cast<Ogre::AreaEmitter*>(emitter);
            if (ae) {
                ae->setSize(typeInfo.width, typeInfo.height, typeInfo.depth);
            }
        }
            break;
        case particle::HollowEllipsoid:
        {
            Ogre::HollowEllipsoidEmitter* hee = dynamic_cast<Ogre::HollowEllipsoidEmitter*>(emitter);
            if (hee) {
                hee->setSize(typeInfo.width, typeInfo.height, typeInfo.depth);
                hee->setInnerSize(typeInfo.innerWidth, typeInfo.innerHeight, typeInfo.innerDepth);
            }
        }
            break;
        case particle::Ring:
        {
            Ogre::RingEmitter* re = dynamic_cast<Ogre::RingEmitter*>(emitter);
            if (re) {
                re->setSize(typeInfo.width, typeInfo.height, typeInfo.depth);
                re->setInnerSize(typeInfo.innerWidth, typeInfo.innerHeight);
            }
        }
            break;
        default:
            break;
        }

        emitter->setName(emitterInfo.name.toStdString());
        // emitter->setEmitted(false);
        if (!emitterInfo.nextName.isEmpty()) {
            emitter->setEmittedEmitter(emitterInfo.nextName.toStdString());
        }

        emitter->setAngle(Ogre::Radian(Ogre::Degree(emitterInfo.degrees)));
        emitter->setPosition({emitterInfo.position[0], emitterInfo.position[1], emitterInfo.position[2] });
        emitter->setDirection({ emitterInfo.direction[0], emitterInfo.direction[1], emitterInfo.direction[2] });
        emitter->setEmissionRate(emitterInfo.sendItemPerSec);
        emitter->setColour(particle::qColor2ColourValue(emitterInfo.color));
        emitter->setColourRangeStart(particle::qColor2ColourValue(emitterInfo.startColor));
        emitter->setColourRangeEnd(particle::qColor2ColourValue(emitterInfo.endColor));
        emitter->setParticleVelocity(emitterInfo.velocity);
        emitter->setMinParticleVelocity(emitterInfo.velocityMin);
        emitter->setMaxParticleVelocity(emitterInfo.velocityMax);
        emitter->setTimeToLive(emitterInfo.timeToLive);
        emitter->setMinTimeToLive(emitterInfo.timeToLiveMin);
        emitter->setMaxTimeToLive(emitterInfo.timeToLiveMax);
        emitter->setDuration(emitterInfo.duration);
        emitter->setMinDuration(emitterInfo.durationMin);
        emitter->setMaxDuration(emitterInfo.durationMax);
        emitter->setRepeatDelay(emitterInfo.repeatDelay);
        emitter->setMinRepeatDelay(emitterInfo.repeatDelayMin);
        emitter->setMaxRepeatDelay(emitterInfo.repeatDelayMax);
    }
    
    reloadParticleToScene();
	return true;
}

bool ParticleCard::updateAffector(const QVariant& var)
{
    Q_UNUSED(var);
    std::vector<QVariant> items;
    if (mpAffectorAnchor->getBindOutputData(items) < 0)
        return true;

    mpParticleSystemTemplate->removeAllAffectors();

    for (const QVariant& item : items)
    {
        assert(item.canConvert<particle::ParticleAffector>() && "???");

        particle::ParticleAffector pa = item.value<particle::ParticleAffector>();

        switch (pa.type)
        {
        case particle::ParticleAffector::ColourFader:
        {
            ParticleColourFaderCard::Data* data = reinterpret_cast<ParticleColourFaderCard::Data*>(pa.pObj);
            Ogre::ColourFaderAffector* affector = dynamic_cast<Ogre::ColourFaderAffector*>(mpParticleSystemTemplate->addAffector("ColourFader"));
            if (data && affector) {
                affector->setAdjust(static_cast<float>(data->color.red()) / 255,
                    static_cast<float>(data->color.green()) / 255,
                    static_cast<float>(data->color.blue()) / 255,
                    static_cast<float>(data->color.alpha()) / 255);

            }
        }
        break;

        case particle::ParticleAffector::ColourFader2:
        {
            ParticleColourFader2Card::Data* data = reinterpret_cast<ParticleColourFader2Card::Data*>(pa.pObj);
            Ogre::ColourFaderAffector2* affector = dynamic_cast<Ogre::ColourFaderAffector2*>(mpParticleSystemTemplate->addAffector("ColourFader2"));
            if (data && affector) {
                affector->setAdjust1(static_cast<float>(data->color1.red()) / 255,
                    static_cast<float>(data->color1.green()) / 255,
                    static_cast<float>(data->color1.blue()) / 255,
                    static_cast<float>(data->color1.alpha()) / 255);
                affector->setAdjust2(static_cast<float>(data->color2.red()) / 255,
                    static_cast<float>(data->color2.green()) / 255,
                    static_cast<float>(data->color2.blue()) / 255,
                    static_cast<float>(data->color2.alpha()) / 255);
                affector->setStateChange(data->time);
            }
        }
        break;

        case particle::ParticleAffector::ColourImage:
        {
            ParticleColourImageCard::Data* data = reinterpret_cast<ParticleColourImageCard::Data*>(pa.pObj);
            Ogre::ColourImageAffector* affector = dynamic_cast<Ogre::ColourImageAffector*>(mpParticleSystemTemplate->addAffector("ColourImage"));
            if (data && affector) {
                affector->setImageAdjust(data->image.toStdString());
            }
        }
        break;

        case particle::ParticleAffector::ColourInterpolator:
        {
            ParticleColourInterpolatorCard::Data* data = reinterpret_cast<ParticleColourInterpolatorCard::Data*>(pa.pObj);
            Ogre::ColourInterpolatorAffector* affector = dynamic_cast<Ogre::ColourInterpolatorAffector*>(mpParticleSystemTemplate->addAffector("ColourInterpolator"));
            if (data && affector) {
                for (int i = 0; i < data->items.size(); ++i)
                {
                    const auto& item = data->items[i];
                    if (!item.enable)
                        continue;

                    affector->setColourAdjust(i, particle::qColor2ColourValue(item.color));
                    affector->setTimeAdjust(i, item.time);
                }
            }
        }
        break;

        case particle::ParticleAffector::DeflctorPlane:
        {
            ParticleDeflectorPlaneCard::Data* data = reinterpret_cast<ParticleDeflectorPlaneCard::Data*>(pa.pObj);
            Ogre::DeflectorPlaneAffector* affector = dynamic_cast<Ogre::DeflectorPlaneAffector*>(mpParticleSystemTemplate->addAffector("DeflectorPlane"));
            if (data && affector) {
                affector->setPlanePoint({ data->point[0], data->point[1], data->point[2] });
                affector->setPlaneNormal({ data->normal[0], data->normal[1], data->normal[2] });
                affector->setBounce({ data->bounce });
            }
        }
        break;

        case particle::ParticleAffector::DirectionRandomiser:
        {
            ParticleDirectionRandomiserCard::Data* data = reinterpret_cast<ParticleDirectionRandomiserCard::Data*>(pa.pObj);
            Ogre::DirectionRandomiserAffector* affector = dynamic_cast<Ogre::DirectionRandomiserAffector*>(mpParticleSystemTemplate->addAffector("DirectionRandomiser"));
            if (data && affector) {
                affector->setKeepVelocity(data->keepVelocity);
                affector->setRandomness(data->randomness);
                affector->setScope(data->scope);
            }
        }
        break;

        case particle::ParticleAffector::LinearForce:
        {
            ParticleLinearForceCard::Data* data = reinterpret_cast<ParticleLinearForceCard::Data*>(pa.pObj);
            Ogre::LinearForceAffector* affector = dynamic_cast<Ogre::LinearForceAffector*>(mpParticleSystemTemplate->addAffector("LinearForce"));
            if (data && affector) {
                affector->setForceApplication(particle::linearAffectorName2Type[data->function]);
                affector->setForceVector({ data->force[0], data->force[1], data->force[2] });
            }
        }
        break;

        case particle::ParticleAffector::Rotator:
        {
            ParticleRotatorCard::Data* data = reinterpret_cast<ParticleRotatorCard::Data*>(pa.pObj);
            Ogre::RotationAffector* affector = dynamic_cast<Ogre::RotationAffector*>(mpParticleSystemTemplate->addAffector("Rotator"));
            if (data && affector) {
                affector->setRotationRangeStart(Ogre::Radian(Ogre::Degree(data->rotStart)));
                affector->setRotationRangeEnd(Ogre::Radian(Ogre::Degree(data->rotEnd)));
                affector->setRotationSpeedRangeStart(Ogre::Radian(Ogre::Degree(data->speedStart)));
                affector->setRotationSpeedRangeEnd(Ogre::Radian(Ogre::Degree(data->speedEnd)));
            }
        }
        break;

        case particle::ParticleAffector::Scaler:
        {
            ParticleScalerCard::Data* data = reinterpret_cast<ParticleScalerCard::Data*>(pa.pObj);
            Ogre::ScaleAffector* affector = dynamic_cast<Ogre::ScaleAffector*>(mpParticleSystemTemplate->addAffector("Scaler"));
            if (data && affector) {
                affector->setAdjust(data->scale);
            }
        }
        break;

        default:
            break;
        }
    }
	
    reloadParticleToScene();
    return true;
}

void ParticleCard::reloadParticleToScene()
{
    if (mpSceneParticleSystem)
    {
        mpBindNode->detachObject(mpSceneParticleSystem);
        mpSceneManager->destroyParticleSystem(mpSceneParticleSystem);

        mpSceneParticleSystem = nullptr;
    }

    mpSceneParticleSystem = mpSceneManager->createParticleSystem(mData.id);
    mpBindNode->attachObject(mpSceneParticleSystem);     
}

#endif