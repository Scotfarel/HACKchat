#include "client.h"
#include "ui_client.h"

Client::Client(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Client),
    id(0) // maybe we should change it
{
    ui->setupUi(this);
    ui->login_line->setFocus();
}

Client::~Client() {
    delete ui;
}

void Client::on_send_button_clicked() {
    if (ui->msg_edit->text().isEmpty()) {
        return;
    }
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
    text->set_msg_text(ui->msg_edit->text().toStdString());

    Timestamp* date = new Timestamp;
    size_t local_time = time(NULL) + 10800;
    date->set_seconds(local_time);
    date->set_nanos(0);
    text->set_allocated_date(date);
    msg->set_allocated_text_msg(text);
    QByteArray f_message(list.SerializeAsString().c_str(), list.ByteSize());
    tcpSock->write(f_message);
    show_msg(*msg);
    ui->msg_edit->clear();
}

void Client::msg_from_server(const Package& msg) { // switch case pls
    if (msg.status_msg().status() == StatusMsg::LOGIN_NOT_FOUND) {
        ui->msg_label->setText("Wrong login! Maybe register?");
        return;
    }
    if (msg.status_msg().status() == StatusMsg::WRONG_PASS) {
        ui->msg_label->setText("Wrong password!");
        return;
    }
    if (msg.status_msg().status() == StatusMsg::AUTH_SUCCESS) {
        nickname = ui->login_line->text();
        id = msg.status_msg().user_id();
        ui->stackedWidget->setCurrentIndex(1);
        this->setWindowTitle(QString::fromStdString(msg.status_msg().user_login()));
    }
    if (msg.status_msg().status() == StatusMsg::CONNECTED) {
        ui->online->addItem(QString::fromStdString(msg.status_msg().user_login()));
        users_online.insert((int)msg.status_msg().user_id(), QString::fromStdString(msg.status_msg().user_login()));
    }
    if (msg.status_msg().status() == StatusMsg::DISCONNECTED) {
        for (int i = 0; i < ui->online->count(); i++) {
            if (QString::compare(ui->online->item(i)->text(), users_online[msg.status_msg().user_id()]) == 0) {
                delete ui->online->item(i);
            }
        }
        users_online.remove(msg.status_msg().user_id());
    }
    if (msg.status_msg().status() == StatusMsg::SEARCH) {
        if (msg.status_msg().user_id() == 0) {
            ui->online->clear();
            return;
        }
        ui->online->addItem(QString::fromStdString(msg.status_msg().user_login()));
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
        if (p.has_text_msg() && p.text_msg().is_feature()) {
            ui->feature->setPlainText(QString::fromStdString(p.text_msg().msg_text()));
        } else {
            show_msg(p);
            ui->feature->clear();
        }
    }
}

void Client::on_msg_edit_textEdited(const QString &arg1)
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
    msg->set_allocated_text_msg(text);
    QByteArray f_message(list.SerializeAsString().c_str(), list.ByteSize());
    tcpSock->write(f_message);
}

void Client::on_log_in_button_pressed()
{
    if (ui->login_line->text().isEmpty()) {
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
    send_user_info(StatusMsg::CONNECTED);
}

void Client::send_user_info(StatusMsg::Status status) {
    PackageList list;
    Package* pckg = list.add_pack();
    pckg->set_sender_id(id);
    pckg->set_host_id(-1);
    StatusMsg* status_msg = new StatusMsg;
    status_msg->set_status(status);
    status_msg->set_user_id(id);
    status_msg->set_user_login(ui->login_line->text().toStdString());

    QByteArray pass = QCryptographicHash::hash(ui->password_line->text().toUtf8(), QCryptographicHash::Md5);
    status_msg->set_user_pass(pass);
    pckg->set_allocated_status_msg(status_msg);
    QByteArray f_message(list.SerializeAsString().c_str(), list.ByteSize());
    tcpSock->write(f_message);
}

void Client::on_login_line_textEdited()
{
    ui->msg_label->clear();
}

void Client::on_password_line_textEdited()
{
    ui->msg_label->clear();
}

void Client::on_msg_edit_returnPressed()
{
    ui->send_button->clicked();
}

void Client::on_sign_in_button_pressed()
{
    send_user_info(StatusMsg::NEW_USER);
}

void Client::show_msg(const Package& p) {
    QString first_str;
    if (p.sender_id() == id) {
        first_str = nickname;
    } else {
        first_str = users_online[p.sender_id()];
    }
    first_str.append(" (");
    std::string time = TimeUtil::ToString(p.text_msg().date());
    std::replace(time.begin(), time.end(), 'T', ' ');
    first_str.append(QString::fromStdString(time.substr(0, time.size()-1)));
    first_str.append("):");
    ui->messages->appendPlainText(first_str);
    ui->messages->appendPlainText(QString::fromStdString(p.text_msg().msg_text()));
}

void Client::on_search_line_textEdited(const QString &arg1)
{
    if (arg1.isEmpty()) {
        ui->online_label->setText("Friends online:");
        ui->online->clear();
        for (auto& u : users_online) {
            ui->online->addItem(u);
        }
        return;
    }
    ui->online_label->setText("Search result:");
    ui->online->clear();
    PackageList list;
    Package* pckg = list.add_pack();
    pckg->set_sender_id(id);
    pckg->set_host_id(-1);
    StatusMsg* status_msg = new StatusMsg;
    status_msg->set_status(StatusMsg::SEARCH);
    status_msg->set_user_login(arg1.toStdString());
    pckg->set_allocated_status_msg(status_msg);
    QByteArray f_message(list.SerializeAsString().c_str(), list.ByteSize());
    tcpSock->write(f_message);
}
