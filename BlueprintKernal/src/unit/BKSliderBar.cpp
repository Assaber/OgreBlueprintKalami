#include "unit/BKSliderBar.h"
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
#include <QValidator>

class BKSliderBar::Impl : public QGraphicsItem
{
public:
    Impl(BKSliderBar* handle, DataType type)
        : QGraphicsItem()
        , mpHandle(handle)
        , mDataType(type)
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
        correctRange();

        // 绘制外边框
        QPen p(QColor(255, 128, 64));
        painter->save();
        {
            painter->setPen(p);
            painter->setBrush(Qt::white);
            painter->drawRoundedRect(mCtrlArea, 2.0f, 2.0f);
            painter->restore();
        }

        // 绘制百分比
        painter->save();
        {
            painter->setPen(p);
            painter->setBrush(QColor(255, 128, 64));

            do 
            {
                float percent = 0;
                if (mDataType == DataType::Int)
                    percent = (mIntRange[1] == mIntRange[0]) ? 0 : 1.0f * (mIntRange[1] - miCurrentValue) / (mIntRange[1] - mIntRange[0]);
                else
                    percent = (mDoubleRange[1] == mDoubleRange[0]) ? 0 : 1.0f * (mDoubleRange[1] - mdCurrentValue) / (mDoubleRange[1] - mDoubleRange[0]);

                percent = percent > 1 ? 1.0f : (percent < 0 ? 0 : percent);
                painter->drawRoundedRect(mCtrlArea - QMargins(0, 0, percent * mCtrlArea.width(), 0), 2.0f, 2.0f);

            } while (false);

            painter->restore();
        }

        // 绘制文字
        painter->save();
        {
            painter->setPen(QColor(142, 47, 0));
            painter->setBrush(Qt::NoBrush);
            painter->drawText(mCtrlArea, mDataType == DataType::Int ? QString::number(miCurrentValue) : QString::number(mdCurrentValue), mOption);
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

    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override
    {
        float x = (event->pos().x() - mCtrlArea.x()) / mCtrlArea.width();
        x = x > 1.0f ? 1.0f : (x < 0 ? 0 : x);
        if (mDataType == DataType::Int)
        {
            miCurrentValue = (mIntRange[1] == mIntRange[0]) ? mIntRange[0] : x * (mIntRange[1] - mIntRange[0]) + mIntRange[0];
            mpHandle->dataChanged(miCurrentValue);
        }   
        else if (mDataType == DataType::Double)
        {
            mdCurrentValue = (mDoubleRange[1] == mDoubleRange[0]) ? mDoubleRange[0] : x * (mDoubleRange[1] - mDoubleRange[0]) + mDoubleRange[0];
            mpHandle->dataChanged(mdCurrentValue);
        }

        update();
    }

    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;

private:
    void correctRange()
    {
        if (mbHasCorrected)
            return;

        if (mDataType == DataType::Int)
        {
            if (mIntRange[0] > mIntRange[1])
                std::swap(mIntRange[0], mIntRange[1]);
        }
        else if (mDataType == DataType::Double)
        {
            if (mDoubleRange[0] > mDoubleRange[1])
                std::swap(mDoubleRange[0], mDoubleRange[1]);
        }
        mbHasCorrected = true;
    }

public:
    BKSliderBar* mpHandle = nullptr;
    // 包围盒
    QRectF mBoundingRect;
    // 文字选项
    QTextOption mOption;
    // 控件区域
    QRect mCtrlArea;
    // 固定上下边距
    static constexpr int mFixedMargin = 2;
    // 编辑器
    static BKSliderBarEditor* mpPublicEditor;
    // 数据类型
    DataType mDataType;
    // Int型范围及当前值
    int miCurrentValue = 0;
    int mIntRange[2] = { 0, 100 };
    // Double型范围
    double mdCurrentValue = 0;
    double mDoubleRange[2] = { 0, 100.0f };
    // 纠正使能
    bool mbHasCorrected = false;
};

BKSliderBarEditor* BKSliderBar::Impl::mpPublicEditor = nullptr;
class BKSliderBarEditor : public QGraphicsProxyWidget
{
public:
    using super = QGraphicsProxyWidget;
    BKSliderBarEditor()
        : super()
    {
        mIntValidator.setRegExp(QRegExp("^-?\\d+$"));
        mDoubleValidator.setRegExp(QRegExp("^-?\\d+(?:\\.\\d+)?(?:[Ee][+-]?\\d+)?$"));

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

        QObject::connect(mpLineEdit, &QLineEdit::returnPressed, this, &BKSliderBarEditor::uploadAndDisband);

        setZValue(2.0f);
    }

public:
    void setExpand(BKSliderBar::Impl* parent)
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

        mpLineEdit->setValidator(mpBindItem->mDataType == BKSliderBar::DataType::Int
            ? &mIntValidator
            : &mDoubleValidator);
        mpLineEdit->setText(mpBindItem->mDataType == BKSliderBar::DataType::Int 
            ? QString::number(mpBindItem->miCurrentValue) 
            : QString::number(mpBindItem->mdCurrentValue));
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
            if (mpBindItem->mDataType == BKSliderBar::DataType::Int  && mpLineEdit->text().toInt() != mpBindItem->miCurrentValue)
                mpBindItem->mpHandle->dataChanged(mpLineEdit->text().toInt());
            else if (mpBindItem->mDataType == BKSliderBar::DataType::Double && mpLineEdit->text().toDouble() != mpBindItem->mdCurrentValue)
                mpBindItem->mpHandle->dataChanged(mpLineEdit->text().toDouble());

            mpBindItem->update();

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
    BKSliderBar::Impl* mpBindItem = nullptr;
    qreal mBindOriginZ = 0;

    QRegExpValidator mIntValidator;
    QRegExpValidator mDoubleValidator;
};

void BKSliderBar::Impl::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        TopmostCardEvent e(mpHandle->mpBindCard->getBindItem());
        qApp->sendEvent(scene(), &e);

        if (!mpPublicEditor)
            mpPublicEditor = new BKSliderBarEditor();

        mpPublicEditor->setExpand(this);
    }
}


BKUnit* BKSliderBar::copy()
{
    L_IMPL(BKSliderBar);
    BKSliderBar* target = BKCreator::create<BKSliderBar>();

    BKSliderBar::Impl* dstImpl = target->mpImpl;
    dstImpl->mDataType = l->mDataType;
    dstImpl->miCurrentValue = l->miCurrentValue;
    memcpy(dstImpl->mIntRange, l->mIntRange, sizeof(l->mIntRange));
    dstImpl->mdCurrentValue = l->mdCurrentValue;
    memcpy(dstImpl->mDoubleRange, l->mDoubleRange, sizeof(l->mDoubleRange));
    _copyBasicAttributeTo(target);
    return target;
}

bool BKSliderBar::loadFromJson(const QJsonValue& val)
{
    L_IMPL(BKSliderBar);

    if (l->mDataType == DataType::Int)
        setCurrentValue(val.toInt());
    else
        setCurrentValue(val.toDouble());

    return true;
}

QVariant BKSliderBar::data()
{
    L_IMPL(BKSliderBar);
    if (l->mDataType == DataType::Int)
        return l->miCurrentValue;
    else
        return l->mdCurrentValue;

    return 0;
}

BKSliderBar::operator QJsonValue() const
{
    L_IMPL(BKSliderBar);

    if (l->mDataType == DataType::Int)
        return l->miCurrentValue;
    else
        return l->mdCurrentValue;

    return 0;
}


BKSliderBar::BKSliderBar(DataType type/* = DateType::Int*/)
    : super()
    , mpImpl(new Impl(this, type))
{
}

BKSliderBar::~BKSliderBar()
{
    delete mpImpl;
    mpImpl = nullptr;
}

BKSliderBar* BKSliderBar::setCurrentValue(const QVariant& value)
{
    L_IMPL(BKSliderBar)

    if (l->mDataType == DataType::Int)
        l->miCurrentValue = value.toInt();
    else
        l->mdCurrentValue = value.toDouble();
    return this;
}

BKSliderBar* BKSliderBar::setMaximum(const QVariant& max)
{
    L_IMPL(BKSliderBar)

    if (l->mDataType == DataType::Int)
        l->mIntRange[1] = max.toInt();
    else
        l->mDoubleRange[1] = max.toDouble();

    l->mbHasCorrected = false;
    return this;
}

BKSliderBar* BKSliderBar::setMinimum(const QVariant& min)
{
    L_IMPL(BKSliderBar)

    if (l->mDataType == DataType::Int)
        l->mIntRange[0] = min.toInt();
    else
        l->mDoubleRange[0] = min.toDouble();
    l->mbHasCorrected = false;
    return this;
}

QGraphicsItem* BKSliderBar::getGraphicsItem()
{
    return mpImpl;
}

void BKSliderBar::resized()
{
    L_IMPL(BKSliderBar)

    l->mBoundingRect = { 0, 0, mSize.width(), mSize.height() };
    l->mCtrlArea = { 0, l->mFixedMargin, static_cast<int>(l->mBoundingRect.width()), static_cast<int>(l->mBoundingRect.height() - 2 * l->mFixedMargin) };
}

void BKSliderBar::dataChanged(const QVariant& data)
{
    L_IMPL(BKSliderBar)

    if (data.isNull())
    {
        if (mpRightAnchor)
            mpRightAnchor->dataChanged(l->mDataType == DataType::Int ? l->miCurrentValue : l->mdCurrentValue);
    }
    else
    {
        setCurrentValue(data);
        l->update();
        if (!mCallbackFunc(data) && mpRightAnchor)
            mpRightAnchor->dataChanged(data);
    }
}
