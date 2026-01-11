#include "mainwindow.h"

#include <QToolBar>
#include <QAction>
#include <QTextEdit>
#include <QSpinBox>
#include <QFileSystemModel>
#include <QTreeView>
#include <QDockWidget>

#include <QOverload>
#include <QFont>
#include <QFileDialog>
#include <QFile>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), m_mainFont("Titillium Web")
{
    auto editorPanel = new QWidget;
    auto editorLayout = new QVBoxLayout;
    editorPanel->setLayout(editorLayout);

    setCentralWidget(editorPanel);

    // Toolbar
    auto toolbar = new QToolBar("Toolbar", this);

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
    editorLayout->addWidget(toolbar);

    // text editor
    editor = new GTextEdit(this);
    setFont(m_mainFont);
    setFontSize(m_default_font_pt);
    editorLayout->addWidget(editor);


    // toolbar connections
    connect(open_action, &QAction::triggered,
            this, &MainWindow::openFileDialog);

    connect(save_action, &QAction::triggered,
            this, &MainWindow::saveFileDialog);

    connect(clear_action, &QAction::triggered,
            editor, &QTextEdit::clear);

    connect(font_size, &QSpinBox::valueChanged,
            this, &MainWindow::setFontSize);

    // directory list
    QDockWidget *dirWindow = new QDockWidget;
    dirWindow->setAllowedAreas(Qt::LeftDockWidgetArea
                                    | Qt::RightDockWidgetArea);
    addDockWidget(Qt::LeftDockWidgetArea, dirWindow);

    QString path = "C:/Notes";

    QFileSystemModel *model = new QFileSystemModel;
    model->setRootPath(path);
    QTreeView *treeView = new QTreeView;
    treeView->setModel(model);

    treeView->setRootIndex(model->index(model->rootPath()));

    dirWindow->setWidget(treeView);

    // directory connections
    connect(treeView, &QTreeView::clicked,
            this, [this, model](const QModelIndex &index)
            {
                QString path = model->filePath(index);
                qDebug() << path;
                openFile(path);
            }
    );

    //hide extra columns
    for (int i = 1; i<4; i++) {
        treeView->hideColumn(i);
    }

    // resizing
    this->resize(800, 500);
    dirWindow->resize(100, dirWindow->height());
}

void MainWindow::setFontSize(int font_size)
{
    QFont f = editor->font();
    f.setPointSize(font_size);
    editor->setFont(f);
}

void MainWindow::openFile(QString filename)
{
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

void MainWindow::openFileDialog()
{

    QString filters = "Text files (*.txt)";

    QFileDialog file_dialog(this);
    file_dialog.setViewMode(QFileDialog::Detail);

    QString filename = file_dialog.getOpenFileName(this, "Open file...", "C:/Notes", filters);
    if (filename.isEmpty()) return;

    openFile(filename);
}

void MainWindow::saveFileDialog()
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
