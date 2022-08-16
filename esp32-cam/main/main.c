// Choose the board by uncommenting one of the next two lines
// #define BOARD_WROVER_KIT
#define BOARD_ESP32CAM_AITHINKER

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "protocol_examples_common.h"

#include "esp_camera.h"

#ifdef BOARD_WROVER_KIT

#define CAM_PIN_PWDN -1
#define CAM_PIN_RESET -1
#define CAM_PIN_XCLK 21
#define CAM_PIN_SIOD 26
#define CAM_PIN_SIOC 27
#define CAM_PIN_D7 35
#define CAM_PIN_D6 34
#define CAM_PIN_D5 39
#define CAM_PIN_D4 36
#define CAM_PIN_D3 19
#define CAM_PIN_D2 18
#define CAM_PIN_D1 5
#define CAM_PIN_D0 4
#define CAM_PIN_VSYNC 25
#define CAM_PIN_HREF 23
#define CAM_PIN_PCLK 22

#endif

#ifdef BOARD_ESP32CAM_AITHINKER

#define CAM_PIN_PWDN 32
#define CAM_PIN_RESET -1
#define CAM_PIN_XCLK 0
#define CAM_PIN_SIOD 26
#define CAM_PIN_SIOC 27
#define CAM_PIN_D7 35
#define CAM_PIN_D6 34
#define CAM_PIN_D5 39
#define CAM_PIN_D4 36
#define CAM_PIN_D3 21
#define CAM_PIN_D2 19
#define CAM_PIN_D1 18
#define CAM_PIN_D0 5
#define CAM_PIN_VSYNC 25
#define CAM_PIN_HREF 23
#define CAM_PIN_PCLK 22

#endif

#include "discord.h"
#include "discord/session.h"
#include "discord/message.h"
#include "estr.h"

static const char* TAG = "discord_bot";

static discord_handle_t bot;

static camera_config_t camera_config = {
    .pin_pwdn = CAM_PIN_PWDN,
    .pin_reset = CAM_PIN_RESET,
    .pin_xclk = CAM_PIN_XCLK,
    .pin_sscb_sda = CAM_PIN_SIOD,
    .pin_sscb_scl = CAM_PIN_SIOC,
    .pin_d7 = CAM_PIN_D7,
    .pin_d6 = CAM_PIN_D6,
    .pin_d5 = CAM_PIN_D5,
    .pin_d4 = CAM_PIN_D4,
    .pin_d3 = CAM_PIN_D3,
    .pin_d2 = CAM_PIN_D2,
    .pin_d1 = CAM_PIN_D1,
    .pin_d0 = CAM_PIN_D0,
    .pin_vsync = CAM_PIN_VSYNC,
    .pin_href = CAM_PIN_HREF,
    .pin_pclk = CAM_PIN_PCLK,
    .xclk_freq_hz = 20000000,
    .ledc_timer = LEDC_TIMER_0,
    .ledc_channel = LEDC_CHANNEL_0,
    .pixel_format = PIXFORMAT_JPEG,
    .frame_size = FRAMESIZE_HD,
    .jpeg_quality = 12,
    .fb_count = 2,
    .grab_mode = CAMERA_GRAB_LATEST,
};

static void bot_event_handler(void* handler_arg, esp_event_base_t base, int32_t event_id, void* event_data) {
    discord_event_data_t* data = (discord_event_data_t*) event_data;

    switch(event_id) {
        case DISCORD_EVENT_CONNECTED: {
                discord_session_t* session = (discord_session_t*) data->ptr;
                ESP_LOGI(TAG, "Bot %s#%s connected", session->user->username, session->user->discriminator);
                ESP_LOGI(TAG, "Open Discord and send message \"!camera\" to get an image.");
            }
            break;
        
        case DISCORD_EVENT_MESSAGE_RECEIVED: {
                discord_message_t* msg = (discord_message_t*) data->ptr;
                discord_message_dump_log(ESP_LOGI, TAG, msg);

                if(! estr_eq(msg->content, "!camera")) {
                    break;
                }

                camera_fb_t* fb = esp_camera_fb_get();

                if (!fb) {
                    ESP_LOGE(TAG, "Camera capture failed");
                    break;
                }

                discord_message_t* message = cu_ctor(discord_message_t,
                    .channel_id = strdup(msg->channel_id),
                );

                discord_attachment_t* image = cu_ctor(discord_attachment_t,
                    .content_type = strdup("image/jpeg"),
                    .filename     = strdup("image.jpg"),
                    ._data        = (char*) fb->buf,
                    .size         = fb->len,
                );

                discord_message_add_attachment(message, image);

                discord_message_add_embed(message, cu_ctor(discord_embed_t,
                    .title = strdup("Camera"),
                    .color = DISCORD_COLOR_BLUE,
                    .image = cu_ctor(discord_embed_image_t,
                        .url = discord_attachment_refence(image),
                    ),
                    .footer = cu_ctor(discord_embed_footer_t, 
                        .text = strdup("Image taken with ESP32-CAM"),
                    ),
                ));

                esp_err_t err = discord_message_send(bot, message, NULL);
                discord_message_free(message);

                esp_camera_fb_return(fb);

                if(err == ESP_OK) {
                    ESP_LOGI(TAG, "Image has been successfully sent");
                } else {
                    ESP_LOGE(TAG, "Fail to send an image");
                }
            }
            break;
        
        case DISCORD_EVENT_DISCONNECTED:
            ESP_LOGW(TAG, "Bot logged out");
            break;
    }
}

void app_main(void) {
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(example_connect());

    ESP_ERROR_CHECK(esp_camera_init(&camera_config));

    discord_config_t cfg = {
        .intents = DISCORD_INTENT_GUILD_MESSAGES | DISCORD_INTENT_MESSAGE_CONTENT
    };

    bot = discord_create(&cfg);
    ESP_ERROR_CHECK(discord_register_events(bot, DISCORD_EVENT_ANY, bot_event_handler, NULL));
    ESP_ERROR_CHECK(discord_login(bot));
}