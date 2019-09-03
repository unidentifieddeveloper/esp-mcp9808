# MCP9808 driver for ESP32 (ESP-IDF)

An MCP9808 digital temperature sensor driver for the Espressif Systems' ESP32.

## Overview

This is a simple driver which builds upon the ESP-IDF I2C driver and adds a few
functions to make it easy to read ambient temperature, etc.

*It does not configure or install the I2C driver - this must be done by the user.*

## Example usage

Add the repository as a submodule (or clone it) into your `components/` directory.

```sh
$ git submodule add https://github.com/unidentifieddeveloper/esp-mcp9808 components/mcp9808
```

Next, setup I2C and then init the MCP9808 driver and read ambient temperature.

```c
#include "mcp9808.h"

void app_main()
{
    // TODO: Setup I2C as you see fit
    // - i2c_param_config
    // - i2c_driver_install

    mcp9808_config_t mcp_config;
    mcp_config.address = 0x18;
    mcp_config.i2c_num = I2C_NUM_1;

    mcp9808_handle_t mcp;
    uint16_t mcp_manuf_id;
    uint16_t mcp_device_id;

    if (mcp9808_init(&mcp_config, &mcp, &mcp_manuf_id, &mcp_device_id) != ESP_OK)
    {
        return;
    }

    float temperature;

    if (mcp9808_ambient_temp(mcp, &temperature) != ESP_OK)
    {
        return;
    }

    ESP_LOGI(TAG, "Temperature: %f", temperature);

    mcp9808_delete(mcp);
}
```
