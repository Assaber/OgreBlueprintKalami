#include "unit/BKLabel.h"
#include <QPainter>
#include <QDebug>
#include "container/BKAnchor.h"

class BKLabel::Impl : public QGraphicsItem
{
public:
    Impl(BKLabel* handle)
        : mpHandle(handle)
    {

    }

    ~Impl()
    {
        qDebug() << "label item destroy";
    }

public:
    BKLabel* mpHandle = nullptr;
    QString mstrText = "";
    Qt::Alignment mAlignment = Qt::AlignVCenter | Qt::AlignLeft;

public:
    BKLabel* setAlignment(Qt::Alignment alignment)
    {
        mAlignment = alignment;
        return mpHandle;
    }

    BKLabel* setText(const QString& text)
    {
        mstrText = text;

        QFont defaultFont;
        QFontMetrics fm(defaultFont);        //assaber: 需要更新到系统字体
                                            //todo...
        mpHandle->mMinSize = { 1.0f * fm.horizontalAdvance(text), 1.0f * fm.height() };
        return mpHandle;
    }

    QRectF boundingRect() const override
    {
        return { 0, 0, mpHandle->mSize.width(), mpHandle->mSize.height() };
    }

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override
    {
        static_cast<void*>(widget);

        // 绘制文字
        painter->save();
        {
            // painter->setFont(CBConfig::getSingleton().getFont());
            // painter->setPen(theme);
            painter->drawText(boundingRect(), mAlignment, mstrText);
            painter->restore();
        }
    }

};

BKLabel::BKLabel()
    : super()
    , mpImpl(new Impl(this))
{
}

BKLabel::~BKLabel()
{
    delete mpImpl;
    mpImpl = nullptr;
}

QJsonValue BKLabel::getValue()
{
    L_IMPL(BKLabel);

    return l->mstrText;
}

bool BKLabel::setValue(const QJsonValue& val)
{
    setText(val.toString());
    return true;
}

BKLabel* BKLabel::setText(const QString& text)
{
    return mpImpl->setText(text);
}

BKLabel* BKLabel::setAlignment(Qt::Alignment alignment)
{
    return mpImpl->setAlignment(alignment);
}

QGraphicsItem* BKLabel::getGraphicsItem()
{
    return mpImpl;
}

void BKLabel::dataChanged(const QVariant& data)
{
    L_IMPL(BKLabel)

    setText(data.toString());
    l->update();

    if (mpRightAnchor && !mCallbackFunc(data))
        mpRightAnchor->dataChanged(data);
}
