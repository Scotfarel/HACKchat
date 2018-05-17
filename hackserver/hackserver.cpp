#include "hackserver.h"
#include "ui_hackserver.h"

hackserver::hackserver(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::hackserver)
{
    ui->setupUi(this);

    QString ip_range = "(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])";
    QRegExp ip_regex("^" + ip_range + "\\." + ip_range + "\\." + ip_range + "\\." + ip_range + "$");
    QRegExpValidator* ip_validator = new QRegExpValidator(ip_regex, this);
    ui->address_line->setValidator(ip_validator);

    QIntValidator* intValidator = new QIntValidator(0, 9999, this);
    ui->port_line->setValidator(intValidator);
}

hackserver::~hackserver() {
    delete ui;
    for (auto& it : clients_map) {
        delete it.second;
        clients_map.erase(it.first);
    }
    delete tcpServ;
}

void hackserver::connect_new() {
    QTcpSocket* sock = tcpServ->nextPendingConnection();
    connect(sock, SIGNAL(readyRead()), this, SLOT(read()));
    connect(sock, SIGNAL(disconnected()), this, SLOT(disconnect()));
}

void hackserver::send_everyone_new(int id, std::string login) {
    ObjectDAO obj_dao;
    QVector<QString> friends = obj_dao.get_friends(id);
    PackageList list;
    Package* msg = list.add_pack();
    prepare_status_msg(msg, StatusMsg::CONNECTED, id, login);
    for (int i = 0; i < friends.size(); i++) {
        int friend_id = friends.at(i).toInt();
        if (clients_map.find(friend_id) == clients_map.end()) {
            continue;
        }
        msg->set_host_id(friend_id);
        QByteArray f_message(list.SerializeAsString().c_str(), list.ByteSize());
        clients_map[friend_id]->write(f_message, f_message.size());
    }
}

void hackserver::send_everyone_disconnected(int id) {
    PackageList list;
    Package* msg = list.add_pack();
    prepare_status_msg(msg, StatusMsg::DISCONNECTED, id);
    for (std::pair<int const, QTcpSocket*>& c : clients_map) {
        msg->set_host_id(c.first);
        QByteArray f_message(list.SerializeAsString().c_str(), list.ByteSize());
        c.second->write(f_message, f_message.size());
    }
}

void hackserver::read() {
    QTcpSocket* client = (QTcpSocket*)sender();
    QByteArray buffer;
    buffer.resize(client->bytesAvailable());
    client->read(buffer.data(), buffer.size());
    PackageList msg;
    if (!msg.ParseFromArray(buffer, buffer.size())) {
      qDebug() << "Failed to parse message.";
    }
    for (const Package& p : msg.pack()) {
        std::string text = std::to_string(p.sender_id());
        text.append(" : ");
        text.append(p.text_msg().msg_text());
        ui->plainTextEdit->appendPlainText(QString::fromStdString(text));
        if (p.host_id() != -1) {
            if (clients_map.find(p.host_id()) == clients_map.end()) {
                qDebug() << "User disconnected!";
                continue;
            }
            if (!p.text_msg().is_feature()) {
                ObjectDAO obj_dao;
                obj_dao.add_message(p.sender_id(), p.host_id(), QString::fromStdString(p.text_msg().msg_text()), QString::number(TimeUtil::TimestampToSeconds(p.text_msg().date())));
            }
            auto need_to = clients_map.find(p.host_id());
            need_to->second->write(buffer);
        } else {
            message_for_server(p, client);
        }
    }
}

void hackserver::message_for_server(const Package& p, QTcpSocket* client) {
    switch (p.status_msg().status()) {
    case StatusMsg::CONNECTED: {
        if (auth(p, client)) {
            client_number++;
        }
        break;
    }
    case StatusMsg::NEW_USER: {
        register_user(p, client);
        break;
    }
    case StatusMsg::SEARCH: {
        ObjectDAO search_obj;
        QMap<QString, QString> friends = search_obj.friend_search(QString::fromStdString(p.status_msg().user_login()));
        PackageList ans;
        Package* empty_pckg = ans.add_pack();
        empty_pckg->set_host_id(p.sender_id());
        QVector<QString> already_friends = search_obj.get_friends(p.sender_id());
        friends.remove(QString::number(p.sender_id()));
        for (auto f : friends.toStdMap()) {
            if (already_friends.contains(f.first)) {
                friends.remove(f.first);
            }
        }
        if (friends.isEmpty()) {
            prepare_status_msg(empty_pckg, StatusMsg::NOT_FOUND);
        } else {
            prepare_status_msg(empty_pckg, StatusMsg::SEARCH);
            for (auto& it : friends.toStdMap()) {
                Package* pckg = ans.add_pack();
                pckg->set_host_id(p.sender_id());
                prepare_status_msg(pckg, StatusMsg::SEARCH, it.first.toInt(), it.second.toStdString());
            }
        }
        QByteArray f_message(ans.SerializeAsString().c_str(), ans.ByteSize());
        client->write(f_message);
        break;
    }
    case StatusMsg::ADD: {
        ObjectDAO user;
        QMap<QString, QString> data = user.get_user_by_login(QString::fromStdString(p.status_msg().user_login()));
        int friend_id = data["id"].toInt();
        user.add_friend(p.sender_id(), friend_id);
        if (clients_map[friend_id]) {
            PackageList msg;
            Package* pck = msg.add_pack();
            pck->set_host_id(p.sender_id());
            prepare_status_msg(pck, StatusMsg::CONNECTED, friend_id, (p.status_msg().user_login()));
            QByteArray f_message(msg.SerializeAsString().c_str(), msg.ByteSize());
            client->write(f_message);

            pck->set_host_id(friend_id);
            data = user.get_user_by_id(p.sender_id());
            prepare_status_msg(pck, StatusMsg::CONNECTED, p.sender_id(), (data["login"].toStdString()));
            QByteArray f_message_second(msg.SerializeAsString().c_str(), msg.ByteSize());
            clients_map.at(friend_id)->write(f_message_second);
        }
        break;
    }
    default:
            break;
    }
}

void hackserver::register_user(const Package& msg, QTcpSocket* user) {
    QString login = QString::fromStdString(msg.status_msg().user_login());
    QByteArray password(msg.status_msg().user_pass().c_str(), msg.status_msg().user_pass().length());

    PackageList list;
    Package* answer = list.add_pack();
    answer->set_host_id(msg.sender_id());

    ObjectDAO obj_dao;
    QMap<QString, QString> users = obj_dao.get_user_by_login(login);
    if (!users.empty()) {
        prepare_status_msg(answer, StatusMsg::LOGIN_FOUND);
    } else {
        obj_dao.add_user(login, password);
        prepare_status_msg(answer, StatusMsg::NEW_USER);
    }
    QByteArray f_message(list.SerializeAsString().c_str(), list.ByteSize());
    user->write(f_message);
    return;
}

bool hackserver::auth(const Package& msg, QTcpSocket* user) {
    QString login = QString::fromStdString(msg.status_msg().user_login());
    QByteArray password(msg.status_msg().user_pass().c_str(), msg.status_msg().user_pass().length());

    ObjectDAO obj_dao;
    QMap<QString, QString> values = obj_dao.get_user_by_login(login);
    PackageList online_list;
    Package* answer = online_list.add_pack();
    answer->set_host_id(0);

    if (values.isEmpty() || QString::compare(values["password"], password)) {
        prepare_status_msg(answer, StatusMsg::AUTH_UNSUCCESS);
        QByteArray f_message(online_list.SerializeAsString().c_str(), online_list.ByteSize());
        user->write(f_message);
        return false;
    }

    int user_id = values["id"].toInt();
    prepare_status_msg(answer, StatusMsg::AUTH_SUCCESS, user_id, login.toStdString());
    ObjectDAO friend_list;
    QVector<QString> friends = friend_list.get_friends(user_id);
    for (int i = 0; i < friends.size(); i++) {
        int friend_id = friends.at(i).toInt();
        if (clients_map.find(friend_id) == clients_map.end()) {
            continue;
        }
        Package* user_online = online_list.add_pack();
        user_online->set_host_id(user_id);
        ObjectDAO obj_dao;
        QMap<QString, QString> values = obj_dao.get_user_by_id(friend_id);
        prepare_status_msg(user_online, StatusMsg::CONNECTED, friend_id, values["login"].toStdString());
    }

    QByteArray f_message(online_list.SerializeAsString().c_str(), online_list.ByteSize());
    user->write(f_message);
    send_everyone_new(user_id, login.toStdString());
    clients_map.emplace(user_id, user);
    ui->users->addItem(QString::number(user_id));
    return true;
}

void hackserver::disconnect() {
    QTcpSocket* disconnected_client = static_cast<QTcpSocket*>(sender());
    int num = -1;
    for (auto it = clients_map.begin(); it != clients_map.end(); ++it) {
      if (it->second == disconnected_client) {
          num = it->first;
      }
    }
    if (num == -1) {
        qDebug() << "Not online!";
        return;
    }
    for (int i = 0; i < ui->users->count(); i++) {
        if (QString::compare(ui->users->item(i)->text(), QString::number(num)) == 0) {
            delete ui->users->item(i);
        }
    }
    clients_map.erase(num);
    send_everyone_disconnected(num);
}

void hackserver::prepare_status_msg(Package* package, StatusMsg::Status status, int user_id, std::string user_login) {
    package->set_sender_id(-1);
    StatusMsg* status_msg = new StatusMsg;
    status_msg->set_status(status);
    status_msg->set_user_id(user_id);
    status_msg->set_user_login(user_login);
    package->set_allocated_status_msg(status_msg);
}

void hackserver::on_startButton_released()
{
    tcpServ = new QTcpServer(this);

    tcpServ->listen(QHostAddress(ui->address_line->text()), ui->port_line->text().toInt());
    connect(tcpServ, SIGNAL(newConnection()), this, SLOT(connect_new()));
    ui->stackedWidget->setCurrentIndex(1);

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("/home/kaito/hackchat/hackserver/hackdb.db3");
    if(!db.open()) {
        qDebug() << db.lastError().text();
    }
}
