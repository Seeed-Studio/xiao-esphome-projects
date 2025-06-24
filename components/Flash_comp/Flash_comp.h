#pragma once

#include "esphome.h"
#include "esphome/core/component.h"
#include <vector>
#include <nvs_flash.h>

namespace esphome {
namespace flash_component {

class Flash_comp : public Component {
public:

  void setup() override;

  bool save_to_nvs(int index, const std::vector<long int>& data);

  std::vector<long int> load_from_nvs(int index);

  bool clear_signal_by_index(int index);
};

}  // namespace flash_component
}  // namespace esphome
