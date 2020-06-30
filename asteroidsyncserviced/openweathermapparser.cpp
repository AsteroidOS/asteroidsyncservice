#include "openweathermapparser.h"

OpenWeatherMapParser::OpenWeatherMapParser(QObject *parent) : QObject(parent)
{
}

QList<short> OpenWeatherMapParser::getWeatherId(QJsonArray list)
{
    QList<short> id;
    QDateTime timestamp;

    for(int i = 0; i < 5; i++) {
        QJsonObject dayInfo = list.takeAt(i).toObject();
        QJsonArray dayWeather = dayInfo.value("weather").toArray();
        QJsonObject firstEntry = dayWeather.takeAt(0).toObject();
        id << firstEntry.value("id").toInt();
    }

    return id;
}

QList<short> OpenWeatherMapParser::getTempMin(QJsonArray list)
{
    QList<short> tempMin;
    QDateTime timestamp;
    QJsonObject dayInfo = list.at(0).toObject();
    timestamp.setTime_t(dayInfo.value("dt").toInt());
    QString actDate = timestamp.toString("dd.MM.yyyy");
    QJsonObject dayMain = dayInfo.value("main").toObject();
    short int tmp = dayMain.value("temp_min").toDouble();
    
    for(int i = 1; i < list.size(); i++) {
        dayInfo = list.at(i).toObject();
        timestamp.setTime_t(dayInfo.value("dt").toInt());
        dayMain = dayInfo.value("main").toObject();
        if(actDate == timestamp.toString("dd.MM.yyyy") && dayMain.value("temp_min").toDouble() < tmp) {
            tmp = dayMain.value("temp_min").toDouble();
        } else if(actDate != timestamp.toString("dd.MM.yyyy")) {
            tempMin << tmp;
            actDate = timestamp.toString("dd.MM.yyyy");
            tmp = dayMain.value("temp_min").toDouble();
        }
    }

    return tempMin;
}

QList<short> OpenWeatherMapParser::getTempMax(QJsonArray list)
{
    QList<short> tempMax;
    QDateTime timestamp;
    QJsonObject dayInfo = list.at(0).toObject();
    timestamp.setTime_t(dayInfo.value("dt").toInt());
    QString actDate = timestamp.toString("dd.MM.yyyy");
    QJsonObject dayMain = dayInfo.value("main").toObject();
    short int tmp = dayMain.value("temp_max").toDouble();
    
    for(int i = 1; i < list.size(); i++) {
        dayInfo = list.at(i).toObject();
        timestamp.setTime_t(dayInfo.value("dt").toInt());
        dayMain = dayInfo.value("main").toObject();
        if(actDate == timestamp.toString("dd.MM.yyyy") && dayMain.value("temp_max").toDouble() > tmp) {
            tmp = dayMain.value("temp_max").toDouble();
        } else if(actDate != timestamp.toString("dd.MM.yyyy")) {
            tempMax << tmp;
            actDate = timestamp.toString("dd.MM.yyyy");
            tmp = dayMain.value("temp_max").toDouble();
        }
    }

    return tempMax;
}

QString OpenWeatherMapParser::getCity(QJsonObject obj)
{
    return obj.value("name").toString().trimmed();
}
