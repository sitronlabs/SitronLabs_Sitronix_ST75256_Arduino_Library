[![Designed by Sitron Labs](https://img.shields.io/badge/Designed_by-Sitron_Labs-FCE477.svg)](https://www.sitronlabs.com/)
[![Join the Discord community](https://img.shields.io/discord/552242187665145866.svg?logo=discord&logoColor=white&label=Discord&color=%237289da)](https://discord.gg/btnVDeWhfW)
[![PayPal Donate](https://img.shields.io/badge/PayPal-Donate-00457C.svg?logo=paypal&logoColor=white)](https://www.paypal.com/donate/?hosted_button_id=QLX8VU9Q3PFFL)
![License](https://img.shields.io/github/license/sitronlabs/SitronLabs_Sitronix_ST75256_Arduino_Library.svg)
![Latest Release](https://img.shields.io/github/release/sitronlabs/SitronLabs_Sitronix_ST75256_Arduino_Library.svg)
[![Arduino Library Manager](https://www.ardu-badge.com/badge/Sitron%20Labs%20ST75256%20Arduino%20Library.svg)](https://www.ardu-badge.com/Sitron%20Labs%20ST75256%20Arduino%20Library)
[![PlatformIO Registry](https://badges.registry.platformio.org/packages/sitronlabs/library/Sitron_Labs_ST75256_Arduino_Library.svg)](https://registry.platformio.org/libraries/sitronlabs/Sitron_Labs_ST75256_Arduino_Library)

# Sitron Labs Sitronix ST75256 Arduino Library

Arduino library for interfacing with LCD displays based on the Sitronix ST75256 graphic LCD controller.

## Description

The ST75256 is a graphic LCD controller capable of driving monochrome displays with various resolutions. This library provides a simple interface to control ST75256-based displays via I2C, with support for buffered graphics operations. The library extends Adafruit_GFX, providing access to all standard graphics functions including text rendering, shapes, and bitmap drawing. Currently, the library supports I2C buffered mode, with SPI and unbuffered I2C support planned for future releases.

## Installation

### Arduino IDE

Install via the Arduino Library Manager by searching for "Sitron Labs ST75256".

Alternatively, install manually:
1. Download or clone this repository
2. Place it in your Arduino `libraries` folder
3. Restart the Arduino IDE

### PlatformIO

Install via the PlatformIO Library Manager by searching for "Sitron Labs ST75256".

Alternatively, add the library manually to your `platformio.ini` file:

```ini
lib_deps = 
    https://github.com/sitronlabs/SitronLabs_Sitronix_ST75256_Arduino_Library.git
```

## Hardware Connections

### I2C Interface

Connect the ST75256 display to your Arduino using I2C:

- VCC → 3.3V or 5V (check your board's specifications)
- GND → GND
- SDA → SDA (I2C Data)
- SCL → SCL (I2C Clock)
- RES → Any digital pin (Reset pin)

The I2C address is typically 0x3C, but may be 0x3D, 0x3E, or 0x3F depending on your display module configuration.

## Usage

### Basic I2C Example

```cpp
#include <Wire.h>
#include <st75256.h>

// Display dimensions (192x96 for ERM19296-1)
const int DISPLAY_WIDTH = 192;
const int DISPLAY_HEIGHT = 96;
const int DISPLAY_RESET_PIN = 5;
const uint8_t I2C_ADDRESS = 0x3C;

// Create display object
st75256 display(DISPLAY_WIDTH, DISPLAY_HEIGHT);

// Display buffer (required for buffered mode)
// Size = width * (height / 8) bytes
uint8_t display_buffer[DISPLAY_WIDTH * (DISPLAY_HEIGHT / 8)];

void setup() {
  Serial.begin(9600);
  
  // Initialize I2C
  Wire.begin();
  
  // Setup the ST75256 display (I2C library, I2C address, reset pin, buffer)
  if (display.setup(Wire, I2C_ADDRESS, DISPLAY_RESET_PIN, display_buffer) != 0) {
    Serial.println("Failed to setup ST75256");
    return;
  }
  
  // Detect the device
  if (!display.detect()) {
    Serial.println("ST75256 not detected");
    return;
  }
  
  // Clear the display buffer
  display.clear();
  
  // Set text size and color
  display.setTextSize(1);
  display.setTextColor(1);  // 1 = white/on, 0 = black/off
  
  // Draw text
  display.setCursor(0, 0);
  display.println("Hello");
  display.println("World!");
  
  // Update the display
  display.display();
  
  Serial.println("ST75256 initialized");
}

void loop() {
  // Your code here
}
```

### Graphics Example

```cpp
#include <Wire.h>
#include <st75256.h>

st75256 display(192, 96);
uint8_t display_buffer[192 * (96 / 8)];
const int DISPLAY_RESET_PIN = 5;
const uint8_t I2C_ADDRESS = 0x3C;

void setup() {
  Wire.begin();
  
  display.setup(Wire, I2C_ADDRESS, DISPLAY_RESET_PIN, display_buffer);
  if (!display.detect()) {
    Serial.println("ST75256 not detected");
    return;
  }
  
  display.clear();
  
  // Draw shapes
  display.drawRect(10, 10, 50, 30, 1);      // Rectangle
  display.fillCircle(100, 30, 20, 1);       // Filled circle
  display.drawLine(0, 60, 192, 60, 1);      // Horizontal line
  
  // Draw text
  display.setTextSize(2);
  display.setCursor(10, 70);
  display.print("GFX!");
  
  display.display();
}

void loop() {
  // Your code here
}
```

### Related Products

| <a href="https://www.buydisplay.com/2-8-inch-white-192x96-graphic-lcd-display-module-st75256-for-arduino"><img src="https://www.buydisplay.com/media/catalog/product/cache/53fd08fe2cb06c4269619329bc634cc8/e/r/erm19296-1_1.jpg" alt="Display Module" width="400" height="auto" /></a> |
|:--:|
| *<a href="https://www.buydisplay.com/2-8-inch-white-192x96-graphic-lcd-display-module-st75256-for-arduino">2.8" White 192x96 Graphic LCD Display Module</a>* |

## API Reference

### st75256(int width, int height)

Constructor for the ST75256 display object.

- `width`: Display width in pixels
- `height`: Display height in pixels

### setup(TwoWire &i2c_library, const uint8_t i2c_address, const int pin_res, uint8_t* const buffer)

Initializes the ST75256 display for I2C buffered communication.

- `i2c_library`: I2C library instance to use (typically `Wire`)
- `i2c_address`: I2C address of the device (0x3C, 0x3D, 0x3E, or 0x3F)
- `pin_res`: GPIO pin number connected to the display's reset pin
- `buffer`: Pointer to display buffer array (size = width * (height / 8) bytes)

Returns 0 on success, or a negative error code otherwise.

### detect(void)

Detects if the ST75256 device is present by checking I2C communication.

Returns true if device is detected, false otherwise.

### clear(void)

Clears the display buffer (sets all pixels to off).

Returns 0 on success, or a negative error code otherwise.

### display(void)

Updates the physical display with the contents of the display buffer. This function must be called after drawing operations to make changes visible.

Returns 0 on success, or a negative error code otherwise.

### pixel_set(const size_t x, const size_t y, const uint16_t color)

Sets a single pixel in the display buffer.

- `x`: X coordinate (0 to width-1)
- `y`: Y coordinate (0 to height-1)
- `color`: Pixel color (0 = off, 1 = on)

Returns 0 on success, or a negative error code otherwise.

### drawPixel(int16_t x, int16_t y, uint16_t color)

Adafruit_GFX-compatible function to set a pixel. This is an alias for `pixel_set`.

- `x`: X coordinate
- `y`: Y coordinate
- `color`: Pixel color (0 = off, 1 = on)

### brightness_set(const float ratio)

Sets the display brightness (planned feature, currently returns -1).

- `ratio`: Brightness ratio from 0.0 to 1.0

Returns 0 on success, or a negative error code otherwise.

### inverted_set(const bool inverted)

Sets display inversion (planned feature, currently returns -1).

- `inverted`: true to invert display, false for normal

Returns 0 on success, or a negative error code otherwise.

### invertDisplay(bool i)

Adafruit_GFX-compatible function to invert the display. This calls `inverted_set`.

- `i`: true to invert, false for normal

### command_send(const uint8_t command)

Sends a command byte to the display controller.

- `command`: Command byte to send

Returns 0 on success, or a negative error code otherwise.

### data_send(const uint8_t data)

Sends a single data byte to the display controller.

- `data`: Data byte to send

Returns 0 on success, or a negative error code otherwise.

### data_send(uint8_t* const data, const size_t length)

Sends multiple data bytes to the display controller.

- `data`: Pointer to data array
- `length`: Number of bytes to send

Returns 0 on success, or a negative error code otherwise.

## Adafruit GFX Functions

Since `st75256` extends `Adafruit_GFX`, all standard Adafruit GFX functions are available, including:

- Text functions: `setTextSize()`, `setTextColor()`, `setCursor()`, `print()`, `println()`
- Shape functions: `drawLine()`, `drawRect()`, `fillRect()`, `drawCircle()`, `fillCircle()`, `drawTriangle()`, `fillTriangle()`
- Bitmap functions: `drawBitmap()`, `drawXBitmap()`
- And many more - see the [Adafruit GFX Library documentation](https://learn.adafruit.com/adafruit-gfx-graphics-library) for details

## Specifications

- Communication interface: I2C (buffered mode supported)
- I2C addresses: 0x3C, 0x3D, 0x3E, or 0x3F
- Display resolution: Configurable (e.g., 192x96 for ERM19296-1)
- Color depth: Monochrome (1-bit)
- Graphics library: Extends Adafruit_GFX
- Features: Text rendering, shapes, bitmaps, buffered graphics operations

## Supported Interfaces

| Interface | Status |
|-----------|:------:|
| SPI, Buffered | ❌ (Planned) |
| I2C, Buffered | ✔️ |
| I2C, Unbuffered | ❌ (Planned) |
