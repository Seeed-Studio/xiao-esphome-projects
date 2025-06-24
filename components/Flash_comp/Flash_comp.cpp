#include "Flash_comp.h"

namespace esphome {
namespace flash_component {

void Flash_comp::setup() {
    nvs_flash_init();
}

bool Flash_comp::save_to_nvs(int index, const std::vector<long int>& data) {
    char key[12];
    sprintf(key, "irsig_%d", index);

    nvs_handle_t handle;
    if (nvs_open("storage", NVS_READWRITE, &handle) != ESP_OK) {
      ESP_LOGE("flash_comp", "Failed to open NVS handle");
      return false;
    }

    // delete old data
    nvs_erase_key(handle, key);

    // store new data
    if (data.size() > 0) {
      esp_err_t err = nvs_set_blob(handle, key, data.data(), data.size() * sizeof(long int));
      if (err != ESP_OK) {
        ESP_LOGE("flash_comp", "Failed to write data: %d", err);
        return false;
      }
    }
    ESP_LOGI("flash_comp", "Saved %d data to NVS", data.size());
    // commit changes
    nvs_commit(handle);
    nvs_close(handle);
    return true;
}

std::vector<long int> Flash_comp::load_from_nvs(int index) {
    char key[12];
    sprintf(key, "irsig_%d", index);
    std::vector<long int> data;

    nvs_handle_t handle;
    if (nvs_open("storage", NVS_READONLY, &handle) != ESP_OK) {
      ESP_LOGE("flash_comp", "Failed to open NVS handle");
      return data;
    }

    // get data size
    size_t required_size = 0;
    esp_err_t err = nvs_get_blob(handle, key, nullptr, &required_size);

    if (err == ESP_OK && required_size > 0) {
      // add data to vector
      data.resize(required_size / sizeof(long int));
      nvs_get_blob(handle, key, data.data(), &required_size);
    } else if (err != ESP_ERR_NVS_NOT_FOUND) {
      ESP_LOGE("flash_comp", "Failed to read data: %d", err);
    }
    ESP_LOGI("flash_comp", "Loaded %d data from NVS", data.size());
    nvs_close(handle);
    return data;
}

bool Flash_comp::clear_signal_by_index(int index) {
    char key[12];
    sprintf(key, "irsig_%d", index);

    nvs_handle_t handle;
    if (nvs_open("storage", NVS_READWRITE, &handle) != ESP_OK) {
      ESP_LOGE("flash_comp", "Failed to open NVS handle");
      return false;
    }

    nvs_erase_key(handle, key);
    nvs_commit(handle);
    nvs_close(handle);
    return true;
}


}  // namespace flash_component
}  // namespace esphome
