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
#include <QFileInfo>
#include <QVBoxLayout>
#include <QList>
#include <QMenu>
#include <QInputDialog>
#include <QMessageBox>

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

    // directory list (treeview)
    QString path = "C:/Notes";
    setupTreeView(path);

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

void MainWindow::setupTreeView(QString path)
{
    model = new QFileSystemModel;
    model->setRootPath(path);

    treeView = new QTreeView;
    treeView->setModel(model);
    treeView->setRootIndex(model->index(model->rootPath()));
    treeView->setContextMenuPolicy(Qt::CustomContextMenu);

    // directory connections
    connect(treeView, &QTreeView::clicked,
            this, [this](const QModelIndex &index)
            {
                QString path = model->filePath(index);
                openFile(path);
            }
            );

    connect(treeView, &QTreeView::customContextMenuRequested,
            this, &MainWindow::showTreeViewContextMenu);
}

void MainWindow::showTreeViewContextMenu(const QPoint &pos)
{
    QDir dir = model->rootDirectory();
    const QModelIndex index = treeView->indexAt(pos);
    if(index.isValid())
    {
        QFileInfo pathInfo = model->fileInfo(index);
        dir = pathInfo.isDir() ? QDir(model->filePath(index)) : pathInfo.dir();
    }

    QMenu contextMenu;
    auto new_file_action = new QAction("New file", this);
    auto new_dir_action = new QAction("New folder", this);
    auto del_action = new QAction("Delete", this);

    contextMenu.addAction(new_file_action);
    contextMenu.addAction(new_dir_action);
    contextMenu.addAction(del_action);
    del_action->setEnabled(index.isValid());

    connect(new_file_action, &QAction::triggered,
            this, [this, dir]()
            {
                bool ok;
                QString filename = QInputDialog::getText(this, "New file",
                                                         "File name:", QLineEdit::Normal,
                                                         "", &ok);
                if (ok && !filename.isEmpty())
                {
                    createFile(dir, filename);
                }
            });

    connect(new_dir_action, &QAction::triggered,
            this, [this, dir]()
            {
                bool ok;
                QString foldername = QInputDialog::getText(this, "New folder",
                                                           "Folder name:", QLineEdit::Normal,
                                                           "", &ok);
                if (ok && !foldername.isEmpty())
                {
                    createFolder(dir, foldername);
                }
            });

    connect(del_action, &QAction::triggered,
            this, [this, index]()
            {
                deleteFile(index);
    });

    contextMenu.exec(treeView->viewport()->mapToGlobal(pos));
}

void MainWindow::createFile(QDir dir, QString filename)
{
    QString filePath = dir.filePath(filename) + ".txt";
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly))
    {
        qDebug() << "Error creating file.";
        return;
    }
    file.close();
    openFile(filePath);
}

void MainWindow::deleteFile(const QModelIndex &index)
{
    if (!index.isValid()) { qDebug() << "Index invalid"; return; }

    QString path = model->filePath(index);
    QFileInfo fileInfo(path);

    if(fileInfo.isDir()) { qDebug() << "Cannot delete folders"; return; }

    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Delete", "Are you sure you want to delete '" + fileInfo.fileName() + "'?",
        QMessageBox::Yes | QMessageBox::No
        );

    if (reply != QMessageBox::Yes) return;

    if (fileInfo.isDir())
    {
        // QDir d(path);
        // if (!d.removeRecursively())
        //     qDebug() << "Error deleting folder.";
    }
    else
    {
        if (!model->remove(index))
            qDebug() << "Error deleting file.";
        qDebug() << "Index valid:" << index.isValid();
    }
}

void MainWindow::createFolder(QDir dir, QString folderName)
{
    if (!dir.mkdir(folderName))
    {
        qDebug() << "Error creating folder.";
    }
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

void MainWindow::setFontSize(int font_size)
{
    QFont f = editor->font();
    f.setPointSize(font_size);
    editor->setFont(f);
}
