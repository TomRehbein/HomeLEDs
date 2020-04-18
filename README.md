# HomeLEDs

This project is about the connection and coordination of several LED strips via a separate WiFi.

Module used:

1. NodeMCU WiFi Module
2. WeMos D1 Mini

The NodeMCU module creates a WiFi, in which all WeMos modules try to connect.
After all WeMos modules are connected to the WiFi, the NodeMCU sends packets to the WeMos modules.

The packets sent consist of the mode you are in and data.
The data sent can be sound information, for example.
The mode is specified by the NodeMCU, but the WeMos modules can react differently to the specified mode.

## For the use of several WeMos modules:

in Master.ino:

```ino

#define NUMBER_OF_CLIENTS

```

This value must correspond to the number of WeMos modules used.

in Slave.ino:

```ino

#define LAMP_ID

```

This value must be unique for each module.
