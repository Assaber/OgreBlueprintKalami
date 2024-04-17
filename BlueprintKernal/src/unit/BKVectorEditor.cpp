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

using IntegerVec = std::vector<int>;
using FloatVec = std::vector<float>;

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
        __debugbreak();
        return QVariant();
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

            int tw = get_max_label_width();
            int lines = mVecName.size() / mItemsInLine + (mVecName.size() % mItemsInLine == 0 ? 0 : 1);
            int ctrlHeight = (size.height() - (lines + 1) * mFixedMargin) / lines;
            int ctrlWidth = 0;
            if (mVecName.size() < mItemsInLine)
                ctrlWidth = (size.width() - (mVecName.size() - 1) * mFixedMargin - mVecName.size() * tw) / mVecName.size();
            else
                ctrlWidth = (size.width() - (mItemsInLine - 1) * mFixedMargin - mItemsInLine * tw) / mItemsInLine;

            mTitleArea.clear();
            mEditorArea.clear();

            int x = 0, y = mFixedMargin;
            for (int i = 0; i < mVecName.size();)
            {
                mTitleArea.push_back(QRectF(x, y, tw, ctrlHeight));
                x += tw;
                mEditorArea.push_back(QRectF(x, y, ctrlWidth, ctrlHeight));
                x += ctrlWidth;

                ++i;
                if (i % mItemsInLine == 0)
                {
                    x = 0;
                    y += ctrlHeight + mFixedMargin;
                }
                else
                    x += mFixedMargin;
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
            int tw = get_max_label_width();
            int oneCellWidth = tw + mLineEditWidth;
            int width = 0;
            int lines = mVecName.size() / mItemsInLine + (mVecName.size() % mItemsInLine == 0 ? 0 : 1);
            int height = lines * BKUnit::minUnitHeight + mFixedMargin * (2 + lines - 1);
            if (mVecName.size() < mItemsInLine)
                width = oneCellWidth * mVecName.size() + (mVecName.size() - 1) * mFixedMargin;
            else
                width = oneCellWidth * mItemsInLine + (mItemsInLine - 1) * mFixedMargin;

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
        // 绘制外边框
        painter->save();
        {
            for(const auto& item : mTitleArea)
                painter->fillRect(item, Qt::red);
            painter->restore();
        }

        painter->save();
        {
            for (const auto& item : mEditorArea)
                painter->fillRect(item, Qt::green);
            painter->restore();
        }
    }

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event) override
    {
        // mpHandle->dataChanged(!mbEnable);
        // update();
    }


public:
    // 包围盒
    QRectF mBoundingRect;
    BKVectorEditor* mpHandle;
    BKVectorEditor::Type mType;
    std::vector<std::tuple<int, int, int>> miVector;
    std::vector<std::tuple<float, float, float>> mfVector;
    // 固定上下边距
    static constexpr int mFixedMargin = 2;
    // 向量名称
    std::vector<QString> mVecName = { "x" , "y" , "z" , "u" , "v" , "w" };
    // 一行中有多少个控件
    int mItemsInLine = 2;
    // 固定一个输入框的宽度
    int mLineEditWidth = 75;
    // 向量标题的范围
    std::vector<QRectF> mTitleArea;
    // 向量编辑框的范围
    std::vector<QRectF> mEditorArea;
};

Q_DECLARE_METATYPE(IntegerVec)
Q_DECLARE_METATYPE(FloatVec)


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
            if (!value.canConvert<FloatVec>())
                break;

            FloatVec dst = value.value<FloatVec>();
            for (size_t i = 0; i < std::min(l->mfVector.size(), dst.size()); ++i)
                std::get<0>(l->mfVector[i]) = dst[i];
        }
        else if (l->mType == BKVectorEditor::Type::Int)
        {
            if (!value.canConvert<IntegerVec>())
                break;

            IntegerVec dst = value.value<IntegerVec>();
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
            l->mVecName.push_back("");
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
