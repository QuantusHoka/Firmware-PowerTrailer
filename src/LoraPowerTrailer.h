#ifndef LORA_POWER_TRAILER_H
#define LORA_POWER_TRAILER_H

#include <DMC_LoRa.h>

//LORA Addresses
enum LoRaAddresses {LORA_YURT_ADDRESS, LORA_POWER_TRAILER_ADDRESS, LORA_BROADCAST_ADDRESS = LORA_BROADCAST_ID};

//Pin Assignments for FireBeetle32
//Outputs
#define GEN_START_PIN D3 //high closes contacts

//SPI
#define SCK_PIN SCK
#define MISO_PIN MISO
#define MOSI_PIN MOSI

//RFM
#define DIO0_PIN D9 //RFM95 DIO0
#define RST_PIN D6 //RFM95 RST
#define NSS_PIN GPIO_NUM_4 //RFM95 NSS

//I2C
#define SDA_PIN SDA //ESP I2C SDA pin (default is 8)
#define SCL_PIN SCL //ESP I2C SDA pin (default is 9)

//INPUTS
#define RELAY_STATUS_PIN D7 //High = closed
#define POWER_MONITOR_ALERT_PIN D2 // high = OK


//System Settings
#define SYSTEM_EVENT_HANDLER_TASK_STACK_DEPTH 0x900
#define SYSTEM_EVENT_HANDLER_TASK_PRIORITY 5U
#define LORA_MESSAGE_PROCESSOR_TASK_STACK_DEPTH 0x900
#define LORA_MESSAGE_PROCESSOR_TASK_PRIORITY 5U
#define LORA_FREQUENCY 915E6 // LoRa Frequency
#define PIN_MONITOR_INTERVAL 100U

//System Events
typedef enum {
    sys_event_Init,
    sys_event_gen_startstop,
    sys_event_OtherGenStart,
    sys_event_OtherGenStop,
    sys_event_AlertOn,
    sys_event_AlertOff,
    sys_event_MsgSendFail,
    sys_event_MsgReceived,
    sys_event_LoRaFailed,
    sys_event_Restart,
    sys_event_UnknownError = 0xff
} SystemEvent_t;

#endif