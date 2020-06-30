#ifndef OPENWEATHERMAPPARSER_H
#define OPENWEATHERMAPPARSER_H

#include <QObject>
#include <QJsonObject>
#include <QList>
#include <QJsonArray>
#include <QDateTime>

class OpenWeatherMapParser : public QObject
{
    Q_OBJECT

public:
    OpenWeatherMapParser(QObject *parent = nullptr);

    QList<short> getWeatherId(QJsonArray list);
    QList<short> getTempMin(QJsonArray list);
    QList<short> getTempMax(QJsonArray list);
    QString getCity(const QJsonObject obj);
};

#endif // OPENWEATHERMAPPARSER_H
