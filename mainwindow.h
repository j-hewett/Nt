#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <qmainwindow.h>
#include <qboxlayout.h>
#include "GTextEdit.h"

class MainWindow : public QWidget{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
private:
    void createToolbar(QVBoxLayout* layout);

private slots:
    void setFontSize(int font_size);
    void openFile(QString filename);
    void openFileDialog();
    void saveFileDialog();

private:
    QFont m_mainFont;
    static constexpr int m_default_font_pt = 11;
    GTextEdit *editor = nullptr;

};

#endif // MAINWINDOW_H
