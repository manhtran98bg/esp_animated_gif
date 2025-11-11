# AnimatedGIF Component for ESP-IDF

This repository provides an **ESP-IDF component port** of the [AnimatedGIF](https://github.com/bitbank2/AnimatedGIF) library by BitBank2.  
It allows ESP32 and ESP32-C3 projects to **decode and display GIF animations** directly on embedded displays (e.g. SPI LCD, TFT).

## Structure
```
components/
└── esp_animated_gif/
    ├── src/           # Original AnimatedGIF library
    ├── CMakeLists.txt # ESP-IDF component definition
    └── component.mk   # (optional) for legacy IDF build system
```

## Usage
1. Add this repo as a submodule in your ESP-IDF project:
   ```bash
   git submodule add -b master https://github.com/bitbank2/AnimatedGIF.git components/esp_animated_gif/src
   ```
2. Include the component in your project `CMakeLists.txt`:
   ```cmake
   idf_component_register(SRCS "main.c"
                          INCLUDE_DIRS "."
                          REQUIRES esp_animated_gif)
   ```
3. Call AnimatedGIF APIs in your app to open, decode, and render GIF frames.

## Notes
- Tested with **ESP-IDF v5.x**
- Compatible with displays driven by **esp_lcd** or **Arduino GFX**
- Ensure sufficient **PSRAM** for larger GIFs

## License
This component follows the [MIT License](https://opensource.org/licenses/MIT) under the original AnimatedGIF project.
