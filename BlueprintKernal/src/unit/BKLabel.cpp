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
    bool mbDontUpdate = false;

public:
    BKLabel* setAlignment(Qt::Alignment alignment)
    {
        mAlignment = alignment;
        return mpHandle;
    }

    BKLabel* setText(const QString& text, bool donotUpdate)
    {
        mstrText = text;
        mbDontUpdate = donotUpdate;

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

bool BKLabel::loadFromJson(const QJsonValue& val)
{
    setText(val.toString());
    return true;
}

QVariant BKLabel::data()
{
    L_IMPL(BKLabel);
    return l->mstrText;
}

BKLabel::operator QJsonValue() const
{
    L_IMPL(BKLabel);
    return l->mstrText;
}

BKUnit* BKLabel::copy()
{
    L_IMPL(BKLabel);
    BKLabel* target = BKCreator::create<BKLabel>();

    BKLabel::Impl* dstImpl = target->mpImpl;
    dstImpl->mstrText = l->mstrText;
    dstImpl->mAlignment = l->mAlignment;
    dstImpl->mbDontUpdate = l->mbDontUpdate;
    _copyBasicAttributeTo(target);
    return target;
}


BKLabel* BKLabel::setText(const QString& text, bool dontUpdate/* = false*/)
{
    return mpImpl->setText(text, dontUpdate);
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

    if (data.isNull())
    {
        if (mpRightAnchor)
            mpRightAnchor->dataChanged(l->mstrText);
    }
    else
    {
        if (!l->mbDontUpdate)
        {
            setText(data.toString());
            l->update();
        }

        if (!mCallbackFunc(data) && mpRightAnchor)
            mpRightAnchor->dataChanged(data);
    }
}
