# Glossary {#glossary} #
This glossary gives brief descriptions of some of the key terms used in this software.

### Bluetooth Low Energy (BLE) {#ble} ###
Bluetooth Low Energy is a specialization of the Bluetooth specification that is designed to be used with devices that need to conserve power.  Since AsteroidOS is designed to run on watches that run on batteries, using BLE is a natural fit.  See https://en.wikipedia.org/wiki/Bluetooth_Low_Energy for more details.

### characteristic {#characteristic} ###
a single data value in BLE that is transferred between devices.  An example is the current volume setting of media player software.

### connected {#connected} ###
devices which are paired and both ready to send or receive data are connected.  Note that it is possible for devices to be paired but not connected, but they cannot be connected and unpaired.

### paired {#paired} ###
two Bluetooth devices which have already completed pairing are said to be paired.

### pairing {#pairing} ###
the process by which a computer and a watch are "introduced" to each other so that they can begin to communicate via Bluetooth 

### service {#service} ###
In the context of BLE, a service can be though of as a collection of related characteristics.

### synchronization client {#synchronization_client} ###
software that provides a means by which data and commands from the a controller machine (generally, a computer, tablet or mobile phone) are sent to the watch.  
