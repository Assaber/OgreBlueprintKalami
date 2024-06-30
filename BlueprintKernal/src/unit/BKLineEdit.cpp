#include "unit/BKLineEdit.h"
#include "container/BKCard.h"
#include "BKEvent.h"

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsProxyWidget>
#include <QCoreApplication>
#include <QGraphicsScene>
#include <QFontMetrics>
#include <QLineEdit>
#include <QPainter>
#include <QStyle>

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

public:
    virtual QRectF boundingRect() const override 
    {
        return mBoundingRect;
    }

    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override
    {
        QPen p(QColor(255, 128, 64));
        painter->save();
        {
            painter->setPen(p);
            painter->setBrush(Qt::NoBrush);
            painter->drawRoundedRect(mCtrlArea, 2.0f, 2.0f);
            painter->restore();
        }

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
        if (!mpHandle->mbEnable)
        {
            event->ignore();
            return;
        }

        event->accept();    
    }

    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;

public:
    BKLineEdit* mpHandle = nullptr;
    QString mstrText;
    QRectF mBoundingRect;
    QRect mCtrlArea;
    QTextOption mOption;
    static constexpr int mFixedMargin = 2;
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

        if (!mpPublicEditor) {
            mpPublicEditor = new BKLineEditor();
        }

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

bool BKLineEdit::loadFromJson(const QJsonValue& val)
{
    dataChanged(val.toString());
    return true;
}

QVariant BKLineEdit::data()
{
    L_IMPL(BKLineEdit);
    return l->mstrText;
}

BKLineEdit::operator QJsonValue() const
{
    L_IMPL(BKLineEdit);
    return l->mstrText;
}

BKUnit* BKLineEdit::copy()
{
    L_IMPL(BKLineEdit);
    BKLineEdit* target = BKCreator::create<BKLineEdit>();

    BKLineEdit::Impl* dstImpl = target->mpImpl;
    dstImpl->mstrText = l->mstrText;
    _copyBasicAttributeTo(target);
    return target;
}

BKLineEdit* BKLineEdit::setText(const QString& text)
{
    L_IMPL(BKLineEdit)

    l->mstrText = text;
    l->update();
    return this;
}

QString BKLineEdit::getCurrentText()
{
    L_IMPL(BKLineEdit);
    return l->mstrText;
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
        if (mpRightAnchor) {
            mpRightAnchor->dataChanged(l->mstrText);
        }
    }
    else
    {
        setText(data.toString());

        if (!mCallbackFunc(this, data) && mpRightAnchor) {
            mpRightAnchor->dataChanged(data);
        }
    }
}
