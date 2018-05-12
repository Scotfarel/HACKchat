#include "client.h"
#include "ui_client.h"

Client::Client(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Client),
    id(0) // maybe we should change it
{
    ui->setupUi(this);
    ui->login_line->setFocus();

    ui->messages->hide();
    ui->messages_label->hide();
    ui->msg_edit->hide();
    ui->send_button->hide();
    ui->feature->hide();

    ui->search_line->setPlaceholderText("Friend search");
}

Client::~Client() {
    delete tcpSock;
    delete ui;
}

void Client::on_send_button_clicked() {
    if (ui->msg_edit->text().isEmpty()) {
        return;
    }
    PackageList list;
    Package* msg = list.add_pack();
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
    prepare_text_msg(msg, false, ui->msg_edit->text().toStdString());

    QByteArray f_message(list.SerializeAsString().c_str(), list.ByteSize());
    tcpSock->write(f_message);
    show_msg(*msg);
    ui->msg_edit->clear();
}

void Client::msg_from_server(const Package& msg) { // switch case pls
    if (msg.status_msg().status() == StatusMsg::AUTH_UNSUCCESS) {
        ui->msg_label->setText("Wrong password or login");
        return;
    }
    if (msg.status_msg().status() == StatusMsg::NEW_USER) {
        ui->msg_label->setText("Register success");
        return;
    }
    if (msg.status_msg().status() == StatusMsg::LOGIN_FOUND) {
        ui->msg_label->setText("Try another login");
        return;
    }
    if (msg.status_msg().status() == StatusMsg::AUTH_SUCCESS) {
        nickname = ui->login_line->text();
        id = msg.status_msg().user_id();
        ui->stackedWidget->setCurrentIndex(1);
        this->setWindowTitle(QString::fromStdString(msg.status_msg().user_login()));
    }
    if (msg.status_msg().status() == StatusMsg::CONNECTED) {
        users_online.insert((int)msg.status_msg().user_id(), QString::fromStdString(msg.status_msg().user_login()));
        if (!ui->online_label->text().compare("Search result:")) {
            return;
        }
        ui->online->addItem(QString::fromStdString(msg.status_msg().user_login()));
    }
    if (msg.status_msg().status() == StatusMsg::DISCONNECTED) {
        for (int i = 0; i < ui->online->count(); i++) {
            if (QString::compare(ui->online->item(i)->text(), users_online[msg.status_msg().user_id()]) == 0) {
                delete ui->online->item(i);
            }
        }
        users_online.remove(msg.status_msg().user_id());
    }
    if (msg.status_msg().status() == StatusMsg::NOT_FOUND) {
        ui->info_label->setText("Users not found");
    }
    if (msg.status_msg().status() == StatusMsg::SEARCH) {
        if (msg.status_msg().user_id() == 0) {
            ui->online->clear();
            return;
        }
        ui->info_label->clear();
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
            if (ui->online->selectedItems().empty() || QString::compare(ui->online->currentItem()->text(), users_online[p.sender_id()])) {
                return;
            }
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
    prepare_text_msg(msg, true, arg1.toStdString());
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

    if (!connected) {
        if(!first_connect()) {
            return;
        }
    }
    send_user_info(StatusMsg::CONNECTED);
}

void Client::send_user_info(StatusMsg::Status status) {
    PackageList list;
    Package* pckg = list.add_pack();
    std::string pass = QCryptographicHash::hash(ui->password_line->text().toUtf8(), QCryptographicHash::Md5).toStdString();
    prepare_status_msg(pckg, status, id, ui->login_line->text().toStdString(), pass);

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
    if (!connected) {
        if(!first_connect()) {
            return;
        }
    }
    send_user_info(StatusMsg::NEW_USER);
}

void Client::show_msg(const Package& p) {
    QString first_str;
    if (p.sender_id() == id) {
        first_str = nickname;
        first_str.append(" to ");
        first_str.append(users_online[p.host_id()]);
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
        ui->info_label->clear();
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
    prepare_status_msg(pckg, StatusMsg::SEARCH, 0, arg1.toStdString());
    QByteArray f_message(list.SerializeAsString().c_str(), list.ByteSize());
    tcpSock->write(f_message);
}

void Client::on_online_itemDoubleClicked(QListWidgetItem *item)
{
    PackageList msg;
    Package* pckg = msg.add_pack();
    prepare_status_msg(pckg, StatusMsg::ADD, 0, item->text().toStdString());
    QByteArray f_message(msg.SerializeAsString().c_str(), msg.ByteSize());
    tcpSock->write(f_message);
    delete item;
}

void Client::on_online_itemSelectionChanged()
{
    if (ui->online->selectedItems().count() == 0) {
        ui->messages->hide();
        ui->messages_label->hide();
        ui->msg_edit->hide();
        ui->send_button->hide();
        ui->feature->hide();
    }
    if (ui->online->selectedItems().count() == 1) {
        ui->messages->show();
        ui->messages_label->show();
        ui->msg_edit->show();
        ui->send_button->show();
        ui->feature->show();
    }
}

void Client::disconnect() {
    ui->stackedWidget->setCurrentIndex(0);
    ui->msg_label->setText("Server shut down :(");
}

bool Client::first_connect() {
    tcpSock = new QTcpSocket(this);
    tcpSock->connectToHost(QHostAddress(ui->address_line->text()), ui->port_line->text().toInt());
    connect(tcpSock, SIGNAL(readyRead()), this, SLOT(leer()));
    connect(tcpSock, SIGNAL(disconnected()), this, SLOT(disconnect()));
    if (!tcpSock->waitForConnected()) {
        ui->msg_label->setText("Server problems!");
        delete tcpSock;
        return false;
    }
    connected = true;
    return true;
}

void Client::prepare_text_msg(Package* package, bool is_feature, std::string msg_text) {
    package->set_sender_id(id);
    TextMsg* text = new TextMsg;
    text->set_is_feature(is_feature);
    text->set_msg_text(msg_text);
    if (!is_feature) {
        Timestamp* date = new Timestamp;
        size_t local_time = time(NULL) + 10800;
        date->set_seconds(local_time);
        date->set_nanos(0);
        text->set_allocated_date(date);
    }
    package->set_allocated_text_msg(text);
}

void Client::prepare_status_msg(Package* package, StatusMsg::Status status, int user_id, std::string user_login, std::string hash) {
    package->set_sender_id(id);
    package->set_host_id(-1);
    StatusMsg* status_msg = new StatusMsg;
    status_msg->set_status(status);
    status_msg->set_user_id(user_id);
    status_msg->set_user_login(user_login);
    if (hash != "") {
        status_msg->set_user_pass(hash);
    }
    package->set_allocated_status_msg(status_msg);
}

void Client::on_log_out_button_released()
{
    users_online.clear();
    id = 0;
    tcpSock->disconnectFromHost();
    connected = false;
    ui->msg_label->clear();
}
