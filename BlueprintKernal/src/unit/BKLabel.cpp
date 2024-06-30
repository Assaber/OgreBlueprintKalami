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

public:
    BKLabel* mpHandle = nullptr;
    QString mstrText = "";
    Qt::Alignment mAlignment = Qt::AlignVCenter | Qt::AlignLeft;
    bool mbDontUpdate = false;

public:
    QRectF boundingRect() const override
    {
        return { 0, 0, mpHandle->mSize.width(), mpHandle->mSize.height() };
    }

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override
    {
        static_cast<void*>(widget);

        painter->save();
        {
            // painter->setFont(CBConfig::getSingleton().getFont());
            // painter->setPen(theme);
            painter->drawText(boundingRect(), mAlignment, mstrText);
            painter->restore();
        }
    }
};

BKLabel::BKLabel(const QString& text/* = ""*/)
    : super()
    , mpImpl(new Impl(this))
{
    setText(text);
}

BKLabel::~BKLabel()
{
    delete mpImpl;
    mpImpl = nullptr;
}

bool BKLabel::loadFromJson(const QJsonValue& val)
{
    dataChanged(val.toString());
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


BKLabel* BKLabel::setText(const QString& text)
{
    L_IMPL(BKLabel);
    l->mstrText = text;

    static QFont defaultFont;
    QFontMetrics fm(defaultFont);        //The only font in the program should be loaded, todo...
    mMinSize = { 1.0f * fm.horizontalAdvance(text), 1.0f * fm.height() };
    return this;
}

BKLabel* BKLabel::setTitleNeverChanges(bool enable)
{
    L_IMPL(BKLabel);

    l->mbDontUpdate = enable;
    return this;
}

BKLabel* BKLabel::setAlignment(Qt::Alignment alignment)
{
    L_IMPL(BKLabel);

    l->mAlignment = alignment;
    return this;
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
        if (mpRightAnchor) {
            mpRightAnchor->dataChanged(l->mstrText);
        }
    }
    else
    {
        if (!l->mbDontUpdate)
        {
            setText(data.toString());
            l->update();
        }

        if (!mCallbackFunc(this, data) && mpRightAnchor) {
            mpRightAnchor->dataChanged(data);
        }
    }
}
