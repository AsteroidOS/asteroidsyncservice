#include <QCoreApplication>
#include <QDebug>
#include "../../dbusinterface.h"
#include "../../watchesmanager.h"
#include "ubuntuplatform.h"

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    
    QLoggingCategory::setFilterRules(QStringLiteral("qt.bluetooth* = true"));

    WatchesManager *watchesManager = new WatchesManager();
    DBusInterface *dbusInterface = new DBusInterface(watchesManager);

    return a.exec();
}
