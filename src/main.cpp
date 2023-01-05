// add heartbeat signal on all lora devices except gateway
// MQTT Structure:
//  messages to gateway for distribution are commands (for now): yurt/lora/cmnd/*DEVICE*/{param: data}json
//  messages from gateway for logging / node-red / monitoring are telemetry: yurt/lora/tele/*DEVICE*/data{param: data}json
//  mqtt status messages published on yurt/lora/tele/*DEVICE*/state

#include <Arduino.h>
#include <SPI.h>
#include <ArduinoJson.h>
#include <LoraPowerTrailer.h>
#include <DMC_LoRa.h>
#include <DMC_PinMonitor.h>

QueueHandle_t SystemEventQueue;


//Button Setup
static DMC_MonitoredPin Alert("AlertPin",POWER_MONITOR_ALERT_PIN, &SystemEventQueue, sys_event_AlertOn, sys_event_AlertOff, PIN_MONITOR_INTERVAL, INPUT_PULLUP);
static DMC_MonitoredPin OtherGenStart("OtherGenStartPin",RELAY_STATUS_PIN, &SystemEventQueue, sys_event_OtherGenStart, sys_event_OtherGenStop, PIN_MONITOR_INTERVAL, INPUT_PULLUP);

// System event handler
void task_systemEventHandler(void *params);
//void systemDispatch(SystemEvent_t system_event);

void task_LoRaMessageProcessor(void *params);

void setup() {

//  Serial.begin(19200);
//  Serial.setDebugOutput(true);
//  delay(500);

  SystemEventQueue = xQueueCreate(20U, sizeof(SystemEvent_t));

  pinMode(GEN_START_PIN, OUTPUT);
  digitalWrite(GEN_START_PIN, LOW);

  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, NSS_PIN);
  DMCLoRa.setSPI(SPI);
  DMCLoRa.setPins(NSS_PIN, RST_PIN, DIO0_PIN);
  DMCLoRa.begin(LORA_FREQUENCY, LORA_YURT_ADDRESS, &SystemEventQueue, sys_event_MsgReceived, sys_event_MsgSendFail, sys_event_LoRaFailed);

  xTaskCreate(task_systemEventHandler, "SystemEventHandler", SYSTEM_EVENT_HANDLER_TASK_STACK_DEPTH, NULL, SYSTEM_EVENT_HANDLER_TASK_PRIORITY, NULL);
  xTaskCreate(task_LoRaMessageProcessor, "LoRaMessageProcessor", LORA_MESSAGE_PROCESSOR_TASK_STACK_DEPTH, NULL, LORA_MESSAGE_PROCESSOR_TASK_PRIORITY, NULL);

}

void loop() {
  DMCLoRa.sendMessage(LORA_YURT_ADDRESS, "test");
  vTaskDelay(pdMS_TO_TICKS(5000));
}

void task_LoRaMessageProcessor(void *params){
  DMC_LoRaMessage_t message;
  String MQTTTopic;

  while(1){
    xQueueReceive(DMCLoRa.RxQueue, &message, portMAX_DELAY);
    if (message.From == LORA_YURT_ADDRESS){
      if (!strcmp(message.payload,"switch")){
        digitalWrite(GEN_START_PIN, !digitalRead(GEN_START_PIN));
      }
    }
  }
  vTaskDelete(NULL);
}

void task_systemEventHandler(void *params){
  SystemEvent_t  systemevent;

  while(1){
    xQueueReceive(SystemEventQueue, &systemevent, portMAX_DELAY);
    log_v("Received system event: %u", systemevent);
    switch (systemevent)
    {
    case sys_event_OtherGenStart:
        DMCLoRa.sendMessage(LORA_YURT_ADDRESS, "other start");
      break;

    case sys_event_OtherGenStop:
        DMCLoRa.sendMessage(LORA_YURT_ADDRESS, "other stop");
      break;
    
    default:
      break;
    }
  }
  vTaskDelete(NULL);
}