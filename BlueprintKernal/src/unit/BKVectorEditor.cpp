#include "unit/BKVectorEditor.h"
#include "container/BKCard.h"
#include "BKEvent.h"
#include <stdexcept>
#include <limits>

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsProxyWidget>
#include <QCoreApplication>
#include <QRegExpValidator>
#include <QGraphicsScene>
#include <QFontMetrics>
#include <QApplication>
#include <QJsonArray>
#include <QLineEdit>
#include <QPainter>
#include <QStyle>

class BKVectorCtrl;
class BKVectorEditor::Impl : public QGraphicsItem
{
public:
    Impl(BKVectorEditor* handle, BKVectorEditor::Type type, uint32_t count)
        : mpHandle(handle)
        , mType(type)
    {
        if (count != 0)
        {
            if (mType == Type::Float)
            {
                mfVector.resize(count);
                for (auto& item : mfVector)
                {
                    std::get<1>(item) = -std::numeric_limits<float>::max();
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

public:
    QVariant getValue()
    {
        QVariant ret;

        if (mType == Type::Float)
        {
            FloatVec vec;
            for (const auto& item : mfVector) {
                vec.push_back(std::get<0>(item));
            }

            ret = QVariant::fromValue(vec);
        }
        else if (mType == Type::Int)
        {
            IntegerVec vec;
            for (const auto& item : miVector) {
                vec.push_back(std::get<0>(item));
            }

            ret = QVariant::fromValue(vec);
        }

        return ret;
    }

    void recalcArea(const QSizeF size = {-1, -1})
    {
        auto get_max_label_width = [this]() -> int {
            int maxWidth = 0;
            QFontMetrics fm = QApplication::fontMetrics();
            for (const QString& title : mVecName) {
                maxWidth = std::max(fm.size(Qt::TextSingleLine, title).width(), maxWidth);
            }

            return maxWidth;
        };


        if (size.isValid()) // Update the edit box size
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
                else {
                    x += mItemsSpacing;
                }
            }
        }
        else    // Refresh minimum range
        {
            if (mVecName.size() == 0)
            {
                mpHandle->mMinSize = { 0, 20 };
                return;
            }

            // Recalculates the width
            int oneCellWidth = get_max_label_width() + mCtrlSpacing + mLineEditWidth;
            int width = 0;
            int lines = mVecName.size() / mItemsInLine + (mVecName.size() % mItemsInLine == 0 ? 0 : 1);
            int height = lines * BKUnit::minUnitHeight + mFixedMargin * (2 + lines - 1);
            if (mVecName.size() < mItemsInLine) {
                width = oneCellWidth * mVecName.size() + (mVecName.size() - 1) * mItemsSpacing;
            }
            else {
                width = oneCellWidth * mItemsInLine + (mItemsInLine - 1) * mItemsSpacing;
            }

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
            for (int i = 0; i < mTitleArea.size(); ++i) {
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

                if (mType == Type::Int) {
                    painter->drawText(mEditorArea[i], QString::number(std::get<0>(miVector[i])), op);
                }
                else if (mType == Type::Float) {
                    painter->drawText(mEditorArea[i], QString::number(std::get<0>(mfVector[i]), 'f', 3), op);
                }
            }

            painter->restore();
        }
    }

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event) override
    {
        if (event->button() != Qt::LeftButton || !mpHandle->mbEnable)
        {
            event->ignore();
            return;
        }

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
    QRectF mBoundingRect;
    BKVectorEditor* mpHandle;
    BKVectorEditor::Type mType;
    std::vector<std::tuple<int, int, int>> miVector;
    std::vector<std::tuple<float, float, float>> mfVector;

    static constexpr int mFixedMargin = 2;
    static constexpr int mCtrlSpacing = 6;
    static constexpr int mItemsSpacing = 10;

    std::vector<QString> mVecName = { "x" , "y" , "z" , "u" , "v" , "w" };

    int mItemsInLine = 2;
    // The fixed width of an input box
    int mLineEditWidth = 56;

    std::vector<QRectF> mTitleArea;
    std::vector<QRectF> mEditorArea;

    static BKVectorCtrl* mpPublicEditor;
};

// These two sentences do not register, after registration, it will lead to the failure of conversion
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
            for (const auto& item : rec) {
                vec.push_back(std::get<0>(item));
            }

            mpBindItem->mpHandle->dataChanged(QVariant::fromValue(vec));
            return true;
        }
        return false;
    };

    void uploadAndDisband()
    {
        if (mpBindItem)
        {
            if (mpBindItem->mType == BKVectorEditor::Type::Int) {
                compareAndCopy(mpLineEdit->text().toInt(), mpBindItem->miVector);
            }
            else if (mpBindItem->mType == BKVectorEditor::Type::Float) {
                compareAndCopy(mpLineEdit->text().toFloat(), mpBindItem->mfVector);
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

            if (!mpPublicEditor) {
                mpPublicEditor = new BKVectorCtrl();
            }

            mpPublicEditor->setExpand(this, i);
            break;
        }
    }

    event->accept();
}

BKUnit* BKVectorEditor::copy()
{
    L_IMPL(BKVectorEditor);
    BKVectorEditor* target = BKCreator::create<BKVectorEditor>(l->mType, l->mVecName.size());

    BKVectorEditor::Impl* dstImpl = target->mpImpl;
    dstImpl->miVector = l->miVector;
    dstImpl->mfVector = l->mfVector;
    dstImpl->mVecName = l->mVecName;
    dstImpl->mItemsInLine = l->mItemsInLine;
    dstImpl->mLineEditWidth = l->mLineEditWidth;
    _copyBasicAttributeTo(target);
    return target;
}


BKVectorEditor::BKVectorEditor(BKVectorEditor::Type type, uint32_t count)
    : mpImpl(new Impl(this, type, count))
{
    mpImpl->recalcArea();
}

BKVectorEditor::~BKVectorEditor()
{
    delete mpImpl;
    mpImpl = nullptr;
}

bool BKVectorEditor::loadFromJson(const QJsonValue& val)
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
    {
        BKVectorEditor::IntegerVec vec;
        int count = std::min((int)l->miVector.size(), rec.count());
        for (int i = 0; i < count; ++i) {
            vec.push_back(rec[i].toInt());
        }
        dataChanged(QVariant::fromValue(vec));
    }
    break;

    case Type::Float:
    {
        BKVectorEditor::FloatVec vec;
        int count = std::min((int)l->mfVector.size(), rec.count());
        for (int i = 0; i < count; ++i) {
            vec.push_back(rec[i].toDouble());
        }
        dataChanged(QVariant::fromValue(vec));
    }
    break;
    }

    return true;
}

QVariant BKVectorEditor::data()
{
    L_IMPL(BKVectorEditor);

    switch (l->mType)
    {
    case Type::Int:
    {
        BKIntegerVector ret;
        for (const auto& item : l->miVector) {
            ret.push_back(std::get<0>(item));
        }

        return QVariant::fromValue(ret);
    }
    
    case Type::Float:
    {
        BKFloatVector ret;
        for (const auto& item : l->mfVector) {
            ret.push_back(std::get<0>(item));
        }

        return QVariant::fromValue(ret);
    }
    }

    return QVariant();
}

BKVectorEditor::operator QJsonValue() const
{
    L_IMPL(BKVectorEditor);

    QJsonArray ret;

    switch (l->mType)
    {
    case Type::Int:
        for (const auto& item : l->miVector) {
            ret.push_back(std::get<0>(item));
        }
        break;
    case Type::Float:
        for (const auto& item : l->mfVector) {
            ret.push_back(std::get<0>(item));
        }
        break;
    }

    return ret;
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
            for (size_t i = 0; i < std::min(l->mfVector.size(), dst.size()); ++i) {
                std::get<0>(l->mfVector[i]) = dst[i];
            }
        }
        else if (l->mType == BKVectorEditor::Type::Int)
        {
            if (!value.canConvert<BKVectorEditor::IntegerVec>())
                break;

            IntegerVec dst = value.value<BKVectorEditor::IntegerVec>();
            for (size_t i = 0; i < std::min(l->miVector.size(), dst.size()); ++i) {
                std::get<0>(l->miVector[i]) = dst[i];
            }
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
    if (l->mType == BKVectorEditor::Type::Float) {
        maxCount = l->mfVector.size();
    }
    else if (l->mType == BKVectorEditor::Type::Int) {
        maxCount = l->miVector.size();
    }

    if (maxCount == 0)
        return this;

    l->mVecName.clear();
    if (names.size() > maxCount) {
        l->mVecName.insert(l->mVecName.begin(), names.begin(), std::next(names.begin(), maxCount));
    }
    else
    {
        l->mVecName.insert(l->mVecName.begin(), names.begin(), names.end());
        int count = maxCount - names.size();
        while (count--) {
            l->mVecName.push_back("?");
        }
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
        if (mpRightAnchor) {
            mpRightAnchor->dataChanged(l->getValue());
        }
    }
    else
    {
        setValue(data);
        l->update();
        if (!mCallbackFunc(this, data) && mpRightAnchor) {
            mpRightAnchor->dataChanged(data);
        }
    }
}

