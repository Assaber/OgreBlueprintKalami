#include "ResetResourceSup.h"
#include <QFileInfo>

ResetResourceSup::ResetResourceSup(const Ogre::String& group /*= ""*/)
    : super()
{
    if (!group.empty()) {
        mstrDefaultResourceGroup = group;
    }
}

void ResetResourceSup::resetResourceDir(const Ogre::String& filepath, const Ogre::String& group)
{
    const Ogre::String& realGroup = group.empty() ? mstrDefaultResourceGroup : group;

    Ogre::ResourceGroupManager* groupMgr = Ogre::ResourceGroupManager::getSingletonPtr();

    if (!groupMgr->resourceGroupExists(group)) {
        groupMgr->createResourceGroup(group);
    }

    QFileInfo fileInfo(filepath.c_str());
    Ogre::String dir = fileInfo.absolutePath().toStdString();
    Ogre::String name = fileInfo.fileName().toStdString();

    if (!groupMgr->resourceLocationExists(dir, group)) {
        groupMgr->addResourceLocation(dir, "FileSystem", group);
    }

    bool needDeclare = true;
    for (const Ogre::ResourceGroupManager::ResourceDeclaration& rd : groupMgr->getResourceDeclarationList(group))
    {
        if (rd.resourceName.compare(name) == 0)
        {
            needDeclare = false;
            break;
        }
    }

    if (needDeclare) {
        groupMgr->declareResource(name, "Texture", group);
    }

    groupMgr->loadResourceGroup(group);
}