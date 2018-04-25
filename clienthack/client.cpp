#include "client.h"
#include "ui_client.h"

Client::Client(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Client),
    id(-1) // maybe we should change it
{
    ui->setupUi(this);
}

Client::~Client() {
    delete ui;
}

void Client::on_pushButton_clicked() {
    FromClient msg;
    msg.set_sender_id(id);
    int send_to = -1;
    if (!ui->online->selectedItems().empty()) {
        send_to = ui->online->selectedItems().at(0)->text().toInt();
    }

    msg.set_host_id(send_to);
    msg.set_is_feature(false);
    msg.set_msg_text(ui->lineEdit->text().toStdString());
    QByteArray f_message(msg.SerializeAsString().c_str(), msg.ByteSize());
    tcpSock->write(f_message);
    ui->lineEdit->clear();
}

void Client::first_msg(const FromClient& msg) {
    id = msg.host_id();
    ui->messages->appendPlainText(QString::number(id));
    QStringList online = QString::fromStdString(msg.msg_text()).split(' ', QString::SkipEmptyParts);
    for (auto o : online) {
        ui->online->addItem(o);
    }
}

void Client::leer() {
    QByteArray buffer;
    buffer.resize(tcpSock->bytesAvailable());
    tcpSock->read(buffer.data(), buffer.size());
    FromClient msg;
    if (!msg.ParseFromArray(buffer, buffer.size())) {
      qDebug() << "Failed to parse message.";
    }
    if (id == -1) {
        first_msg(msg);
        return;
    }
    if (msg.sender_id() == -1) {
        ui->online->addItem(QString::fromStdString(msg.msg_text()));
        return;
    }
    if (msg.is_feature()) {
        ui->feature->setPlainText(QString::fromStdString(msg.msg_text()));
    } else {
        ui->messages->appendPlainText(QString::fromStdString(msg.msg_text()));
    }
}

void Client::on_lineEdit_textEdited(const QString &arg1)
{
    FromClient msg;
    msg.set_sender_id(id);

    int send_to = -1;
    if (!ui->online->selectedItems().empty()) {
        send_to = ui->online->selectedItems().at(0)->text().toInt();
    }

    msg.set_host_id(send_to);
    msg.set_is_feature(true);
    msg.set_msg_text(arg1.toStdString());
    QByteArray f_message(msg.SerializeAsString().c_str(), msg.ByteSize());
    tcpSock->write(f_message);
}

void Client::on_pushButton_2_pressed()
{
    ui->stackedWidget->setCurrentIndex(1);
    tcpSock = new QTcpSocket(this);
    tcpSock->connectToHost(QHostAddress(ui->address_line->text()), ui->port_line->text().toInt());
    connect(tcpSock, SIGNAL(readyRead()), this, SLOT(leer()));
}
