#include "unit/BKComboBox.h"
#include <QGraphicsProxyWidget>
#include <QComboBox>
#include <QPainterPath>
#include <QListView>
#include <QStringListModel>
#include <QDebug>
#include <QGraphicsItemGroup>

class BKComboBoxInnerListView : public QListView
{
public:
    BKComboBoxInnerListView(QWidget* parent, QGraphicsProxyWidget* graphicsItem)
        : QListView(parent)
        , mpBindItem(graphicsItem)
    {
        // setStyleSheet("background-color:blue;");
    }

protected:
    /*
        这里好坑呀： 在demo里是一个group里挂着两个普通item，测试没问题；然后在这个工程里就一直错（现象是弹出的菜单一直在下面）。
        感觉是Z的问题，最后最后才想起来整体结构不一样
        在工程里是card包裹多个cell，每个cell再包裹unit，而不是所有unit在一个cell
        所以Z比较就不一样了，这里将每个comboBox的上升group进行Z提升再进行比较就对了！
        焯！
    */

    void hideEvent(QHideEvent* event) override
    {
        QListView::hideEvent(event);
        auto cell = mpBindItem->group();
        if(cell)
            cell->setZValue(0);
    }

    void showEvent(QShowEvent* e) override
    {
        QListView::showEvent(e);
        auto cell = mpBindItem->group();
        if (cell)
            cell->setZValue(1.0f);
    }

private:
    QGraphicsProxyWidget* mpBindItem = nullptr;
};

class BKComboBoxWidget : public QComboBox
{
public:
    using super = QComboBox;
    using super::super;

public:
    void setGraphicsThingBind(QGraphicsProxyWidget* graphicsItem)
    {
        setModel(new QStringListModel());
        setView(new BKComboBoxInnerListView(this, graphicsItem));
    }
};

class BKComboBox::Impl : public QGraphicsProxyWidget
{
public:
    Impl(BKComboBox* handle)
        : QGraphicsProxyWidget()
        , mpHandle(handle)
    {
        mpWidget = new BKComboBoxWidget();
        setWidget(mpWidget);
        mpWidget->setGraphicsThingBind(this);
    }

    ~Impl()
    {
        //父类会在析构时释放窗体
    }
public:
    BKComboBox* mpHandle = nullptr;
    // 窗体控件
    BKComboBoxWidget* mpWidget = nullptr;
};

BKComboBox::BKComboBox()
    : super()
    , mpImpl(new Impl(this))
{

}

BKComboBox::~BKComboBox()
{
    delete mpImpl;
    mpImpl = nullptr;
}

BKComboBox* BKComboBox::setDefaultIndex(int index)
{
    BKComboBoxWidget* comb = mpImpl->mpWidget;

    if (index < comb->count())
        comb->setCurrentIndex(index);
   
    return this;
}

BKComboBox* BKComboBox::setItems(const QStringList& items)
{
    BKComboBoxWidget* comb = mpImpl->mpWidget;
    dynamic_cast<QStringListModel*>(comb->model())->setStringList(items);

    return this;
}

QGraphicsItem* BKComboBox::getGraphicsItem()
{
    return mpImpl;
}

void BKComboBox::resized()
{
    mpImpl->mpWidget->setFixedSize(mSize.width(), mSize.height());
}
