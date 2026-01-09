#include "mainwindow.h"

#include <QToolBar>
#include <QAction>
#include <QTextEdit>
#include <QSpinBox>
#include <QOverload>
#include <QFont>
#include <QFileDialog>
#include <QFile>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), mainFont("Titillium Web")
{
    editor = new GTextEdit(this);
    setCentralWidget(editor);
    setFont(mainFont);
    setFontSize(m_default_font_pt);

    auto toolbar = new QToolBar("Toolbar", this);
    addToolBar(toolbar);

    auto open_action = new QAction("Open", this);
    auto save_action = new QAction("Save", this);
    auto clear_action = new QAction("Clear", this);

    toolbar->addAction(open_action);
    toolbar->addAction(save_action);
    toolbar->addAction(clear_action);

    auto font_size = new QSpinBox(this);
    font_size->setRange(1, 100);
    font_size->setValue(m_default_font_pt);

    toolbar->addWidget(font_size);

    connect(open_action, &QAction::triggered,
            this, &MainWindow::openFile);

    connect(save_action, &QAction::triggered,
            this, &MainWindow::saveFile);

    connect(clear_action, &QAction::triggered,
            editor, &QTextEdit::clear);

    connect(font_size, &QSpinBox::valueChanged,
            this, &MainWindow::setFontSize);

    this->resize(800, 500);
}

void MainWindow::setFontSize(int font_size)
{
    QFont f = editor->font();
    f.setPointSize(font_size);
    editor->setFont(f);
}

void MainWindow::openFile()
{

    QString filters = "Text files (*.txt)";

    QFileDialog file_dialog(this);
    file_dialog.setViewMode(QFileDialog::Detail);

    QString filename = file_dialog.getOpenFileName(this, "Open file...", "C:/Notes", filters);
    if (filename.isEmpty()) return;

    QFile file(filename);
    // check if can load
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "Error loading file.";
        return;
    }

    const QString contents = QString::fromUtf8(file.readAll());
    editor->setPlainText(contents);
}

void MainWindow::saveFile()
{
    QString filters = "Text files (*.txt)";

    QFileDialog file_dialog(this);
    file_dialog.setViewMode(QFileDialog::List);

    QString filename = file_dialog.getSaveFileName(this, "Save file...", "C:/Notes", filters);

    QFile file(filename);

    if (!file.open(QIODevice::WriteOnly))
    {
        qDebug() << "Error saving file.";
        return;
    }

    const QString contents = editor->toPlainText();
    file.write(contents.toUtf8());
}
