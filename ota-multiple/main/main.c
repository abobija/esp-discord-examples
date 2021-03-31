#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "protocol_examples_common.h"

#include "nvs_token_helper.h"
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
                // Token is valid. Save it to nvs
                save_token_to_nvs();
            }
            break;
        
        case DISCORD_EVENT_MESSAGE_RECEIVED: {
                discord_message_t* msg = (discord_message_t*) data->ptr;
                discord_message_dump_log(ESP_LOGI, TAG, msg);
                discord_ota(bot, msg, NULL);
            }
            break;
        
        case DISCORD_EVENT_DISCONNECTED:
            ESP_LOGW(TAG, "Bot logged out");
            discord_close_code_t close_code;
            discord_get_close_code(bot, &close_code);
            if(close_code == DISCORD_CLOSEOP_AUTHENTICATION_FAILED) {
                // Token is invalid. Remove it from nvs
                remove_token_from_nvs();
            }
            break;
    }
}

void app_main(void) {
    discord_ota_keep(true);

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(example_connect());

    char* nvs_token = read_token_from_nvs();

    ESP_LOGI(TAG, "Token loaded from %s",
        nvs_token ? "nvs" : "configuration"
    );

    discord_config_t cfg = {
        .intents = DISCORD_INTENT_GUILD_MESSAGES,
        .token = nvs_token ? nvs_token : CONFIG_DISCORD_TOKEN
    };

    bot = discord_create(&cfg);
    ESP_ERROR_CHECK(discord_register_events(bot, DISCORD_EVENT_ANY, bot_event_handler, NULL));
    ESP_ERROR_CHECK(discord_login(bot));

    free(nvs_token);
}