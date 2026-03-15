#include "DoorWebServer.h"
#include "config/Config.h"

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
.c{max-width:420px;width:100%;padding-top:30px}
h1{text-align:center;font-size:1.5em;margin-bottom:24px;color:#fff}
.status{text-align:center;padding:18px;margin-bottom:8px;border-radius:12px;background:#16213e;font-size:1.3em;font-weight:bold;border-left:4px solid #888;transition:border-color .3s}
.status.open{border-color:#4ecca3}
.status.closed{border-color:#e94560}
.status.opening,.status.closing{border-color:#e9a045}
.status.calibrating{border-color:#9c27b0}
.status.error{border-color:#ff4444}
.status.stopped{border-color:#888}
.error-msg{color:#ff4444;font-size:.85em;margin-top:6px;font-weight:normal}
.pos-bar{height:8px;border-radius:4px;background:#16213e;margin-bottom:6px;overflow:hidden}
.pos-fill{height:100%;background:#4ecca3;border-radius:4px;transition:width .5s;width:0}
.info{display:flex;justify-content:space-between;margin-bottom:20px;font-size:.85em;color:#888}
.info .warn{color:#e9a045}
.buttons{display:flex;flex-direction:column;gap:10px}
button{padding:15px;font-size:1.1em;font-weight:bold;border:none;border-radius:12px;cursor:pointer;transition:opacity .2s}
button:active{opacity:.7}
.btn-open{background:#0f3460;color:#fff}
.btn-close{background:#e94560;color:#fff}
.btn-stop{background:#555;color:#fff}
.settings{margin-top:20px;padding-top:16px;border-top:1px solid #333}
.settings h2{font-size:1em;color:#888;margin-bottom:10px}
.btn-sm{padding:10px;font-size:.95em;background:#333;color:#aaa;border:none;border-radius:8px;cursor:pointer;width:100%;margin-bottom:6px}
.btn-sm:active{opacity:.7}
</style>
</head>
<body>
<div class="c">
<h1>Door Controller</h1>
<div id="st" class="status">Connecting...</div>
<div class="pos-bar"><div id="bar" class="pos-fill"></div></div>
<div class="info">
  <span id="pos"></span>
  <span id="enc"></span>
  <span id="rpm"></span>
</div>
<div class="buttons">
<button class="btn-open" onclick="cmd('open')">Open</button>
<button class="btn-close" onclick="cmd('close')">Close</button>
<button class="btn-stop" onclick="cmd('stop')">Stop</button>
</div>
<div class="settings">
<h2>Settings</h2>
<button class="btn-sm" onclick="cmd('reset')">Reset Error</button>
<button class="btn-sm" onclick="cmd('calibrate')">Calibrate</button>
<button class="btn-sm" onclick="if(confirm('Clear calibration data?'))cmd('clearCalibration')">Clear Calibration</button>
</div>
</div>
<script>
function cmd(a){fetch('/api/'+a,{method:'POST'}).then(()=>poll())}
function poll(){
  fetch('/api/status').then(r=>r.json()).then(d=>{
    var e=document.getElementById('st');
    e.textContent=d.state;
    e.className='status '+d.state.toLowerCase();
    if(d.error){e.innerHTML+=`<div class="error-msg">${d.error}</div>`}
    var bar=document.getElementById('bar');
    if(d.position>=0){bar.style.width=d.position+'%';document.getElementById('pos').textContent=Math.round(d.position)+'%'}
    else{bar.style.width='0%';document.getElementById('pos').textContent=''}
    document.getElementById('enc').textContent=d.encoder?(d.calibrated?'':'Not calibrated'):'No encoder';
    document.getElementById('enc').className=d.encoder?(d.calibrated?'':'warn'):'';
    document.getElementById('rpm').textContent=d.speed>0?Math.abs(d.rpm).toFixed(1)+' RPM':'';
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
    _server.on("/",                  HTTP_GET,  [this]() { handleRoot(); });
    _server.on("/api/status",        HTTP_GET,  [this]() { handleStatus(); });
    _server.on("/api/open",          HTTP_POST, [this]() { handleOpen(); });
    _server.on("/api/close",         HTTP_POST, [this]() { handleClose(); });
    _server.on("/api/stop",          HTTP_POST, [this]() { handleStop(); });
    _server.on("/api/reset",         HTTP_POST, [this]() { handleReset(); });
    _server.on("/api/calibrate",     HTTP_POST, [this]() { handleCalibrate(); });
    _server.on("/api/clearCalibration", HTTP_POST, [this]() { handleClearCalibration(); });

    _server.begin();
    Serial.printf("[Web] Server started on port %d\n", WEB_SERVER_PORT);
}

void DoorWebServer::handle() {
    _server.handleClient();
}

void DoorWebServer::handleRoot() {
    _server.send_P(200, "text/html", INDEX_HTML);
}

void DoorWebServer::handleStatus() {
    float pos = _door.getPositionPercent();
    float rpm = _door.getRPM();

    String json = "{\"state\":\"";
    json += _door.getStateString();
    json += "\",\"speed\":";
    json += _door.getCurrentSpeed();
    json += ",\"encoder\":";
    json += _door.hasEncoder() ? "true" : "false";
    json += ",\"calibrated\":";
    json += _door.isCalibrated() ? "true" : "false";
    json += ",\"position\":";
    json += String(pos, 1);
    json += ",\"rpm\":";
    json += String(rpm, 1);
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

void DoorWebServer::handleCalibrate() {
    _door.calibrate();
    _server.send(200, "application/json", "{\"ok\":true}");
}

void DoorWebServer::handleClearCalibration() {
    _door.clearCalibration();
    _server.send(200, "application/json", "{\"ok\":true}");
}
