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

void MainWindow::errorMessage(QString errorMessage) {
    QMessageBox messageBox;
    messageBox.critical(0, "Error", errorMessage);
    messageBox.setFixedSize(500,200);
    messageBox.show();
}

void MainWindow::setCell(int index) {
    QCheckBox* checkbox = checkboxes[index];
    checkbox->setCheckable(false);
    if (counter % 2 == 0)
        checkbox->setStyleSheet("background-color: #FF0000");
    else
        checkbox->setStyleSheet("background-color: #00FF00");
}

void MainWindow::checkbox_checked(bool checked) {
    if (!checked)
        return;
    QCheckBox* checkbox = static_cast<QCheckBox*>(sender());
    int index = checkboxes.indexOf(checkbox);
    setCell(index);
    counter++;
    saveMove(index);
}
QByteArray MainWindow::getHash(int i, int j, QString time, QByteArray hash) {
    return QCryptographicHash::hash(
        (QString::number(i)+QString::number(j)+time).toUtf8()+hash, QCryptographicHash::Sha256);
}

void MainWindow::saveMove(int index) {
    const auto now = std::chrono::system_clock::now();
    const std::time_t t_c = std::chrono::system_clock::to_time_t(now);
    std::tm* tm_c = std::localtime(&t_c);
    char buffer[20];
    std::strftime(buffer, sizeof(buffer), "%Y.%m.%d_%H:%M:%S", tm_c);
    previousHash = getHash(index / 4, index % 4, QString(buffer), previousHash);
    QByteArray data = readFile();
    qDebug() << "before" << data;
    data.append((QString::number(index/4)+"\n").toUtf8());
    data.append((QString::number(index%4)+"\n").toUtf8());
    data.append((QString(buffer)+"\n").toUtf8());
    data.append((QString(previousHash.toHex())+"\n").toUtf8());
    qDebug() << "after" << data;
    saveFile(data);
}

QByteArray MainWindow::readFile() {
    QFile file("record.txt");
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray hexEncData = file.readAll();
        QByteArray encData = QByteArray::fromHex(hexEncData);
        QByteArray unEncData;
        int errorcode = decryptByteArray(encData, unEncData);
        if (errorcode) {
            errorMessage("Произошла ошибка при расшифровке файла");
            return QByteArray();
        }
        return unEncData;
    }
    QMessageBox messageBox;
    errorMessage("Произошла ошибка при чтении файла");
    return QByteArray();
}

void MainWindow::loadFile(QByteArray &decryptedData) {
    QTextStream in(&decryptedData);
    QByteArray prevHash = QByteArray();
    while (!in.atEnd()) {
        int i = in.readLine().toInt();
        int j = in.readLine().toInt();
        QString time = in.readLine();
        QByteArray hash = QByteArray::fromHex(in.readLine().toUtf8());
        QByteArray newhash = getHash(i, j, time, prevHash);
        prevHash = newhash;
        if (hash != newhash) {
            errorMessage("Хеши ходов не совпадают");
            return;
        }
    }
    in.seek(0);
    clear();
    while (!in.atEnd()) {
        int i = in.readLine().toInt();
        int j = in.readLine().toInt();
        QString time = in.readLine();
        previousHash = QByteArray::fromHex(in.readLine().toUtf8());
        setCell(i*4+j);
        counter++;
    }
}

void MainWindow::saveFile(QByteArray &unencData) {
    QByteArray encData;
    int errorcode = encryptByteArray(unencData, encData);
    if (errorcode) {
        errorMessage("Произошла ошибка при сохранении файла");
        return;
    }
    QByteArray hexEncData = encData.toHex();
    QFile file("record.txt");
    if (!file.open(QIODevice::WriteOnly)) {
        errorMessage("Произошла ошибка при сохранении файла");
        return;
    }
    file.write(hexEncData);
    file.close();
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
    previousHash = QByteArray();
}

void MainWindow::on_clear_clicked() {
    clear();
}

void MainWindow::on_load_clicked() {
    QByteArray data = readFile();
    loadFile(data);
}

int MainWindow::encryptByteArray(QByteArray &inputBytes, QByteArray &outputBytes) {
    QByteArray key_qba = QByteArray::fromHex("b48d1d3c947c425b07b3de8bf6d96e84b48d1d3c947c425b07b3de8bf6d96e84");
    QByteArray iv_qba = QByteArray::fromHex("b48d1d3c947c425b07b3de8bf6d96e84");
    QDataStream decryptedStream(inputBytes);
    QDataStream encryptedStream(&outputBytes, QIODevice::ReadWrite);
    const int bufferLen = 256;
    int encryptedLen, decryptedLen;
    unsigned char key[32], iv[16];
    unsigned char encryptedBuffer[bufferLen] = {0}, decryptedBuffer[bufferLen] = {0};

    memcpy(key, key_qba.data(), 32);
    memcpy(iv, iv_qba.data(), 16);
    iv_qba.clear();

    EVP_CIPHER_CTX *ctx;
    ctx = EVP_CIPHER_CTX_new();
    if (!EVP_EncryptInit_ex2(ctx, EVP_aes_256_cbc(), key, iv, NULL)) {
        EVP_CIPHER_CTX_free(ctx);
        return 1;
    }
    do {
        decryptedLen = decryptedStream.readRawData(reinterpret_cast<char*>(decryptedBuffer), bufferLen);
        if (!EVP_EncryptUpdate(ctx, encryptedBuffer, &encryptedLen, decryptedBuffer, decryptedLen)) {
            EVP_CIPHER_CTX_free(ctx);
            return 2;
        }
        encryptedStream.writeRawData(reinterpret_cast<char*>(encryptedBuffer), encryptedLen);
    } while (decryptedLen > 0);
    if (!EVP_EncryptFinal_ex(ctx, encryptedBuffer, &encryptedLen)) {
        EVP_CIPHER_CTX_free(ctx);
        return 3;
    }
    encryptedStream.writeRawData(reinterpret_cast<char*>(encryptedBuffer), encryptedLen);
    EVP_CIPHER_CTX_free(ctx);
    return 0;
}

int MainWindow::decryptByteArray(QByteArray &inputBytes, QByteArray &outputBytes) {
    QByteArray key_qba = QByteArray::fromHex("b48d1d3c947c425b07b3de8bf6d96e84b48d1d3c947c425b07b3de8bf6d96e84");
    QByteArray iv_qba = QByteArray::fromHex("b48d1d3c947c425b07b3de8bf6d96e84");
    QDataStream encryptedStream(inputBytes);
    QDataStream decryptedStream(&outputBytes, QIODevice::ReadWrite);
    const int bufferLen = 256;
    int encryptedLen, decryptedLen, tmplen;
    unsigned char key[32], iv[16];
    unsigned char encryptedBuffer[bufferLen] = {0}, decryptedBuffer[bufferLen] = {0};

    memcpy(key, key_qba.data(), 32);
    memcpy(iv, iv_qba.data(), 16);
    iv_qba.clear();

    EVP_CIPHER_CTX *ctx;
    ctx = EVP_CIPHER_CTX_new();
    if (!EVP_DecryptInit_ex2(ctx, EVP_aes_256_cbc(), key, iv, NULL)) {
        EVP_CIPHER_CTX_free(ctx);
        return 1;
    }
    do {
        encryptedLen = encryptedStream.readRawData(reinterpret_cast<char*>(encryptedBuffer), bufferLen);
        if (!EVP_DecryptUpdate(ctx, decryptedBuffer, &decryptedLen, encryptedBuffer, encryptedLen)) {
            EVP_CIPHER_CTX_free(ctx);
            return 2;
        }
        decryptedStream.writeRawData(reinterpret_cast<char*>(decryptedBuffer), decryptedLen);
    } while (encryptedLen > 0);

    if (!EVP_DecryptFinal_ex(ctx, decryptedBuffer, &decryptedLen)) {
        EVP_CIPHER_CTX_free(ctx);
        return 3;
    }
    decryptedStream.writeRawData(reinterpret_cast<char*>(decryptedBuffer), decryptedLen);
    EVP_CIPHER_CTX_free(ctx);
    return 0;
}
