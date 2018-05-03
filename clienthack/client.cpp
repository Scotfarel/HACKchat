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
    Package msg;
    msg.set_sender_id(id);
    int send_to = -1;
    if (!ui->online->selectedItems().empty()) {
        send_to = ui->online->selectedItems().at(0)->text().toInt();
    }

    msg.set_host_id(send_to);
    TextMsg* text = new TextMsg;
    text->set_is_feature(false);
    text->set_msg_text(ui->lineEdit->text().toStdString());
    msg.set_allocated_text(text);
    QByteArray f_message(msg.SerializeAsString().c_str(), msg.ByteSize());
    tcpSock->write(f_message);
    ui->lineEdit->clear();
}

void Client::msg_from_server(const Package& msg) {
    qDebug() << "arrived" << msg.status().connected_id();
    if (msg.status().connected()) {
        ui->online->addItem(QString::number(msg.status().connected_id()));
    } else {
        for (int i = 0; i < ui->online->count(); i++) {
            if (QString::compare(ui->online->item(i)->text(), QString::number(msg.status().connected_id())) == 0) {
                delete ui->online->item(i);
            }
        }
    }
}

void Client::leer() {
    qDebug() << "meaasage!";
    QByteArray buffer;
    buffer.resize(tcpSock->bytesAvailable());
    tcpSock->read(buffer.data(), buffer.size());
    Package msg;
    if (!msg.ParseFromArray(buffer, buffer.size())) {
      qDebug() << "Failed to parse message.";
    }
    if (id == -1) {
        id = msg.host_id();
    }
    if (msg.sender_id() == -1) {
        msg_from_server(msg);
    }
    if (msg.has_text() && msg.text().is_feature()) {
        ui->feature->setPlainText(QString::fromStdString(msg.text().msg_text()));
    } else {
        ui->messages->appendPlainText(QString::fromStdString(msg.text().msg_text()));
    }
}

void Client::on_lineEdit_textEdited(const QString &arg1)
{
    Package msg;
    msg.set_sender_id(id);

    int send_to = -1;
    if (!ui->online->selectedItems().empty()) {
        send_to = ui->online->selectedItems().at(0)->text().toInt();
    }

    msg.set_host_id(send_to);
    TextMsg* text = new TextMsg;
    text->set_is_feature(true);
    text->set_msg_text(arg1.toStdString());
    msg.set_allocated_text(text);
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
