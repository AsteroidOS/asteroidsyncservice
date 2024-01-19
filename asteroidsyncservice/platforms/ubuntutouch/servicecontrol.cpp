#include "servicecontrol.h"

#include <QFile>
#include <QDir>
#include <QDebug>
#include <QCoreApplication>
#include <QProcess>

ServiceControl::ServiceControl(QObject *parent) : QObject(parent)
{

}

QString ServiceControl::serviceName() const
{
    return m_serviceName;
}

void ServiceControl::setServiceName(const QString &serviceName)
{
    if (m_serviceName != serviceName) {
        m_serviceName = serviceName;
        emit serviceNameChanged();
    }
}

bool ServiceControl::serviceFileInstalled() const
{
    if (m_serviceName.isEmpty()) {
        qDebug() << "Service name not set.";
        return false;
    }
    QFile f(QDir::homePath() + "/.config/systemd/user/"+ m_serviceName + ".service");
    return f.exists();
}

bool ServiceControl::installServiceFile()
{
    if (m_serviceName.isEmpty()) {
        qDebug() << "Service name not set. Cannot generate service file.";
        return false;
    }

    QFile f(QDir::homePath() + "/.config/systemd/user/"+ m_serviceName + ".service");
    if (f.exists()) {
        qDebug() << "Service file already existing...";
        return false;
    }

    if (!f.open(QFile::WriteOnly | QFile::Truncate)) {
        qDebug() << "Cannot create service file";
        return false;
    }

    QString appDir = qApp->applicationDirPath();
    // Try to replace version with "current" to be more robust against updates
    appDir.replace(QRegExp("[0-9].[0-9].[0-9]"), "current");

    f.write("[Unit]\n");
    f.write("Description=asteroidsync\n");
    f.write("After=graphical.target\n");
    f.write("[Service]\n");
    f.write("ExecStart=" + appDir.toUtf8() + "/usr/bin/" + m_serviceName.toUtf8() + "\n");

    f.write("Restart=always\n");
    f.write("RestartSec=5\n");
    f.write("Environment=LD_LIBRARY_PATH=" + appDir.toUtf8() + "/usr/bin/../:$LD_LIBRARY_PATH\n");
    f.write("Environment=HOME=%h XDG_CONFIG_HOME=/home/%u/.config DBUS_SESSION_BUS_ADDRESS=unix:path=/run/user/%U/bus XDG_RUNTIME_DIR=/run/user/%U\n");
    f.write("[Install]\n");
    f.write("WantedBy=default.target\n");


    f.close();
    return true;
}

bool ServiceControl::removeServiceFile()
{
    if (m_serviceName.isEmpty()) {
        qDebug() << "Service name not set.";
        return false;
    }
    QFile f(QDir::homePath() + "/.config/systemd/user/"+ m_serviceName + ".service");
    return f.remove();
}

bool ServiceControl::serviceRunning() const
{
    QProcess p;
    p.start("systemctl", {"is-active", "--user", m_serviceName});
    p.waitForFinished();
    QByteArray output = p.readAll();
    qDebug() << output;
    return output.contains("active");
}

bool ServiceControl::setServiceRunning(bool running)
{
    if (running && !serviceRunning()) {
        return startService();
    } else if (!running && serviceRunning()) {
        return stopService();
    }
    return true; // Requested state is already the current state.
}

bool ServiceControl::startService()
{
    int ret = QProcess::execute("systemctl", {"start", "--user", m_serviceName});
    qDebug() << "start service" << ret;
    return ret == 0;
}

bool ServiceControl::stopService()
{
    int ret = QProcess::execute("systemctl", {"stop", "--user", m_serviceName});
    qDebug() << "stop service" << ret;
    return ret == 0;
}

bool ServiceControl::restartService()
{
    int ret = QProcess::execute("systemctl", {"restart", "--user", m_serviceName});
    qDebug() << "restart service" << ret;
    return ret == 0;
}
