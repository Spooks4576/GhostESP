#pragma once
#include "AppController.h"

enum RokuKeyPress {
  RokuKeyPress_UP,
  RokuKeyPress_DOWN,
  RokuKeyPress_LEFT,
  RokuKeyPress_RIGHT,
  RokuKeyPress_HOME,
  RokuKeyPress_PLAY
};


class RokuController : public AppController {
public:

  // Make sure to set this before calling LaunchApp
  const char* AppIDToLaunch;

  RokuController();

  HandlerType getType() const override {
    return HandlerType::RokuController;
  }

  virtual void launchApp(const String& appUrl) override;

  virtual int checkAppStatus(const String& appUrl, Device& device) override;

  bool isRokuDevice(const char* appURL);

  void ExecuteKeyCommand(RokuKeyPress key, const char* AppUrl);
};