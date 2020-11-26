#!/bin/bash
sudo apt-get update && \
sudo apt-get upgrade -y && \
sudo apt-get install -y git cmake build-essential curl libcurl4-openssl-dev libssl-dev uuid-dev i2c-tools libi2c-dev librtimulib-dev && \
sudo raspi-config nonint do_memory_split 16 && \
sudo raspi-config nonint do_spi 0 && \
sudo raspi-config nonint do_i2c 0 && \
sudo raspi-config nonint do_wifi_country US && \
sudo raspi-config nonint do_change_locale en_US.UTF-8 && \
sudo raspi-config nonint do_configure_keyboard us && \
sudo sh -c "echo hdmi_force_hotplug=1 >> /boot/config.txt" && \
sudo reboot now