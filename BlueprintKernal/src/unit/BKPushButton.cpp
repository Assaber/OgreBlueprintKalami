﻿#include "unit/BKPushButton.h"
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QFontMetrics>
#include "BKCreator.h"

class BKPushButton::Impl : public QGraphicsItem
{
public:
    Impl(BKPushButton* handle)
        : QGraphicsItem()
        , mpHandle(handle)
    {
        mOption.setWrapMode(QTextOption::NoWrap);
        mOption.setAlignment(Qt::AlignCenter);
    }

    ~Impl()
    {
    }

public:
    virtual QRectF boundingRect() const override
    {
        return mBoundingRect;
    }

    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override
    {
        QRectF rect = mbPressed ? mButtonArea - QMargins(2, 2, 0, 0) : mButtonArea - QMargins(1, 1, 1, 1);

        // 绘制按钮
        painter->save();
        {
            painter->setPen(Qt::NoPen);
            painter->setBrush(QColor(255, 128, 64));
            painter->drawRoundedRect(rect, 1.0f, 1.0f);
            painter->restore();
        }

        // 绘制文字
        painter->save();
        {
            painter->setPen(Qt::white);
            painter->setBrush(Qt::NoBrush);
            painter->drawText(rect, mstrButtonText, mOption);
            painter->restore();
        }
    }

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event) override
    {
        if (event->button() == Qt::LeftButton)
        {
            if (mButtonArea.contains(event->pos().toPoint()))
            {
                mbPressed = true;
                update();
            }
        }
        event->accept();
    }

    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override
    {
        if (event->button() == Qt::LeftButton)
        {
            if (mButtonArea.contains(event->pos().toPoint()))
            {
                mpHandle->dataChanged(QVariant());
            }
            mbPressed = false;
            update();
        }
        event->accept();
    }

public:
    BKPushButton* mpHandle = nullptr;
    // 固定上下边距
    static constexpr int mFixedMargin = 2;
    // 包围盒
    QRectF mBoundingRect;
    // 按钮区域
    QRectF mButtonArea;
    // 是否为按下状态
    bool mbPressed = false;
    // 文字绘制选项
    QTextOption mOption;
    // 按钮文字
    QString mstrButtonText = "";
};

BKPushButton::BKPushButton()
    : super()
    , mpImpl(new Impl(this))
{
}

bool BKPushButton::loadFromJson(const QJsonValue& val)
{
    return true;
}

QVariant BKPushButton::data()
{
    return true;
}

BKPushButton::operator QJsonValue() const 
{
    return true;
}

BKUnit* BKPushButton::copy()
{
    L_IMPL(BKPushButton);
    BKPushButton* target = BKCreator::create<BKPushButton>();

    BKPushButton::Impl* dstImpl = target->mpImpl;
    dstImpl->mstrButtonText = l->mstrButtonText;
    _copyBasicAttributeTo(target);
    return target;
}

BKPushButton* BKPushButton::setText(const QString& text, bool comfy/* = false*/)
{
    L_IMPL(BKPushButton)

    l->mstrButtonText = text;

    if (comfy) {
        QFont defaultFont;
        QFontMetrics fm(defaultFont);           //assaber: 需要更新到系统字体
                                                //todo...
        setFixedSize({ 1.0f * fm.horizontalAdvance(text) + 4 * l->mFixedMargin, 1.0f * fm.height() + 4 * l->mFixedMargin });
    }

    return this;
}

QGraphicsItem* BKPushButton::getGraphicsItem()
{
    return mpImpl;
}

void BKPushButton::resized()
{
    L_IMPL(BKPushButton)

    l->mBoundingRect = { 0, 0, mSize.width(), mSize.height() };
    l->mButtonArea = { l->mFixedMargin, l->mFixedMargin, mSize.width() - 2 * l->mFixedMargin, mSize.height() - 2 * l->mFixedMargin };
}

void BKPushButton::dataChanged(const QVariant& data)
{
    mCallbackFunc(true);
}