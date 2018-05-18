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
    ui->msg_edit->hide();
    ui->send_button->hide();
    ui->feature->hide();
    ui->line_2->hide();
    ui->line_6->hide();
    ui->line_11->hide();
    ui->line_10->hide();
    ui->line_8->hide();
    ui->line_5->hide();

    QString ip_range = "(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])";
    QRegExp ip_regex("^" + ip_range + "\\." + ip_range + "\\." + ip_range + "\\." + ip_range + "$");
    QRegExpValidator* ip_validator = new QRegExpValidator(ip_regex, this);
    ui->address_line->setValidator(ip_validator);

    QIntValidator* intValidator = new QIntValidator(0, 9999, this);
    ui->port_line->setValidator(intValidator);
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

void Client::msg_from_server(const Package& msg) {
    switch (msg.status_msg().status()) {
    case StatusMsg::AUTH_UNSUCCESS: {
        ui->msg_label->setText("Wrong password or login");
        break;
    }
    case StatusMsg::NEW_USER: {
        ui->msg_label->setText("Register success");
        break;
    }
    case StatusMsg::LOGIN_FOUND: {
        ui->msg_label->setText("Try another login");
        break;
    }
    case StatusMsg::AUTH_SUCCESS: {
        nickname = ui->login_line->text();
        id = msg.status_msg().user_id();
        ui->stackedWidget->setCurrentIndex(1);
        this->setWindowTitle(QString::fromStdString(msg.status_msg().user_login()));
        break;
    }
    case StatusMsg::CONNECTED: {
        users_online.insert((int)msg.status_msg().user_id(), QString::fromStdString(msg.status_msg().user_login()));
        if (!ui->online_label->text().compare(search_res)) {
            return;
        }
        QListWidgetItem* item = new QListWidgetItem(QString::fromStdString(msg.status_msg().user_login()));
        item->setBackgroundColor(Qt::black);
        ui->online->addItem(item);
        break;
    }
    case StatusMsg::DISCONNECTED: {
        for (int i = 0; i < ui->online->count(); i++) {
            if (QString::compare(ui->online->item(i)->text(), users_online[msg.status_msg().user_id()]) == 0) {
                delete ui->online->item(i);
            }
        }
        users_online.remove(msg.status_msg().user_id());
        break;
    }
    case StatusMsg::NOT_FOUND: {
        ui->online_label->setText("Users not found");
        break;
    }
    case StatusMsg::SEARCH: {
        if (msg.status_msg().user_id() == 0) {
            ui->online->clear();
            return;
        }
        QListWidgetItem* item = new QListWidgetItem(QString::fromStdString(msg.status_msg().user_login()));
        item->setBackgroundColor(Qt::black);
        ui->online->addItem(item);
        break;
    }
    default:
        break;
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
            return;
        }
    }
}

void Client::on_msg_edit_textChanged(const QString &arg1) {
    if (!arg1.compare("BOMB")) {
        QPixmap      pix;
        pix.load("/home/kaito/final_hack/hackchat/clienthack/design.jpg");
        QPalette pal;
        pal.setBrush(this->backgroundRole(), QBrush(pix));
        this->setPalette(pal);
        this->setAutoFillBackground(true);
        ui->online->addItem("Майор");
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
    prepare_text_msg(msg, true, arg1.toStdString());
    QByteArray f_message(list.SerializeAsString().c_str(), list.ByteSize());
    tcpSock->write(f_message);
}

void Client::on_log_in_button_pressed() {
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
    QByteArray salt = QCryptographicHash::hash("is_it+salt", QCryptographicHash::Md5).toHex();
    Package* pckg = list.add_pack();
    QByteArray pass = QCryptographicHash::hash(ui->password_line->text().toUtf8(), QCryptographicHash::Md5);
    std::string result = QCryptographicHash::hash(salt+pass, QCryptographicHash::Md5).toStdString();
    prepare_status_msg(pckg, status, id, ui->login_line->text().toStdString(), result);

    QByteArray f_message(list.SerializeAsString().c_str(), list.ByteSize());
    tcpSock->write(f_message);
}

void Client::on_login_line_textEdited() {
    ui->msg_label->clear();
}

void Client::on_password_line_textEdited() {
    ui->msg_label->clear();
}

void Client::on_msg_edit_returnPressed() {
    ui->send_button->clicked();
}

void Client::on_sign_in_button_pressed() {
    if (ui->password_line->text().length() < 5) {
        ui->msg_label->setText("Too short password!");
        return;
    }
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
        if (ui->online->selectedItems().at(0)->text().compare("Майор")) {
            first_str.append(users_online[p.host_id()]);
        } else {
            first_str.append("Майор");
        }
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

void Client::on_search_line_textChanged(const QString &arg1) {
    if (arg1.isEmpty()) {
        ui->online_label->clear();
        ui->online_label->setText("Friends online:");
        ui->online->clear();
        for (auto& u : users_online) {
            QListWidgetItem* item = new QListWidgetItem(u);
            item->setBackgroundColor(Qt::black);
            ui->online->addItem(item);
        }
        return;
    }
    ui->online_label->setText(search_res);
    ui->online->clear();
    PackageList list;
    Package* pckg = list.add_pack();
    prepare_status_msg(pckg, StatusMsg::SEARCH, 0, arg1.toStdString());
    QByteArray f_message(list.SerializeAsString().c_str(), list.ByteSize());
    tcpSock->write(f_message);
}

void Client::on_online_itemDoubleClicked(QListWidgetItem *item) {
    if (ui->search_line->text().isEmpty()) {
        return;
    }
    PackageList msg;
    Package* pckg = msg.add_pack();
    prepare_status_msg(pckg, StatusMsg::ADD, 0, item->text().toStdString());
    QByteArray f_message(msg.SerializeAsString().c_str(), msg.ByteSize());
    tcpSock->write(f_message);
    delete item;
}

void Client::on_online_itemSelectionChanged() {
    ui->feature->clear();
    if (ui->online->selectedItems().count() == 0) {
        ui->messages->hide();
        ui->msg_edit->hide();
        ui->send_button->hide();
        ui->feature->hide();
        ui->line_2->hide();
        ui->line_6->hide();
        ui->line_11->hide();
        ui->line_10->hide();
        ui->line_8->hide();
        ui->line_5->hide();
    }
    if (ui->online->selectedItems().count() == 1 && ui->online_label->text().compare(search_res) != 0) {
        ui->messages->show();
        ui->msg_edit->show();
        ui->send_button->show();
        ui->feature->show();
        ui->line_2->show();
        ui->line_6->show();
        ui->line_11->show();
        ui->line_10->show();
        ui->line_8->show();
        ui->line_5->show();
    }
}

void Client::disconnect() {
    connected = false;
    users_online.clear();
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

void Client::on_log_out_button_released() {
    ui->msg_edit->clear();
    ui->search_line->clear();
    ui->online->clear();
    users_online.clear();
    id = 0;
    tcpSock->disconnectFromHost();
    connected = false;
    ui->msg_label->clear();
}

void Client::on_online_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    if (!previous) {
        return;
    }
    if (ui->msg_edit->text().isEmpty()) {
        return;
    }
    PackageList msg;
    Package* package = msg.add_pack();
    for (auto& u : users_online.toStdMap()) {
        if (!QString::compare(u.second, previous->text())) {
         package->set_host_id(u.first);
         break;
        }
    }
    prepare_text_msg(package, true, "");
    QByteArray f_message(msg.SerializeAsString().c_str(), msg.ByteSize());
    tcpSock->write(f_message);
    ui->msg_edit->clear();
}
