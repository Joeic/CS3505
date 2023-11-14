//Code style reviewed by Joey Cai on 4/5/2023
#ifndef FRAMEITEMDELEGATE_H
#define FRAMEITEMDELEGATE_H

#include <QStyledItemDelegate>

///
/// \brief The FrameItemDelegate class is a helper for displaying frames in QListView
/// \author Kyle Holland
///
class FrameItemDelegate
    : public QStyledItemDelegate
{
    Q_OBJECT

public:
    FrameItemDelegate();
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

#endif // FRAMEITEMDELEGATE_H
