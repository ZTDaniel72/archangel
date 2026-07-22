#include "webui.h"
#include <WiFi.h>
#include <WebServer.h>

static WebServer server(80);
static volatile float sRoll = 0, sPitch = 0, sAlt = 0;
static volatile int sThr = 0;
static volatile bool sHealthy = false;

static const char PAGE[] PROGMEM = R"HTML(
  <!DOCTYPE html><html><head>
  <meta name="viewport" content="width=device-width,initial-scale=1"/>
  <title>ARCHANGEL</title>
  <style>
  body{margin:0;background:#000;color:#d7e0ee;font-family:monospace;text-align:center}
  h1{font-size:20px;letter-spacing:3px;margin:14px}
  .adi{width:220px;height:220px;border-radius:50%;overflow:hidden;
  border:3px solid #2a3546;position:relative;margin:16px auto}
  .ball{position:absolute;left:-50%;top:-50%;width:200%;height:200%;
  background:linear-gradient(#2f6fb0 50%, #6b4a2a 50%)}
  .mark{position:absolute;top:50%;left:50%;width:60px;height:2px;
  background:#ffd633;transform:translate(-50%,-50%)}
  .row{display:flex;justify-content:space-around;max-width:340px;margin:10px auto}
  .lbl{color:#7f92b0;font-size:12px}.val{font-size:22px;font-weight:bold}
  </style></head><body>
  <h1>ARCHANGEL</h1>
  <div class="adi"><div class="ball" id="ball"></div><div class="mark"></div></div>
  <div class="row">
  <div><div class="lbl">ROLL</div><div class="val" id="roll">0</div></div>
  <div><div class="lbl">PITCH</div><div class="val" id="pitch">0</div></div>
  </div><div class="row">
  <div><div class="lbl">ALT m</div><div class="val" id="alt">0</div></div>
  <div><div class="lbl">THR</div><div class="val" id="thr">0</div></div>
  </div><div class="row">
  <div><div class="lbl">LINK</div><div class="val" id="link">--</div></div>
  </div>
  <script>
  async function tick(){
    try{
      const d=await(await fetch('/t')).json();
      roll.textContent=d.roll.toFixed(1); pitch.textContent=d.pitch.toFixed(1);
      alt.textContent=d.alt.toFixed(1); thr.textContent=d.thr;
      link.textContent=d.healthy?'OK':'FAILSAFE';
      link.style.color=d.healthy?'#39d98a':'#ff4d5e';
      ball.style.transform='rotate('+(-d.roll)+'deg) translateY('+(-d.pitch*2)+'px)';
    }catch(e){}
    setTimeout(tick,150);
  }
  tick();
  </script></body></html>
)HTML";

static void handleRoot() {server.send_P(200, "text/html", PAGE); }

static void handleT() {
  char buf[128];
  snprintf(buf, sizeof(buf),
  "{\"roll\":%.1f,\"pitch\":%.1f,\"alt\":%.1f,\"thr\":%d,\"healthy\":%d}",
  sRoll, sPitch, sAlt, sThr, sHealthy ? 1 : 0);
  server.send(200, "application/json", buf);
}

void webuiInit() {
  WiFi.mode(WIFI_AP);
  WiFi.softAP("ARCHANGEL", "archangel123");
  server.on("/", handleRoot);
  server.on("/t", handleT);
  server.begin();
}

void webuiUpdate() { server.handleClient(); }

void webuiSet(float roll, float pitch, float alt, int thr, bool healthy) {
  sRoll = roll; sPitch = pitch; sAlt = alt; sThr = thr; sHealthy = healthy;
}