#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "protocol_examples_common.h"
#include "driver/gpio.h"

#include "discord.h"
#include "estr.h"
#include "discord/session.h"
#include "discord/voice_state.h"

static const char* TAG = "discord_bot";

static discord_handle_t bot;

/**
 * To find out your user ID first run application
 * without setting the user id. Then join you user to
 * one of the discord voice channels and from the terminal output
 * copy the user_id and paste it here
 */
const char* VOICE_USER_ID = "YOUR_USER_ID_HERE";

/**
 * GPIO number of the LED that will be ON when the user is muted.
 * Otherwise, when the user is not muted, LED will be OFF.
 */
const gpio_num_t LED_GPIO = GPIO_NUM_2;

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
        
        case DISCORD_EVENT_VOICE_STATE_UPDATED: {
                discord_voice_state_t* vstate = (discord_voice_state_t*) data->ptr;

                ESP_LOGI(TAG, "voice_state (user_id=%s, channel_id=%s, mute=%d, self_mute=%d, deaf=%d, self_deaf=%d)",
                    vstate->user_id,
                    vstate->channel_id ? vstate->channel_id : "NULL",
                    vstate->mute,
                    vstate->self_mute,
                    vstate->deaf,
                    vstate->self_deaf
                );

                if(estr_eq(VOICE_USER_ID, vstate->user_id)) {
                    if(vstate->mute || vstate->self_mute) {
                        // user is muted
                        gpio_set_level(LED_GPIO, 1);
                    }
                    else {
                        // user is not muted
                        gpio_set_level(LED_GPIO, 0);
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
    gpio_reset_pin(LED_GPIO);
    gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_level(LED_GPIO, 0);

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(example_connect());

    discord_config_t cfg = {
        .intents = DISCORD_INTENT_GUILD_VOICE_STATES
    };

    bot = discord_create(&cfg);
    ESP_ERROR_CHECK(discord_register_events(bot, DISCORD_EVENT_ANY, bot_event_handler, NULL));
    ESP_ERROR_CHECK(discord_login(bot));
}