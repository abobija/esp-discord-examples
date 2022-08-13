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

static const char* TAG = "discord_bot";

static discord_handle_t bot;

static esp_err_t download_handler(discord_download_info_t* file, void* arg) {
    ESP_LOGI(TAG, "downloaded %d [%d/%d]", file->length, file->offset + file->length, file->total_length);
    ESP_LOGI(TAG, "Data: [%.*s]", file->length, (const char*) file->data);

    return ESP_OK;
}

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

                ESP_LOGI(TAG, "New message (dm=%s, autor=%s#%s, bot=%s, channel=%s, guild=%s, content=%s)",
                    ! msg->guild_id ? "true" : "false",
                    msg->author->username,
                    msg->author->discriminator,
                    msg->author->bot ? "true" : "false",
                    msg->channel_id,
                    msg->guild_id ? msg->guild_id : "NULL",
                    msg->content
                );

                if(msg->_attachments_len == 0) {
                    ESP_LOGI(TAG, "There is no attachments in message");
                } else {
                    for(uint8_t i = 0; i < msg->_attachments_len; i++) {
                        ESP_LOGI(TAG, "attachment (id=%s, filename=%s, type=%s, size=%d, url=%s)",
                            msg->attachments[i]->id,
                            msg->attachments[i]->filename,
                            msg->attachments[i]->content_type,
                            msg->attachments[i]->size,
                            msg->attachments[i]->url
                        );
                    }

                    // message can contain more than one attachment
                    // but here we gonna to download only first one (index 0)
                    if(discord_message_download_attachment(bot, msg, 0, &download_handler, NULL) == ESP_OK) {
                        ESP_LOGI(TAG, "Attachment has been successfully downloaded");
                    } else {
                        ESP_LOGE(TAG, "Fail to download attachment");
                    }
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