"""Host-test application state transitions, not Zigbee delivery or SDK behavior."""
from pathlib import Path
import subprocess
import tempfile

HEADER = Path(__file__).resolve().parents[1] / 'components/soil_status_light/soil_status_light.h'
with tempfile.TemporaryDirectory() as directory:
    root = Path(directory)
    headers = {
        'freertos/FreeRTOS.h': '#pragma once\n#define pdMS_TO_TICKS(x) (x)\n',
        'esp_zigbee.h': '#pragma once\n',
        'esphome/core/component.h': '''#pragma once
#include <cstdint>
namespace esphome {
class Component { public:
  virtual ~Component() = default;
  virtual void setup() {} virtual void loop() {}
  void disable_loop() { enabled = false; }
  void enable_loop() { enabled = true; }
  bool enabled = true;
}; }
''',
        'esphome/core/log.h': '#pragma once\n',
        'esphome/components/zigbee/zigbee_attribute_esp32.h': '''#pragma once
#include <cstdint>
namespace esphome::zigbee {
class ZigbeeComponent { public: bool started = true; bool is_started() { return started; } };
class ZigbeeAttribute { public:
  template<class T> void set_attr(T v) { value = v; }
  uint32_t value = 0;
}; }
''',
        'ezbee/nwk.h': '''#pragma once
#include <cstdint>
constexpr uint8_t EZB_NWK_ED_TIMEOUT_1024MIN = 11;
inline bool lock_available = true;
inline unsigned last_lock_wait = 0, releases = 0;
inline uint32_t keepalive = 4000, fast_poll = 500;
inline uint8_t timeout = 0;
inline bool esp_zigbee_lock_acquire(unsigned wait) { last_lock_wait = wait; return lock_available; }
inline void esp_zigbee_lock_release() { ++releases; }
inline void ezb_nwk_set_ed_timeout(uint8_t value) { timeout = value; }
inline uint32_t ezb_nwk_get_keepalive_interval() { return keepalive; }
inline uint32_t ezb_nwk_get_fast_poll_interval() { return fast_poll; }
inline void ezb_nwk_set_keepalive_interval(uint32_t value) { keepalive = value; }
inline void ezb_nwk_set_fast_poll_interval(uint32_t value) { fast_poll = value; }
''',
    }
    for relative, content in headers.items():
        file = root / relative
        file.parent.mkdir(parents=True, exist_ok=True)
        file.write_text(content)
    source = root / 'test.cpp'
    source.write_text('''#include <cassert>
#include "soil_status_light.h"
int main() {
  esphome::zigbee::ZigbeeComponent zb;
  esphome::soil_status_light::SoilStatusLight status;
  esphome::zigbee::ZigbeeAttribute on, level, x, y;
  status.set_zigbee(&zb); status.set_attributes(&on, &level, &x, &y);
  status.setup(); assert(!status.enabled);
  status.configure_network(); lock_available = false; status.loop();
  assert(timeout == 0 && status.enabled && last_lock_wait == 10 && releases == 0);
  lock_available = true; status.loop();
  assert(timeout == EZB_NWK_ED_TIMEOUT_1024MIN && !status.enabled);
  status.begin_receive_window(); status.loop();
  assert(keepalive == 100 && fast_poll == 100);
  // Restarting a measurement must not replace saved defaults with 100 ms.
  status.begin_receive_window(); status.loop();
  status.restore_polling(); lock_available = false; status.loop();
  assert(!status.polling_restored() && status.enabled && keepalive == 100);
  lock_available = true; status.loop();
  assert(status.polling_restored() && keepalive == 4000 && fast_poll == 500);
  // Restore without an active window must not write uninitialized defaults.
  status.restore_polling(); status.loop();
  assert(keepalive == 4000 && fast_poll == 500);
  status.publish_status(1); assert(on.value == 1 && level.value == 254 && x.value == 11298 && y.value == 48942);
  status.publish_status(2); assert(x.value == 29111 && y.value == 33849);
  status.publish_status(3); assert(x.value == 45914 && y.value == 19615);
}
''')
    binary = root / 'status-test'
    subprocess.run(['c++', '-std=c++17', '-Wall', '-Wextra', '-Werror',
                    '-I', str(root), '-I', str(HEADER.parent), str(source), '-o', str(binary)], check=True)
    subprocess.run([str(binary)], check=True)
print('PASS: bounded-lock retry, polling restoration and logical colors (SDK mocked)')
