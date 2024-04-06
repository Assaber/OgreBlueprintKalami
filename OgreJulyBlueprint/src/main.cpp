#include "Application.h"
#include "OgreBlueprint.h"

#include "BKCreator.h"
#include "TestCard.h"
#include "PrintCard.h"

int main(int argc, char** argv)
{
    Application a(argc, argv);

    BKCreator::registCard<TestCard>();
    BKCreator::registCard<PrintCard>();

    OgreBlueprint b;
    b.showMaximized();

    int ret = a.join(
        [&]() {
            b.drive();
        }
    );

    return ret;
}