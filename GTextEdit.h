#ifndef G_EDITOR_H
#define G_EDITOR_H

#include <QPlainTextEdit>
#include <QEvent>
#include <QWheelEvent>

class GTextEdit : public QPlainTextEdit
{
    Q_OBJECT

public:
    explicit GTextEdit(QWidget* parent = nullptr) : QPlainTextEdit(parent) {}
    ~GTextEdit() override = default;
private:

    // Handle gesture zooming event
    void wheelEvent(QWheelEvent* event) override
    {
        if (event->modifiers() & Qt::ControlModifier) {
            const int dy = event->angleDelta().y() != 0 ? event->angleDelta().y()
                                                    : event->pixelDelta().y();

            if (dy > 0) zoomIn();
            else if (dy < 0) zoomOut();

            event->accept();
            return;
        }
        QPlainTextEdit::wheelEvent(event); // base class handles normal scrolling
        }
    };

#endif // G_EDITOR_H
