# Understanding D-Bus {#dbus} #
This is not a comprehensive tutorial on D-Bus, but an overview of generally what D-Bus is, how it works, and how it is used within `asteroidsyncservice`.  For a more thorough view, see https://www.freedesktop.org/wiki/IntroductionToDBus/

D-Bus is a way for different pieces of software to communicate with each other in a standarized way.  This is how the `asteroidsyncservice` software can, for instance, turn the volume up or down on media player software without needing to know exactly which media player software is being used.\footnote{Although the StarfishOS supports D-Bus, it is not used for this purpose on that platform}

Generally, software can expose a standard interface on D-Bus that other software can use.  An interface may include both *methods* and *signals*.  

On the StarfishOS and desktop platforms, the plugin uses D-Bus to interact with `systemd` via ServiceControl to start, stop, and restart the daemon and read properties.  For an example, see ServiceControl::getUnitProperties.

On the Ubuntu Touch and desktop platforms, the daemon uses D-Bus to interact with other software, such as the media player.  An example of this can be seen in the Platform::onMediaVolumeChange code.

On all platforms, BlueZ uses D-Bus to send messages to `asteroidsyncservice`.  See BluezClient::BluezClient to see an example of this interaction.

## Further reading ##

- [Introduction](@ref mainpage)
- [Software Architecture](@ref architecture)
- [Glossary](@ref glossary)
