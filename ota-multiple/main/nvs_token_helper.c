#include "nvs_token_helper.h"
#include "nvs_flash.h"

static bool token_loaded_from_nvs = false;

char* read_token_from_nvs() {
    nvs_handle_t nvs;
    char* nvs_token = NULL;
    size_t nvs_token_len = 0;

    if(nvs_open(NVS_TOKEN_NS, NVS_READONLY, &nvs) != ESP_OK) {
        return NULL;
    }

    nvs_get_str(nvs, NVS_TOKEN_KEY, NULL, &nvs_token_len);

    if(nvs_token_len > 0) {
        nvs_token = malloc(nvs_token_len);
        nvs_get_str(nvs, NVS_TOKEN_KEY, nvs_token, &nvs_token_len);
    }

    nvs_close(nvs);

    if(nvs_token) {
        token_loaded_from_nvs = true;
    }

    return nvs_token;
}

void save_token_to_nvs() {
    if(token_loaded_from_nvs) {
        return; // token is already in nvs
    }

    nvs_handle_t nvs;
    ESP_ERROR_CHECK(nvs_open(NVS_TOKEN_NS, NVS_READWRITE, &nvs));
    nvs_set_str(nvs, NVS_TOKEN_KEY, CONFIG_DISCORD_TOKEN);
    nvs_commit(nvs);
    nvs_close(nvs);
}

void remove_token_from_nvs() {
    if(!token_loaded_from_nvs) {
        return; // token is not in nvs
    }

    nvs_handle_t nvs;
    ESP_ERROR_CHECK(nvs_open(NVS_TOKEN_NS, NVS_READWRITE, &nvs));
    nvs_erase_key(nvs, NVS_TOKEN_KEY);
    nvs_commit(nvs);
    nvs_close(nvs);
}