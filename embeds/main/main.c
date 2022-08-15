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

                discord_message_t* message_with_embed = cu_ctor(discord_message_t,
                    .content    = strdup("Hey, check this awesome ESP32 chip!"),
                    .channel_id = strdup(msg->channel_id),
                );

                discord_embed_t* embed = cu_ctor(discord_embed_t,
                    .title       = strdup("ESP32 by Espressif"),
                    .description = strdup(
                        "A feature-rich MCU with integrated Wi-Fi and Bluetooth connectivity for a wide-range of applications"
                    ),
                    .url         = strdup("https://www.espressif.com"),
                    .color       = DISCORD_COLOR_GREEN,
                    .footer      = cu_ctor(discord_embed_footer_t,
                        .text     = strdup("Brought by Espressif"),
                        .icon_url = strdup("https://abobija.com/wp-content/uploads/2022/08/espressif-logo.png"),
                    ),
                    .thumbnail   = cu_ctor(discord_embed_image_t,
                        .url = strdup("https://abobija.com/wp-content/uploads/2022/08/esp32-logo.png"),
                    ),
                    .image   = cu_ctor(discord_embed_image_t,
                        .url = strdup("https://abobija.com/wp-content/uploads/2022/08/esp32-pinout.png"),
                    ),
                    .author  = cu_ctor(discord_embed_author_t,
                        .name     = strdup("Alija Bobija"),
                        .url      = strdup("https://abobija.com"),
                        .icon_url = strdup("https://abobija.com/wp-content/uploads/2021/08/bitmoji-20191124033504.v1-edited.png"),
                    ),
                );

                discord_embed_add_field(embed, cu_ctor(discord_embed_field_t,
                    .name  = strdup("Memory"),
                    .value = strdup("320 KiB RAM, 448 KiB ROM"),
                ));

                discord_embed_add_field(embed, cu_ctor(discord_embed_field_t,
                    .name      = strdup("Bluetooth"),
                    .value     = strdup("v4.2 BR/EDR and BLE"),
                    .is_inline = true,
                ));

                discord_embed_add_field(embed, cu_ctor(discord_embed_field_t,
                    .name      = strdup("GPIOs"),
                    .value     = strdup("34 programmable GPIOs"),
                    .is_inline = true,
                ));

                discord_message_add_embed(message_with_embed, embed);

                esp_err_t err = discord_message_send(bot, message_with_embed, NULL);
                discord_message_free(message_with_embed);

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