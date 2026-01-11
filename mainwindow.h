#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include "GTextEdit.h"

class MainWindow : public QMainWindow{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

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
