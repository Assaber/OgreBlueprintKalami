﻿#include "unit/BKComboBox.h"
#include <QGraphicsItem>
#include <QPainter>
#include <QTextOption>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsProxyWidget>
#include <QListWidget>
#include <QGraphicsScene>
#include "container/BKCard.h"
#include <QCoreApplication>
#include "BKEvent.h"
#include <QToolTip>

class BKComboBox::Impl : public QGraphicsItem
{
public:
    Impl(BKComboBox* handle)
        : mpHandle(handle)
    {
        mOption.setWrapMode(QTextOption::NoWrap);
        mOption.setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    }

public:
    virtual QRectF boundingRect() const override {
        return mBoundingRect;
    }

    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override
    {
        // 绘制边框
        painter->save();
        {
            painter->setPen(QColor(255, 128, 64));
            painter->setBrush(Qt::NoBrush);
            painter->drawRoundedRect(mCtrlArea, 1.0f, 1.0f);
            painter->restore();
        }

        // 绘制文字
        if (mCurrentIndex > -1)
        {
            painter->save();
            {
                painter->setPen(Qt::black);
                painter->setBrush(Qt::NoBrush);
                painter->drawText(mTextArea, mItems[mCurrentIndex], mOption);
                painter->restore();
            }
        }

        // 绘制按钮
        painter->save();
        {
            painter->setPen(mDropAnchorPen);
            painter->setBrush(Qt::NoBrush);
            painter->drawPath(mDropAnchorPainterPath);
            painter->restore();
        }
    }

    QString getCurrentText() {
        return (mCurrentIndex > -1 && mCurrentIndex < mItems.size()) ? mItems[mCurrentIndex] : "";
    }

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event) override
    {
        if (!mpHandle->mbEnable)
        {
            event->ignore();
            return;
        }

        if (event->button() == Qt::LeftButton)
        {
            if (mCtrlArea.contains(event->pos().toPoint()))
            {
                mbPressed = true;

                // 在按下的时候就发，早处理早省心
                // 很快的
                TopmostCardEvent e(mpHandle->mpBindCard->getBindItem());
                qApp->sendEvent(scene(), &e);

                update();
            }
        }
        event->accept();
    }

    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;


public:
    BKComboBox* mpHandle = nullptr;
    // 固定上下边距
    static constexpr int mFixedMargin = 2;
    // 下拉列表
    QStringList mItems;
    // 当前索引
    int32_t mCurrentIndex = -1;
    // 包围盒
    QRectF mBoundingRect;
    // 控件区域
    QRect mCtrlArea;
    // 按钮区域
    QRect mButtonArea;
    // 下拉按钮路径
    QPainterPath mDropAnchorPainterPath;
    // 文字区域
    QRect mTextArea;
    // 是否为按下状态
    bool mbPressed = false;
    // 文字绘制选项
    QTextOption mOption;
    // 展开窗体
    static BKComboBoxItemView* mpPublicView;
    // 回调参数类型
    CallbackParamType mCbType = CallbackParamType::Data;
    // 绘制按钮的画笔
    static QPen mDropAnchorPen;
};

BKComboBoxItemView* BKComboBox::Impl::mpPublicView = nullptr;
QPen BKComboBox::Impl::mDropAnchorPen = QPen(QColor("#FF8040"), 1);


class BKComboBoxItemView : public QGraphicsProxyWidget
{
public:
    using super = QGraphicsProxyWidget;
    BKComboBoxItemView()
        : super()
    {
        mpView = new QListWidget();
        mpView->setFrameShape(QFrame::NoFrame);
        mpView->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
        mpView->setStyleSheet("QListWidget{" \
            "	padding: 0px;" \
            "	margin: 0px;" \
            "	outline: 0px;" \
            "}" \
            "" \
            "QListView::item:selected {" \
            "	background: rgb(255, 128, 0);" \
            "	outline: 0px;" \
            "}" \
            "" \
            "QListView::item:hover {" \
            "	background: rgba(255, 128, 64, 120);" \
            "}" \
            "" \
            "QListWidget QScrollBar::vertical {" \
            "	background-color: rgba(0, 0, 0, 64);" \
            "	width: 8px;" \
            "	border-radius: 4px;" \
            "}" \
            "" \
            "QListWidget QScrollBar::handle:vertical {" \
            "	background-color:rgb(255, 128, 64);" \
            "	width: 8px;" \
            "	border-radius: 4px;" \
            "}" \
            "" \
            "QListWidget QScrollBar::add-line:vertical," \
            "QListWidget QScrollBar::sub-line:vertical," \
            "QListWidget QScrollBar::add-page:vertical," \
            "QListWidget QScrollBar::sub-page:vertical {" \
            "	background:none;" \
            "	border:none;" \
            "}");
        mpView->style()->polish(mpView);
        setWidget(mpView);

        QObject::connect(mpView, &QListWidget::itemClicked, this, [this]() {
            if (mpBindItem)
            {
                auto root = mpBindItem->mpHandle;
                int index = mpView->currentIndex().row();
                mpBindItem->mpHandle->dataChanged(mpBindItem->mItems[index]);
            }
                
            resetBind();
            });

        setZValue(2.0f);

        mpView->installEventFilter(this);
        mpView->setMouseTracking(true);
        QObject::connect(mpView, &QListWidget::entered, mpView, [=](const QModelIndex& item) {
            QToolTip::showText(QCursor::pos(), item.data().toString());
            });
    }

public:
    void setExpand(BKComboBox::Impl* parent)
    {
        mpView->clear();
        mpView->addItems(parent->mItems);

        mpBindItem = parent;
        if (auto card = mpBindItem->mpHandle->getBindCard())
        {
            auto cardItem = card->getBindItem();
            mBindOriginZ = cardItem->zValue();
            cardItem->setZValue(2.0f);
        }

        const QRectF& pr = mpBindItem->boundingRect();
        this->setPos(mpBindItem->mapToScene(pr.bottomLeft()));

        QSizeF size( pr.width(), 110 );
        mpView->setFixedSize(size.toSize());
        this->setMinimumSize(size);
        this->setMaximumSize(size);

        mpScene = mpBindItem->scene();

        mpScene->addItem(this);
        this->setFocus();
    }

protected:
    void focusOutEvent(QFocusEvent* event) override
    {
        super::focusOutEvent(event);
        resetBind();
    }

    bool eventFilter(QObject* object, QEvent* event) override
    {
        if (object == mpView && event->type() == QEvent::Wheel)
        {
            event->accept();
            return true;
        }

        return super::eventFilter(object, event);
    }

private:
    void resetBind()
    {
        if (mpBindItem)
        {
            if (auto card = mpBindItem->mpHandle->getBindCard())
            {
                auto cardItem = card->getBindItem();
                cardItem->setZValue(mBindOriginZ);
            }

            mpScene->setFocusItem(mpBindItem);
        }

        mpScene->removeItem(this);
        mpBindItem = nullptr;
    }

private:
    QListWidget* mpView = nullptr;
    QGraphicsScene* mpScene = nullptr;
    BKComboBox::Impl* mpBindItem = nullptr;
    qreal mBindOriginZ = 0;
};


void BKComboBox::Impl::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        if (mCtrlArea.contains(event->pos().toPoint()))
        {
            // 展开菜单
            if (!mpPublicView)
                mpPublicView = new BKComboBoxItemView();

            mpPublicView->setExpand(this);
        }

        mbPressed = false;
        update();
    }

    event->accept();
}

BKUnit* BKComboBox::copy()
{
    L_IMPL(BKComboBox);
    BKComboBox* target = BKCreator::create<BKComboBox>();

    BKComboBox::Impl* dstImpl = target->mpImpl;
    dstImpl->mItems = l->mItems;
    dstImpl->mCbType = l->mCbType;
    dstImpl->mCurrentIndex = l->mCurrentIndex;
    _copyBasicAttributeTo(target);
    return target;
}

BKComboBox::operator QJsonValue() const
{
    L_IMPL(BKComboBox);
    return (l->mCurrentIndex < 0 || l->mItems.size() == 0) ? "" : l->mItems.at(l->mCurrentIndex);
}

bool BKComboBox::loadFromJson(const QJsonValue& val)
{
    L_IMPL(BKComboBox);

    dataChanged(val.toString());
    return true;
}

QVariant BKComboBox::data()
{
    L_IMPL(BKComboBox);
    return QJsonValue(*this).toString();
}

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

BKComboBox* BKComboBox::setCurrentIndex(int index, bool notify /*= true*/)
{
    L_IMPL(BKComboBox)
    if (index > -1 && index < l->mItems.size())
        l->mCurrentIndex = index;
    else
        l->mCurrentIndex = -1;

    mpImpl->update();

    if (notify)
        dataChanged(l->getCurrentText());

    return this;
}


BKComboBox* BKComboBox::setCurrentItem(const QString& item, bool notify /*= true*/)
{
    L_IMPL(BKComboBox);
    l->mCurrentIndex = l->mItems.indexOf(item);
    mpImpl->update();

    return this;
}

BKComboBox* BKComboBox::setItems(const QStringList& items)
{
    mpImpl->mItems = items;
    mpImpl->mCurrentIndex = items.size() > 0 ? 0 : -1;

    mpImpl->update();
    return this;
}

BKComboBox* BKComboBox::setCallbackParamType(CallbackParamType type)
{
    L_IMPL(BKComboBox);
    l->mCbType = type;

    return this;
}

QGraphicsItem* BKComboBox::getGraphicsItem()
{
    return mpImpl;
}

void BKComboBox::resized()
{
    L_IMPL(BKComboBox)

    l->mBoundingRect = { 0, 0, mSize.width(), mSize.height() };
    l->mCtrlArea = { 0, l->mFixedMargin, static_cast<int>(l->mBoundingRect.width()), static_cast<int>(l->mBoundingRect.height() - 2 * l->mFixedMargin) };

    int split = l->mCtrlArea.width() - l->mCtrlArea.height();
    l->mButtonArea = { split, l->mFixedMargin, l->mCtrlArea.height(), l->mCtrlArea.height() };
    l->mTextArea = { 4, l->mFixedMargin, split - 4, l->mCtrlArea.height() };        // 4 = 左侧内边距

    
    QPoint center(l->mButtonArea.topLeft() + QPoint(l->mCtrlArea.height() / 2, l->mCtrlArea.height() / 2));
    float offset = 1.0f * l->mButtonArea.width() / 6;
    float delta = l->mButtonArea.width() / 2 - offset;
    l->mDropAnchorPainterPath = QPainterPath(QPointF(center.x() - delta, center.y() - offset));
    l->mDropAnchorPainterPath.lineTo(QPointF(center.x(), center.y() + offset));
    l->mDropAnchorPainterPath.lineTo(QPointF(center.x() + delta, center.y() - offset));
}

void BKComboBox::dataChanged(const QVariant& data)
{
    L_IMPL(BKComboBox);

    if (data.isNull())
    {
        if (mpRightAnchor)
            mpRightAnchor->dataChanged(l->getCurrentText());
    }
    else
    {
        int index = l->mItems.indexOf(data.toString());
        setCurrentIndex(index, false);
        l->update();

        // 阻断传递
        // 吉人自有天相吧
        // if (index < 0)      
        //     return;
        QVariant param = l->mCbType == CallbackParamType::Data ? data : index;
        if (!mCallbackFunc(this, param) && mpRightAnchor)
            mpRightAnchor->dataChanged(data);
    }
}
