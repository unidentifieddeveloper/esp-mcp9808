#include "mcp9808.h"

#define MCP9808_I2CADDR_DEFAULT 0x18 ///< I2C address
#define MCP9808_REG_CONFIG 0x01      ///< MCP9808 config register

#define MCP9808_REG_CONFIG_SHUTDOWN 0x0100   ///< shutdown config
#define MCP9808_REG_CONFIG_CRITLOCKED 0x0080 ///< critical trip lock
#define MCP9808_REG_CONFIG_WINLOCKED 0x0040  ///< alarm window lock
#define MCP9808_REG_CONFIG_INTCLR 0x0020     ///< interrupt clear
#define MCP9808_REG_CONFIG_ALERTSTAT 0x0010  ///< alert output status
#define MCP9808_REG_CONFIG_ALERTCTRL 0x0008  ///< alert output control
#define MCP9808_REG_CONFIG_ALERTSEL 0x0004   ///< alert output select
#define MCP9808_REG_CONFIG_ALERTPOL 0x0002   ///< alert output polarity
#define MCP9808_REG_CONFIG_ALERTMODE 0x0001  ///< alert output mode

#define MCP9808_REG_UPPER_TEMP 0x02   ///< upper alert boundary
#define MCP9808_REG_LOWER_TEMP 0x03   ///< lower alert boundery
#define MCP9808_REG_CRIT_TEMP 0x04    ///< critical temperature
#define MCP9808_REG_AMBIENT_TEMP 0x05 ///< ambient temperature
#define MCP9808_REG_MANUF_ID 0x06     ///< manufacture ID
#define MCP9808_REG_DEVICE_ID 0x07    ///< device ID
#define MCP9808_REG_RESOLUTION 0x08   ///< resolutin

#define ACK_CHECK_EN 0x1                        /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS 0x0                       /*!< I2C master will not check ack from slave */
#define ACK_VAL 0x0                             /*!< I2C ack value */
#define NACK_VAL 0x1                            /*!< I2C nack value */

typedef struct
{
    uint8_t address;
    i2c_port_t i2c_num;
} mcp9808_desc_t;

esp_err_t mcp9808_read16(const mcp9808_desc_t* desc, uint8_t reg, uint16_t* res)
{
    uint8_t upper;
    uint8_t lower;
    int ret;

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, 0x30, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, reg, ACK_CHECK_EN);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, 0x31, ACK_CHECK_EN);
    i2c_master_read_byte(cmd, &upper, ACK_VAL);
    i2c_master_read_byte(cmd, &lower, NACK_VAL);
    i2c_master_stop(cmd);

    ret = i2c_master_cmd_begin(desc->i2c_num, cmd, 1000 / portTICK_RATE_MS);

    i2c_cmd_link_delete(cmd);

    if (ret != ESP_OK)
    {
        return ESP_FAIL;
    }

    *res = (upper << 8 ) | (lower & 0xff);

    return ESP_OK;
}

esp_err_t mcp9808_read8(const mcp9808_desc_t* desc, uint8_t reg, uint8_t* res)
{
    uint8_t result;
    int ret;

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, 0x30, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, reg, ACK_CHECK_EN);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, 0x31, ACK_CHECK_EN);
    i2c_master_read_byte(cmd, &result, NACK_VAL);
    i2c_master_stop(cmd);

    ret = i2c_master_cmd_begin(desc->i2c_num, cmd, 1000 / portTICK_RATE_MS);

    i2c_cmd_link_delete(cmd);

    if (ret != ESP_OK)
    {
        return ESP_FAIL;
    }

    *res = result;

    return ESP_OK;
}

void mcp9808_delete(mcp9808_handle_t handle)
{
    if (handle == NULL)
    {
        return;
    }

    free(handle);
}

esp_err_t mcp9808_init(const mcp9808_config_t* conf, mcp9808_handle_t* handle, uint16_t* manuf_id, uint16_t* dev_id)
{
    mcp9808_desc_t* desc = (mcp9808_desc_t*) calloc(1, sizeof(mcp9808_desc_t));
    desc->address = conf->address;
    desc->i2c_num = conf->i2c_num;

    uint16_t mid;
    uint16_t did;

    if (mcp9808_read16(desc, MCP9808_REG_MANUF_ID, &mid) != ESP_OK)
    {
        return ESP_FAIL;
    }

    if (mcp9808_read16(desc, MCP9808_REG_DEVICE_ID, &did) != ESP_OK)
    {
        return ESP_FAIL;
    }

    *handle = (mcp9808_handle_t*) desc;
    *manuf_id = mid;
    *dev_id = did;

    return ESP_OK;
}

esp_err_t mcp9808_ambient_temp(mcp9808_handle_t handle, float* res)
{
    mcp9808_desc_t* desc = (mcp9808_desc_t*) handle;

    float temp = 0.0;
    uint16_t val;

    if (mcp9808_read16(desc, MCP9808_REG_AMBIENT_TEMP, &val) != ESP_OK)
    {
        return ESP_FAIL;
    }

    if (val == 0xFFFF)
    {
        return ESP_FAIL;
    }

    temp = val & 0x0FFF;
    temp /= 16.0;

    if (val & 0x1000)
    {
        temp -= 256;
    }

    *res = temp;

    return ESP_OK;
}
