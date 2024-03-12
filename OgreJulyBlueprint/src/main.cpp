#include <QApplication>
#include "OgreBlueprint.h"

int main(int argc, char** argv)
{
    QApplication a(argc, argv);

    OgreBlueprint blueprint;
    blueprint.showMaximized();

    int ret = a.exec();
    return 0;
}