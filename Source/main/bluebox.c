/**
 * @file bluebox.c
 * @version 0.9
 * @author  Cbaurtx
 * @date    11.12.2025
 * @copyright cbaurtx
 * @license MIT License, read LICENSE.md
 * @brief    BlueBox Bluetooth sink and I2S source for RetroBox
 * @details
 * BlueBox is a Bluetooth sink to be connected via I2S to RetroBox
 * Uses the Espressif ADF audio framework version 2.7. and has been adapted from
 * the Espressif player/pipeline_a2dp_sink_stream example
 */

#include <string.h>

#include "esp_log.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_bt_device.h"
#include "esp_gap_bt_api.h"
#include "esp_a2dp_api.h"
#include "esp_avrc_api.h"
#include "esp_peripherals.h"
#include "esp_timer.h"

#include "nvs_flash.h"
#include "esp_spiffs.h"
#include "periph_spiffs.h"
#include "esp_peripherals.h"

#include "audio_mem.h"
#include "audio_element.h"
#include "audio_pipeline.h"
#include "audio_event_iface.h"
#include "bluetooth_service.h"
#include "i2s_stream.h"
#include "spiffs_stream.h"
#include "filter_resample.h"
#include "mp3_decoder.h"
#include "periph_touch.h"
#include "board.h"
#include "a2dp_stream.h"

/* Announcements are in these files */
#define NO_BLUETOOTH "/spiffs/no_bluetooth.mp3"
#define CONNECTED "/spiffs/connected.mp3"

static const char* TAG = "bluebox.c";


/**
 * @brief AVRCP Controller (CT) callback
 *
 * AVRCP (Audio / Video Remote Control Profile) is not supported (no need for that) and
 * we only log AVRCP events
 *
 * @param[in]  esp_avrc_ct_cb_event_t event
 *             AVRC Controller callback events (refer to esp_avrc_api.h)
 *
 * @param[in] esp_avrc_ct_cb_param_t* p_param
 *            AVRC controller callback parameters (refer to esp_avrc_api.h)
 *
 * @return void
 *
 * @note Needed as we must register a callback
 */
static void bt_app_avrc_ct_cb(esp_avrc_ct_cb_event_t event, esp_avrc_ct_cb_param_t* p_param)
{
    esp_avrc_ct_cb_param_t* rc = p_param;
    switch (event)
    {
        case ESP_AVRC_CT_METADATA_RSP_EVT:
        {
            uint8_t* tmp = audio_calloc(1, rc->meta_rsp.attr_length + 1);
            memcpy(tmp, rc->meta_rsp.attr_text, rc->meta_rsp.attr_length);
            ESP_LOGI(TAG, "AVRC metadata rsp: attribute id 0x%x, %s", rc->meta_rsp.attr_id, tmp);
            audio_free(tmp);
            break;
        }
        default:
            break;
    }
}



void app_main(void)
{
    bool play_no_bluetooth = true;          // Output audio indicating that Bluetooth is not connected
    bool play_connected    = false;         // Output audio indicating that Bluetooth has been connected

    audio_pipeline_handle_t pipeline;
    audio_element_handle_t  bt_stream_reader, i2s_stream_writer, spiffs_stream_reader, mp3_decoder;
    audio_element_status_t  audio_spiffs_stat;

    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES)
    {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }

    esp_log_level_set("*", ESP_LOG_INFO);
    esp_log_level_set(TAG, ESP_LOG_DEBUG);

    ESP_LOGI(TAG, "[ 1 ] Create Bluetooth service");
    bluetooth_service_cfg_t bt_cfg = {
        .device_name                   = "BlueBox",
        .mode                          = BLUETOOTH_A2DP_SINK,
        .user_callback.user_avrc_ct_cb = bt_app_avrc_ct_cb,
    };
    bluetooth_service_start(&bt_cfg);

    ESP_LOGI(TAG, "[ 2 ] Start codec chip");
    audio_board_handle_t board_handle = audio_board_init();
    audio_hal_ctrl_codec(board_handle->audio_hal, AUDIO_HAL_CODEC_MODE_DECODE, AUDIO_HAL_CTRL_START);

    ESP_LOGI(TAG, "[ 3 ] Create audio pipeline for playback");
    audio_pipeline_cfg_t pipeline_cfg = DEFAULT_AUDIO_PIPELINE_CONFIG();
    pipeline                          = audio_pipeline_init(&pipeline_cfg);

    ESP_LOGI(TAG, "[4] Create I2S stream to write data to codec chip");
    i2s_stream_cfg_t i2s_cfg = I2S_STREAM_CFG_DEFAULT();
    i2s_cfg.type             = AUDIO_STREAM_WRITER;
    i2s_cfg.buffer_len       = 3600 * 6; // must be multiple of 3; default 3600
    i2s_cfg.stack_in_ext     = true;     // use PSRAM as stack/buffer is large
    i2s_cfg.task_stack       = 8 * 1024;

    i2s_stream_writer = i2s_stream_init(&i2s_cfg);

    ESP_LOGI(TAG, "[4.1] Get Bluetooth stream");
    bt_stream_reader = bluetooth_service_create_stream();

    ESP_LOGI(TAG, "[4.2] Create spiffs stream");
    spiffs_stream_cfg_t flash_cfg = SPIFFS_STREAM_CFG_DEFAULT();
    flash_cfg.type                = AUDIO_STREAM_READER;
    flash_cfg.out_rb_size         = 24 * 1024;      // size of largest announcement file
    spiffs_stream_reader          = spiffs_stream_init(&flash_cfg);

    ESP_LOGI(TAG, "[2.5] Create MP3 decoder to decode audio files (from SPIFFS)");
    mp3_decoder_cfg_t mp3_cfg = DEFAULT_MP3_DECODER_CONFIG();
    mp3_decoder               = mp3_decoder_init(&mp3_cfg);

    ESP_LOGI(TAG, "[4.3] Register all elements to audio pipeline");
    audio_pipeline_register(pipeline, bt_stream_reader, "bt");
    audio_pipeline_register(pipeline, i2s_stream_writer, "i2s_out");
    audio_pipeline_register(pipeline, spiffs_stream_reader, "spif");
    audio_pipeline_register(pipeline, mp3_decoder, "mp3");

    ESP_LOGI(TAG, "[4.3] Link it together [Bluetooth]-->bt_stream_reader-->i2s_stream_writer-->[codec_chip]");

    /* Constant with link patterns. One to play files on SPIFFS, the other one to play a Bluetoothstream */
    static const char* link_tags[][3] = {{"bt", "i2s_out", ""}, {"spif", "mp3", "i2s_out"}};

    /* At start-up Bluetooth cannot be connected */
    audio_pipeline_link(pipeline, &link_tags[1][0], 3);

    ESP_LOGI(TAG, "[ 5 ] Initialize peripherals");
    esp_periph_config_t     periph_cfg = DEFAULT_ESP_PERIPH_SET_CONFIG();
    esp_periph_set_handle_t set        = esp_periph_set_init(&periph_cfg);

    ESP_LOGI(TAG, "[5.1] Create Bluetooth peripheral");
    esp_periph_handle_t bt_periph = bluetooth_service_create_periph();

    ESP_LOGI(TAG, "[5.2] Start  Bluetooth service");
    esp_periph_start(set, bt_periph);

    ESP_LOGI(TAG, "[5.3] Initialize and mount SPIFFS");
    periph_spiffs_cfg_t spiffs_cfg = {
        .root = "/spiffs", .partition_label = NULL, .max_files = 5, .format_if_mount_failed = true};
    esp_periph_handle_t spiffs_handle = periph_spiffs_init(&spiffs_cfg);

    ESP_LOGI(TAG, "[5.4] start SPIFFS service");
    esp_periph_start(set, spiffs_handle);

    // Wait until spiffs is mounted (just wiating here is OK as this runs only during initialization
    while (!periph_spiffs_is_mounted(spiffs_handle))
    {
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }

    ESP_LOGI(TAG, "[ 6 ] Set up  event listeners");
    audio_event_iface_cfg_t    evt_cfg = AUDIO_EVENT_IFACE_DEFAULT_CFG();
    audio_event_iface_handle_t evt     = audio_event_iface_init(&evt_cfg);

    ESP_LOGI(TAG, "[6.1] Listening event from all elements of pipeline");
    audio_pipeline_set_listener(pipeline, evt);
    audio_element_set_uri(spiffs_stream_reader, NO_BLUETOOTH);

    ESP_LOGI(TAG, "[6.2] Listening event from peripherals");
    audio_event_iface_set_listener(esp_periph_set_get_event_iface(set), evt);

    ESP_LOGI(TAG, "[ 7 ] Start audio_pipeline");
    audio_pipeline_run(pipeline);

    ESP_LOGI(TAG, "[ 8 ] Listen for all pipeline events");
    ESP_LOGI(TAG, "bt_stream_reader %p, i2s_stream_writer %p, spiffs_stream_reader %p", bt_stream_reader,
             i2s_stream_writer, spiffs_stream_reader);
    ESP_LOGI(TAG, "mp3_decoder %p", mp3_decoder);


    /* Main app event loop */
    while (1)
    {
        audio_event_iface_msg_t msg;
        esp_err_t               ret;

        ret = audio_event_iface_listen(evt, &msg, portMAX_DELAY);
        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "[ * ] Event interface error : %d", ret);
            continue;
        }
        ESP_LOGI(TAG, "Main event loop received event: type %x; source %p; cmd %x, data %x", msg.source_type,
                 msg.source, msg.cmd, (int) msg.data);
        ESP_LOGI(TAG, "Play No Bluetooth, %d", (int) play_no_bluetooth);
        ESP_LOGI(TAG, "Play Connected, %d", (int) play_connected);

        /* Bluetooth events */
        if ((msg.source_type == PERIPH_ID_BLUETOOTH) && (msg.source == (void*) bt_periph))
        {
            ESP_LOGI(TAG, "Mainloop PERIPH BLUETOOTH event");
            switch ((int) msg.cmd)
            {
                case PERIPH_BLUETOOTH_CONNECTED:  // Bluetooth has been connected, play announcement file
                    ESP_LOGI(TAG, "Mainloop Bluetooth connected");
                    play_no_bluetooth = false;
                    play_connected    = true;

                    audio_pipeline_stop(pipeline);
                    audio_pipeline_wait_for_stop(pipeline);
                    audio_pipeline_breakup_elements(pipeline, spiffs_stream_reader);
                    audio_element_reset_output_ringbuf(spiffs_stream_reader);
                    audio_element_set_uri(spiffs_stream_reader, CONNECTED);
                    audio_pipeline_relink(pipeline, &link_tags[1][0], 3);
                    audio_pipeline_set_listener(pipeline, evt);
                    audio_pipeline_run(pipeline);
                    break;

                case PERIPH_BLUETOOTH_DISCONNECTED: // Bluetooth is disconnected, play announcement file
                    ESP_LOGI(TAG, "Mainloop Bluetooth disconnected");
                    play_no_bluetooth = true;
                    play_connected    = false;

                    audio_pipeline_stop(pipeline);
                    audio_pipeline_wait_for_stop(pipeline);
                    audio_pipeline_breakup_elements(pipeline, spiffs_stream_reader);
                    audio_element_reset_output_ringbuf(spiffs_stream_reader);
                    audio_element_set_uri(spiffs_stream_reader, NO_BLUETOOTH);
                    audio_pipeline_relink(pipeline, &link_tags[1][0], 3);
                    audio_pipeline_set_listener(pipeline, evt);
                    audio_pipeline_run(pipeline);
                    break;

                default:
                    ESP_LOGI(TAG, "Other Bluetooth event: cmd=%d", msg.cmd);
                    break;
            }
            continue;
        }

        /* I2S writer finished events */
        if (msg.source_type == AUDIO_ELEMENT_TYPE_ELEMENT && msg.source == (void*) i2s_stream_writer &&
            msg.cmd == AEL_MSG_CMD_REPORT_STATUS && (int) msg.data == AEL_STATUS_STATE_FINISHED)
        {
            if (play_no_bluetooth == true)       // replay announcment file, so reset the audio pipeline
            {
                ESP_LOGI(TAG, "Replay No Bluetooth");
                audio_pipeline_stop(pipeline);
                audio_pipeline_wait_for_stop(pipeline);
                audio_pipeline_breakup_elements(pipeline, spiffs_stream_reader);
                audio_element_reset_output_ringbuf(spiffs_stream_reader);
                audio_pipeline_relink(pipeline, &link_tags[1][0], 3);
                audio_pipeline_set_listener(pipeline, evt);
                audio_pipeline_run(pipeline);
                continue;
            }
            if (play_connected == true)  // announcement file was played once, so switch to Bluetooth stream
            {
                ESP_LOGI(TAG, "switch to Bluetooth stream");
                play_connected = false;
                audio_pipeline_stop(pipeline);
                audio_pipeline_wait_for_stop(pipeline);
                audio_pipeline_breakup_elements(pipeline, spiffs_stream_reader);
                audio_element_reset_output_ringbuf(spiffs_stream_reader);
                audio_pipeline_relink(pipeline, &link_tags[0][0], 2);

                audio_pipeline_set_listener(pipeline, evt);
                audio_pipeline_run(pipeline);

                audio_element_info_t music_info = {0};
                audio_element_getinfo(bt_stream_reader, &music_info);

                ESP_LOGI(TAG, "[ * ] Receive music info from Bluetooth, sample_rates=%d, bits=%d, ch=%d",
                         music_info.sample_rates, music_info.bits, music_info.channels);

                audio_element_set_music_info(i2s_stream_writer, music_info.sample_rates, music_info.channels,
                                             music_info.bits);
                i2s_stream_set_clk(i2s_stream_writer, music_info.sample_rates, music_info.bits, music_info.channels);
                ESP_LOGI(TAG, "Sample rate set to %d", music_info.sample_rates);
            }
        }
    }
}
