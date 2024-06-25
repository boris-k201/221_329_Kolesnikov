#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    states = QList<int>(16);
    checkboxes.append(this->ui->a1);
    checkboxes.append(this->ui->a2);
    checkboxes.append(this->ui->a3);
    checkboxes.append(this->ui->a4);
    checkboxes.append(this->ui->b1);
    checkboxes.append(this->ui->b2);
    checkboxes.append(this->ui->b3);
    checkboxes.append(this->ui->b4);
    checkboxes.append(this->ui->c1);
    checkboxes.append(this->ui->c2);
    checkboxes.append(this->ui->c3);
    checkboxes.append(this->ui->c4);
    checkboxes.append(this->ui->d1);
    checkboxes.append(this->ui->d2);
    checkboxes.append(this->ui->d3);
    checkboxes.append(this->ui->d4);
    for (int i = 0; i < checkboxes.size(); i++)
        connect(checkboxes[i], &QCheckBox::clicked, this, &MainWindow::checkbox_checked);
    counter = 0;
    previousHash = QByteArray();
}

void MainWindow::checkbox_checked(bool checked) {
    QCheckBox* checkbox = static_cast<QCheckBox*>(sender());
    int index = checkboxes.indexOf(checkbox);
    checkbox->setCheckable(false);
    if (counter % 2 == 0)
        checkbox->setStyleSheet("background-color: #FF0000");
    else
        checkbox->setStyleSheet("background-color: #00FF00");
    counter++;
    saveMove(index);
}

void MainWindow::saveMove(int index) {
    const auto now = std::chrono::system_clock::now();
    const std::time_t t_c = std::chrono::system_clock::to_time_t(now);
    std::tm* tm_c = std::localtime(&t_c);
    char buffer[20];
    std::strftime(buffer, sizeof(buffer), "%Y.%m.%d_%H:%M:%S", tm_c);
    previousHash = QCryptographicHash::hash((QString::number(index / 4)+QString::number(index % 4)+QString(buffer)).toUtf8()+previousHash, QCryptographicHash::Sha256);
    QByteArray dataTogether = (QString::number(index / 4)+QString::number(index % 4)+QString(buffer)).toUtf8()+previousHash.toHex();
    QFile file("record.txt");
    if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
        QTextStream out(&file);
        out << index / 4 << "\n";
        out << index % 4 << "\n";
        out << buffer << "\n";
        out << previousHash.toHex() << "\n";
        qDebug() << dataTogether << dataTogether.size();
    }
}

void MainWindow::readFile() {
    QFile file("record.txt");
    if (file.open(QIODevice::ReadOnly)) {
        clear();
        QTextStream in(&file);
        QByteArray prevHash = QByteArray();
        int move = 0;
        while (!in.atEnd()) {
            int i = in.readLine().toInt();
            int j = in.readLine().toInt();
            QString time = in.readLine();
            QByteArray hash = QByteArray::fromHex(in.readLine().toUtf8());
            QByteArray dataTogether = (QString::number(i)+QString::number(j)+time).toUtf8()+prevHash;
            QByteArray newhash = QCryptographicHash::hash((QString::number(i)+QString::number(j)+time).toUtf8()+prevHash, QCryptographicHash::Sha256);
            prevHash = newhash;
            qDebug() << hash << newhash;
            if (hash != newhash) {
                QMessageBox messageBox;
                messageBox.critical(0,"Error","Хеши ходов не совпадают");
                messageBox.setFixedSize(500,200);
                messageBox.show();
                return;
            } else {
                int index = i * 4 + j;
                checkboxes[index]->setChecked(true);
                checkboxes[index]->setCheckable(false);
                if (move % 2 == 0)
                    checkboxes[index]->setStyleSheet("background-color: #FF0000");
                else
                    checkboxes[index]->setStyleSheet("background-color: #00FF00");

            }
            move++;
        }
    }
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::clear() {
    for (int i = 0; i < checkboxes.size(); i++) {
        checkboxes[i]->setChecked(false);
        checkboxes[i]->setCheckable(true);
        checkboxes[i]->setStyleSheet("");
    }
    for (int i = 0; i < states.size(); i++)
        states[i] = 0;
    counter = 0;
}

void MainWindow::on_clear_clicked() {
    clear();
}

void MainWindow::on_load_clicked() {
    readFile();
}

