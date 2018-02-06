#ifndef DOMAIN_ESP8266_SYS_INFO_H
#define DOMAIN_ESP8266_SYS_INFO_H

#include "JsonEntity.h"

class Esp8266SysInfo : public JsonEntity {
public:
  bool updateJsonFromEntity(JsonObject &json, ErrorCallbackFunctionType errorCallback) {
    json["vcc"] = ESP.getVcc();
    json["freeHeap"] = ESP.getFreeHeap();
    json["chipId"] = ESP.getChipId();
    json["sdkVersion"] = ESP.getSdkVersion();
    json["coreVersion"] = ESP.getCoreVersion();
    json["bootVersion"] = ESP.getBootVersion();
    json["bootMode"] = ESP.getBootMode();
    json["cpuFreqMHz"] = ESP.getCpuFreqMHz();
    json["flashChipId"] = ESP.getFlashChipId();
    json["flashChipRealSize"] = ESP.getFlashChipRealSize();
    json["flashChipSize"] = ESP.getFlashChipSize();
    json["flashChipSpeed"] = ESP.getFlashChipSpeed();
    json["flashChipMode"] = ESP.getFlashChipMode();
    json["flashChipSizeByChipId"] = ESP.getFlashChipSizeByChipId();
    json["sketchSize"] = ESP.getSketchSize();
    json["sketchMD5"] = ESP.getSketchMD5();
    json["freeSketchSpace"] = ESP.getFreeSketchSpace();
    json["resetReason"] = ESP.getResetReason();
    json["resetInfo"] = ESP.getResetInfo();
    json["cycleCount"] = ESP.getCycleCount();
    return true;
  };
};

#endif
