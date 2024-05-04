#pragma once
#include "container/BKCard.h"
#include "OgreResourceGroupManager.h"

class ResetResourceSup : public BKCard
{
    using super = BKCard;
protected:
    ResetResourceSup(const Ogre::String& group = "");

protected:
    void resetResourceDir(const Ogre::String& filepath, const Ogre::String& group = "");

protected:
    Ogre::String mstrDefaultResourceGroup = Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME;
};