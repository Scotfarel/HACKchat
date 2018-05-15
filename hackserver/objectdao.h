#ifndef OBJECTDAO_H
#define OBJECTDAO_H

#include <QtSql/QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMap>
#include <QVector>

class ObjectDAO {
private:
    QString user_table = "users";
    QString friends_table = "friends";
    QString message_table = "messages";
    QString user_id_col = "rowid";
    QString login_col = "login";
    QString pass_col = "p_hash";
    QSqlQuery query;
    QMap<QString, QString> values;
    QVector<QString> vector_values;
    void execute();
    bool move();
public:
    QMap<QString, QString> get_user_by_id(int id);
    QMap<QString, QString> get_user_by_login(QString login);
    void add_user(QString login, QString pass);
    QMap<QString, QString> friend_search(QString searchStr);
    QVector<QString> get_friends(int id);
    void add_friend(int id1, int id2);
    void add_message(int sender_id, int host_id, QString text, QString time);
};

#endif // OBJECTDAO_H
