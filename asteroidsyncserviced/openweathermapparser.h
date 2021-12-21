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

    void prepareData(QJsonObject obj);
    QList<short> getWeatherId();
    QList<short> getTempMin();
    QList<short> getTempMax();
    QString getCity();

private:
    void setWeatherId(const QJsonArray weather, const QTime time);
    void setMinTemp(const QJsonObject obj, const QDate date);
    void setMaxTemp(const QJsonObject obj, const QDate date);
    void setCity(const QJsonObject obj);

    QList<short> m_weatherId;
    QList<short> m_tempMin;
    QList<short> m_tempMax;
    QDate m_dateTempMin;
    QDate m_dateTempMax;
    QString m_city;
};

#endif // OPENWEATHERMAPPARSER_H
