# IoT Plug and Play Technical Workshop

This repo is preprared for contents sharing for Hands-on-Labs (HOLs) during IoT Plug and Play workshop.

## Requirements

- Azure subscription
- A device  
  - Seeed Wio Terminal

  - Linux environment to run a simulator  
    - Ubuntu 18, Ubuntu running in WSL, or Raspbian running on Raspberry Pi 3/4

- Additional Providers  
  Please ensure to enable following providers  
  - Conatiner Instance (Microsoft.ContainerInstance)  
  - Azure Digital Twins  (Microsoft.DigitalTwins)

    Reference : [How to register resource providers](docs/RegisterProviders.md)

## Agenda

1. Please follow [Deploy a sample IoT Solution with IoT Plug and Play  ](docs/Deployment.md)
1. Experience IoT Plug and Play with IoT Central
1. Prepare an IoT Device  
    - If you have Seeed Wio Terminal, please follow this [instruction](demo/Seeed_Wio_Terminal/docs/README.md)
    - If you do NOT have Seeed Wio Terminal, please use [Simple Thermostat simulator app](demo/SimpleThermostat/docs/README.md)