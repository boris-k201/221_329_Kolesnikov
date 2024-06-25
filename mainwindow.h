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

#include <openssl/evp.h>

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
    void setCell(int index);
    QByteArray getHash(int i, int j, QString time, QByteArray hash);
    void clear();
    void saveMove(int index);
    QByteArray readFile();
    void loadFile(QByteArray& unencryptedData);
    void saveFile(QByteArray& encryptedData);

    int encryptByteArray(QByteArray &inputBytes, QByteArray &outputBytes);
    int decryptByteArray(QByteArray &inputBytes, QByteArray &outputBytes);

    Ui::MainWindow *ui;
    QList<QCheckBox*> checkboxes;
    QList<int> states;
    QByteArray previousHash;
    int counter;
};
#endif // MAINWINDOW_H
