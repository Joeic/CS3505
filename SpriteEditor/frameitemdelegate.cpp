//Code style reviewed by Joey Cai on 4/5/2023
#include "frameitemdelegate.h"
#include "animation.h"
#include "frame.h"
#include <QPainter>

///
/// \brief FrameItemDelegate::FrameItemDelegate constructor.
///
FrameItemDelegate::FrameItemDelegate()
{

}

///
/// \brief FrameItemDelegate::sizeHint tell QListView how big to make items
/// \param option unused
/// \param index unused
/// \return QSize for how big each list item should be
///
QSize FrameItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);
    return QSize(100, 115);
}

///
/// \brief FrameItemDelegate::paint draw an element in a list of Frames
/// \param painter to use for draw operations
/// \param option to get standard colors
/// \param index which element to draw
///
void FrameItemDelegate::paint(QPainter *painter,
                              const QStyleOptionViewItem &option,
                                const QModelIndex &index) const
{
    if (index.isValid() && index.model())
    {
        const Animation* animModel = static_cast<const Animation*>(index.model());
        int row = index.data().toInt();
        std::shared_ptr<Frame> frame = animModel->getFrame(row);

        if (option.state & QStyle::State_Selected)
        {
            painter->fillRect(option.rect, option.palette.highlight());
        }
        painter->save();

        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->setPen(QPen(option.palette.windowText(), 1));
        painter->setBrush(Qt::NoBrush);

        painter->drawRect(option.rect.marginsRemoved(QMargins(5, 5, 5, 20)));
        painter->drawText(QRect(option.rect.x(), option.rect.y() + option.rect.height() - 15, option.rect.width(), 15),
                          Qt::AlignHCenter,
                          QString::fromStdString("Frame " + std::to_string(row)));

        painter->drawImage(option.rect.marginsRemoved(QMargins(5, 5, 5, 20)), frame->canvas);
        painter->restore();
    } else
    {
        QStyledItemDelegate::paint(painter, option, index);
    }
}
