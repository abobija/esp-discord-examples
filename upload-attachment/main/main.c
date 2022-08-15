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

extern const uint8_t rickroll_start[] asm("_binary_rickroll_jpg_start");
extern const uint8_t rickroll_end[]   asm("_binary_rickroll_jpg_end");

static void bot_event_handler(void* handler_arg, esp_event_base_t base, int32_t event_id, void* event_data) {
    discord_event_data_t* data = (discord_event_data_t*) event_data;

    switch(event_id) {
        case DISCORD_EVENT_CONNECTED: {
                discord_session_t* session = (discord_session_t*) data->ptr;

                ESP_LOGI(TAG, "Bot %s#%s connected",
                    session->user->username,
                    session->user->discriminator
                );
            }
            break;
        
        case DISCORD_EVENT_MESSAGE_RECEIVED: {
                discord_message_t* msg = (discord_message_t*) data->ptr;
                discord_message_dump_log(ESP_LOGI, TAG, msg);

                discord_message_t* hello = cu_ctor(discord_message_t,
                    .content    = estr_cat("Hey ", msg->author->username, ", check this out!"),
                    .channel_id = strdup(msg->channel_id),
                );

                discord_attachment_t* rickroll = cu_ctor(discord_attachment_t,
                    .content_type = strdup("image/jpeg"),
                    .filename     = strdup("rickroll.jpg"),
                    ._data        = (uint8_t*) rickroll_start,
                    .size         = rickroll_end - rickroll_start,
                );

                discord_message_add_attachment(hello, rickroll);
                esp_err_t err = discord_message_send(bot, hello, NULL);
                discord_message_free(hello);

                if(err == ESP_OK) {
                    ESP_LOGI(TAG, "Message sent");
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