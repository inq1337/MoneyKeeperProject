#include "progressbardelegate.h"

ProgressBarDelegate::ProgressBarDelegate( QObject* parent ) : QStyledItemDelegate( parent ) {
}

void ProgressBarDelegate::paint(
    QPainter* painter,
    const QStyleOptionViewItem& option,
    const QModelIndex& index
) const {
    int progress = index.data().toInt();

    QString style = "QProgressBar { border: 1px solid rgb(206,212,218); border-radius: 6px; }";
    style += "QProgressBar::chunk { background-color: rgb(13,110,253); border-radius: 5px; }";

    QSize barSize;
    barSize.setHeight(option.rect.height() - 14);
    barSize.setWidth(option.rect.width());

    QPoint barPosition;
    barPosition.setX(option.rect.left());
    barPosition.setY(option.rect.top() + 7);

    QProgressBar renderer;
    renderer.resize(barSize);
    renderer.setRange(0, 100);
    if (progress > 100) {
        renderer.setValue(100);
        renderer.setFormat(QString::number(progress) + "%");
    }
    else {
        renderer.setValue(progress);
    }
    renderer.setAlignment(Qt::AlignCenter);
    renderer.setFont(QFont("Trebuchet MS", 10, QFont::Bold));
    renderer.setStyleSheet(style);
    renderer.setContentsMargins(-10, -10, -10, -10);

    painter->save();
    painter->translate(barPosition);
    renderer.render(painter);
    painter->restore();
}
