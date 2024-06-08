#pragma once
#include "global_blueprint_kernal.h"
#include <QFrame>
#include "BKCreator.hpp"

class BKCard;
class BlueprintLoader;
class _BlueprintKernalExport CardFilterComboBox : public QFrame
{
    Q_OBJECT
public:
    CardFilterComboBox(BlueprintLoader* parent);
    ~CardFilterComboBox();

public:
    void registItems(const QString& item, const QString& group = DefaultGroupName);
    void registItems(const QStringList& items, const QString& group = DefaultGroupName);

    template<typename T, typename = std::enable_if_t<std::is_base_of_v<BKCard, T>>>
    void registCard(QString group = DefaultGroupName, QString display = "") {
        if (group.isEmpty())
            group = DefaultGroupName;

        auto& g = mRegistItems[group];

        if (display.isEmpty())
            display = T::Factory::_cardName;

        g.insert({ display , T::Factory::createOne });
        registItems(display, group);
    }

    CardCreatorPtr getCreator(const QString& name, const QString& group = DefaultGroupName);

protected:
    virtual void showEvent(QShowEvent* e) override;

private:
    class Impl;
    Impl* mpImpl = nullptr;
    std::map<QString, std::map<QString, CardCreatorPtr>> mRegistItems;

public:
    static constexpr char DefaultGroupName[] = "Default";
};