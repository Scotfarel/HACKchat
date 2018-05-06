#include "client.h"
#include "ui_client.h"

Client::Client(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Client),
    id(0) // maybe we should change it
{
    ui->setupUi(this);
    ui->nickname_line->setFocus();
}

Client::~Client() {
    delete ui;
}

void Client::on_pushButton_clicked() {
    PackageList list;
    Package* msg = list.add_pack();
    msg->set_sender_id(id);
    int send_to = -1;
    if (!ui->online->selectedItems().empty()) {
        for (auto user : users_online.toStdMap()) {
            if (QString::compare(ui->online->selectedItems().at(0)->text(), user.second) == 0) {
                send_to = user.first;
                break;
            }
        }
    }

    msg->set_host_id(send_to);
    TextMsg* text = new TextMsg;
    text->set_is_feature(false);
    text->set_msg_text(ui->lineEdit->text().toStdString());
    msg->set_allocated_text(text);
    QByteArray f_message(list.SerializeAsString().c_str(), list.ByteSize());
    tcpSock->write(f_message);
    ui->lineEdit->clear();
}

void Client::msg_from_server(const Package& msg) {
    if (msg.host_id() == 0) {
        ui->msg_label->setText("Wrong login or password!");
        return;
    }
    if (!msg.has_status() && !msg.has_text() && id == 0) {
        id = msg.host_id();
        ui->stackedWidget->setCurrentIndex(1);
        this->setWindowTitle(ui->nickname_line->text());
    }
    if (msg.status().connected()) {
        ui->online->addItem(QString::fromStdString(msg.status().connected_login()));
        users_online.insert((int)msg.status().connected_id(), QString::fromStdString(msg.status().connected_login()));
    } else {
        for (int i = 0; i < ui->online->count(); i++) {
            if (QString::compare(ui->online->item(i)->text(), users_online[msg.status().connected_id()]) == 0) {
                delete ui->online->item(i);
            }
        }
        users_online.remove(msg.status().connected_id());
    }
}

void Client::leer() {
    QByteArray buffer;
    buffer.resize(tcpSock->bytesAvailable());
    tcpSock->read(buffer.data(), buffer.size());
    PackageList msg;
    if (!msg.ParseFromArray(buffer, buffer.size())) {
      qDebug() << "Failed to parse message.";
    }
    for (const Package& p : msg.pack()) {
        if (p.sender_id() == -1) {
            msg_from_server(p);
            continue;
        }
        if (p.has_text() && p.text().is_feature()) {
            ui->feature->setPlainText(QString::fromStdString(p.text().msg_text()));
        } else {
            ui->messages->appendPlainText(QString::fromStdString(p.text().msg_text()));
            ui->feature->clear();
        }
    }
}

void Client::on_lineEdit_textEdited(const QString &arg1)
{
    PackageList list;
    Package* msg = list.add_pack();
    msg->set_sender_id(id);

    int send_to = -1;
    if (!ui->online->selectedItems().empty()) {
        for (auto user : users_online.toStdMap()) {
            if (QString::compare(ui->online->selectedItems().at(0)->text(), user.second) == 0) {
                send_to = user.first;
                break;
            }
        }
    }

    msg->set_host_id(send_to);
    TextMsg* text = new TextMsg;
    text->set_is_feature(true);
    text->set_msg_text(arg1.toStdString());
    msg->set_allocated_text(text);
    QByteArray f_message(list.SerializeAsString().c_str(), list.ByteSize());
    tcpSock->write(f_message);
}

void Client::on_pushButton_2_pressed()
{
    if (ui->nickname_line->text().isEmpty()) {
        ui->msg_label->setText("Login field is empty!");
        return;
    }
    if (ui->password_line->text().isEmpty()) {
        ui->msg_label->setText("Password field is empty!");
        return;
    }

    if (first_connect == false) {
        tcpSock = new QTcpSocket(this);
        tcpSock->connectToHost(QHostAddress(ui->address_line->text()), ui->port_line->text().toInt());
        connect(tcpSock, SIGNAL(readyRead()), this, SLOT(leer()));
        first_connect = true;
    }
    send_user_info();
}

void Client::send_user_info() {
    PackageList list;
    Package* msg = list.add_pack();
    msg->set_sender_id(id);
    msg->set_host_id(-1);
    TextMsg* text = new TextMsg;
    text->set_is_feature(false);
    QString user_info(ui->nickname_line->text());
    user_info.append(' ');
    user_info.append(ui->password_line->text());
    text->set_msg_text(user_info.toStdString());
    msg->set_allocated_text(text);
    QByteArray f_message(list.SerializeAsString().c_str(), list.ByteSize());
    tcpSock->write(f_message);
}

void Client::on_nickname_line_textEdited()
{
    ui->msg_label->clear();
}

void Client::on_password_line_textEdited()
{
    ui->msg_label->clear();
}

void Client::on_lineEdit_returnPressed()
{
    ui->pushButton->clicked();
}
