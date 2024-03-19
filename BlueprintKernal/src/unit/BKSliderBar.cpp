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

class BKLineEditor;
class BKSliderBar::Impl : public QGraphicsItem
{
public:
    Impl(BKSliderBar* handle, DateType type)
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
                if (mDataType == DateType::Int)
                {
                    if (mIntRange[1] == mIntRange[0])
                        break;
                    else if (mIntRange[1] < mIntRange[0])
                        std::swap(mIntRange[0], mIntRange[1]);


                    percent = 1.0f * (miCurrentValue - mIntRange[0]) / (mIntRange[1] - mIntRange[0]);    
                }
                else
                {
                    if (mDoubleRange[1] == mDoubleRange[0])
                        break;
                    else if (mDoubleRange[1] < mDoubleRange[0])
                        std::swap(mDoubleRange[0], mDoubleRange[1]);

                    percent = 1.0f * (mdCurrentValue - mDoubleRange[0]) / (mDoubleRange[1] - mDoubleRange[0]);
                }

                percent = percent > 1 ? 1.0f : (percent < 0 ? 0 : percent);
                percent = 1.0f - percent;
                painter->drawRoundedRect(mCtrlArea + QMargins(0, 0,  - percent * mCtrlArea.width(), 0), 2.0f, 2.0f);

            } while (false);

            painter->restore();
        }

        // 绘制文字
        painter->save();
        {
            painter->setPen(QColor(142, 47, 0));
            painter->setBrush(Qt::NoBrush);
            painter->drawText(mCtrlArea, mDataType == DateType::Int ? QString::number(miCurrentValue) : QString::number(mdCurrentValue), mOption);
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
    DateType mDataType;
    // Int型范围及当前值
    int miCurrentValue = 0;
    int mIntRange[2] = { 0, 100 };
    // Double型范围
    double mdCurrentValue = 0;
    double mDoubleRange[2] = { 0, 100.0f };
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

        mpLineEdit->setValidator(mpBindItem->mDataType == BKSliderBar::DateType::Int
            ? &mIntValidator
            : &mDoubleValidator);
        mpLineEdit->setText(mpBindItem->mDataType == BKSliderBar::DateType::Int 
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
            bool valChanged = false;
            if (mpBindItem->mDataType == BKSliderBar::DateType::Int 
                && mpLineEdit->text().toInt() != mpBindItem->miCurrentValue)
            {
                mpBindItem->miCurrentValue = mpLineEdit->text().toInt();
                valChanged = true;
            }
            else if (mpBindItem->mDataType == BKSliderBar::DateType::Double
                && mpLineEdit->text().toDouble() != mpBindItem->mdCurrentValue)
            {
                mpBindItem->mdCurrentValue = mpLineEdit->text().toDouble();
                valChanged = true;
            }

            if (valChanged)
            {
                //todo...
            }

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


BKSliderBar::BKSliderBar(DateType type/* = DateType::Int*/)
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

    if (l->mDataType == DateType::Int)
        l->miCurrentValue = value.toInt();
    else
        l->mdCurrentValue = value.toDouble();
    return this;
}

BKSliderBar* BKSliderBar::setMaximum(const QVariant& max)
{
    L_IMPL(BKSliderBar)

    if (l->mDataType == DateType::Int)
        l->mIntRange[1] = max.toInt();
    else
        l->mDoubleRange[1] = max.toDouble();
    return this;
}

BKSliderBar* BKSliderBar::setMinimum(const QVariant& min)
{
    L_IMPL(BKSliderBar)

    if (l->mDataType == DateType::Int)
        l->mIntRange[0] = min.toInt();
    else
        l->mDoubleRange[0] = min.toDouble();
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
