#include "mainwindow.h"

#include <QToolBar>
#include <QAction>
#include <QTextEdit>
#include <QSpinBox>
#include <QFileSystemModel>
#include <QTreeView>
#include <QSplitter>
#include <QOverload>
#include <QFont>
#include <QFileDialog>
#include <QFile>
#include <QVBoxLayout>
#include <QList>

MainWindow::MainWindow(QWidget *parent) :
    QWidget(parent), m_mainFont("Titillium Web")
{
    QVBoxLayout *outer = new QVBoxLayout(this);
    QSplitter *mainPanel = new QSplitter;

    // text editor
    editor = new GTextEdit(this);
    setFont(m_mainFont);
    setFontSize(m_default_font_pt);

    //Create and add to outer
    createToolbar(outer);

    outer->addWidget(mainPanel);

    // directory list
    QString path = "C:/Notes";

    QFileSystemModel *model = new QFileSystemModel;
    model->setRootPath(path);
    QTreeView *treeView = new QTreeView;
    treeView->setModel(model);

    treeView->setRootIndex(model->index(model->rootPath()));

    // directory connections
    connect(treeView, &QTreeView::clicked,
            this, [this, model](const QModelIndex &index)
            {
                QString path = model->filePath(index);
                qDebug() << path;
                openFile(path);
            }
    );

    //Add widgets to splitter layout
    mainPanel->addWidget(treeView);
    mainPanel->addWidget(editor);

    // resizing
    const int WIDTH = 800;
    const int HEIGHT = 500;
    this->resize(WIDTH, HEIGHT);

    QList<int> defaultSizes = {150, WIDTH-150};
    mainPanel->setSizes(defaultSizes);

    //hide extra columns
    for (int i = 1; i<4; i++) {
        treeView->hideColumn(i);
    }
}

void MainWindow::createToolbar(QVBoxLayout* editorLayout)
{
    auto toolbar = new QToolBar("Toolbar", this);

    auto open_action = new QAction("Open", this);
    auto save_action = new QAction("Save", this);
    auto clear_action = new QAction("Clear", this);

    toolbar->addAction(open_action);
    toolbar->addAction(save_action);
    toolbar->addAction(clear_action);

    editorLayout->addWidget(toolbar);

    // toolbar connections
    connect(open_action, &QAction::triggered,
            this, &MainWindow::openFileDialog);

    connect(save_action, &QAction::triggered,
            this, &MainWindow::saveFileDialog);

    connect(clear_action, &QAction::triggered,
            editor, &GTextEdit::clear);
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
