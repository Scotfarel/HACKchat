#ifndef CLIENT_H
#define CLIENT_H

#include <time.h>
#include <google/protobuf/message.h>
#include <google/protobuf/util/time_util.h>
#include <QMainWindow>
#include <QTcpSocket>
#include <QHostAddress>
#include <QCryptographicHash>
#include <QListWidgetItem>

#include "../proto/hackmessage.pb.h"

using hackchat::Package;
using hackchat::TextMsg;
using hackchat::StatusMsg;
using hackchat::PackageList;
using google::protobuf::Timestamp;
using google::protobuf::util::TimeUtil;

namespace Ui {
class Client;
}

class Client : public QMainWindow
{
    Q_OBJECT

public:
    explicit Client(QWidget *parent = 0);
    ~Client();

public slots:
    void leer();
    void disconnect();

private:
    Ui::Client *ui;
    QTcpSocket *tcpSock;
    QMap<int, QString> users_online;
    QString nickname;
    bool connected = false;
    int id;
    void msg_from_server(const Package& msg);
    void send_user_info(StatusMsg::Status status);
    void show_msg(const Package& p);
    void first_connect();
    void prepare_text_msg(Package* package, bool is_feature, std::string msg_text);
    void prepare_status_msg(Package* package, StatusMsg::Status status, int user_id, std::string user_login, std::string hash = "");
private slots:
    void on_send_button_clicked();
    void on_msg_edit_textEdited(const QString &arg1);
    void on_log_in_button_pressed();
    void on_login_line_textEdited();
    void on_password_line_textEdited();
    void on_msg_edit_returnPressed();
    void on_sign_in_button_pressed();
    void on_search_line_textEdited(const QString &arg1);
    void on_online_itemDoubleClicked(QListWidgetItem *item);
    void on_online_itemSelectionChanged();
};

#endif // CLIENT_H
