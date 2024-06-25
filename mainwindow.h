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
///
/// Главный класс, в котором содержится весь код программы
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_clear_clicked();

    void on_load_clicked();

private:
    /// Вызывается при нажатии на кнопку
    void checkbox_checked(bool checked);
    /// Устанавливает состояние кнопки в зависимости от счётчика
    void setCell(int index);
    /// Получает хэш для сохранения ходов
    QByteArray getHash(int i, int j, QString time, QByteArray hash);
    /// Очищает все клетки, обнуляет предыдущий хэш и сбрасывает счётчик
    void clear();
    /// Добавляет в файл текущий ход
    void saveMove(int index);
    /// Считывает и расшифровывает файл с ходами
    QByteArray readFile();
    /// Устанавливает состояние игры из файла
    void loadFile(QByteArray& unencryptedData);
    /// Сохраняет и шифрует данные в файл
    void saveFile(QByteArray& encryptedData);
    /// Показывает всплывающее окно об ошибке
    void errorMessage(QString errorMessage);

    /// Шифрует данные и возвращает код ошибки
    int encryptByteArray(QByteArray &inputBytes, QByteArray &outputBytes);
    /// Расшифрует данные и возвращает код ошибки
    int decryptByteArray(QByteArray &inputBytes, QByteArray &outputBytes);

    Ui::MainWindow *ui;
    QList<QCheckBox*> checkboxes;
    QList<int> states;
    QByteArray previousHash;
    int counter;
};
#endif // MAINWINDOW_H
