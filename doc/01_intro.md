# Introduction {#mainpage} #
The purpose of a [synchronization client](@ref synchronization_client) is to synchronize data between a device (a computer, phone or tablet) and a watch running AsteroidOS. Although either a computer or mobile phone are commonly used for this, the rest of this document will refer to "the computer" to mean the computer on which the synchronization client runs.

It's important to understand that this project doesn't implement an entire synchronization client but provides relatively portable code on which a sychronization client can be built.  Two such examples are Telescope which runs on Ubuntu Touch, and starship which runs on SailfishOS.

The kinds of things that are synchronized can include the time of day and weather data for the next few days, or commands such as taking a screenshot of the watch face from your phone or turning the volume of music playing on your computer up or down from your watch.

Note that some things flow *from* the computer to the watch.  An example of this is weather forecast data which would be sent *from* the computer to the watch.  Some things flow *to* the computer from the watch, such as music player commands to turn the volume up or down or skip to the next track.  

For all of these, the communications between the watch and the computer uses [Bluetooth](https://www.bluetooth.com/learn-about-bluetooth/tech-overview/).  Later pages describe the details of this and how it works. 

## An example

An example may be useful to clarify.  In this example, we trace a single request to turn the volume up via the watch's music app.  From the user's point of view, here's a very simplified sequence diagram.  This assumes that the sofware is already running on the computer, that the watch is [paired](@ref paired), and that the watch is [connected](@ref connected).  The music player shown here isn't any particular software, but a class of software that can play media.  Real examples of such software include [vlc](https://www.videolan.org/) and [Rhythmbox](https://wiki.gnome.org/Apps/Rhythmbox), but as we will see later, as long as the software implements a particular kind of interface, it can be used.

\startuml
actor user
user -> Music: press Volume Up
Music -> Computer: "increase volume"
Computer -> music_player:  increase volume
\enduml

From the developer's point of view, the story is a little more complex.  Since this is the `androidsyncservice` project, we will only describe the reception of a Bluetooth message and trace as this signal is processed on the computer.  First, the message is received by the computer's Bluetooth hardware.  A low level driver, which depends on the particular hardware used, takes care of this part and hands off the message to [BlueZ](http://www.bluez.org/) which is the official Linux Bluetooth protocol stack. 
The BlueZ software then translates this incoming message into [D-Bus](https://www.freedesktop.org/wiki/Software/dbus/) messages.  D-Bus is an Inter-Process Communication (IPC) system for Linux processes.  In this instance, BlueZ handles many of the underlying complexity of Bluetooth for us.  AsteroidOS uses [Bluetooth Low Energy](@ref ble) or BLE, to communicate.  BLE uses the General Attribute Profile (GATT) which collects individual data items called [characteristics](@ref characteristic) into [services](@ref service).  The `asteroidsyncservice` daemon subscribes to notifications of changes for particular characteristics of interest.  For AsteroidOS, these services and characteristics are [documented](https://asteroidos.org/wiki/ble-profiles/) When a characteristic changes, the software is notified.  In this particular example, that change is received by the `MediaService::onCharacteristicChanged` function. The notification also contains a passed value which is the desired volume level percentage expressed as an integer from 0 to 100.  This results in a signal, `MediaService::volume` being emitted, which is connected to the `Platform::onMediaVolumeChange` function.  That function then uses D-Bus once again to send a request to a media player to set its volume.  Unlike the `MediaService` class, however, the interface for the media player (defined by a freedesktop standard called [Media Player Remote Interfacing Specification](https://specifications.freedesktop.org/mpris-spec/latest/) or MPRIS) expects to receive the volume setting as a `double` in the range 0.0 to 1.0 so we divide by 100 to scale the value appropriately.

\startuml
BlueZ -> MediaService : media service characteristic changed
MediaService -> Platform : MediaService::volume(volume)
Platform -> media_player : use D-bus to set volume
\enduml

## Going further
That is a very quick introduction to generally how the `asteroidsyncservice` software works.  Here are links for more details:

- [Software Architecture](@ref architecture)
- [Understanding D-Bus](@ref dbus)
- [Glossary](@ref glossary)
