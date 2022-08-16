# :robot: esp-discord-examples

This repository represent examples of using [esp-discord](https://github.com/abobija/esp-discord) component for making Discord Bot on the ESP32 device.

To get examples, just clone this repository with next command:

```
git clone --recursive https://github.com/abobija/esp-discord-examples
```

Tu update, run:

```
git pull --recurse-submodules
```

## How to build

Every example has `README.md` file. Open that file and read building instructions for that particular example.

## Esp-Discord demo video

[![Discord Bots on the ESP32 ... (ESP-IDF library)](https://img.youtube.com/vi/p5qzRH2abvw/mqdefault.jpg)](https://www.youtube.com/watch?v=p5qzRH2abvw)

## Development update

```
git pull && git submodule foreach --recursive 'folder=$(basename "$PWD"); if [ "$folder" = "esp-discord" ]; then git checkout main && git pull; fi'
```

## Author

GitHub: [abobija](https://github.com/abobija)<br>
Homepage: [abobija.com](https://abobija.com)

## License

[MIT](LICENSE)
