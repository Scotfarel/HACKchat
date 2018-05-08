#ifndef USERBUILDER_H
#define USERBUILDER_H

#include <QString>

class UserBuilder {
public:
    UserBuilder() {}
    QString get(int id) {
        QString query = QString("SELECT * FROM users WHERE rowid = %1").arg(QString::number(id));
        return query;
    }

    QString insert(QVector<QString> values) {
        QString query = QString("INSERT INTO users VALUES ('%1', '%2')").arg(values[0], values[1]);
        qDebug() << query;
        return query;
    }

    QString by_log(QString login) {
        QString query = QString("SELECT rowid, p_hash FROM users WHERE login = '%1'").arg(login);
        return query;
    }
};

#endif // USERBUILDER_H
