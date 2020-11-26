#!/bin/bash
sudo apt-get update && \
sudo apt-get upgrade -y && \
sudo apt-get install -y git && \
sudo raspi-config nonint do_memory_split 16 && \
sudo raspi-config nonint do_spi 0 && \
sudo raspi-config nonint do_i2c 0 && \
sudo raspi-config nonint do_wifi_country US && \
sudo raspi-config nonint do_change_locale en_US.UTF-8 && \
sudo raspi-config nonint do_configure_keyboard us && \
sudo reboot now
