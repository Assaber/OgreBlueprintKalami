#include "Application.h"
#include "OgreBlueprint.h"

#include "BKCreator.h"
#include "TestCard.h"
#include "PrintCard.h"
#include "RenderItemCard.h"
#include "Card/Hlms/PbsDatablock.h"

int main(int argc, char** argv)
{
    Application a(argc, argv);

    BKCreator::registCard<TestCard>();
    BKCreator::registCard<PrintCard>();
    BKCreator::registCard<RenderItemCard>();
    BKCreator::registCard<PbsDatablockCard>();

    OgreBlueprint b;
    b.showMaximized();

    int ret = a.join(
        [&]() {
            b.drive();
        }
    );

    return ret;
}