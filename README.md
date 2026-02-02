# ESPNow-UDP-Bridge
Bridge device to receive ESPNow signals on a Wemos D1 Mini and apply UDP on a second Wemos D1 Mini

Running ESPNow and WiFi on the same device is problematic, so the easy solution was to accept ESPNow data in a Wemos D1 Mini and trigger corresponding GPIO
to another Wemos D1 Mini which is connected to WiFi and sends appropriate UDP commands.

This project is used in a Halloween experience. An operator presses a button on a keypad which starts a video played on an Intel NUC.

In this project, the initiating device is a keypad with a third Wemos D1 Mini, but the source can be any device capable of ESPNow.
The receiver on the bridge gets the key number from the keypad. A corresponding GPIO is pulsed briefly.
The sender sees the GPIO output from the receiver on corresponding GPIO inputs. The code on the sender then sends a UDP message to the video player on the NUC.

Future- The sender does not need to send UDP. It could as easily cnnect to the Home Assistant API to trigger scripts or automations.
