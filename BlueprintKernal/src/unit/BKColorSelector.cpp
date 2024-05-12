#include "unit/BKColorSelector.h"
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QGraphicsProxyWidget>
#include <QStyle>
#include <QGraphicsScene>
#include "container/BKCard.h"
#include "BKEvent.h"
#include <QCoreApplication>
#include <QDebug>
#include <QColorDialog>

class BKColorSelector::Impl : public QGraphicsItem
{
public:
    Impl(BKColorSelector* handle, BKColorSelector::Type type)
        : QGraphicsItem()
        , mpHandle(handle)
        , mType(type)
    {
        syncColor();
    }

public:
    virtual QRectF boundingRect() const override { return mBoundingRect; }

    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override
    {
        painter->save();
        {
            painter->setPen("#D2D2D2");
            painter->setBrush(mColor);
            painter->drawRoundedRect(mCtrlArea, 2.0f, 2.0f);
            painter->restore();
        }
    }

    QString getCurrentColor() {
        if (mType == Type::Vector3)
            return QString("%1 %2 %3").arg(QString::number(mColorData[0], 'f', 3))
            .arg(QString::number(mColorData[1], 'f', 3))
            .arg(QString::number(mColorData[2], 'f', 3));
        else if (mType == Type::Vector4)
            return QString("%1 %2 %3 %4").arg(QString::number(mColorData[0], 'f', 3))
            .arg(QString::number(mColorData[1], 'f', 3))
            .arg(QString::number(mColorData[2], 'f', 3))
            .arg(QString::number(mColorData[3], 'f', 3));

        return "";
    }

    bool loadFromColorString(const QString& color) 
    {
        QStringList split;
        if (color.indexOf(',') < 0)     // 空格分隔
            split = color.split(' ', QString::SkipEmptyParts);
        else
            split = color.split(',', QString::SkipEmptyParts);

        if (split.count() > 2)
        {
            mColorData[0] = split[0].toFloat();
            mColorData[1] = split[1].toFloat();
            mColorData[2] = split[2].toFloat();
        }
        else
        {
            qWarning() << "颜色解析戳啦，戳啦！";
            return false;
        }
        
        if (split.count() > 3)
            mColorData[3] = split[3].toFloat();
        
        syncColor();

        return true;
    }

    void syncColor(bool vec2color = true) 
    {
        if (vec2color)
        {
            mColor.setRed(255 * mColorData[0]);
            mColor.setGreen(255 * mColorData[1]);
            mColor.setBlue(255 * mColorData[2]);
            mColor.setAlpha(255 * mColorData[3]);
        }
        else
        {
            mColorData[0] = 1.0f * mColor.red() / 255;
            mColorData[1] = 1.0f * mColor.green() / 255;
            mColorData[2] = 1.0f * mColor.blue() / 255;
            mColorData[3] = 1.0f * mColor.alpha() / 255;
        };
    }

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event) override
    {
        if (!mBoundingRect.contains(event->pos()))
        {
            event->ignore();
        }
        else
        {
            QColorDialog dlg(mColor);
            if (dlg.exec() != QDialog::Accepted)
                return;

            mColor = dlg.currentColor();
            update();

            event->accept();
        }
    }

public:
    BKColorSelector* mpHandle = nullptr;
    // 包围盒
    QRectF mBoundingRect;
    // 控件区域
    QRect mCtrlArea;
    // 固定上下边距
    static constexpr int mFixedMargin = 2;
    // 当前颜色
    Color4f mColorData = {1.0f, 1.0f, 1.0f, 1.0f};
    QColor mColor;
    // 颜色格式
    BKColorSelector::Type mType;
};

BKColorSelector::BKColorSelector(Type type)
    : super()
    , mpImpl(new Impl(this, type))
{
    setFixedSize({ 35, 20 });
}

BKColorSelector::~BKColorSelector()
{
    delete mpImpl;
    mpImpl = nullptr;
}

bool BKColorSelector::loadFromJson(const QJsonValue& val)
{
    L_IMPL(BKColorSelector);
    return l->loadFromColorString(val.toString());
}

QVariant BKColorSelector::data()
{
    L_IMPL(BKColorSelector);
    return l->mColor;
}

BKColorSelector::operator QJsonValue() const
{
    L_IMPL(BKColorSelector);
    return  l->getCurrentColor();
}

BKUnit* BKColorSelector::copy()
{
    L_IMPL(BKColorSelector);
    BKColorSelector* target = BKCreator::create<BKColorSelector>(l->mType);

    BKColorSelector::Impl* dstImpl = target->mpImpl;
    dstImpl->mColor = l->mColor;
    dstImpl->mColorData = l->mColorData;
    _copyBasicAttributeTo(target);
    return target;
}

BKColorSelector* BKColorSelector::setColor(const Color4f& color)
{
    L_IMPL(BKColorSelector);
    memcpy(l->mColorData.data(), color.data(), 4 * sizeof(float));
    l->syncColor();
    l->update();

    return this;
}

BKColorSelector* BKColorSelector::setColor(const Color3f& color)
{
    L_IMPL(BKColorSelector);
    memcpy(l->mColorData.data(), color.data(), 3 * sizeof(float));
    l->mColorData[3] = 1.0f;
    l->syncColor();
    l->update();

    return this;
}

BKColorSelector* BKColorSelector::setColor(const QColor& color)
{
    L_IMPL(BKColorSelector);
    l->mColor = color;
    l->syncColor(false);
    l->update();

    return this;
}

QGraphicsItem* BKColorSelector::getGraphicsItem()
{
    return mpImpl;
}

void BKColorSelector::resized()
{
    L_IMPL(BKColorSelector);

    l->mBoundingRect = { 0, 0, mSize.width(), mSize.height() };
    l->mCtrlArea = { 0, l->mFixedMargin, static_cast<int>(l->mBoundingRect.width()), static_cast<int>(l->mBoundingRect.height() - 2 * l->mFixedMargin) };
}

void BKColorSelector::dataChanged(const QVariant& data)
{
    L_IMPL(BKColorSelector);

    if (data.isNull())
    {
        if (mpRightAnchor)
            mpRightAnchor->dataChanged(l->mColor);
    }
    else
    {
        setColor(data.value<QColor>());
        l->update();
        if (!mCallbackFunc(data) && mpRightAnchor)
            mpRightAnchor->dataChanged(data);
    }
}
