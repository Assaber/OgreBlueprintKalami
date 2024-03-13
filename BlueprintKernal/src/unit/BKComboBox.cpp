#include "unit/BKComboBox.h"
#include <QGraphicsProxyWidget>
#include <QComboBox>
#include <QPainterPath>

class BKComboBoxWidget : public QComboBox
{
public:
    using super = QComboBox;
    using super::super;
};

class BKComboBox::Impl : public QGraphicsProxyWidget
{
public:
    Impl(BKComboBox* handle)
        : QGraphicsProxyWidget()
        , mpHandle(handle)
        , mpWidget(new BKComboBoxWidget())
    {
        setWidget(mpWidget);
    }

    ~Impl()
    {
        //父类会在析构时释放窗体
    }

    virtual QPainterPath shape() const override
    {
        return mShape;
    }


    virtual QRectF boundingRect() const
    {
        return mBoundingRect;
    }


public:
    BKComboBox* mpHandle = nullptr;
    // 窗体控件
    BKComboBoxWidget* mpWidget = nullptr;
    // 
    QPainterPath mShape;
    QRectF mBoundingRect;
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
    comb->clear();
    comb->addItems(items);

    return this;
}

QGraphicsItem* BKComboBox::getGraphicsItem()
{
    return mpImpl;
}

void BKComboBox::resized()
{
    mpImpl->mBoundingRect = { {0, 0}, mSize };

    mpImpl->mShape = QPainterPath();
    mpImpl->mShape.addRect(mpImpl->mBoundingRect);

    mpImpl->mpWidget->setFixedSize(mSize.width(), mSize.height());
}
