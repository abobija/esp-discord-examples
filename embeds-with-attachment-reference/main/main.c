#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "protocol_examples_common.h"

#include "discord.h"
#include "discord/session.h"
#include "discord/message.h"
#include "estr.h"

static const char* TAG = "discord_bot";

static discord_handle_t bot;

extern const uint8_t esp32_start[] asm("_binary_esp32_png_start");
extern const uint8_t esp32_end[]   asm("_binary_esp32_png_end");

static void bot_event_handler(void* handler_arg, esp_event_base_t base, int32_t event_id, void* event_data) {
    discord_event_data_t* data = (discord_event_data_t*) event_data;

    switch(event_id) {
        case DISCORD_EVENT_CONNECTED: {
                discord_session_t* session = (discord_session_t*) data->ptr;
                ESP_LOGI(TAG, "Bot %s#%s connected", session->user->username, session->user->discriminator);
                ESP_LOGI(TAG, "Send \"!embed\" message on Discord in order to get embed from the Bot.");
            }
            break;
        
        case DISCORD_EVENT_MESSAGE_RECEIVED: {
                discord_message_t* msg = (discord_message_t*) data->ptr;
                discord_message_dump_log(ESP_LOGI, TAG, msg);

                // Bot will send embed to you only when you send "!embed" to him

                if(! estr_eq(msg->content, "!embed")) { 
                    break;
                }

                discord_message_t* message = cu_ctor(discord_message_t,
                    .content    = strdup("Hey, check this awesome ESP32 chip!"),
                    .channel_id = strdup(msg->channel_id),
                );

                discord_attachment_t* esp32 = cu_ctor(discord_attachment_t,
                    .content_type = strdup("image/png"),
                    .filename     = strdup("esp32.png"),
                    ._data        = (char*) esp32_start,
                    .size         = esp32_end - esp32_start,
                );

                discord_message_add_attachment(message, esp32);

                discord_message_add_embed(message, cu_ctor(discord_embed_t,
                    .title = strdup("ESP32 logo"),
                    .description = strdup(
                        "A feature-rich MCU with integrated Wi-Fi and "
                        "Bluetooth connectivity for a wide-range of applications"
                    ),
                    .color = DISCORD_COLOR_PURPLE,
                    .image = cu_ctor(discord_embed_image_t,
                        .url = discord_attachment_refence(esp32),
                    ),
                ));

                esp_err_t err = discord_message_send(bot, message, NULL);
                discord_message_free(message);

                if(err == ESP_OK) {
                    ESP_LOGI(TAG, "Message with embed has been sent");
                } else {
                    ESP_LOGE(TAG, "Fail to send message");
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

    discord_config_t cfg = {
        .intents = DISCORD_INTENT_GUILD_MESSAGES | DISCORD_INTENT_MESSAGE_CONTENT
    };

    bot = discord_create(&cfg);
    ESP_ERROR_CHECK(discord_register_events(bot, DISCORD_EVENT_ANY, bot_event_handler, NULL));
    ESP_ERROR_CHECK(discord_login(bot));
}