#include "unit/BKLineEdit.h"
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QFontMetrics>
#include <QGraphicsProxyWidget>
#include <QLineEdit>
#include <QStyle>
#include <QGraphicsScene>
#include "container/BKCard.h"
#include "BKEvent.h"
#include <QCoreApplication>

class BKLineEditor;
class BKLineEdit::Impl : public QGraphicsItem
{
public:
    Impl(BKLineEdit* handle)
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
        // 绘制外边框
        QPen p(QColor(255, 128, 64));
        painter->save();
        {
            painter->setPen(p);
            painter->setBrush(Qt::NoBrush);
            painter->drawRoundedRect(mCtrlArea, 2.0f, 2.0f);
            painter->restore();
        }

        // 绘制文字
        painter->save();
        {
            painter->setPen(p);
            painter->setBrush(Qt::NoBrush);
            painter->drawText(mCtrlArea, mstrText, mOption);
            painter->restore();
        }
    }

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event) override
    {
        event->accept();
    }

    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;

public:
    BKLineEdit* mpHandle = nullptr;
    // 显示的文字
    QString mstrText;
    // 包围盒
    QRectF mBoundingRect;
    // 文字选项
    QTextOption mOption;
    // 控件区域
    QRect mCtrlArea;
    // 固定上下边距
    static constexpr int mFixedMargin = 2;
    // 编辑器
    static BKLineEditor* mpPublicEditor;
};

BKLineEditor* BKLineEdit::Impl::mpPublicEditor = nullptr;
class BKLineEditor : public QGraphicsProxyWidget
{
public:
    using super = QGraphicsProxyWidget;
    BKLineEditor()
        : super()
    {
        mpLineEdit = new QLineEdit();
        mpLineEdit->setStyleSheet("QLineEdit {" \
            "	border: 1px solid #FF8000;" \
            "	border-radius: 2;" \
            "	padding: 0 0 0 0;" \
            "	background: white;" \
            "	selection-background-color: #FF8000;" \
            "	color:#FF8000;" \
            "}");
        mpLineEdit->style()->polish(mpLineEdit);
        setWidget(mpLineEdit);

        QObject::connect(mpLineEdit, &QLineEdit::returnPressed, this, &BKLineEditor::uploadAndDisband);

        setZValue(2.0f);
    }

public:
    void setExpand(BKLineEdit::Impl* parent)
    {
        mpBindItem = parent;
        if (auto card = mpBindItem->mpHandle->getBindCard())
        {
            auto cardItem = card->getBindItem();
            mBindOriginZ = cardItem->zValue();
            cardItem->setZValue(2.0f);
        }

        const QRect& pr = mpBindItem->mCtrlArea;
        this->setPos(mpBindItem->mapToScene(pr.topLeft()));

        QSizeF size = pr.size();
        mpLineEdit->setFixedSize(size.toSize());
        this->setMinimumSize(size);
        this->setMaximumSize(size);

        mpLineEdit->setText(parent->mstrText);
        parent->setVisible(false);

        mpScene = mpBindItem->scene();
        mpScene->addItem(this);
        mpLineEdit->setFocus();
    }

protected:
    void focusOutEvent(QFocusEvent* event) override
    {
        super::focusOutEvent(event);
        uploadAndDisband();
    }

private:
    void uploadAndDisband()
    {
        if (mpBindItem)
        {
            if (mpBindItem->mstrText != mpLineEdit->text())
            {
                mpBindItem->mpHandle->dataChanged(mpLineEdit->text());
                mpBindItem->update();
            }

            if (auto card = mpBindItem->mpHandle->getBindCard())
            {
                auto cardItem = card->getBindItem();
                cardItem->setZValue(mBindOriginZ);
            }

            mpScene->setFocusItem(mpBindItem);
            mpBindItem->setVisible(true);
        }
        
        mpScene->removeItem(this);
        mpBindItem = nullptr;
    }

private:
    QLineEdit* mpLineEdit = nullptr;
    QGraphicsScene* mpScene = nullptr;
    BKLineEdit::Impl* mpBindItem = nullptr;
    qreal mBindOriginZ = 0;
};

void BKLineEdit::Impl::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        TopmostCardEvent e(mpHandle->mpBindCard->getBindItem());
        qApp->sendEvent(scene(), &e);

        if (!mpPublicEditor)
            mpPublicEditor = new BKLineEditor();

        mpPublicEditor->setExpand(this);
    }
}


BKLineEdit::BKLineEdit()
    : super()
    , mpImpl(new Impl(this))
{
    mMinSize.setWidth(130);
}

BKLineEdit::~BKLineEdit()
{
    delete mpImpl;
    mpImpl = nullptr;
}

QJsonValue BKLineEdit::getValue()
{
    L_IMPL(BKLineEdit);

    return l->mstrText;
}

bool BKLineEdit::setValue(const QJsonValue& val)
{
    setText(val.toString());
    return true;
}

BKLineEdit* BKLineEdit::setText(const QString& text)
{
    L_IMPL(BKLineEdit)

    l->mstrText = text;
    return this;
}

QGraphicsItem* BKLineEdit::getGraphicsItem()
{
    return mpImpl;
}

void BKLineEdit::resized()
{
    L_IMPL(BKLineEdit)

    l->mBoundingRect = { 0, 0, mSize.width(), mSize.height() };
    l->mCtrlArea = { 0, l->mFixedMargin, static_cast<int>(l->mBoundingRect.width()), static_cast<int>(l->mBoundingRect.height() - 2 * l->mFixedMargin) };
}

void BKLineEdit::dataChanged(const QVariant& data)
{
    L_IMPL(BKLineEdit)

    if (data.isNull())
    {
        if (mpRightAnchor)
            mpRightAnchor->dataChanged(l->mstrText);
    }
    else
    {
        setText(data.toString());
        l->update();

        if (mpRightAnchor && !mCallbackFunc(data))
            mpRightAnchor->dataChanged(data);
    }
}
