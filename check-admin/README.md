# ESP32 check-admin Discord bot

This is example application which represent [ESP32 Discord bot](https://github.com/abobija/esp-discord) that checks if user which sends message have administrator privileges.

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

## Demo

On this first picture I have assigned **Member** role (green) that has no adminitrator privileges.

![No admin](docs/no-admin.png)

On this picture I have assign myself a **Owner** role (yellow) that has administrator privileges.

![Yes admin](docs/yes-admin.png)

Output:

```
I (7438) discord_bot: Bot ESP32#9421 connected
I (17378) discord_bot: New message (dm=false, autor=abobija#5474, bot=false, channel=808419120159916042, guild=805838862634451014, content=admin?)
I (135238) discord_bot: New message (dm=false, autor=abobija#5474, bot=false, channel=808419120159916042, guild=805838862634451014, content=admin?)
```