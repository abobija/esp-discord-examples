# :robot: esp-discord-examples

This repository represent examples of using [esp-discord](https://github.com/abobija/esp-discord) component for making Discord Bot on the ESP32 device.

To get examples, just clone this repository with next command:

```
git clone --recursive https://github.com/abobija/esp-discord-examples.git
```

Tu update, run:

```
git pull --recurse-submodules
```

## How to build

Go inside of desired example folder and do next:

- Run `./components/esp-discord/certgen.sh` to generate discord certificates
- Run `idf.py set-target esp32`
- Open `idf.py menuconfig` and set next configs:
  - `Example Connection Configuration`
    - \> `WiFi SSID` with your wifi name
    - \> `WiFi Password` with you wifi password
  - `Component Config` > `Discord` > `Token` with your bot authentication token
- Now run `idf.py build` to build the app
- Flash and monitor app.

## Development update

```
git pull && git submodule foreach --recursive git pull origin main
```

## Author

GitHub: [abobija](https://github.com/abobija)<br>
Homepage: [abobija.com](https://abobija.com)

## License

[MIT](LICENSE)