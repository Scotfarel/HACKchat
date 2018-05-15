#ifndef USERBUILDER_H
#define USERBUILDER_H

#include <QString>

class UserBuilder {
private:
    QString user_table = "users";
    QString friends_table = "friends";
    QString id_col = "rowid";
    QString login_col = "login";
    QString pass_col = "p_hash";
public:
    UserBuilder() {}
    QString get(int id) {
        QString query = QString("SELECT * FROM users WHERE rowid = %1").arg(QString::number(id));
        return query;
    }

    QString insert(QVector<QString> values) {
        QString query = QString("INSERT INTO users VALUES ('%1', '%2')").arg(values[0], values[1]);
        return query;
    }

    QString by_log(QString login) {
        QString query = QString("SELECT rowid, p_hash FROM users WHERE login = '%1'").arg(login);
        return query;
    }

    QString get_first_friends(int id) {
        QString query = QString("SELECT user_1, user_2 FROM friends WHERE user_1 = '%1'").arg(id); // OR didn't work, idk
        return query;
    }

    QString get_second_friends(int id) {
        QString query = QString("SELECT user_1, user_2 FROM friends WHERE user_2 = '%1'").arg(id); // OR didn't work, idk
        return query;
    }

    QString friend_search(QString searchStr) {
        QString query = QString("SELECT rowid, login FROM users WHERE login LIKE '%%1%'").arg(searchStr);

        return query;
    }

    QString add_friend(int id1, int id2) {
        QString query = QString("INSERT INTO friends VALUES ('%1', '%2')").arg(QString::number(id1), QString::number(id2));
        return query;
    }
};

#endif // USERBUILDER_H
