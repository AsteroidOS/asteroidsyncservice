# Software Architecture {#architecture} #
As mentioned in the [Introduction](@ref mainpage), `asteroidsyncservice` is just one component of a synchronization client.  This section describes a whole synchronization and shows how this software fits into the overall picture.  This project is intended to be used by a [QML](https://doc.qt.io/qt-5/qmlapplications.html) application without the application needing to know about the details of this software.  This project contains two major components: a QML plugin named `asteroidsyncserviceplugin` and a daemon with which it interacts named `asteroidsyncserviced`.  

\startuml
[QML app]
package "asteroidsyncservice" {
    [asteroidsyncserviceplugin] - qml
    note left of [asteroidsyncserviceplugin]
        the QML plugin
    end note
    [QML app] --> qml
    [asteroidsyncserviced] - dbus
    note left of [asteroidsyncserviced]
        the daemon
    end note
    [asteroidsyncserviceplugin] --> dbus
}
[system] 
dbus --> [system]
\enduml

When an external QML application wants to use this software it uses the line `import org.asteroid.syncservice 1.0` and then can instantiate a regular QML object:

```
ServiceController {
    id: serviceController
    Component.onCompleted: serviceController.startService()
    Component.onDestruction: serviceController.stopService()
}
```

In the `SyncServicePlugin::registerTypes` function, the plugin exports three types: `Watch`, `Watches` and `ServiceControl` which is exported to QML as `ServiceController` as shown above.  Once the `ServiceController` is started, the `Watches` collection of `Watch` objects may be used to interact with a selected watch.

Consult the details of those classes to read about the signals and properties that are available to a QML application.

## Platform support ##
The QML plugin provides two different ways to automatically start the daemon.  One is via [D-Bus](@ref dbus), which is used by the SailfishOS platform and the Linux desktop platform, and the other is via `upstart` which is used by the Ubuntu Touch platform.

Similarly, the daemon uses D-Bus to interact with other software (such as a media player) on the Linux desktop and Ubuntu Touch platforms, or by using the [watchfish](@ref watchfish) library which is unique to the SailfishOS platform.

## Further reading ##

- [Introduction](@ref mainpage)
- [Understanding D-Bus](@ref dbus)
- [Glossary](@ref glossary)
