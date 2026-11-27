"""Check resolved configuration, not just the requested sdkconfig.defaults."""

import json
from pathlib import Path
import sys


def main():
    build = Path(sys.argv[1] if len(sys.argv) > 1 else "build")
    config = json.loads((build / "config/sdkconfig.json").read_text())
    expected = {
        "IDF_TARGET": "esp32c5",
        "ESPTOOLPY_FLASHSIZE": "8MB",
        "OPENTHREAD_ENABLED": True,
        "OPENTHREAD_RADIO": True,
        "OPENTHREAD_RADIO_NATIVE": True,
        "OPENTHREAD_RCP_USB_SERIAL_JTAG": True,
        "ESP_CONSOLE_SECONDARY_USB_SERIAL_JTAG": True,
        "LOG_DEFAULT_LEVEL": 0,
        "BOOTLOADER_LOG_LEVEL": 0,
        "OPENTHREAD_LOG_LEVEL": 0,
    }
    for key, value in expected.items():
        if config.get(key) != value:
            raise SystemExit(f"{key}: expected {value!r}, got {config.get(key)!r}")
    for key in (
        "OPENTHREAD_FTD", "OPENTHREAD_MTD", "OPENTHREAD_RCP_UART",
        "OPENTHREAD_RCP_SPI", "OPENTHREAD_CLI", "OPENTHREAD_CONSOLE_ENABLE",
        "OPENTHREAD_BORDER_ROUTER", "OPENTHREAD_LOG_LEVEL_DYNAMIC",
    ):
        if config.get(key):
            raise SystemExit(f"Unexpected enabled feature: {key}")
    flash = json.loads((build / "flasher_args.json").read_text())
    if flash["flash_settings"]["flash_size"] != "8MB":
        raise SystemExit("Generated flash arguments do not select 8MB")
    description = json.loads((build / "project_description.json").read_text())
    if description["git_revision"] != "v5.5.2":
        raise SystemExit(f"Unverified ESP-IDF version: {description['git_revision']}")
    if not (build / description["app_bin"]).is_file():
        raise SystemExit("Application binary is missing")
    print("PASS: ESP-IDF v5.5.2, ESP32-C5, 8MB, USB RCP, silent Spinel channel")


if __name__ == "__main__":
    main()
