#include "GTextEdit.h"
#include <QEvent>
#include <QWheelEvent>

GTextEdit::GTextEdit(QWidget *parent) :
    QTextEdit(parent)
{
}

GTextEdit::~GTextEdit()
{
}


void GTextEdit::wheelEvent(QWheelEvent *e)
{
    if (e->angleDelta() == QPoint(0, 120))
    {
        zoomIn();
    }

    if (e->angleDelta() == QPoint(0, -120))
    {
        zoomOut();
    }
    QTextEdit::wheelEvent(e);
}
