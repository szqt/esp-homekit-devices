#include <stdarg.h>
#include <string.h>

#ifdef ESP_PLATFORM

#include "esp_random.h"
#include "adv_logger.h"

uint32_t homekit_random() {
    return esp_random();
}

void homekit_random_fill(uint8_t* data, size_t size) {
    uint32_t x;
    for (int i = 0; i < size; i += sizeof(x)) {
        x = esp_random();
        memcpy(data + i, &x, (size - i >= sizeof(x)) ? sizeof(x) : size - i);
    }
}
/*
#include <mdns.h>

void homekit_mdns_init() {
    mdns_init();
}

void homekit_mdns_configure_init(const char *instance_name, int port) {
    mdns_hostname_set(instance_name);
    mdns_instance_name_set(instance_name);
    mdns_service_add(instance_name, "_hap", "_tcp", port, NULL, 0);
}

void homekit_mdns_add_txt(const char *key, const char *format, ...) {
    va_list arg_ptr;
    va_start(arg_ptr, format);

    char value[128];
    int value_len = vsnprintf(value, sizeof(value), format, arg_ptr);

    va_end(arg_ptr);

    if (value_len && value_len < sizeof(value) - 1) {
        mdns_service_txt_item_set("_hap", "_tcp", key, value);
    }
}

void homekit_mdns_configure_finalize(const uint16_t mdns_ttl, const uint16_t mdns_ttl_period) {
    //printf("mDNS announcement: Name=%s %s Port=%d TTL=%d\n",
           name->value.string_value, txt_rec, PORT, 0);
}
*/
#else

#include <esp/hwrand.h>
#include <espressif/esp_common.h>
#include <esplibs/libmain.h>

uint32_t homekit_random() {
    return hwrand();
}

void homekit_random_fill(uint8_t *data, size_t size) {
    hwrand_fill(data, size);
}
#endif

#include "mdnsresponder.h"

static char mdns_instance_name[65] = {0};
static char mdns_txt_rec[128] = {0};
static int mdns_port = 80;

void homekit_mdns_buffer_set(const uint16_t size) {
    mdns_buffer_deinit();
    mdns_buffer_init(size);
}

void homekit_mdns_init() {
    mdns_init();
}

void homekit_mdns_configure_init(const char *instance_name, int port) {
    memcpy(mdns_instance_name, instance_name, sizeof(mdns_instance_name));
    mdns_txt_rec[0] = 0;
    mdns_port = port;
}

void homekit_mdns_add_txt(const char *key, const char *format, ...) {
    va_list arg_ptr;
    va_start(arg_ptr, format);

    char value[128];
    size_t value_len = vsnprintf(value, sizeof(value), format, arg_ptr);
    
    va_end(arg_ptr);

    if (value_len && ((value_len + 1) < sizeof(value))) {
        char buffer[128];
        size_t buffer_len = snprintf(buffer, sizeof(buffer), "%s=%s", key, value);

        if ((buffer_len + 1) < sizeof(buffer))
            mdns_TXT_append(mdns_txt_rec, sizeof(mdns_txt_rec), buffer, buffer_len);
    }
}

void homekit_mdns_configure_finalize(const uint16_t mdns_ttl, const uint16_t mdns_ttl_period) {
    mdns_clear();
    mdns_add_facility(mdns_instance_name, "_hap", mdns_txt_rec, mdns_TCP, mdns_port, mdns_ttl, mdns_ttl_period);

#ifdef ESP_PLATFORM
    adv_logger_printf("mDNS Name=%s %s Port=%d TTL=%d\n", mdns_instance_name, mdns_txt_rec, mdns_port, mdns_ttl);
#else
    printf("mDNS Name=%s %s Port=%d TTL=%d\n", mdns_instance_name, mdns_txt_rec, mdns_port, mdns_ttl);
#endif
}

void homekit_port_mdns_announce() {
    mdns_announce();
}

void homekit_port_mdns_announce_pause() {
    mdns_announce_pause();
}
