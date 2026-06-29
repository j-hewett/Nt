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
#include <QShortcut>
#include <QTextDocument>
#include <QLabel>

MainWindow::MainWindow(QWidget *parent) :
    QWidget(parent), m_mainFont("Titillium Web")
{
    QVBoxLayout *outer = new QVBoxLayout(this);
    QSplitter *mainPanel = new QSplitter(this);

    // text editor
    m_editor = new GTextEdit(this);
    setFont(m_mainFont);
    setFontSize(m_default_font_pt);

    //add anonymous start-up doc to the cache
    QTextDocument *doc = new QTextDocument(this);
    m_documentCache.insert("", doc);
    m_editor->setDocument(doc);

    // Connect crtl-S
    auto *saveShortcut = new QShortcut(QKeySequence::Save, this);
    connect(saveShortcut, &QShortcut::activated, this, &MainWindow::saveCurrentFile);

    m_optionsDialog = new QDialog(this);
    setupOptionsDialog();

    setupToolbar(outer);     //Create and add to outer

    // Add document name label
    m_docName = new QLabel(this);
    m_docName->setText("Unnamed document");
    m_docName->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    outer->addWidget(m_docName);
    outer->addWidget(mainPanel);

    //Align docName and adjust spacing of outer layout
    outer->setAlignment(m_docName, Qt::AlignRight);

    // directory list (treeview)
    QString path = "C:/Notes";
    setupTreeView(path);

    //Add widgets to splitter layout
    mainPanel->addWidget(m_treeView);
    mainPanel->addWidget(m_editor);

    // resizing
    const int WIDTH = 800;
    const int HEIGHT = 500;
    this->resize(WIDTH, HEIGHT);

    QList<int> defaultSizes = {150, WIDTH-150};
    mainPanel->setSizes(defaultSizes);

    //hide extra columns
    for (int i = 1; i<4; i++) {
        m_treeView->hideColumn(i);
    }
}

void MainWindow::setupToolbar(QVBoxLayout* editorLayout)
{
    auto toolbar = new QToolBar("Toolbar", this);

    auto open_action = new QAction("Open", this);
    auto save_action = new QAction("Save", this);
    auto clear_action = new QAction("Clear", this);
    auto options_action = new QAction("Options", this);

    auto *spacer = new QWidget(this);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    toolbar->addAction(open_action);
    toolbar->addAction(save_action);
    toolbar->addAction(clear_action);
    toolbar->addWidget(spacer);
    toolbar->addAction(options_action);


    editorLayout->addWidget(toolbar);

    // toolbar connections
    connect(open_action, &QAction::triggered,
            this, &MainWindow::openFileDialog);

    connect(save_action, &QAction::triggered,
            this, &MainWindow::saveFileDialog);

    connect(clear_action, &QAction::triggered,
            this, [this]()
            {
                QTextCursor cursor = m_editor->textCursor();
                cursor.select(QTextCursor::Document);
                cursor.removeSelectedText();
            });
    connect(options_action, &QAction::triggered,
            m_optionsDialog, &QDialog::exec);
}

void MainWindow::setupOptionsDialog()
{
    m_optionsDialog->setWindowTitle("Options");
    m_optionsDialog->setMinimumWidth(300);

    QFontMetrics metrics(m_editor->font());
    qreal minPixels = metrics.horizontalAdvance(QChar::VisualTabCharacter);

    auto *layout = new QHBoxLayout(m_optionsDialog);
    auto *spinBox = new QSpinBox(m_optionsDialog);
    spinBox->setRange(minPixels, 200);
    spinBox->setValue(80);
    spinBox->setSingleStep(5);

    layout->addWidget(new QLabel("Tab width (px):"));
    layout->addWidget(spinBox);

    connect(spinBox, &QSpinBox::valueChanged,
            m_editor, &GTextEdit::setTabStopDistance);
}

void MainWindow::setupTreeView(QString path)
{
    m_fileModel= new QFileSystemModel;
    m_fileModel->setRootPath(path);

    m_treeView = new QTreeView;
    m_treeView->setModel(m_fileModel);
    m_treeView->setRootIndex(m_fileModel->index(m_fileModel->rootPath()));
    m_treeView->setContextMenuPolicy(Qt::CustomContextMenu);

    // directory connections
    connect(m_treeView, &QTreeView::clicked,
            this, [this](const QModelIndex &index)
            {

                QString path = m_fileModel->filePath(index);
                openFile(path);
            }
            );

    connect(m_treeView, &QTreeView::customContextMenuRequested,
            this, &MainWindow::showTreeViewContextMenu);
}

void MainWindow::showTreeViewContextMenu(const QPoint &pos)
{
    QDir dir = m_fileModel->rootDirectory();
    const QModelIndex index = m_treeView->indexAt(pos);
    if(index.isValid())
    {
        QFileInfo pathInfo = m_fileModel->fileInfo(index);
        dir = pathInfo.isDir() ? QDir(m_fileModel->filePath(index)) : pathInfo.dir();
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

    contextMenu.exec(m_treeView->viewport()->mapToGlobal(pos));
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

    QString path = m_fileModel->filePath(index);
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
        if (!m_fileModel->remove(index))
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
    //outgoing file
    if (!m_currentFilePath.isEmpty())
    {
        m_documentCache[m_currentFilePath] = m_editor->document();
    }

    //incoming file

    m_currentFilePath = filename;

    if (m_documentCache.contains(filename))
    {
        m_editor->setDocument(m_documentCache[filename]);
        return;
    }

    QFile file(filename);
    // check if can load
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "Error loading file.";
        return;
    }

    const QString contents = QString::fromUtf8(file.readAll());
    file.close();

    QTextDocument *doc = new QTextDocument(this);
    doc->setPlainText(contents);
    m_documentCache.insert(filename, doc);
    m_editor->setDocument(doc);
    setDocNameHints();
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

void MainWindow::saveFile(QString filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly))
    {
        qDebug() << "Error saving file.";
        return;
    }
    const QString contents = m_editor->toPlainText();
    file.write(contents.toUtf8());
}

void MainWindow::saveFileDialog()
{
    QString filters = "Text files (*.txt)";

    QFileDialog file_dialog(this);
    file_dialog.setViewMode(QFileDialog::List);

    QString filename = file_dialog.getSaveFileName(this, "Save file...", "C:/Notes", filters);

    saveFile(filename);
}

void MainWindow::saveCurrentFile()
{
    if (m_currentFilePath.isEmpty() || m_currentFilePath == "")
        saveFileDialog();
    else
        saveFile(m_currentFilePath);
}

void MainWindow::setDocNameHints()
{
    // File hints
    QFile file(m_currentFilePath);
    QFileInfo info(file);
    QString baseFileName = info.fileName(); //Get just the filename not the full path
    m_docName->setText(baseFileName);
    m_docName->setToolTip(info.absoluteFilePath()); // full path in tooltip
    setWindowTitle(baseFileName + "[*]" + " - Nt");
    setWindowModified(false);
}

void MainWindow::setFontSize(int font_size)
{
    QFont f = m_editor->font();
    f.setPointSize(font_size);
    m_editor->setFont(f);
}
