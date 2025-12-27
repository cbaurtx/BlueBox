/**
 * @file generic_i2s.c
 * @version 0.9
 * @author  Cbaurtx
 * @date    21.04.2025
 * @copyright cbaurtx
 * @license MIT License, read LICENSE.md
 * @brief Generic driver for i2s
 * @details
 */

#include "generic_i2s.h"
#include "board.h"
#include "esp_log.h"
#include "audio_volume.h"

static const char* TAG = "generic_i2s.c";

#define PCM5102A_ASSERT(a, format, b, ...)                                                                             \
    if ((a) != 0)                                                                                                      \
    {                                                                                                                  \
        ESP_LOGE(TAG, format, ##__VA_ARGS__);                                                                          \
        return b;                                                                                                      \
    }

esp_err_t generic_i2s_ctrl(audio_hal_codec_mode_t mode, audio_hal_ctrl_t ctrl_state);
esp_err_t generic_i2s_config_iface(audio_hal_codec_mode_t mode, audio_hal_codec_i2s_iface_t* iface);

audio_hal_func_t AUDIO_CODEC_PCM5102A_DEFAULT_HANDLE = {
    .audio_codec_initialize   = generic_i2s_init,
    .audio_codec_deinitialize = generic_i2s_deinit,
    .audio_codec_ctrl         = generic_i2s_ctrl,
    .audio_codec_config_iface = generic_i2s_config_iface,
    .audio_codec_set_mute     = generic_i2s_set_mute,
    .audio_codec_set_volume   = generic_i2s_set_volume,
    .audio_codec_get_volume   = generic_i2s_get_volume,
    .audio_codec_enable_pa    = NULL,
    .audio_hal_lock           = NULL,
    .handle                   = NULL,
};

/**
 * @brief Initialize PCM5102A codec
 * @details
 * Only the GPIO pin for the mute signal needs to be setuo
 * @param[in] codec_cfg
 * @return
 *         - ESP_OK
 */
esp_err_t generic_i2s_init(audio_hal_codec_config_t* codec_cfg)
{
    esp_err_t err = ESP_OK;
    return (err);
}

/**
 * @brief Set mute / unmute
 * @details
 * Set the mute GPIO to mute / unmute the PCM5102A
 * @return
 *         - ESP_OK
 */
esp_err_t generic_i2s_set_mute(bool enable)
{
    ESP_LOGI (TAG, "Generic I2S does not support mute");
    esp_err_t err = ESP_OK;
    return (err);
}

/**
 * @brief Get mute state
 * @details
 * Get the state of the mute GPIO
 * @param[out] value muted / not muted
 * @return
 *         - ESP_OK
 */
esp_err_t generic_i2s_get_mute(int* value)
{
    *value = 1;
    ESP_LOGI(TAG, "Generig I2S does not support mute");
    return (ESP_OK);
}

/**
 * @brief Deinitialize i2s
 * @details
 * This function has no effect.
 * @return
 *         - ESP_OK
 */
esp_err_t generic_i2s_deinit(void)
{
    // nothing to do
    ESP_LOGW(TAG, "generic_i2s_deinit n.a.");
    return ESP_OK;
}

/**
 * @brief Control the genric I2S
 * @details
 * This function has no effect.
 * @return
 *         - ESP_OK
 */
esp_err_t generic_i2s_ctrl(audio_hal_codec_mode_t mode, audio_hal_ctrl_t ctrl_state)
{
    // Generiv I2S does not have control registers
    ESP_LOGW(TAG, "generic_i2s_ctrl n.a.");
    return ESP_OK;
}

esp_err_t generic_i2s_config_iface(audio_hal_codec_mode_t mode, audio_hal_codec_i2s_iface_t* iface)
{
    // TODO
    ESP_LOGW(TAG, "generic_i2s_config_iface not implemented");
    return ESP_OK;
}

/**
 * @brief Set volume
 * @details
 * This function has no effect as the PCM5102A has no volume control.
 * param[in] volume
 * @return
 *         - ESP_OK
 */
esp_err_t generic_i2s_set_volume(int volume)
{ // generic_i2s has no volume control
    ESP_LOGW(TAG, "Generic I2S does not support volume control");
    return ESP_OK;
}

/**
 * @brief Get volume
 * @details
 * This function sets the param volume tp -1 (invald)
 * param[in] volume
 * @return
 *         - ESP_OK
 */
esp_err_t generic_i2s_get_volume(int* volume)
{
    *volume = -1;
    ESP_LOGW(TAG, "Generic I2S does not support volume control");
    return ESP_OK;
}
