idf_component_register(
    SRCS "main.c"
    PRIV_REQUIRES esp_event nvs_flash openthread esp_driver_usb_serial_jtag vfs
)
