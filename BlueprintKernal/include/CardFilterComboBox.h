#pragma once
#include "global_blueprint_kernal.h"
#include <QFrame>
#include "BKCreator.hpp"

class BKCard;
class BlueprintLoader;
class _BlueprintKernalExport CardFilterComboBox : public QFrame
{
public:
    CardFilterComboBox(BlueprintLoader* parent);
    ~CardFilterComboBox();

public:
    template<typename T, typename = std::enable_if_t<std::is_base_of_v<BKCard, T>>>
    void registCard(QString group = DefaultGroupName, QString alias = "") {
        if (group.isEmpty()) {
            group = DefaultGroupName;
        }
        auto& registGroup = mRegistItems[group];

        QString cardName = T::Factory::_cardName;
        if (alias.isEmpty()) {
            alias = cardName;
        }

        registGroup.insert({ cardName , T::Factory::createOne });
        registItems(cardName, alias, group);
    }

    /**
     * @brief:                                          通过卡片名字获取构建器
     * @param: const QString & name                     卡片名称
     * @param: const QString & group                    卡片隶属组，当传入参数为""时将遍历所有已存在的组来寻找卡片
     * @return: CardCreatorPtr                          构建器函数指针，未查找到则返回nullptr
     * @remark: 
     */
    CardCreatorPtr getCreator(const QString& alias, const QString& group = DefaultGroupName);

protected:
    virtual void showEvent(QShowEvent* e) override;

private:
    void registItems(const QString& cardName, const QString& alias, const QString& group = DefaultGroupName);

private:
    class Impl;
    Impl* mpImpl = nullptr;
    std::map<QString, std::map<QString, CardCreatorPtr>> mRegistItems;

public:
    static constexpr char DefaultGroupName[] = "Default";
};