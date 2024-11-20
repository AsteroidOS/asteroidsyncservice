# D-Bus interface {#daemon} #
The `asteroidsyncserviced` (daemon) component can be used by itself or in combination with the portion of the software that exports the QML interface.  This section describes the operation of the daemon.  A daemon, in Linux, is just a program that is intended to run in the background without direct user input.  In the case of `asteroidsyncserviced`, this daemon provides a D-Bus interface on a host Linux computer to interact via Bluetooth with a connected and paired watch.

## Interface details ##
To understand this section, it's useful to already have read and understood [Understanding D-Bus](@ref dbus).

One way to understand the `asteroidsyncserviced` D-Bus interface is to use the [`busctl`](https://www.freedesktop.org/software/systemd/man/latest/busctl.html) software to examine and manipulate D-Bus interfaces.  

To explore this interface on your own computer, first start the `asteroidsyncserviced` daemon.  If it has already been installed, simply type `asteroidsyncserviced` from the command line.  If we do this, it will run and not return because it's intended to be a long-term application that runs in the background.  For the purpose of exploring or testing, it's useful to run in as a foreground task and then use another terminal window to run the various `busctl` commands.

### Tree structure ###
With `asteroidsyncserviced` running, we can examine the tree structure of the D-Bus interface:

```
$ busctl --user tree org.asteroidsyncservice
\ /org
  \- /org/asteroidsyncservice
  | \- /org/asteroidsyncservice/E1_02_F7_CC_86_55
  | \- /org/asteroidsyncservice/Manager
  \- /org/freedesktop
    \- /org/freedesktop/Notifications
```

We can see that, in this case, there are two interfaces under `/org/asteroidsyncservice`, and we can examine each in turn.

```
$ busctl --user introspect org.asteroidsyncservice /org/asteroidsyncservice/Manager 
NAME                                TYPE      SIGNATURE RESULT/VALUE FLAGS
org.asteroidsyncservice.Manager     interface -         -            -
.ListWatches                        method    -         ao           -
.SelectedWatch                      method    -         o            -
.SelectedWatchConnected             method    -         b            -
.Version                            method    -         s            -
.NameChanged                        signal    -         -            -
.SelectedWatchChanged               signal    -         -            -
.SelectedWatchConnectedChanged      signal    -         -            -
.WatchesChanged                     signal    -         -            -
org.freedesktop.DBus.Introspectable interface -         -            -
.Introspect                         method    -         s            -
org.freedesktop.DBus.Peer           interface -         -            -
.GetMachineId                       method    -         s            -
.Ping                               method    -         -            -
org.freedesktop.DBus.Properties     interface -         -            -
.Get                                method    ss        v            -
.GetAll                             method    s         a{sv}        -
.Set                                method    ssv       -            -
.PropertiesChanged                  signal    sa{sv}as  -            -
```

This shows the `Manager` interface which is, as the name suggests, the portion of the interface that manages the collection of watches currently connected to this computer.  In our case, there is only one, which we can similarly explore:

```
$ busctl --user introspect org.asteroidsyncservice /org/asteroidsyncservice/E1_02_F7_CC_86_55 
NAME                                TYPE      SIGNATURE      RESULT/VALUE FLAGS
org.asteroidsyncservice.Watch       interface -              -            -
.Address                            method    -              s            -
.BatteryLevel                       method    -              y            -
.Name                               method    -              s            -
.RequestScreenshot                  method    -              -            -
.SelectWatch                        method    -              -            -
.SendNotify                         method    usssss         -            -
.SetTime                            method    ((iii)(iiii)i) -            -
.StatusNotifyService                method    -              b            -
.StatusScreenshotService            method    -              b            -
.StatusTimeService                  method    -              b            -
.StatusWeatherService               method    -              b            -
.WeatherSetCityName                 method    s              -            -
.WeatherSetWeather                  method    s              -            -
.BatteryServiceReady                signal    -              -            -
.Connected                          signal    -              -            -
.LevelChanged                       signal    y              -            -
.NotifyServiceChanged               signal    -              -            -
.ProgressChanged                    signal    u              -            -
.ScreenshotReceived                 signal    ay             -            -
.ScreenshotServiceChanged           signal    -              -            -
.TimeServiceChanged                 signal    -              -            -
.WeatherServiceChanged              signal    -              -            -
org.freedesktop.DBus.Introspectable interface -              -            -
.Introspect                         method    -              s            -
org.freedesktop.DBus.Peer           interface -              -            -
.GetMachineId                       method    -              s            -
.Ping                               method    -              -            -
org.freedesktop.DBus.Properties     interface -              -            -
.Get                                method    ss             v            -
.GetAll                             method    s              a{sv}        -
.Set                                method    ssv            -            -
.PropertiesChanged                  signal    sa{sv}as       -            -
```

We can attempt to read the battery level of this device:

```
$ busctl --user call org.asteroidsyncservice /org/asteroidsyncservice/E1_02_F7_CC_86_55 org.asteroidsyncservice.Watch BatteryLevel
y 0
```

This seemed to work but we get back a battery level of 0% which is not plausible.  This is because we need to invoke `SelectWatch` first, before doing anything useful with this particular watch.  

```
$ busctl --user call org.asteroidsyncservice /org/asteroidsyncservice/E1_02_F7_CC_86_55 org.asteroidsyncservice.Watch SelectWatch
```

Now we can attempt to read the battery level of this device again:

```
$ busctl --user call org.asteroidsyncservice /org/asteroidsyncservice/E1_02_F7_CC_86_55 org.asteroidsyncservice.Watch BatteryLevel
y 85
```

This shows a much more reasonable 85% battery, verifying that the interface is working as intended.  

We can also try to invoke a method with arguments.  In this case we'll send a message to the watch:

```
busctl --user call org.asteroidsyncservice /org/asteroidsyncservice/E1_02_F7_CC_86_55 org.asteroidsyncservice.Watch SendNotify usssss 54 "Test app" "ios-happy" "My Title" "This might be a useful message in a real app." "none"
```

The `usssss` portion of that is the signature of the arguments.  Each `u` is an unsigned number and each `s` is a string.  However, to understand what they really mean in the context of this particular call, we must consult the source code for DBusWatch::SendNotify.

A screen shot of what it looks like on the watch when received is shown below.

@image html screenshot.jpg "Screen shot of message"
@image latex screenshot.jpg "Screen shot of message"

#### Weather functions ####

There are two weather function which can be used.  One is to set the name of the city, and the other is used to send a JSON representation of the forecast data.

For the city name, it's quite simple:

```
busctl --user call org.asteroidsyncservice /org/asteroidsyncservice/E1_02_F7_CC_86_55 org.asteroidsyncservice.Watch WeatherSetCityName s "New York, NY"
```

For the weather forecast, the software requires a JSON string of the kind that comes from the open-meteo server.  With the latitude and longitude of the desired location, we can call the service and save the result.

```
wget -O forecast.json 'https://api.open-meteo.com/v1/forecast?latitude=40.7590&longitude=-74.0516&timezone=auto&daily=weather_code,temperature_2m_max,temperature_2m_min'
```

The contents of the `forecast.json` file should now look something like this:
```json
{"latitude":40.75003,"longitude":-74.05296,"generationtime_ms":0.14698505401611328,"utc_offset_seconds":-18000,"timezone":"America/New_York","timezone_abbreviation":"EST","elevation":12.0,"daily_units":{"time":"iso8601","weather_code":"wmo code","temperature_2m_max":"°C","temperature_2m_min":"°C"},"daily":{"time":["2024-11-20","2024-11-21","2024-11-22","2024-11-23","2024-11-24","2024-11-25","2024-11-26"],"weather_code":[3,71,85,85,3,3,3],"temperature_2m_max":[16.3,12.0,4.4,8.1,8.9,10.4,10.8],"temperature_2m_min":[11.7,3.5,1.8,4.0,6.4,5.5,8.6]}}
```

We can now send this data to the watch:

```
busctl --user call org.asteroidsyncservice /org/asteroidsyncservice/E1_02_F7_CC_86_55 org.asteroidsyncservice.Watch WeatherSetWeather s "$(cat forecast.json)"
```

### Key Points ###

- Get a list of connected waches by calling the ListWatches method of the Manager
- Select a particular watch by calling its SelectWatch method
- Call individual watch methods as needed


## Further reading ##

- [Introduction](@ref mainpage)
- [Understanding D-Bus](@ref dbus)
- [Software Architecture](@ref architecture)
- [Glossary](@ref glossary)
