#include "openweathermapparser.h"

OpenWeatherMapParser::OpenWeatherMapParser(QObject *parent) : QObject(parent)
{
}

void OpenWeatherMapParser::prepareData(QJsonObject obj)
{
    m_weatherId.clear();
    m_tempMin.clear();
    m_tempMax.clear();
    m_dateTempMin.setDate(0, 0, 0);
    m_dateTempMax.setDate(0, 0, 0);

    QDateTime timestamp;
    QJsonArray list = obj.value("list").toArray();

    for(auto element : list) {
        QJsonObject dayInfo = element.toObject();
        timestamp.setTime_t(dayInfo.value("dt").toInt());

        setWeatherId(dayInfo.value("weather").toArray(), timestamp.time());
        setMinTemp(dayInfo.value("main").toObject(), timestamp.date());
        setMaxTemp(dayInfo.value("main").toObject(), timestamp.date());
    }

    setCity(obj.value("city").toObject());
}

void OpenWeatherMapParser::setWeatherId(QJsonArray weather, QTime time)
{
    QTime elevenoclock, thirteenoclock;
    elevenoclock.setHMS(11,0,0,0);
    thirteenoclock.setHMS(13,0,0,0);

    if(time >= elevenoclock && time <= thirteenoclock) {
        QJsonObject firstEntry = weather.first().toObject();
        m_weatherId << firstEntry.value("id").toInt();
    }
}

QList<short> OpenWeatherMapParser::getWeatherId()
{
    return m_weatherId;
}

void OpenWeatherMapParser::setMinTemp(const QJsonObject obj, const QDate date)
{
    if(m_tempMin.isEmpty()) {
        m_tempMin << obj.value("temp_min").toDouble();
        m_dateTempMin = date;
        return;
    }

    if(m_dateTempMin == date && obj.value("temp_min").toDouble() < m_tempMin[m_tempMin.size() - 1]) {
        m_tempMin.replace(m_tempMin.size() - 1, obj.value("temp_min").toDouble());
    } else if(m_dateTempMin != date) {
        if(m_tempMin.size() >= 5) return;

        m_tempMin << obj.value("temp_min").toDouble();
        m_dateTempMin = date;
    }
}

QList<short> OpenWeatherMapParser::getTempMin()
{
    return m_tempMin;
}

void OpenWeatherMapParser::setMaxTemp(const QJsonObject obj, const QDate date)
{
    if(m_tempMax.isEmpty()) {
        m_tempMax << obj.value("temp_max").toDouble();
        m_dateTempMax = date;
        return;
    }

    if(m_dateTempMax == date && obj.value("temp_max").toDouble() > m_tempMax[m_tempMax.size() - 1]) {
        m_tempMax.replace(m_tempMax.size() - 1, obj.value("temp_max").toDouble());
    } else if(m_dateTempMax != date) {
        if(m_tempMax.size() >= 5) return;

        m_tempMax << obj.value("temp_max").toDouble();
        m_dateTempMax = date;
    }
}

QList<short> OpenWeatherMapParser::getTempMax()
{
    return m_tempMax;
}

void OpenWeatherMapParser::setCity(const QJsonObject obj)
{
    m_city = obj.value("name").toString().trimmed();
}

QString OpenWeatherMapParser::getCity()
{
    return m_city;
}
