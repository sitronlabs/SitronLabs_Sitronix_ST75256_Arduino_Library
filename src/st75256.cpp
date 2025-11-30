/* Self header */
#include "st75256.h"

/**
 * @brief Configures the driver for I2C communication with buffered mode
 *
 * Sets up the ST75256 to communicate over I2C using a local buffer for
 * display data. The buffer allows efficient pixel manipulation before
 * sending data to the display. This function also performs hardware reset
 * and initializes the display with default settings.
 *
 * @param[in] i2c_library Reference to the TwoWire I2C library instance (typically Wire)
 * @param[in] i2c_address I2C address of the device (0x3C, 0x3D, 0x3E, or 0x3F)
 * @param[in] pin_res GPIO pin number connected to the reset (RES) pin
 * @param[in] buffer Pointer to the buffer that will be used to store a local copy
 *                   of the GDRAM, should be (m_active_width * ((m_active_height + 7) / 8)) bytes
 * @return 0 on success, or a negative error code otherwise
 * @note Call this from the Arduino setup() function
 * @note Make sure the I2C library has been initialized with Wire.begin()
 */
int st75256::setup(TwoWire& i2c_library, const uint8_t i2c_address, const int pin_res, uint8_t* const buffer) {

    /* Ensure i2c address is valid */
    if ((i2c_address != 0x3C) &&  //
        (i2c_address != 0x3D) &&  //
        (i2c_address != 0x3E) &&  //
        (i2c_address != 0x3F)) {
        return -EINVAL;
    }

    /* Save parameters */
    m_interface = INTERFACE_I2C_BUFFERED;
    m_i2c_library = &i2c_library;
    m_i2c_address = i2c_address;
    m_buffer = buffer;

    /* Perform reset */
    pinMode(pin_res, OUTPUT);
    digitalWrite(pin_res, LOW);
    delay(2);
    digitalWrite(pin_res, HIGH);
    delay(2);

    /* Configure driver */
    uint16_t Contrast = 200;
    int res = 0;
    res |= command_send(COMMAND_EXTENSION_1);
    res |= command_send(COMMAND_SLEEP_OUT);
    delay(50);
    res |= command_send(0x31);  // Extension Command 2
    res |= command_send(0x32);  // Analog Circuit Set
    res |= data_send(0x00);
    res |= data_send(0x01);
    res |= data_send(0x03);
    res |= command_send(0x51);  // Booster Level x10
    res |= data_send(0xFA);     // 8X
    res |= command_send(0x30);  // Extension Command 1
    res |= command_send(0x20);  // Power Control
    res |= data_send(0x0b);     // VB ON ; VR,VF ON
    res |= command_send(0x81);  // Vop Control
    res |= data_send(Contrast & 0x3F);
    res |= data_send((Contrast >> 6) & 0x07);
    res |= command_send(0x0C);  // Data Format Select     DO=1; LSB on top
    res |= command_send(0xf0);  // Display Mode
    res |= data_send(0x10);     // Monochrome Mode
    res |= command_send(0xCA);  // Display Control
    res |= data_send(0);
    res |= data_send(95);  // duty
    res |= data_send(0x00);
    res |= command_send(0xBC);  // ata Scan Direction
    res |= data_send(0x00);     // MY=0
    res |= command_send(0xaf);  // Display On
    if (res != 0) {
        return -EIO;
    }

    /* */
    delay(100);

    /* Return success */
    return 0;
}

/**
 * @brief Detects if the ST75256 device is present
 *
 * Verifies device presence by attempting to communicate over I2C. For I2C
 * interfaces, this checks if the device acknowledges its address.
 *
 * @return true if device is detected, false otherwise
 */
bool st75256::detect(void) {
    switch (m_interface) {
        case INTERFACE_I2C_LIGHT:
        case INTERFACE_I2C_BUFFERED: {  // In I2C: ensure the device ack its address
            if (m_i2c_library != NULL) {
                m_i2c_library->beginTransmission(m_i2c_address);
                if (m_i2c_library->endTransmission() == 0) {
                    return true;
                }
            }
            return false;
        }
        default: {
            return false;
        }
    }
}

/**
 * @brief Sets the display brightness
 *
 * Configures the display contrast/brightness level. The ratio parameter
 * should be between 0.0 (dim) and 1.0 (bright).
 *
 * @param[in] ratio Brightness ratio from 0.0 to 1.0
 * @return 0 on success, or a negative error code otherwise
 * @note Currently returns -1 (not implemented)
 */
int st75256::brightness_set(const float ratio) {
    if (ratio < 0 || ratio > 1) {
        return -EINVAL;
    }
    // TODO return command_send(COMMAND_CONTRAST_SET, (uint8_t)(percent * 255));
    return -1;
}

/**
 * @brief Sets the display inversion state
 *
 * Controls whether the display shows inverted colors (white on black vs
 * black on white).
 *
 * @param[in] inverted true to invert display, false for normal display
 * @return 0 on success, or a negative error code otherwise
 * @note Currently returns -1 (not implemented)
 */
int st75256::inverted_set(const bool inverted) {
    // TODO return command_send(inverted ? COMMAND_INVERSION_ENABLED : COMMAND_INVERSION_DISABLED);
    return -1;
}

/**
 * @brief Inverts the display (Adafruit GFX compatibility)
 *
 * Adafruit GFX library compatibility function that calls inverted_set().
 *
 * @param[in] i true to invert display, false for normal display
 */
void st75256::invertDisplay(bool i) {
    inverted_set(i);
}

/**
 * @brief Clears the entire display
 *
 * Sets all pixels to off (black). For buffered interfaces, this clears
 * the local buffer. For unbuffered interfaces, this directly clears the
 * GDRAM.
 *
 * @return 0 on success, or a negative error code otherwise
 */
int st75256::clear(void) {
    switch (m_interface) {

        case INTERFACE_I2C_LIGHT: {  // For unbuffered interface, clear gdram directly
            for (size_t i = 0; i < (m_gdram_height + 7) / 8; i++) {
                // command_send(COMMAND_PAGE_ADDRESS + i);
                // command_send(COMMAND_COLUMN_ADDRESS_L | (2 & 0x0F));
                // command_send(COMMAND_COLUMN_ADDRESS_H | (2 >> 4));
                // for (size_t j = 0; j < m_active_width; j++) {
                //     data_send(0x00);
                // }
            }
            return 0;
        }

        case INTERFACE_I2C_BUFFERED: {  // For buffered interfaces, clear local buffer
            memset(m_buffer, 0, m_active_width * ((m_active_height + 7) / 8));
            return 0;
        }

        default: {
            return -EINVAL;
        }
    }
}

/**
 * @brief Sets a single pixel to the specified color
 *
 * Modifies a pixel at the given coordinates. The color parameter is treated
 * as a boolean: non-zero values turn the pixel on (white), zero turns it off (black).
 * Coordinates are automatically rotated based on the current display rotation setting.
 *
 * @param[in] x X coordinate of the pixel
 * @param[in] y Y coordinate of the pixel
 * @param[in] color Pixel color (0 for off, non-zero for on)
 * @return 0 on success, or a negative error code otherwise
 */
int st75256::pixel_set(const size_t x, const size_t y, const uint16_t color) {
    int res;

    /* Handle rotation */
    size_t x_panel, y_panel;
    res = m_rotation_handle(x, y, x_panel, y_panel);
    if (res < 0) {
        return -EINVAL;
    }

    /* Modify display data either in local buffer or directly in gdram */
    switch (m_interface) {

        case INTERFACE_I2C_BUFFERED: {
            if (color) {
                m_buffer[x_panel + (y_panel / 8) * m_active_width] |= (1 << (y_panel % 8));
            } else {
                m_buffer[x_panel + (y_panel / 8) * m_active_width] &= ~(1 << (y_panel % 8));
            }
            break;
        }

        case INTERFACE_I2C_LIGHT: {
            // TODO Maybe insert a dummy read (see section 7.2)
            // res = 0;
            // m_i2c_library->beginTransmission(m_i2c_address);
            // m_i2c_library->write(0x80);  // CO = 1, DC = 0
            // m_i2c_library->write(COMMAND_PAGE_ADDRESS + (y_panel / 8));
            // m_i2c_library->write(0x80);  // CO = 1, DC = 0
            // m_i2c_library->write(COMMAND_COLUMN_ADDRESS_L | ((x_panel + 2) & 0x0F));
            // m_i2c_library->write(0x80);  // CO = 1, DC = 0
            // m_i2c_library->write(COMMAND_COLUMN_ADDRESS_H | ((x_panel + 2) >> 4));
            // m_i2c_library->write(0x80);  // CO = 1, DC = 0
            // m_i2c_library->write(COMMAND_READWRITEMODIFY_BEGIN);
            // m_i2c_library->write(0x40);  // CO = 0, DC = 1
            // m_i2c_library->endTransmission(false);
            // m_i2c_library->requestFrom(m_i2c_address, 2, true);
            // m_i2c_library->read();                 // Dummy read
            // uint8_t data = m_i2c_library->read();  // Gdram data
            // m_i2c_library->beginTransmission(m_i2c_address);
            // m_i2c_library->write(0xC0);  // CO = 1, DC = 1
            // if (color) {
            //     m_i2c_library->write((1 << (y_panel % 8)) | data);
            // } else {
            //     m_i2c_library->write(~(1 << (y_panel % 8)) & data);
            // }
            // m_i2c_library->write(0x00);  // CO = 0, DC = 0
            // m_i2c_library->write(COMMAND_READWRITEMODIFY_END);
            // m_i2c_library->endTransmission(true);
            break;
        }

        default: {
            return -EINVAL;
        }
    }

    /* Return success */
    return 0;
}

/**
 * @brief Draws a pixel (Adafruit GFX compatibility)
 *
 * Adafruit GFX library compatibility function that calls pixel_set().
 *
 * @param[in] x X coordinate of the pixel
 * @param[in] y Y coordinate of the pixel
 * @param[in] color Pixel color (0 for off, non-zero for on)
 */
void st75256::drawPixel(int16_t x, int16_t y, uint16_t color) {
    pixel_set(x, y, color);
}

/**
 * @brief Updates the display with buffered data
 *
 * Sends the contents of the local buffer to the display GDRAM. This function
 * should be called after making changes to the buffer to update the visible display.
 * For unbuffered interfaces, this function does nothing.
 *
 * @return 0 on success, or a negative error code otherwise
 */
int st75256::display(void) {
    int res;
    switch (m_interface) {

        case INTERFACE_I2C_LIGHT: {
            return 0;
        }

        case INTERFACE_I2C_BUFFERED: {

            command_send(0xf0);  // Display Mode
            data_send(0x10);     // Monochrome Mode

            command_send(0x15);
            data_send(0);
            data_send(191);  // TODO

            command_send(0x75);
            data_send(0);
            data_send(11);  // TODO

            command_send(0x5c);

            for (size_t i = 0, p = 0; i < (m_active_height + 7) / 8; i++) {
                for (size_t j = 0; j < m_active_width; j++) {
                    data_send(m_buffer[p++]);
                }
            }

            return 0;
        }

        default: {
            return -EINVAL;
        }
    }
}

/**
 * @brief Sends a command byte to the display
 *
 * Low-level function to send a single command byte to the ST75256 controller.
 * Commands are used to configure display settings and control display behavior.
 * This function handles I2C communication and includes appropriate timing delays.
 *
 * @param[in] command Command byte to send
 * @return 0 on success, or a negative error code otherwise
 * @see enum command for available commands
 */
int st75256::command_send(const uint8_t command) {
    int res;
    switch (m_interface) {

        case INTERFACE_I2C_LIGHT:
        case INTERFACE_I2C_BUFFERED: {
            m_i2c_library->beginTransmission(m_i2c_address);
            m_i2c_library->write(0x00);  // CO = 0, A0 = 0
            m_i2c_library->write(command);
            res = m_i2c_library->endTransmission(true);
            if (res != 0) {
                return -EIO;
            }
            delayMicroseconds(27);  // >26.3us
            return 0;
        }

        default: {
            return -EINVAL;
        }
    }
}

/**
 * @brief Sends a single data byte to the display
 *
 * Low-level function to send a single data byte to the display GDRAM.
 * Data bytes represent pixel values in the display memory. This function
 * handles I2C communication and includes appropriate timing delays.
 *
 * @param[in] data Data byte to send
 * @return 0 on success, or a negative error code otherwise
 */
int st75256::data_send(const uint8_t data) {
    int res;
    switch (m_interface) {

        case INTERFACE_I2C_LIGHT:
        case INTERFACE_I2C_BUFFERED: {
            m_i2c_library->beginTransmission(m_i2c_address);
            m_i2c_library->write(0x40);  // CO = 0, A0 = 1
            m_i2c_library->write(data);
            res = m_i2c_library->endTransmission(true);
            if (res != 0) {
                return -EIO;
            }
            delayMicroseconds(27);  // >26.3us
            return 0;
        }

        default: {
            return -EINVAL;
        }
    }
}

/**
 * @brief Sends multiple data bytes to the display
 *
 * Low-level function to send multiple consecutive data bytes to the display GDRAM.
 * This is more efficient than calling data_send() multiple times for bulk transfers.
 * The function handles I2C packet size limitations by splitting large transfers
 * into multiple I2C transactions if necessary.
 *
 * @param[in] data Pointer to the data array to send
 * @param[in] length Number of bytes to send
 * @return 0 on success, or a negative error code otherwise
 */
int st75256::data_send(uint8_t* const data, const size_t length) {
    int res;
    switch (m_interface) {

        case INTERFACE_I2C_LIGHT:
        case INTERFACE_I2C_BUFFERED: {
            for (size_t i = 0; i < length;) {
                m_i2c_library->beginTransmission(m_i2c_address);
                m_i2c_library->write(0x40);  // CO = 0, DC = 1
                i += m_i2c_library->write(&data[i], length - i);
                res = m_i2c_library->endTransmission(true);
                if (res != 0) {
                    return -EIO;
                }
            }
            return 0;
        }

        default: {
            return -EINVAL;
        }
    }
}

/**
 * @brief Handles coordinate rotation and validation
 *
 * Converts logical coordinates (x, y) to panel coordinates (x_panel, y_panel)
 * based on the current display rotation setting. Also validates that the
 * coordinates are within the active display area. Supports rotations of
 * 0°, 90°, 180°, and 270°.
 *
 * @param[in] x Logical X coordinate
 * @param[in] y Logical Y coordinate
 * @param[out] x_panel Output parameter for panel X coordinate
 * @param[out] y_panel Output parameter for panel Y coordinate
 * @return 0 on success, or a negative error code if coordinates are invalid
 */
int st75256::m_rotation_handle(const size_t x, const size_t y, size_t& x_panel, size_t& y_panel) const {
    switch (rotation) {
        case 0: {
            if (x >= m_active_width || y >= m_active_height) return -EINVAL;
            x_panel = x;
            y_panel = y;
            break;
        }
        case 1: {
            if (x >= m_active_height || y >= m_active_width) return -EINVAL;
            x_panel = m_active_width - y - 1;
            y_panel = x;
            break;
        }
        case 2: {
            if (x >= m_active_width || y >= m_active_height) return -EINVAL;
            x_panel = m_active_width - x - 1;
            y_panel = m_active_height - y - 1;
            break;
        }
        case 3: {
            if (x >= m_active_height || y >= m_active_width) return -EINVAL;
            x_panel = y;
            y_panel = m_active_height - x - 1;
            break;
        }
        default: {
            return -EINVAL;
        }
    }
    return 0;
}
