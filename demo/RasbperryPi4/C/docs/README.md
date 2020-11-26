# IoT Plug and Play Sample Code in C

This sample is for Raspberry Pi and SenseHat written in in C

## Requirements

- Raspberry Pi 3 or 4
- SenseHat
- Raspbian  
    Raspbian Buster Lite May 2020 version
    <https://downloads.raspberrypi.org/raspios_lite_armhf_latest>

## Setting up Raspberry Pi 4

1. Install Raspbian OS  
    [Installation Guide](https://www.raspberrypi.org/documentation/installation/installing-images/README.md)

1. Add an empty file `ssh` in the root of `boot` partition  

    This will enable SSH automatically, so you do not need keyboard/mouse etc.

1. Install git  

    Run following commands to configure the Raspberry Pi 4 to configures Raspberry Pi 4 with following configurations.
    Based on your location, change country code, locale, and keyboard layout appropriately.

    - Sets memory allocated for GPU to be 16GB
    - Enables I2C and SPI
    - Sets WiFi country to US  
        Change country code if you are not in the U.S.  
        To see other country codes, run :

        ```bash
        cat /usr/share/zoneinfo/iso3166.tab
        ```

    - Sets system locale to US

        To see other country codes, run :

        ```bash
        cat /usr/share/i18n/SUPPORTED
        ```

    - Sets keyboard layout to US
    - Adds `hdmi_force_hotplug` to `/boot/config.txt` for running Raspberry Pi 4 without HDMI monitor.

    ```bash
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
    ```

1. Upon reboot, clone this repo with :

    ```bash
    cd ~
    git clone https://github.com/microsoft/IoT-Plug-and-Play-Workshop.git
    ```

1. Run `setup.sh` to install tools etc

    ```bash
    cd ~/IoT-Plug-and-Play-Workshop/demo/RasbperryPi4/C/script/
    ./setup.sh
    ```

1. Open ./run.sh with your favorite text editor
1. `set` IOTHUB_CS for the demo device

    Example :

    ```bash
    # Set IoT Hub Connection String
    export IOTHUB_CS='HostName=myiothub.azure-devices.net;DeviceId=pnp-thermostat136;SharedAccessKey=123456789'
    ```

## Running the demo

1. Run the app with  

    ```bash
    ./run.sh
    ```
