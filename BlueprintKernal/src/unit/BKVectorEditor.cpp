#include "unit/BKVectorEditor.h"
#include "container/BKCard.h"
#include "BKEvent.h"

#include <QGraphicsSceneMouseEvent>
#include <QCoreApplication>
#include <QGraphicsScene>
#include <QPainter>
#include <QStyle>
#include <stdexcept>
#include <limits>
#include <QFontMetrics>
#include <QApplication>
#include <QGraphicsProxyWidget>
#include <QLineEdit>


class BKVectorCtrl;
class BKVectorEditor::Impl : public QGraphicsItem
{
public:
    Impl(BKVectorEditor* handle, BKVectorEditor::Type type, uint32_t count)
        : mpHandle(handle)
        , mType(type)
    {
        // 开辟内存
        if (count != 0)
        {
            if (mType == Type::Float)
            {
                mfVector.resize(count);
                for (auto& item : mfVector)
                {
                    std::get<1>(item) = std::numeric_limits<float>::min();
                    std::get<2>(item) = std::numeric_limits<float>::max();
                }
            }
            else if (mType == Type::Int)
            {
                miVector.resize(count);
                for (auto& item : miVector)
                {
                    std::get<1>(item) = std::numeric_limits<int>::min();
                    std::get<2>(item) = std::numeric_limits<int>::max();
                }
            }
        }

        mVecName.resize(count);
    }

    ~Impl()
    {
    }

public:
    QVariant getValue()
    {
        QVariant ret;

        if (mType == Type::Float)
        {
            FloatVec vec;
            for (const auto& item : mfVector)
                vec.push_back(std::get<0>(item));

            ret = QVariant::fromValue(vec);
        }
        else if (mType == Type::Int)
        {
            IntegerVec vec;
            for (const auto& item : miVector)
                vec.push_back(std::get<0>(item));

            ret = QVariant::fromValue(vec);
        }

        return ret;
    }

    void recalcArea(const QSizeF size = {-1, -1})
    {
        auto get_max_label_width = [this]() -> int {
            int maxWidth = 0;
            QFontMetrics fm = QApplication::fontMetrics();
            for (const QString& title : mVecName)
                maxWidth = std::max(fm.size(Qt::TextSingleLine, title).width(), maxWidth);

            return maxWidth;
        };


        if (size.isValid()) // 自行调整，更新编辑框大小
        {
            if (mVecName.size() == 0)
                return;

            int tableWidth = get_max_label_width();
            int holdWidth = tableWidth + mCtrlSpacing;
            int lines = mVecName.size() / mItemsInLine + (mVecName.size() % mItemsInLine == 0 ? 0 : 1);
            int ctrlHeight = (size.height() - (lines + 1) * mFixedMargin) / lines;
            int ctrlWidth = 0;
            if (mVecName.size() < mItemsInLine)
                ctrlWidth = (size.width() - (mVecName.size() - 1) * mItemsSpacing - mVecName.size() * holdWidth) / mVecName.size();
            else
                ctrlWidth = (size.width() - (mItemsInLine - 1) * mItemsSpacing - mItemsInLine * holdWidth) / mItemsInLine;

            mTitleArea.clear();
            mEditorArea.clear();

            int x = 0, y = mFixedMargin;
            
            for (int i = 0; i < mVecName.size();)
            {
                mTitleArea.push_back(QRectF(x, y, tableWidth, ctrlHeight));
                x += holdWidth;
                mEditorArea.push_back(QRectF(x, y, ctrlWidth, ctrlHeight));
                x += ctrlWidth;

                ++i;
                if (i % mItemsInLine == 0)
                {
                    x = 0;
                    y += ctrlHeight + mFixedMargin;
                }
                else
                    x += mItemsSpacing;
            }
        }
        else    // 更新最小范围
        {
            if (mVecName.size() == 0)
            {
                mpHandle->mMinSize = { 0, 20 };
                return;
            }

            // 重新计算宽度
            int oneCellWidth = get_max_label_width() + mCtrlSpacing + mLineEditWidth;
            int width = 0;
            int lines = mVecName.size() / mItemsInLine + (mVecName.size() % mItemsInLine == 0 ? 0 : 1);
            int height = lines * BKUnit::minUnitHeight + mFixedMargin * (2 + lines - 1);
            if (mVecName.size() < mItemsInLine)
                width = oneCellWidth * mVecName.size() + (mVecName.size() - 1) * mItemsSpacing;
            else
                width = oneCellWidth * mItemsInLine + (mItemsInLine - 1) * mItemsSpacing;

            mpHandle->mMinSize = { 1.0f * width, 1.0f * height};
        }
    }

public:
    virtual QRectF boundingRect() const override
    {
        return mBoundingRect;
    }

    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override
    {
        static QTextOption op = QTextOption(Qt::AlignCenter);
        painter->save();
        {
            painter->setPen(QColor(255, 128, 0));
            for (int i = 0; i < mTitleArea.size(); ++i)
            {
                painter->drawText(mTitleArea[i], mVecName[i], op);
            }
                
            painter->restore();
        }

        painter->save();
        {
            painter->setPen(QColor(255, 128, 0));
            painter->setBrush(Qt::NoBrush);

            for (int i = 0; i < mEditorArea.size(); ++i)
            {
                painter->drawRoundedRect(mEditorArea[i], 2.0f, 2.0f);

                if(mType == Type::Int)
                    painter->drawText(mEditorArea[i], QString::number(std::get<0>(miVector[i])), op);
                else if(mType == Type::Float)
                    painter->drawText(mEditorArea[i], QString::number(std::get<0>(mfVector[i]), 'f', 3), op);
            }

            painter->restore();
        }
    }

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event) override
    {
        if (event->button() != Qt::LeftButton)
            return;

        for (auto item : mEditorArea)
        {
            if (item.contains(event->pos()))
            {
                update();
                event->accept();
                return;
            }
        }
    }

    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;


public:
    // 包围盒
    QRectF mBoundingRect;
    BKVectorEditor* mpHandle;
    BKVectorEditor::Type mType;
    std::vector<std::tuple<int, int, int>> miVector;
    std::vector<std::tuple<float, float, float>> mfVector;
    // 固定上下边距
    static constexpr int mFixedMargin = 2;
    // 标题与编辑框的间隔
    static constexpr int mCtrlSpacing = 6;
    // 单元间的间隔
    static constexpr int mItemsSpacing = 10;
    // 向量名称
    std::vector<QString> mVecName = { "x" , "y" , "z" , "u" , "v" , "w" };
    // 一行中有多少个控件
    int mItemsInLine = 2;
    // 固定一个输入框的宽度
    int mLineEditWidth = 56;
    // 向量标题的范围
    std::vector<QRectF> mTitleArea;
    // 向量编辑框的范围
    std::vector<QRectF> mEditorArea;
    // 编辑框
    static BKVectorCtrl* mpPublicEditor;
};


// 这两句不要注册，注册后会导致无法正常转换
// Q_DECLARE_METATYPE(BKVectorEditor::IntegerVec)
// Q_DECLARE_METATYPE(BKVectorEditor::FloatVec)

BKVectorCtrl* BKVectorEditor::Impl::mpPublicEditor = nullptr;
class BKVectorCtrl : public QGraphicsProxyWidget
{
public:
    using super = QGraphicsProxyWidget;
    BKVectorCtrl()
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

        QObject::connect(mpLineEdit, &QLineEdit::returnPressed, this, &BKVectorCtrl::uploadAndDisband);

        setZValue(2.0f);
    }

public:
    void setExpand(BKVectorEditor::Impl* parent, int index)
    {
        mpBindItem = parent;
        mLastIndex = index;

        if (auto card = mpBindItem->mpHandle->getBindCard())
        {
            auto cardItem = card->getBindItem();
            mBindOriginZ = cardItem->zValue();
            cardItem->setZValue(2.0f);
        }

        const QRectF& pr = mpBindItem->mEditorArea[mLastIndex];
        this->setPos(mpBindItem->mapToScene(pr.topLeft()));

        QSizeF size = pr.size();
        mpLineEdit->setFixedSize(size.toSize());
        this->setMinimumSize(size);
        this->setMaximumSize(size);

        mpLineEdit->setValidator(mpBindItem->mType == BKVectorEditor::Type::Int
            ? &mIntValidator
            : &mDoubleValidator);
        mpLineEdit->setText(mpBindItem->mType == BKVectorEditor::Type::Int
            ? QString::number(std::get<0>(mpBindItem->miVector[mLastIndex]))
            : QString::number(std::get<0>(mpBindItem->mfVector[mLastIndex]), 'f', 3));
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

    template<typename  T>
    bool compareAndCopy(T newVal, std::vector<std::tuple<T, T, T>>& rec)
    {
        T oldVal, min, max;
        std::tie(oldVal, min, max) = rec[mLastIndex];
        if (newVal >= min && newVal <= max && oldVal != newVal)
        {
            std::get<0>(rec[mLastIndex]) = newVal;

            std::vector<T> vec;
            for (const auto& item : rec)
                vec.push_back(std::get<0>(item));

            mpBindItem->mpHandle->dataChanged(QVariant::fromValue(vec));
            return true;
        }
        return false;
    };

    void uploadAndDisband()
    {
        if (mpBindItem)
        {
            if (mpBindItem->mType == BKVectorEditor::Type::Int)
                compareAndCopy(mpLineEdit->text().toInt(), mpBindItem->miVector);
            else if(mpBindItem->mType == BKVectorEditor::Type::Float)
                compareAndCopy(mpLineEdit->text().toFloat(), mpBindItem->mfVector);

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
    BKVectorEditor::Impl* mpBindItem = nullptr;
    qreal mBindOriginZ = 0;

    QRegExpValidator mIntValidator;
    QRegExpValidator mDoubleValidator;

    int mLastIndex = -1;
};

void BKVectorEditor::Impl::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->button() != Qt::LeftButton)
        return;

    for (int i = 0; i < mEditorArea.size(); ++i)
    {
        auto& item = mEditorArea[i];
        if (item.contains(event->pos()))
        {
            TopmostCardEvent e(mpHandle->mpBindCard->getBindItem());
            qApp->sendEvent(scene(), &e);

            if (!mpPublicEditor)
                mpPublicEditor = new BKVectorCtrl();

            mpPublicEditor->setExpand(this, i);
            break;
        }
    }

    event->accept();
}

BKVectorEditor::BKVectorEditor(BKVectorEditor::Type type, uint32_t count)
    : mpImpl(new Impl(this, type, count))
{

}

BKVectorEditor::~BKVectorEditor()
{
    delete mpImpl;
    mpImpl = nullptr;
}

QJsonValue BKVectorEditor::getValue()
{
    L_IMPL(BKVectorEditor);

    QJsonArray ret;

    switch (l->mType)
    {
    case Type::Int:
        for (const auto& item : l->miVector)
            ret.push_back(std::get<0>(item));
        break;
    case Type::Float:
        for (const auto& item : l->mfVector)
            ret.push_back(std::get<0>(item));
        break;
    }

    return ret;
}

bool BKVectorEditor::setValue(const QJsonValue& val)
{
    L_IMPL(BKVectorEditor);
    if (!val.isArray())
        return false;

    QJsonArray rec = val.toArray();
    if (rec.count() == 0)
        return true;

    switch (l->mType)
    {
    case Type::Int:
        for(int i = 0; i < std::min((int)l->miVector.size(), rec.count()); ++i)
            std::get<0>(l->miVector[i]) = rec[i].toInt();
        break;
    case Type::Float:
        for (int i = 0; i < std::min((int)l->mfVector.size(), rec.count()); ++i)
            std::get<0>(l->mfVector[i]) = rec[i].toDouble();
        break;
    }

    return true;
}

BKVectorEditor* BKVectorEditor::setValue(const QVariant& value)
{
    L_IMPL(BKVectorEditor);

    do 
    {
        if (l->mType == BKVectorEditor::Type::Float)
        {
            if (!value.canConvert<BKVectorEditor::FloatVec>())
                break;

            FloatVec dst = value.value<BKVectorEditor::FloatVec>();
            for (size_t i = 0; i < std::min(l->mfVector.size(), dst.size()); ++i)
                std::get<0>(l->mfVector[i]) = dst[i];
        }
        else if (l->mType == BKVectorEditor::Type::Int)
        {
            if (!value.canConvert<BKVectorEditor::IntegerVec>())
                break;

            IntegerVec dst = value.value<BKVectorEditor::IntegerVec>();
            for (size_t i = 0; i < std::min(l->miVector.size(), dst.size()); ++i)
                std::get<0>(l->miVector[i]) = dst[i];
        }

    } while (false);

    return this;
}

BKVectorEditor* BKVectorEditor::setRange(int index, const std::pair<QVariant, QVariant>& minmax)
{
    L_IMPL(BKVectorEditor);

    if (index < 0)
    {
        if (l->mType == BKVectorEditor::Type::Int)
        {
            for (auto& item : l->miVector)
            {
                std::get<1>(item) = minmax.first.toInt();
                std::get<2>(item) = minmax.second.toInt();
            }
        }
        else if (l->mType == BKVectorEditor::Type::Float)
        {
            for (auto& item : l->mfVector)
            {
                std::get<1>(item) = minmax.first.toFloat();
                std::get<2>(item) = minmax.second.toFloat();
            }
        }
    }
    else
    {
        if (l->mType == BKVectorEditor::Type::Int && index < l->miVector.size())
        {
            auto& item = l->miVector[index];
            std::get<1>(item) = minmax.first.toInt();
            std::get<2>(item) = minmax.second.toInt();
        }
        else if(l->mType == BKVectorEditor::Type::Float && index < l->mfVector.size())
        {
            auto& item = l->mfVector[index];
            std::get<1>(item) = minmax.first.toFloat();
            std::get<2>(item) = minmax.second.toFloat();
        }
    }

    return this;
}

BKVectorEditor* BKVectorEditor::setItemInLine(int count)
{
    L_IMPL(BKVectorEditor);
    l->mItemsInLine = count < 1 ? 1 : count;
    l->recalcArea();
    return this;
}

BKVectorEditor* BKVectorEditor::setNames(const std::vector<QString>& names)
{
    L_IMPL(BKVectorEditor);

    int maxCount = 0;
    if (l->mType == BKVectorEditor::Type::Float)
        maxCount = l->mfVector.size();
    else if(l->mType == BKVectorEditor::Type::Int)
        maxCount = l->miVector.size();

    if (maxCount == 0)
        return this;

    l->mVecName.clear();
    if (names.size() > maxCount)
        l->mVecName.insert(l->mVecName.begin(), names.begin(), std::next(names.begin(), maxCount));
    else
    {
        l->mVecName.insert(l->mVecName.begin(), names.begin(), names.end());
        int count = maxCount - names.size();
        while (count--)
            l->mVecName.push_back("?");
    }

    l->recalcArea();
    return this;
}

QGraphicsItem* BKVectorEditor::getGraphicsItem()
{
    return mpImpl;
}

void BKVectorEditor::resized()
{
    L_IMPL(BKVectorEditor);

    l->recalcArea(mSize);
    l->mBoundingRect = { 0, 0, mSize.width(), mSize.height() };
}

void BKVectorEditor::dataChanged(const QVariant& data)
{
    L_IMPL(BKVectorEditor);

    if (data.isNull())
    {
        if (mpRightAnchor)
            mpRightAnchor->dataChanged(l->getValue());
    }
    else
    {
        setValue(data);
        l->update();
        if (!mCallbackFunc(data) && mpRightAnchor)
            mpRightAnchor->dataChanged(data);
    }
}
