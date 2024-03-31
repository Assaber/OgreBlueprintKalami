#include <QApplication>
#include "OgreBlueprint.h"

#include "BKCreator.h"
#include "TestCard.h"
#include "PrintCard.h"

int main(int argc, char** argv)
{
    QApplication a(argc, argv);

    BKCreator::registCard<TestCard>();
    BKCreator::registCard<PrintCard>();

    OgreBlueprint blueprint;
    blueprint.showMaximized();

    int ret = a.exec();
    return 0;
}