#ifndef _UD_MCP9808_H_
#define _UD_MCP9808_H_

#include "driver/i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    uint8_t address;
    i2c_port_t i2c_num;
} mcp9808_config_t;

typedef void* mcp9808_handle_t;

esp_err_t mcp9808_init(const mcp9808_config_t* conf, mcp9808_handle_t* handle, uint16_t* manuf_id, uint16_t* dev_id);

esp_err_t mcp9808_ambient_temp(mcp9808_handle_t handle, float* res);

void mcp9808_delete(mcp9808_handle_t handle);

#ifdef __cplusplus
}
#endif

#endif /* _UD_MCP9808_H_*/
