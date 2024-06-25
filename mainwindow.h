#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCheckBox>
#include <QList>
#include <chrono>
#include <QByteArray>
#include <QCryptographicHash>
#include <QFile>
#include <QIODevice>
#include <QTextStream>
#include <QMessageBox>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_clear_clicked();

    void on_load_clicked();

private:
    void checkbox_checked(bool checked);
    void clear();
    void saveMove(int index);
    void readFile();

    Ui::MainWindow *ui;
    QList<QCheckBox*> checkboxes;
    QList<int> states;
    QByteArray previousHash;
    int counter;
};
#endif // MAINWINDOW_H
