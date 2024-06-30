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
    template<typename T, typename = typename std::enable_if<std::is_base_of<BKCard, T>::value>::type>
    void registCard(QString group = "Default", QString alias = "") {
        if (group.isEmpty()) {
            group = "Default";
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
     * @remark: When the input parameter group is "", all existing groups will be traversed to find the card
     */
    CardCreatorPtr getCreator(const QString& alias, const QString& group = "Default");

protected:
    virtual void showEvent(QShowEvent* e) override;

private:
    void registItems(const QString& cardName, const QString& alias, const QString& group = "Default");

private:
    class Impl;
    Impl* mpImpl = nullptr;
    std::map<QString, std::map<QString, CardCreatorPtr>> mRegistItems;
};
