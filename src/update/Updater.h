#ifndef UPDATE_UPDATER_H
#define UPDATE_UPDATER_H

#include "../Types.h"

typedef std::function<void(const uint8_t, float)> UpdaterStatusCallbackFunctionType;

#define UPDATER_STATUS_START 1
#define UPDATER_STATUS_PROGRESS 2
#define UPDATER_STATUS_END 3

class Updater {
public:
  Updater(UpdaterStatusCallbackFunctionType sc) : statusCallback(sc){};

  bool startUpdate(ErrorCallbackFunctionType errorCallback) {
    bool result = true;
    statusCallback(UPDATER_STATUS_START, 0);
    result = update(errorCallback);
    if (result) {
      statusCallback(UPDATER_STATUS_END, 1);
    }

    return result;
  }

protected:
  UpdaterStatusCallbackFunctionType statusCallback;

  virtual bool update(ErrorCallbackFunctionType errorCallback) {}
};

#endif
