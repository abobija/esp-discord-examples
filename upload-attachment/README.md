# Esp32 Discord Bot uploads attachment

This is example that represent how to upload attachment with [ESP32 Discord bot](https://github.com/abobija/esp-discord).

## Configuration steps:

- Run `idf.py set-target esp32`
- Open `idf.py menuconfig` and set next configs:
  - `Example Connection Configuration`
    - \> `WiFi SSID` with your wifi name
    - \> `WiFi Password` with you wifi password
  - `Component Config` > `Discord` > `Token` with your bot authentication token
- Now build, flash and monitor the app.

For more examples please visit [esp-discord-examples](https://github.com/abobija/esp-discord-examples) repository or check out official [esp-discord](https://github.com/abobija/esp-discord) component repository.

## Demo

![Bot uploads rickroll](docs/chat.png)

Output:

```
I (7311) discord_bot: Bot ESP32#0578 connected
I (15091) discord_bot: New message (content=Hey bot!, autor=abobija#5474, bot=false, attachments_len=0, channel=805838862634451017, dm=false, guild=805838862634451014)
I (17601) discord_bot: Message sent
```