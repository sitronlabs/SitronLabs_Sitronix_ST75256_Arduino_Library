/* Self header */
#include "st75256.h"

/**
 *
 * @param[in] i2c_library
 * @param[in] i2c_address
 * @param[in] pin_res
 * @param[in] buffer
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
#if 0
    /* Configure driver */
    int res = 0;
    res |= command_send(COMMAND_EXTENSION_1);
    res |= command_send(COMMAND_SLEEP_OUT);
    res |= command_send(COMMAND_DISPLAY_OFF);
    res |= command_send(COMMAND_EXTENSION_2);
    res |= command_send(0xD7);  // disable auto read
    res |= data_send(0x9F);     // disable auto read
    res |= command_send(0x32);  // analog circuit set
    res |= data_send(0x00);     // code example: OSC Frequency adjustment
    res |= data_send(0x01);     // Frequency on booster capacitors 1 = 6KHz?
    res |= data_send(0x03);     // Bias: 1: 1/13, 2: 1/12, 3: 1/11, 4:1/10, 5:1/9
    res |= command_send(0x20);  // gray levels
    res |= data_send(0x01);
    res |= data_send(0x03);
    res |= data_send(0x05);
    res |= data_send(0x07);
    res |= data_send(0x09);
    res |= data_send(0x0b);
    res |= data_send(0x0d);
    res |= data_send(0x10);
    res |= data_send(0x11);
    res |= data_send(0x13);
    res |= data_send(0x15);
    res |= data_send(0x17);
    res |= data_send(0x19);
    res |= data_send(0x1b);
    res |= data_send(0x1d);
    res |= data_send(0x1f);
    res |= command_send(COMMAND_EXTENSION_1);
    res |= command_send(0x75);  // row range
    res |= data_send(0);        // row range
    res |= data_send(0x4f);     // row range
    res |= command_send(21);    // col range TOOO Maybe change?
    res |= data_send(0);        // col range
    res |= data_send(255);      // col range TOOO Maybe change?
    res |= command_send(0xbc);  // Data Scan Direction
    res |= data_send(0x00);     // Data Scan Direction
    res |= data_send(0xa6);     // ???
    res |= command_send(0x0c);  // data format LSB top
    res |= command_send(0xca);  // display control; 3 args follow
    res |= data_send(0x00);     // 0x00: no clock division; 0x04: devide clock
    res |= data_send(0x9f);     // 1/160 duty value from the DS example code
    res |= data_send(0x20);     // nline off
    res |= command_send(0xf0);  // monochrome mode  = 0x10
    res |= command_send(0x10);  // monochrome mode  = 0x10
    res |= command_send(0x81);  // Volume control
    res |= data_send(0x2e);     // Volume control
    res |= data_send(0x03);     // Volume control
    res |= command_send(0x20);  // Power control: Regulator; follower & booster on
    res |= data_send(0x0b);     // Power control: Regulator; follower & booster on
    if (res != 0) {
        return -EIO;
    }
#endif

    /* */
    delay(100);

    /* Return success */
    return 0;
}

#if 0
/**
 *
 * @param[in] i2c_library
 * @param[in] i2c_address
 * @param[in] pin_res
 */
int st75256::setup(TwoWire& i2c_library, const uint8_t i2c_address, const int pin_res) {

    /* Ensure i2c address is valid */
    if ((i2c_address != 0x3C) &&  //
        (i2c_address != 0x3D) &&  //
        (i2c_address != 0x3E) &&  //
        (i2c_address != 0x3F)) {
        return -EINVAL;
    }

    /* Save parameters */
    m_interface = INTERFACE_I2C_LIGHT;
    m_i2c_library = &i2c_library;
    m_i2c_address = i2c_address;

    /* Perform reset */
    pinMode(pin_res, OUTPUT);
    digitalWrite(pin_res, LOW);
    delay(2);
    digitalWrite(pin_res, HIGH);
    delay(2);

    /* Configure driver */
    uint16_t Contrast = 20;
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
#if 0
    /* Configure driver */
    int res = 0;
    res |= command_send(COMMAND_EXTENSION_1);
    res |= command_send(COMMAND_SLEEP_OUT);
    res |= command_send(COMMAND_DISPLAY_OFF);
    res |= command_send(COMMAND_EXTENSION_2);
    res |= command_send(0xD7);  // disable auto read
    res |= data_send(0x9F);     // disable auto read
    res |= command_send(0x32);  // analog circuit set
    res |= data_send(0x00);     // code example: OSC Frequency adjustment
    res |= data_send(0x01);     // Frequency on booster capacitors 1 = 6KHz?
    res |= data_send(0x03);     // Bias: 1: 1/13, 2: 1/12, 3: 1/11, 4:1/10, 5:1/9
    res |= command_send(0x20);  // gray levels
    res |= data_send(0x01);
    res |= data_send(0x03);
    res |= data_send(0x05);
    res |= data_send(0x07);
    res |= data_send(0x09);
    res |= data_send(0x0b);
    res |= data_send(0x0d);
    res |= data_send(0x10);
    res |= data_send(0x11);
    res |= data_send(0x13);
    res |= data_send(0x15);
    res |= data_send(0x17);
    res |= data_send(0x19);
    res |= data_send(0x1b);
    res |= data_send(0x1d);
    res |= data_send(0x1f);
    res |= command_send(COMMAND_EXTENSION_1);
    res |= command_send(0x75);  // row range
    res |= data_send(0);        // row range
    res |= data_send(0x4f);     // row range
    res |= command_send(21);    // col range TOOO Maybe change?
    res |= data_send(0);        // col range
    res |= data_send(255);      // col range TOOO Maybe change?
    res |= command_send(0xbc);  // Data Scan Direction
    res |= data_send(0x00);     // Data Scan Direction
    res |= data_send(0xa6);     // ???
    res |= command_send(0x0c);  // data format LSB top
    res |= command_send(0xca);  // display control; 3 args follow
    res |= data_send(0x00);     // 0x00: no clock division; 0x04: devide clock
    res |= data_send(0x9f);     // 1/160 duty value from the DS example code
    res |= data_send(0x20);     // nline off
    res |= command_send(0xf0);  // monochrome mode  = 0x10
    res |= command_send(0x10);  // monochrome mode  = 0x10
    res |= command_send(0x81);  // Volume control
    res |= data_send(0x2e);     // Volume control
    res |= data_send(0x03);     // Volume control
    res |= command_send(0x20);  // Power control: Regulator; follower & booster on
    res |= data_send(0x0b);     // Power control: Regulator; follower & booster on
    if (res != 0) {
        return -EIO;
    }
#endif

    /* */
    delay(100);

    /* Return success */
    return 0;
}
#endif

/**
 *
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
 *
 * @param[in] ratio
 * @return
 */
int st75256::brightness_set(const float ratio) {
    if (ratio < 0 || ratio > 1) {
        return -EINVAL;
    }
    // TODO return command_send(COMMAND_CONTRAST_SET, (uint8_t)(percent * 255));
    return -1;
}

/**
 *
 */
int st75256::inverted_set(const bool inverted) {
    // TODO return command_send(inverted ? COMMAND_INVERSION_ENABLED : COMMAND_INVERSION_DISABLED);
    return -1;
}
void st75256::invertDisplay(bool i) {
    inverted_set(i);
}

/**
 * @return
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
 *
 * @param[in] x
 * @param[in] y
 * @param[in] color
 * @return
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
void st75256::drawPixel(int16_t x, int16_t y, uint16_t color) {
    pixel_set(x, y, color);
}

/**
 *
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
 *
 * @param[in] command
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
 *
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
 *
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
 *
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
