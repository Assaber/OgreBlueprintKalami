#include "CardFilterComboBox.h"
#include "BlueprintLoader.h"

#include <QStyledItemDelegate>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QLineEdit>
#include <QListView>
#include <QPainter>
#include <QEvent>
#include <QDebug>

#include <set>
#include <map>

#define L_IMPL(x) x::Impl* l = mpImpl;

using DataRecordMap = std::map<QString, QStringList>;

class TreeView : public QListView
{
public:
    using super = QListView;
    using StringSet = std::set<QString>;
    struct Item
    {
        QString name;
        bool isGroup = false;
        DataRecordMap::const_iterator origin;
        int parentIndex = -1;
    };

    class Model : public QAbstractItemModel
    {
    public:
        static constexpr int IsGroupRole = Qt::UserRole + 1;
        static constexpr int GroupNameRole = Qt::UserRole + 2;

        using super = QAbstractItemModel;
        Model(TreeView* parent = nullptr)
            : super(parent)
            , mpParent(parent)
        {
        }

    public:
        virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override { return mpParent->mVisibleItems.size(); }

        virtual int columnCount(const QModelIndex& parent = QModelIndex()) const override { return 1; }

        virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override { return createIndex(row, column); }

        virtual QModelIndex parent(const QModelIndex& child) const { return QModelIndex(); }

        virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override
        {
            if (!index.isValid() || index.row() >= mpParent->mVisibleItems.size())
                return QVariant();

            const Item& item = *mpParent->mVisibleItems[index.row()];
            if (role == Qt::DisplayRole || role == Qt::ToolTipRole) {
                return item.name;
            }
            else if (role == Qt::TextAlignmentRole) {
                return static_cast<uint32_t>(Qt::AlignVCenter | Qt::AlignLeft);
            }
            else if (role == IsGroupRole) {
                return item.isGroup;
            }
            else if (role == GroupNameRole) {
                return item.origin->first;
            }
            return QVariant();
        }

    public:
        friend class TreeView;
        TreeView* mpParent = nullptr;
    };

    class Delegate : public QStyledItemDelegate
    {
    public:
        using super = QStyledItemDelegate;
        using super::super;

    public:
        void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override
        {
            bool isGroup = index.data(Model::IsGroupRole).toBool();
            TreeView* tv = getRealParent();
            QString text = index.data().toString();
            qreal height = option.rect.height();
            QRect textRect;

            if (index.data(Model::IsGroupRole).toBool())
            {
                static constexpr qreal offset = 5;
                qreal b = 0.866 * (height - 2 * offset);
                qreal c = (height - b) / 2;

                painter->save();
                painter->setPen(Qt::NoPen);
                painter->setBrush(QColor(255, 128, 0));

                if (tv->mHideGroups.find(text) != tv->mHideGroups.end())
                {
                    QPointF polygon[3] = {
                        QPointF( c, offset) + option.rect.topLeft(),
                        QPointF( c, height - offset ) + option.rect.topLeft(),
                        QPointF( height - c, height / 2 ) + option.rect.topLeft()
                    };
                    painter->drawPolygon(polygon, 3);
                }
                else
                {
                    QPointF polygon[3] = {
                        QPointF( offset, c ) + option.rect.topLeft(),
                        QPointF( height / 2, height - c) + option.rect.topLeft(),
                        QPointF( height - offset, c ) + option.rect.topLeft()
                    };
                    painter->drawPolygon(polygon, 3);
                }

                painter->restore();

                textRect = option.rect.adjusted(height, 0, 0, 0);
            }
            else {
                textRect = option.rect.adjusted(16 + height, 0, 0, 0);
                if (index.row() == mHoverRow) { 
                    painter->fillRect(textRect, QColor(255, 128, 0, 126));
                }
            }

            painter->drawText(textRect, text);
        }

        bool editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index) override
        {
            if (event->type() == QEvent::MouseMove) 
            {
                mHoverRow = index.row();
                if (mLastHowerRow != mHoverRow)
                {
                    TreeView* tv = dynamic_cast<TreeView*>(parent());
                    tv->update(model->index(mLastHowerRow, 0));
                    mLastHowerRow = mHoverRow;
                }
            }
            else if (event->type() == QEvent::MouseButtonPress 
                && dynamic_cast<QMouseEvent*>(event)->button() == Qt::LeftButton) 
            {
                TreeView* tv = getRealParent();
                bool isGroup = index.data(Model::IsGroupRole).toBool();
                QString name = index.data().toString();

                if (isGroup)                        // Collapse or expand
                {
                    auto itor = tv->mHideGroups.find(name);
                    if (itor == tv->mHideGroups.end()) {
                        tv->mHideGroups.insert(name);
                    }
                    else {
                        tv->mHideGroups.erase(itor);
                    }
                    tv->refreshVisibleDict();
                }
                else if (tv->mItemClickedCallback) { // Pick one of the lucky ones
                    tv->mItemClickedCallback(index);
                }

                event->ignore();
            }

            return false;
        }

        TreeView* getRealParent() const {
            return dynamic_cast<TreeView*>(parent());
        }

    private:
        int mHoverRow = -1, mLastHowerRow = -1;
    };

public:
    TreeView(CardFilterComboBox* parent = nullptr)
        : QListView(parent)
        , mpModel(new Model(this))
        , mpDelegate(new Delegate(this))
    {
        setMouseTracking(true);
        setModel(mpModel);
        setItemDelegate(mpDelegate);
    }

public:
    void updateAll(const DataRecordMap& dict)
    {
        refreshOriginDict(dict);
        reload();
    }

    void reload()
    {
        mHideGroups.clear();
        mstrFilter = "";
        refreshVisibleDict();
    }

    void refreshOriginDict(const DataRecordMap& dict)
    {
        mItems.clear();

        DataRecordMap::const_iterator itor = dict.begin();
        int j = 0, parentIndex = 0;
        while (itor != dict.end())
        {
            Item gi;
            gi.isGroup = true;
            gi.name = itor->first;
            gi.origin = itor;
            gi.parentIndex = -1;
            mItems.push_back(gi);
            parentIndex = j++;

            for (int i = 0; i < itor->second.size(); ++i, ++j)
            {
                Item gi;
                gi.isGroup = false;
                gi.name = itor->second[i];
                gi.origin = itor;
                gi.parentIndex = parentIndex;
                mItems.push_back(gi);
            }
            ++itor;
        }
    }

    void refreshVisibleDict()
    {
        mpModel->beginResetModel();

        mVisibleItems.clear();

        decltype(mVisibleItems) cacheList;
        QList<Item>::const_iterator parentItor;

        auto push_data_in_group_mode = [&]() {
            if (cacheList.size()) 
            {
                mVisibleItems.push_back(parentItor);

                if (mHideGroups.find(parentItor->name) == mHideGroups.end()) {          // Only visible groups need to be expanded
                    for (const auto& item : cacheList) {
                        mVisibleItems.push_back(item);
                    }
                }
            }
            cacheList.clear();
        };

        QList<Item>::const_iterator itor = mItems.begin();
        bool skip = false;
        while (itor != mItems.end())
        {
            do 
            {
                if (mstrFilter.isEmpty())
                {
                    if (itor->isGroup)          //This method of judgment needs to be strictly adhered: Group1 - Member - Member - ... - Member - Group2 - Member...
                    {
                        skip = false;
                        if (mHideGroups.find(itor->name) != mHideGroups.end()) 
                        {
                            mVisibleItems.push_back(itor);      // The group name needs to be retained
                            skip = true;
                            break;
                        }
                    }
                    else if (skip)
                        break;

                    mVisibleItems.push_back(itor);
                }
                else
                {
                    if (itor->isGroup)
                    {
                        skip = false;
                        push_data_in_group_mode();
                        parentItor = itor;
                    }
                    else if (skip)
                        break;
                    else {
                        if (itor->name.indexOf(mstrFilter) < 0)
                            break;

                        cacheList.push_back(itor);
                    }
                }

            } while (false);

            ++itor;
        }

        push_data_in_group_mode();

        mpModel->endResetModel();
    }

    void setFilter(const QString& filter)
    {
        mstrFilter = filter;
        refreshVisibleDict();
    }

    QModelIndex checkAvailable(const QString& name)
    {
        QString group = "";
        for (int i = 0; i < mVisibleItems.size(); ++i)
        {
            const auto& item = mVisibleItems[i];
            if (item->name.compare(name) == 0) {
                return mpModel->createIndex(i, 0);
            }
        }

        return QModelIndex();
    }

    QStringList getAllAvailable()
    {
        QStringList ret;

        for (const auto& item : mVisibleItems)
        {
            if(item->isGroup)
                continue;

            ret << item->name;
        }

        return ret;
    }

    void setDelegateItemClickedCallback(std::function<void(const QModelIndex&)> func) {
        mItemClickedCallback = func;
    }

public:
    Model* mpModel;
    Delegate* mpDelegate;
    QList<Item> mItems;
    QList<QList<Item>::const_iterator> mVisibleItems;
    StringSet mHideGroups;
    QString mstrFilter = "";
    std::function<void(const QModelIndex&)> mItemClickedCallback = nullptr;
};

class CardFilterComboBox::Impl
{
public:
    Impl(CardFilterComboBox* handle, BlueprintLoader* loader)
        : mpHandle(handle)
        , mpLoader(loader)
    {
        mpLineEdit = new QLineEdit(mpHandle);
        mpPopView = new TreeView(mpHandle);
        mpPopView->setFixedHeight(260);
        mpPopView->setDelegateItemClickedCallback(std::bind(&Impl::sendItemSelected, this, std::placeholders::_1));

        QVBoxLayout* mainLayout = new QVBoxLayout(mpHandle);
        mainLayout->setMargin(0);
        mainLayout->setSpacing(0);
        mainLayout->addWidget(mpLineEdit);
        mainLayout->addWidget(mpPopView);

        QObject::connect(mpLineEdit, &QLineEdit::textChanged, mpHandle, [this](const QString& text) {
            mpPopView->setFilter(text);
            });

        QObject::connect(mpLineEdit, &QLineEdit::returnPressed, mpHandle, [this]() {
            QModelIndex index = mpPopView->checkAvailable(mpLineEdit->text());
            sendItemSelected(index);
            });
    }

public:
    void sendItemSelected(const QModelIndex& index)
    {
        if (!index.isValid())
            return;

        QString group = index.data(TreeView::Model::GroupNameRole).toString();
        QString name = transAlias2CardName(group, index.data().toString());
        CardCreatorPtr creator = mpHandle->getCreator(name, group);
        if (creator) {
            mpLoader->setDarling(creator(mpLoader));
        }
        mpHandle->hide();
    }

    void init()
    {
        mpLineEdit->setText("");
        mpPopView->reload();
    }

    void updatePopView()
    {
        mActiveRecordMap.clear();
        for (const auto& group : mItems)
        {
            auto& groupItems = mActiveRecordMap[group.first];
            for (const auto& child : group.second) {
                groupItems.push_back(child.first);
            }
        }

        mpPopView->updateAll(mActiveRecordMap);
    }

    QString transAlias2CardName(const QString& group, const QString& alias) const 
    {
        auto itor = mItems.find(group);
        if (itor == mItems.end())
            return "";

        const auto& groupItems = itor->second;
        auto gItor = groupItems.find(alias);
        if (gItor == groupItems.end())
            return "";

        return gItor->second;
    }

public:
    CardFilterComboBox* mpHandle = nullptr;
    TreeView* mpPopView = nullptr;
    QLineEdit* mpLineEdit = nullptr;

    // group-<alias, cardName>
    std::map<QString, std::map<QString, QString>> mItems;
    DataRecordMap mActiveRecordMap;

    BlueprintLoader* mpLoader = nullptr;
};

CardFilterComboBox::CardFilterComboBox(BlueprintLoader* parent)
    : QFrame(parent)
    , mpImpl(new Impl(this, parent))
{
    setFixedWidth(140);
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
}

CardFilterComboBox::~CardFilterComboBox()
{
    delete mpImpl;
    mpImpl = nullptr;
}


void CardFilterComboBox::registItems(const QString& cardName, const QString& alias, const QString& group)
{
    L_IMPL(CardFilterComboBox);
    QString groupName = group.isEmpty() ? "Default" : group;
    auto& items = l->mItems[groupName];

    if (items.find(alias) == items.end())
    {
        items.insert({ alias, cardName });
        l->updatePopView();
    }
}

CardCreatorPtr CardFilterComboBox::getCreator(const QString& name, const QString& group /*= strDefaultGroupName*/)
{
    auto get_item_in_group = [&](const std::map<QString, CardCreatorPtr>& group) -> CardCreatorPtr {
        auto childItem = group.find(name);
        return childItem == group.end() ? nullptr : childItem->second;
    };

    CardCreatorPtr creator = nullptr;

    if (group.isEmpty())
    {
        for (const auto& items : mRegistItems)
        {
            creator = get_item_in_group(items.second);
            if (creator)
                break;
        }
    }
    else
    {
        auto gi = mRegistItems.find(group);
        if (gi == mRegistItems.end())
            return nullptr;

        creator = get_item_in_group(gi->second);
    }

    return creator;
}

void CardFilterComboBox::showEvent(QShowEvent* e)
{
    L_IMPL(CardFilterComboBox);
    l->init();

    QFrame::showEvent(e);
}
