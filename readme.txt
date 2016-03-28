# ESP8266_RTOS_SDK #

----------

ESP8266 SDK based on FreeRTOS.

## Note ##

APIs of "ESP8266_RTOS_SDK" are same as "ESP8266_NONOS_SDK"

More details in "Wiki" !

## Requrements ##

You can use both xcc and gcc to compile your project, gcc is recommended.
For gcc, please refer to [esp-open-sdk](https://github.com/pfalcon/esp-open-sdk).

FreeRTOS [APIs](http://web.ist.utl.pt/~ist11993/FRTOS-API/index.html)


## Compile ##

Clone ESP8266_RTOS_SDK, e.g., to ~/ESP8266_RTOS_SDK.

    $git clone https://github.com/espressif/ESP8266_RTOS_SDK.git
    $git clone ssh://<userid>@52.192.25.100:29418/BBiQ_ESP8266

Modify Makefile:
    SDK_PATH?=/d/works/BBiQ/code/ESP8266_RTOS_SDK
    BIN_PATH?=/d/works/BBiQ/code/ESP8266_RTOS_SDK/bin
    SDK_TOOLS?= /c/Espressif/utils
    ESPTOOL ?= $(SDK_TOOLS)/esptool.exe
    ESPPORT ?= COM5
    BAUD ?= 115200

Build binary:
    make

## Download ##

eagle.app.v6.flash.bin, downloads to flash 0x00000

eagle.app.v6.irom0text.bin, downloads to flash 0x40000

blank.bin, downloads to flash 0x7E000

or just simple
    make flash ESPPORT=COM4
