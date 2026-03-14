#pragma once

#include <WebServer.h>
#include "door/DoorController.h"

class DoorWebServer {
public:
    DoorWebServer(DoorController& door, uint16_t port = 80);

    void begin();
    void handle();      // Call every loop() iteration

private:
    void handleRoot();
    void handleStatus();
    void handleOpen();
    void handleClose();
    void handleStop();
    void handleReset();

    DoorController& _door;
    WebServer _server;
};
