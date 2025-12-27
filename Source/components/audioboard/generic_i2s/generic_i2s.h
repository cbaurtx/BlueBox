/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2020 <ESPRESSIF SYSTEMS (SHANGHAI) CO., LTD>
 *
 * Permission is hereby granted for use on all ESPRESSIF SYSTEMS products, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef _GENERIC_I2S_H_
#define _GENERIC_I2S_H_

#include "audio_hal.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

#define VOLUME_CONTROL 0

/**
 * @brief Initialize I2S
 *
 * @param cfg configuration of I2S
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t generic_i2s_init(audio_hal_codec_config_t *codec_cfg);

/**
 * @brief Deinitialize generic I2S
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t generic_i2s_deinit(void);


/**
 * @brief  Set voice volume
 *
 * @param volume:  voice volume (0~100)
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t generic_i2s_set_volume(int vol);

/**
 * @brief Get voice volume
 *
 * @param[out] *volume:  voice volume (0~100)
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t generic_i2s_get_volume(int *value);

/**
 * @brief Set mute or not
 *
 * @param enable enable(1) or disable(0)
 *
 * @return
 *     - ESP_FAIL Parameter error
 *     - ESP_OK   Success
 */
esp_err_t generic_i2s_set_mute(bool enable);

/**
 * @brief Mute gradually by (value)ms
 *
 * @param value  Time for mute with millisecond.
 * @return
 *     - ESP_FAIL Parameter error
 *     - ESP_OK   Success
 *
 */
esp_err_t generic_i2s_set_mute_fade(int value);

/**
 * @brief Get mute status
 *
 *  @return
 *     - ESP_FAIL Parameter error
 *     - ESP_OK   Success
 */
esp_err_t generic_i2s_get_mute(int *value);

/**
 * @brief Set DAMP mode
 *
 * @param value  DAMP_MODE_BTL or DAMP_MODE_PBTL
 *
 * Generic I2S does not have a damp mode
 *
 * @return
 *     - ESP_FAIL Parameter error
 *     - ESP_OK   Success
 *
 */
esp_err_t generic_i2s_set_damp_mode(int value);

#ifdef __cplusplus
}
#endif

#endif
