#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <qmainwindow.h>
#include <qboxlayout.h>

#include <QFileSystemModel>
#include <QTreeView>
#include <QTextDocument>

#include "GTextEdit.h"

class MainWindow : public QWidget{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
private:
    void setupToolbar(QVBoxLayout* layout);
    void setupTreeView(QString path);
    void showTreeViewContextMenu(const QPoint &pos);

private slots:
    void setFontSize(int font_size);
    void createFile(QDir dir, QString filename);
    void deleteFile(const QModelIndex &index);
    void createFolder(QDir dir, QString folderName);
    void openFile(QString filename);
    void saveFile(QString filename);
    void openFileDialog();
    void saveFileDialog();
    void saveCurrentFile();

private:
    QFont m_mainFont;
    static constexpr int m_default_font_pt = 11;
    QHash<QString, QTextDocument*> m_documentCache;
    QString m_currentFilePath;

    GTextEdit *editor = nullptr;
    QFileSystemModel *model = nullptr;
    QTreeView *treeView = nullptr;


};

#endif // MAINWINDOW_H
