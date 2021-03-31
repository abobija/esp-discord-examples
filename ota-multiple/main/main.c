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
#include "discord_ota.h"

static const char* TAG = "discord_bot";

static discord_handle_t bot;

static void bot_event_handler(void* handler_arg, esp_event_base_t base, int32_t event_id, void* event_data) {
    discord_event_data_t* data = (discord_event_data_t*) event_data;

    switch(event_id) {
        case DISCORD_EVENT_CONNECTED: {
                discord_session_t* session = (discord_session_t*) data->ptr;
                discord_session_dump_log(ESP_LOGI, TAG, session);
            }
            break;
        
        case DISCORD_EVENT_MESSAGE_RECEIVED: {
                discord_message_t* msg = (discord_message_t*) data->ptr;
                if(!msg->author->bot) {
                    discord_message_dump_log(ESP_LOGI, TAG, msg);
                }
            }
            break;
        
        case DISCORD_EVENT_DISCONNECTED:
            ESP_LOGW(TAG, "Bot logged out");
            break;
    }
}

void app_main(void) {
    discord_ota_keep(true);

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(example_connect());

    discord_config_t cfg = {
        .intents = DISCORD_INTENT_GUILD_MESSAGES
    };

    discord_ota_config_t ota_cfg = {
        .multiple_ota = true
    };

    bot = discord_create(&cfg);
    ESP_ERROR_CHECK(discord_ota_init(bot, &ota_cfg));
    ESP_ERROR_CHECK(discord_register_events(bot, DISCORD_EVENT_ANY, bot_event_handler, NULL));
    ESP_ERROR_CHECK(discord_login(bot));
}