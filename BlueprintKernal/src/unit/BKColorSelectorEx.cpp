#include "unit/BKColorSelectorEx.h"
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
#include <QLineEdit>
#include <QRegExpValidator>

class BKColorEditor;
class BKColorSelectorEx::Impl : public QGraphicsItem
{
public:
    Impl(BKColorSelectorEx* handle, BKColorSelectorEx::Type type)
        : QGraphicsItem()
        , mpHandle(handle)
        , mType(type)
    {
        mOption.setWrapMode(QTextOption::NoWrap);
        mOption.setAlignment(Qt::AlignCenter);

        syncColor(true, true);
    }

public:
    virtual QRectF boundingRect() const override { return mBoundingRect; }

    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override
    {
        painter->save();
        {
            painter->setPen("#D2D2D2");
            painter->setBrush(mColor);
            painter->drawRoundedRect(mColorDrawableArea, 2.0f, 2.0f);
            painter->restore();
        }

        // 绘制外边框
        painter->save();
        {
            painter->setPen("#FF8000");
            painter->setBrush(Qt::NoBrush);
            painter->drawRoundedRect(mEditorArea, 2.0f, 2.0f);
            painter->restore();
        }

        // 绘制文字
        painter->save();
        {
            painter->setPen("#FF8000");
            painter->setBrush(Qt::NoBrush);
            painter->drawText(mEditorArea, mstrColor, mOption);
            painter->restore();
        }
    }

    QString getCurrentColor() {
        if (mType == Type::Vector3)
            return QString("%1 %2 %3").arg(QString::number(mColorData[0], 'f', 3))
            .arg(QString::number(mColorData[1], 'f', 3))
            .arg(QString::number(mColorData[2], 'f', 3));
        else if(mType == Type::Vector4)
            return QString("%1 %2 %3 %4").arg(QString::number(mColorData[0], 'f', 3))
            .arg(QString::number(mColorData[1], 'f', 3))
            .arg(QString::number(mColorData[2], 'f', 3))
            .arg(QString::number(mColorData[3], 'f', 3));

        return "";
    }


    /**
     * 1. #FF8000    rgb
     * 2. #FFFF8000  argb
     * 3. 1.0 0.5 0  rgb
     * 4. 1 0.5 0 1  rgba
     */
    QColor transString2Color(const QString& color, bool* ok = nullptr)
    {
        QColor ret;

        do 
        {
            if (color.size() == 0)
                break;

            if (color[0] == '#' && (color.size() == 7 || color.size() == 9))
            {
                static QRegExp re("^#[0-9a-f]+$");
                re.setCaseSensitivity(Qt::CaseInsensitive);
                if (!re.exactMatch(color))
                    break;

                ret = QColor(color);
            }
            else
            {
                QStringList split;
                if (color.indexOf(',') < 0)     // 空格分隔
                    split = color.split(' ', QString::SkipEmptyParts);
                else
                    split = color.split(',', QString::SkipEmptyParts);

                if (split.count() < 3)
                    break;

                ret.setRed(255 * split[0].toFloat());
                ret.setGreen(255 * split[1].toFloat());
                ret.setBlue(255 * split[2].toFloat());

                if (split.count() > 3)
                    ret.setAlpha(255 * split[3].toFloat());
            }

            if (ok)
                *ok = true;

            return ret;

        } while (false);

        if (ok)
            *ok = false;

        return ret;
    }

    bool loadFromColorString(const QString& color) 
    {
        bool ok;

        QColor parse = transString2Color(color, &ok);
        if (ok)
        {
            mColor = parse;
            syncColor(false, false);
            mstrColor = color;
        } 
        else
            qWarning() << "颜色解析错误";

        return ok;
    }

    void syncColor(bool vec2color, bool updateText) 
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
        }

        if (updateText)
            mstrColor = getCurrentColor();
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
            if (mColorDrawableArea.contains(event->pos()))
            {
                QColorDialog dlg(mColor);
                if (mType == Type::Vector4)
                    dlg.setOption(QColorDialog::ShowAlphaChannel);

                if (dlg.exec() != QDialog::Accepted)
                    return;

                mColor = dlg.currentColor();
                syncColor(false, true);
                mpHandle->dataChanged(mColor);
                update();

                this->setFocus();
                event->ignore();
            }
            else
                event->accept();
        }
    }

    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;

public:
    BKColorSelectorEx* mpHandle = nullptr;
    // 包围盒
    QRectF mBoundingRect;
    // 编辑框区域
    QRectF mEditorArea;
    // 控件区域
    QRectF mColorDrawableArea;
    // 固定上下边距
    static constexpr int mFixedMargin = 2;
    // 颜色选择区域固定宽度
    static constexpr int mFixedColorAreaWidth = 35;
    // 两个控件间的间隔
    static constexpr int mSpacing = 2;
    // 当前颜色
    Color4f mColorData = {1.0f, 1.0f, 1.0f, 1.0f};
    QColor mColor;
    QString mstrColor;
    // 文字选项
    QTextOption mOption;
    // 编辑器
    static BKColorEditor* mpPublicEditor;
    // 显示向量
    BKColorSelectorEx::Type mType;
};

BKColorEditor* BKColorSelectorEx::Impl::mpPublicEditor = nullptr;
class BKColorEditor : public QGraphicsProxyWidget
{
public:
    using super = QGraphicsProxyWidget;
    BKColorEditor()
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

        QObject::connect(mpLineEdit, &QLineEdit::returnPressed, this, &BKColorEditor::uploadAndDisband);

        setZValue(2.0f);
    }

public:
    void setExpand(BKColorSelectorEx::Impl* parent)
    {
        mpBindItem = parent;
        if (auto card = mpBindItem->mpHandle->getBindCard())
        {
            auto cardItem = card->getBindItem();
            mBindOriginZ = cardItem->zValue();
            cardItem->setZValue(2.0f);
        }

        const QRectF& pr = mpBindItem->mEditorArea;
        this->setPos(mpBindItem->mapToScene(pr.topLeft()));

        QSizeF size = pr.size();
        mpLineEdit->setFixedSize(size.toSize());
        this->setMinimumSize(size);
        this->setMaximumSize(size);

        mpLineEdit->setText(parent->mstrColor);
        // parent->setVisible(false);

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
            if (mpBindItem->mstrColor != mpLineEdit->text() && mpBindItem->loadFromColorString(mpLineEdit->text()))
            {
                mpBindItem->mpHandle->dataChanged(mpBindItem->mColor);
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
    BKColorSelectorEx::Impl* mpBindItem = nullptr;
    qreal mBindOriginZ = 0;
};


void BKColorSelectorEx::Impl::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
    if (!mEditorArea.contains(event->pos()))
        event->ignore();

    if (event->button() == Qt::LeftButton)
    {
        TopmostCardEvent e(mpHandle->mpBindCard->getBindItem());
        qApp->sendEvent(scene(), &e);

        if (!mpPublicEditor)
            mpPublicEditor = new BKColorEditor();

        mpPublicEditor->setExpand(this);
    }
}

BKColorSelectorEx::BKColorSelectorEx(Type type /*= Type::Vector3*/)
    : super()
    , mpImpl(new Impl(this, type))
{
    // setMinSize({ 120 + Impl::mSpacing + Impl::mFixedColorAreaWidth, 20 });
    setMinWidth((type == Type::Vector3 ? 120 : 140) + Impl::mSpacing + Impl::mFixedColorAreaWidth);
}

BKColorSelectorEx::~BKColorSelectorEx()
{
    delete mpImpl;
    mpImpl = nullptr;
}

QJsonValue BKColorSelectorEx::getValue()
{
    L_IMPL(BKColorSelectorEx);
    return  l->getCurrentColor();
}

bool BKColorSelectorEx::setValue(const QJsonValue& val)
{
    L_IMPL(BKColorSelectorEx);
    l->loadFromColorString(val.toString());

    return true;
}

BKColorSelectorEx* BKColorSelectorEx::setColor(const Color4f& color)
{
    L_IMPL(BKColorSelectorEx);
    memcpy(l->mColorData.data(), color.data(), 4 * sizeof(float));
    l->syncColor(true, true);
    l->update();

    return this;
}

BKColorSelectorEx* BKColorSelectorEx::setColor(const Color3f& color)
{
    L_IMPL(BKColorSelectorEx);
    memcpy(l->mColorData.data(), color.data(), 3 * sizeof(float));
    l->syncColor(true, true);
    l->update();

    return this;
}

BKColorSelectorEx* BKColorSelectorEx::setColor(const QColor& color)
{
    L_IMPL(BKColorSelectorEx);
    l->mColor = color;
    l->syncColor(false, true);
    l->update();

    return this;
}

QGraphicsItem* BKColorSelectorEx::getGraphicsItem()
{
    return mpImpl;
}

void BKColorSelectorEx::resized()
{
    L_IMPL(BKColorSelectorEx);

    float height = mSize.height() - 2 * l->mFixedMargin;
    l->mBoundingRect = { 0, 0, mSize.width(), mSize.height() };
    l->mColorDrawableArea = { mSize.width() - l->mFixedMargin - l->mFixedColorAreaWidth, l->mFixedMargin, l->mFixedColorAreaWidth, height };
    l->mEditorArea = { 0, l->mFixedMargin , l->mBoundingRect.width() - l->mColorDrawableArea.width() - Impl::mSpacing, height };
}

void BKColorSelectorEx::dataChanged(const QVariant& data)
{
    L_IMPL(BKColorSelectorEx);

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
