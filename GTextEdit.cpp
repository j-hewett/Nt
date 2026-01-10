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
    if (e->modifiers() & Qt::ControlModifier) {
            const int dy = e->angleDelta().y() != 0 ? e->angleDelta().y()
                                                    : e->pixelDelta().y();

            if (dy > 0) zoomIn();
            else if (dy < 0) zoomOut();

            e->accept();
            return;
        }
}
