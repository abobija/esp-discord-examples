#ifndef _NVS_TOKEN_HELPER_H_
#define _NVS_TOKEN_HELPER_H_

#ifdef __cplusplus
extern "C" {
#endif

#define NVS_TOKEN_NS "discord_bot"
#define NVS_TOKEN_KEY "token"

char* read_token_from_nvs();
void save_token_to_nvs();
void remove_token_from_nvs();

#ifdef __cplusplus
}
#endif

#endif