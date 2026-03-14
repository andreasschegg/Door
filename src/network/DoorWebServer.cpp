#include "DoorWebServer.h"
#include "config/Config.h"

// ---------------------------------------------------------
// Embedded HTML — mobile-friendly single-page UI
// ---------------------------------------------------------
static const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>Door Controller</title>
<style>
*{box-sizing:border-box;margin:0;padding:0}
body{font-family:-apple-system,BlinkMacSystemFont,sans-serif;background:#1a1a2e;color:#e0e0e0;display:flex;justify-content:center;min-height:100vh;padding:20px}
.c{max-width:400px;width:100%;padding-top:40px}
h1{text-align:center;font-size:1.5em;margin-bottom:30px;color:#fff}
.status{text-align:center;padding:20px;margin-bottom:30px;border-radius:12px;background:#16213e;font-size:1.3em;font-weight:bold;border-left:4px solid #888;transition:border-color .3s}
.status.open{border-color:#4ecca3}
.status.closed{border-color:#e94560}
.status.opening,.status.closing{border-color:#e9a045}
.status.error{border-color:#ff4444}
.status.stopped{border-color:#888}
.error-msg{color:#ff4444;font-size:.85em;margin-top:8px;font-weight:normal}
.buttons{display:flex;flex-direction:column;gap:12px}
button{padding:16px;font-size:1.1em;font-weight:bold;border:none;border-radius:12px;cursor:pointer;transition:opacity .2s}
button:active{opacity:.7}
.btn-open{background:#0f3460;color:#fff}
.btn-close{background:#e94560;color:#fff}
.btn-stop{background:#555;color:#fff}
.btn-reset{background:#333;color:#aaa;font-size:.9em;margin-top:8px}
.speed{text-align:center;margin-top:16px;color:#666;font-size:.85em}
</style>
</head>
<body>
<div class="c">
<h1>Door Controller</h1>
<div id="st" class="status">Connecting...</div>
<div class="buttons">
<button class="btn-open" onclick="cmd('open')">Open</button>
<button class="btn-close" onclick="cmd('close')">Close</button>
<button class="btn-stop" onclick="cmd('stop')">Stop</button>
<button class="btn-reset" onclick="cmd('reset')">Reset</button>
</div>
<div id="spd" class="speed"></div>
</div>
<script>
function cmd(a){fetch('/api/'+a,{method:'POST'}).then(()=>poll())}
function poll(){
  fetch('/api/status').then(r=>r.json()).then(d=>{
    var e=document.getElementById('st');
    e.textContent=d.state;
    e.className='status '+d.state.toLowerCase();
    if(d.error){e.innerHTML+=`<div class="error-msg">${d.error}</div>`}
    document.getElementById('spd').textContent=d.speed>0?'Speed: '+d.speed:'';
  }).catch(()=>{
    document.getElementById('st').textContent='Connection lost';
  })
}
setInterval(poll,1000);
poll();
</script>
</body>
</html>
)rawliteral";

// ---------------------------------------------------------
DoorWebServer::DoorWebServer(DoorController& door, uint16_t port)
    : _door(door), _server(port) {}

void DoorWebServer::begin() {
    _server.on("/",           HTTP_GET,  [this]() { handleRoot();   });
    _server.on("/api/status", HTTP_GET,  [this]() { handleStatus(); });
    _server.on("/api/open",   HTTP_POST, [this]() { handleOpen();   });
    _server.on("/api/close",  HTTP_POST, [this]() { handleClose();  });
    _server.on("/api/stop",   HTTP_POST, [this]() { handleStop();   });
    _server.on("/api/reset",  HTTP_POST, [this]() { handleReset();  });

    _server.begin();
    Serial.printf("[Web] Server started on port %d\n", WEB_SERVER_PORT);
}

void DoorWebServer::handle() {
    _server.handleClient();
}

// ---------------------------------------------------------
void DoorWebServer::handleRoot() {
    _server.send_P(200, "text/html", INDEX_HTML);
}

void DoorWebServer::handleStatus() {
    String json = "{\"state\":\"";
    json += _door.getStateString();
    json += "\",\"speed\":";
    json += _door.getCurrentSpeed();
    if (_door.getState() == DoorState::ERROR && _door.getErrorMessage().length() > 0) {
        json += ",\"error\":\"";
        json += _door.getErrorMessage();
        json += "\"";
    }
    json += "}";
    _server.send(200, "application/json", json);
}

void DoorWebServer::handleOpen() {
    _door.open();
    _server.send(200, "application/json", "{\"ok\":true}");
}

void DoorWebServer::handleClose() {
    _door.close();
    _server.send(200, "application/json", "{\"ok\":true}");
}

void DoorWebServer::handleStop() {
    _door.stop();
    _server.send(200, "application/json", "{\"ok\":true}");
}

void DoorWebServer::handleReset() {
    _door.reset();
    _server.send(200, "application/json", "{\"ok\":true}");
}
