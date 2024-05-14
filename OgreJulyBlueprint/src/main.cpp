#include "Application.h"
#include "OgreBlueprint.h"

#include "BKCreator.h"
#include "TestCard.h"
#include "PrintCard.h"
#include "RenderItemCard.h"
#include "Card/Hlms/PbsDatablockCard.h"
#include "Card/Hlms/PbsDetailTextureCard.h"
#include "Card/Hlms/PbsTransparentCard.h"
#include "Card/Hlms/PbsCommonTextureCard.h"
#include "Card/Hlms/BlendblockCard.h"
#include "Card/Hlms/MacroblockCard.h"
#include "Card/Hlms/UnlitDatablockCard.h"
#include "Card/Hlms/UnlitDiffuseMapCard.h"
// #include "Card/Material/MaterialCard.h"
#include "Card/ReadFileCard.h"
#include <functional>
#include <QDateTime>

int main(int argc, char** argv)
{
    Application a(argc, argv);

    qsrand(QDateTime::currentSecsSinceEpoch());

    // 注册可以被识别的卡片
    BKCreator::registCard<TestCard>();
    BKCreator::registCard<PrintCard>();
    BKCreator::registCard<RenderItemCard>();
    BKCreator::registCard<PbsDatablockCard>();
    BKCreator::registCard<PbsCommonTextureCard>();
    BKCreator::registCard<PbsDetailTextureCard>();
    BKCreator::registCard<PbsTransparentCard>();
    BKCreator::registCard<BlendblockCard>();
    BKCreator::registCard<MacroblockCard>();
    BKCreator::registCard<UnlitDatablockCard>();
    BKCreator::registCard<UnlitDiffuseMapCard>();
    // BKCreator::registCard<MaterialCard>();
    BKCreator::registCard<ReadFileCard>();

    // 注册自定义结构体的锚点识别及颜色
    BKAnchor::registDataType(QMetaTypeId<PbsCommonTextureCard::Info>::qt_metatype_id());
    BKAnchor::registDataType(QMetaTypeId<PbsDetailTextureCard::Info>::qt_metatype_id());
    BKAnchor::registDataType(QMetaTypeId<PbsTransparentCard::Info>::qt_metatype_id());
    BKAnchor::registDataType(QMetaTypeId<Ogre::HlmsBlendblock>::qt_metatype_id());
    BKAnchor::registDataType(QMetaTypeId<Ogre::HlmsMacroblock>::qt_metatype_id());
    BKAnchor::registDataType(QMetaTypeId<UnlitDiffuseMapCard::Info>::qt_metatype_id());
  
    OgreBlueprint b;
    b.showMaximized();

    int ret = a.join(std::bind(&OgreBlueprint::drive, &b));

    return ret;
}