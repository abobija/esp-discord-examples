# ESP32 voice state tracking Discord bot

This is example application which represent [ESP32 Discord bot](https://github.com/abobija/esp-discord) that track the voice state of desired user and set the LED state according to user mute status.

## Demonstration video

[![Monitoring voice mute status with ESP32 Discord bot](https://img.youtube.com/vi/-fCbepT2BNc/mqdefault.jpg)](https://www.youtube.com/watch?v=-fCbepT2BNc)

## Configuration steps:

- Run `./components/esp-discord/certgen.sh` to generate discord certificates
- Run `idf.py set-target esp32`
- Open `idf.py menuconfig` and set next configs:
  - `Example Connection Configuration`
    - \> `WiFi SSID` with your wifi name
    - \> `WiFi Password` with you wifi password
  - `Component Config` > `Discord` > `Token` with your bot authentication token
- Now build, flash and monitor app.

For more examples please visit [esp-discord-examples](https://github.com/abobija/esp-discord-examples) repository or check out official [esp-discord](https://github.com/abobija/esp-discord) component repository.
