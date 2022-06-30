# Arduino remote controller
This repository is result of high school student project.
Project is put in use in small hydropower plant as main automatic controller.

## Description
Project consists of two parts: desktop GUI for remote controlling and monitoring, Arduino which responds to command and automatically controls plant.

### Arduino
Arduino mega is wired up to sensors:
- hall effect sensor for measuring turbine RPM
- voltage, current sensor to calculate power, power factor
- buttons for manual control
- status of surge protection devices
- temperature sensors
- RTC module

Control is accomplished using 16-relay module.

### Internet communication
Communication is accomplished using ESP8266(AT commands firmware) and communication over arduino serial port.
Plant have public IP address and one port is forwarded to ESP.
Communication consists of JSON documents,
that contains both layout of widgets at desktop client and 
values. Client periodically sends update command requesting new values.
Widgets are sent only once and are stored at arduino,
so client could possibly in future work with different plants.

See also similar project done for STM32 nucleo board with Ethernet port, [here](https://github.com/Ketchp/nucleoRemoteController).