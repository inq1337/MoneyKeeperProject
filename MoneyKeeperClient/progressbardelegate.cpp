#include "progressbardelegate.h"

ProgressBarDelegate::ProgressBarDelegate( QObject* parent ) : QStyledItemDelegate( parent ) {
}

void ProgressBarDelegate::paint(
    QPainter* painter,
    const QStyleOptionViewItem& option,
    const QModelIndex& index
) const {
    int progress = index.data().toInt();

    QString style = "QProgressBar { border: 2px solid rgb(206,212,218); border-radius: 6px; }";
    style += "QProgressBar::chunk { background-color: rgb(13,110,253); border-radius: 5px; }";

    QSize barSize;
    barSize.setHeight(option.rect.height() - 14);
    barSize.setWidth(option.rect.width());

    QPoint barPosition;
    barPosition.setX(option.rect.left());
    barPosition.setY(option.rect.top() + 7);

    QProgressBar progressBar;
    progressBar.resize(barSize);
    progressBar.setRange(0, 100);
    if (progress > 100) {
        progressBar.setValue(100);
        progressBar.setFormat(QString::number(progress) + "%");
    }
    else {
        progressBar.setValue(progress);
    }
    progressBar.setAlignment(Qt::AlignCenter);
    progressBar.setFont(QFont("Trebuchet MS", 10, QFont::Bold));
    progressBar.setStyleSheet(style);
    progressBar.setContentsMargins(-10, -10, -10, -10);

    painter->save();
    painter->translate(barPosition);
    progressBar.render(painter);
    painter->restore();
}
