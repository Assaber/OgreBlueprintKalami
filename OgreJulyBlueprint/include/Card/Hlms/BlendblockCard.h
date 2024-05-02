#pragma once
#include "container/BKCard.h"
#include "OgreHlmsDatablock.h"
#include <array>

#define SCENE_BLEND_TYPES \
    SBT_ITEM(TRANSPARENT_ALPHA) \
    SBT_ITEM(TRANSPARENT_COLOUR) \
    SBT_ITEM(ADD) \
    SBT_ITEM(MODULATE) \
    SBT_ITEM(REPLACE)
   
#define SBT_ITEM(x) x = Ogre::SceneBlendType::SBT_##x,
enum ShadowSceneBlendType {
    SCENE_BLEND_TYPES
};
#undef SBT_ITEM

class BKCell;
class BKCheckBox;
class BlendblockCard : public BKCard
{
    CREATE_CARD_FACTORY(BlendblockCard)
public:
    BlendblockCard();
    virtual QVariant getCurrentCardValue() override;

private:
    /**
     * @brief:                              更新绑定通道掩码
     * @param: BKCheckBox * ptr             控件指针
     * @param: bool enable                  开关使能
     * @return: void                        
     * @remark: 
     */
    void updateBlendChannelMask(BKCheckBox* ptr, bool enable);

    /**
     * @brief:                              更新混合类型                      
     */
    void updateBlendType();
     
private:
    // 掩码四骑士
    std::array<BKCheckBox*, 4> mChannelMaskCheckBoies;
    // 绑定块结构体
    Ogre::HlmsBlendblock mBlendblock;
    // 混合类型
    ShadowSceneBlendType mBlendType = ShadowSceneBlendType::REPLACE;
    // 透明混合类型使能
    bool mbAlphaBlendTypeEnable = true;
    // 透明混合类型
    ShadowSceneBlendType mAlphaBlendType = ShadowSceneBlendType::REPLACE;
};

Q_DECLARE_METATYPE(Ogre::HlmsBlendblock)