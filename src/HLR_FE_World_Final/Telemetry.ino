// Read-only Wi-Fi telemetry for Final round. No commands can change the robot.
static WebServer telemetryServer(80);
static bool telemetryReady = false;
static uint32_t telemetryLastTickMs = 0;
static uint32_t telemetryLastRecordMs = 0;

int telemetryMotorPwm = 0;
static const char *telemetryParkingStage = "Not started";
static const char *telemetryFault = "";
static const char *telemetryEvent = "Boot";
static const char *telemetryLastCamera = "None";
static uint32_t telemetryLastCameraMs = 0;
static float telemetryCameraSteer = 0.0f;
static bool telemetryParkingStarted = false;
static double telemetryParkingTarget = 0.0;
static bool telemetryFrontTestStopped = false;
static uint32_t telemetryLastFrontMs = 0;
static int telemetryFrontLeft = -1, telemetryFrontRight = -1;
extern float imuAccelX, imuAccelY, imuAccelZ;
extern float imuPitchDeg, imuRollDeg, imuAccelMagnitude;
void imu_read_acceleration();
static int telemetryFrontR2C3 = -1, telemetryFrontR3C3 = -1;
static int telemetrySideR2C3 = -1;

struct TelemetryHistorySample {
  uint32_t timeMs;
  float yaw, target, rate;
  int16_t pwm;
  uint8_t stage;
};
static const uint16_t TELEMETRY_HISTORY_CAP = 480;  // 48 seconds at 10 Hz
static TelemetryHistorySample telemetryHistory[TELEMETRY_HISTORY_CAP];
static uint16_t telemetryHistoryHead = 0;
static uint16_t telemetryHistoryCount = 0;

void telemetry_note_camera(const String &cmd) {
  if (cmd.startsWith("S")) {
    telemetryLastCamera = "FOLLOW S";
    telemetryCameraSteer = cmd.substring(1).toFloat();
  }
  else if (cmd == "R" || cmd == "RED") telemetryLastCamera = "RED close";
  else if (cmd == "G" || cmd == "GREEN") telemetryLastCamera = "GREEN close";
  else telemetryLastCamera = "Other";
  telemetryLastCameraMs = millis();
}
void telemetry_note_event(const char *msg) { telemetryEvent = msg; }
void telemetry_parking_stage(const char *name, double target) {
  telemetryParkingStarted = true;
  telemetryParkingStage = name;
  telemetryParkingTarget = target;
  telemetryFrontTestStopped = String(name) == "STOP AT FRONT 270";
  telemetryEvent = name;
}
void telemetry_parking_fail(const char *reason) {
  telemetryParkingStarted = true;
  telemetryParkingStage = "STOPPED / FAULT";
  telemetryFault = reason;
  telemetryEvent = reason;
}
void telemetry_front_cells(int a, int b) {
  telemetryFrontR2C3 = a;
  telemetryFrontR3C3 = b;
}
void telemetry_side_cell(int mm) { telemetrySideR2C3 = mm; }

static const char *telemetry_state_name() {
  switch (currentState) {
    case PID: return "PID";
    case FOLLOW_CUBE: return "FOLLOW_CUBE";
    case AVOID_CUBE: return "AVOID_CUBE";
    case AFTER_CUBE: return "AFTER_CUBE";
    case PARK: return "PARK";
    default: return "UNKNOWN";
  }
}
static const char *telemetry_cube_name() {
  switch (finalCubePhase) {
    case FC_TURN_OUT: return "TURN_OUT";
    case FC_DRIVE_OUT: return "DRIVE_OUT";
    case FC_RECOVER_TURN: return "RECOVER_TURN";
    case FC_RECOVER_FORWARD: return "RECOVER_FORWARD";
    default: return "IDLE";
  }
}
static double telemetry_target_yaw() {
  if (telemetryParkingStarted && currentState == PARK) return telemetryParkingTarget;
  if ((currentState == AVOID_CUBE || currentState == AFTER_CUBE) &&
      finalCubePhase != FC_IDLE) return finalCubeTarget;
  return current_angle_gyro;
}
static uint8_t telemetry_stage_id() {
  if (telemetryParkingStarted) return 2;
  if (currentState == AVOID_CUBE || currentState == AFTER_CUBE) return 1;
  return 0;
}
static void telemetry_record_sample() {
  if (!gyroAvailable) return;
  const uint32_t now = millis();
  if ((uint32_t)(now - telemetryLastRecordMs) < 100UL) return;
  telemetryLastRecordMs = now;
  TelemetryHistorySample &s = telemetryHistory[telemetryHistoryHead];
  s.timeMs = now;
  s.yaw = (float)gz;
  s.target = (float)telemetry_target_yaw();
  s.rate = (float)gyroRateDps;
  s.pwm = (int16_t)telemetryMotorPwm;
  s.stage = telemetry_stage_id();
  telemetryHistoryHead = (telemetryHistoryHead + 1) % TELEMETRY_HISTORY_CAP;
  if (telemetryHistoryCount < TELEMETRY_HISTORY_CAP) ++telemetryHistoryCount;
}

static String telemetry_json() {
  const double target = telemetry_target_yaw();
  String j;
  j.reserve(750);
  j += "{\"ms\":"; j += String(millis());
  j += ",\"gyro\":"; j += String(gz, 3);
  j += ",\"target\":"; j += String(target, 3);
  j += ",\"error\":"; j += String(target - gz, 3);
  j += ",\"rate\":"; j += String(gyroRateDps, 3);
  j += ",\"bias\":"; j += String(drifts_z, 5);
  j += ",\"accel_x\":"; j += String(imuAccelX, 3);
  j += ",\"accel_y\":"; j += String(imuAccelY, 3);
  j += ",\"accel_z\":"; j += String(imuAccelZ, 3);
  j += ",\"accel_norm\":"; j += String(imuAccelMagnitude, 3);
  j += ",\"roll\":"; j += String(imuRollDeg, 2);
  j += ",\"pitch\":"; j += String(imuPitchDeg, 2);
  j += ",\"pwm\":"; j += String(telemetryMotorPwm);
  j += ",\"ticks\":"; j += String((long)encoder_ticks);
  j += ",\"turns\":"; j += String(turn_count);
  j += ",\"gyro_ok\":"; j += (gyroAvailable ? "true" : "false");
  j += ",\"state\":\""; j += telemetry_state_name(); j += "\"";
  j += ",\"cube_phase\":\""; j += telemetry_cube_name(); j += "\"";
  j += ",\"park_stage\":\""; j += telemetryParkingStage; j += "\"";
  j += ",\"last_camera\":\""; j += telemetryLastCamera; j += "\"";
  j += ",\"camera_age\":";
  j += String(telemetryLastCameraMs ? (uint32_t)(millis() - telemetryLastCameraMs) : 0UL);
  j += ",\"front_left\":"; j += String(telemetryFrontLeft);
  j += ",\"front_right\":"; j += String(telemetryFrontRight);
  j += ",\"front_a\":"; j += String(telemetryFrontR2C3);
  j += ",\"front_b\":"; j += String(telemetryFrontR3C3);
  j += ",\"side\":"; j += String(telemetrySideR2C3);
  j += ",\"event\":\""; j += telemetryEvent; j += "\"";
  j += ",\"fault\":\""; j += telemetryFault; j += "\"";
  j += ",\"camera_steer\":"; j += String(telemetryCameraSteer, 3);
  j += ",\"follow_seen\":"; j += String((int)finalPassFollowCount);
  j += "}";
  return j;
}
static void telemetry_send_history() {
  // A large download in the middle of a gyro turn can delay angle integration.
  // The browser keeps building live history while the wheels are moving.
  if (telemetryMotorPwm != 0) {
    telemetryServer.send(200, "application/json", "[]");
    return;
  }
  String out;
  out.reserve(50000);
  out += "[";
  const uint16_t start = (telemetryHistoryHead + TELEMETRY_HISTORY_CAP - telemetryHistoryCount)
                           % TELEMETRY_HISTORY_CAP;
  for (uint16_t i = 0; i < telemetryHistoryCount; ++i) {
    const TelemetryHistorySample &s = telemetryHistory[(start + i) % TELEMETRY_HISTORY_CAP];
    if (i) out += ",";
    out += "["; out += String(s.timeMs);
    out += ","; out += String(s.yaw, 2);
    out += ","; out += String(s.target, 2);
    out += ","; out += String(s.rate, 2);
    out += ","; out += String((int)s.pwm);
    out += ","; out += String((int)s.stage);
    out += "]";
  }
  out += "]";
  telemetryServer.send(200, "application/json", out);
}

static const char TELEMETRY_HTML[] PROGMEM = R"HLRHTML(
<!doctype html><html lang="en"><head><meta name="viewport" content="width=device-width,initial-scale=1">
<meta charset="utf-8"><title>HyperLine | Live telemetry</title>
<style>
:root{color-scheme:dark;font-family:system-ui,-apple-system,Segoe UI,Arial,sans-serif;background:#091522;color:#eff6ff}
*{box-sizing:border-box}body{max-width:1050px;margin:auto;padding:18px}h1{font-size:22px;margin:3px 0 5px}
.sub{font-size:13px;color:#93aac2;margin:0 0 20px}.grid{display:grid;grid-template-columns:repeat(3,minmax(0,1fr));gap:10px}
.card{background:#152638;border:1px solid #294357;border-radius:12px;padding:13px;min-width:0}
.label{color:#9fb5cb;font-size:12px}.value{font-weight:750;font-size:25px;margin-top:4px;overflow-wrap:anywhere}
.wide{grid-column:1/-1}.good{color:#77eac0}.warn{color:#ffb274}.bad{color:#ff8690}
canvas{width:100%;height:245px;display:block}.key{font-size:12px;color:#acbfd4}
.key span{margin-right:16px} .key .a{color:#6bdfff}.key .b{color:#ffbc73}
.status{display:flex;justify-content:space-between;gap:10px;align-items:center;margin-bottom:15px}
.small{font-size:13px;color:#c3d5e4;overflow-wrap:anywhere}.footer{margin-top:12px;color:#879cb0;font-size:12px}
@media(max-width:650px){.grid{grid-template-columns:repeat(2,minmax(0,1fr))}.value{font-size:21px}body{padding:12px}h1{font-size:19px}}
</style></head><body>
<div class="status"><div><h1>HyperLine Robotics</h1><p class="sub">WRO 2026 | Final round | BMI088 telemetry</p></div><div id="connected" class="small">Connecting...</div></div>
<div class="grid">
  <div class="card"><div class="label">Gyro yaw</div><div id="yaw" class="value">--</div></div>
  <div class="card"><div class="label">Target heading</div><div id="target" class="value">--</div></div>
  <div class="card"><div class="label">Heading error</div><div id="error" class="value">--</div></div>
  <div class="card"><div class="label">Angular rate</div><div id="rate" class="value">--</div></div>
  <div class="card"><div class="label">Gyro bias</div><div id="bias" class="value">--</div></div>
  <div class="card"><div class="label">Accel X (m/s²)</div><div id="accel_x" class="value">--</div></div>
  <div class="card"><div class="label">Accel Y (m/s²)</div><div id="accel_y" class="value">--</div></div>
  <div class="card"><div class="label">Accel Z (m/s²)</div><div id="accel_z" class="value">--</div></div>
  <div class="card"><div class="label">Acceleration magnitude</div><div id="accel_norm" class="value">--</div></div>
  <div class="card"><div class="label">IMU roll (tilt)</div><div id="roll" class="value">--</div></div>
  <div class="card"><div class="label">IMU pitch (tilt)</div><div id="pitch" class="value">--</div></div>
  <div class="card"><div class="label">Motor PWM</div><div id="pwm" class="value">--</div></div>
  <div class="card wide"><div class="label">Yaw history, last 48 seconds (also saved on the robot)</div>
    <canvas id="chart"></canvas><div class="key"><span class="a">● Actual gyro</span><span class="b">● Target heading</span></div></div>
  <div class="card"><div class="label">Robot state</div><div id="state" class="value">--</div></div>
  <div class="card"><div class="label">Cube phase</div><div id="cube_phase" class="value">--</div></div>
  <div class="card"><div class="label">Parking stage</div><div id="park_stage" class="value">--</div></div>
  <div class="card"><div class="label">Front R2 C3</div><div id="front_a" class="value">--</div></div>
  <div class="card"><div class="label">Front R3 C3</div><div id="front_b" class="value">--</div></div>
  <div class="card"><div class="label">Front LEFT zone (C2)</div><div id="front_left" class="value">--</div></div>
  <div class="card"><div class="label">Front RIGHT zone (C4)</div><div id="front_right" class="value">--</div></div>
  <div class="card"><div class="label">Side R2 C3</div><div id="side" class="value">--</div></div>
  <div class="card"><div class="label">Last camera command</div><div id="last_camera" class="value">--</div><div id="camera_steer" class="small">--</div></div>
  <div class="card"><div class="label">Camera frame age</div><div id="camera_age" class="value">--</div></div>
  <div class="card"><div class="label">Turns / encoder</div><div id="turns" class="value">--</div><div id="ticks" class="small"></div></div>
  <div class="card wide"><div class="label">Last event / stop reason</div><div id="event" class="small">--</div><div id="fault" class="small bad"></div></div>
</div>
<div class="footer">Read-only dashboard. No motor control is exposed over Wi-Fi. SSID: HyperLine • 192.168.4.1</div>
<script>
const el=id=>document.getElementById(id), chart=el('chart');
let history=[],latest=0;
function graph(){
 const ctx=chart.getContext('2d'),dpr=devicePixelRatio||1,w=chart.clientWidth,h=chart.clientHeight;
 chart.width=Math.floor(w*dpr);chart.height=Math.floor(h*dpr);ctx.setTransform(dpr,0,0,dpr,0,0);
 ctx.clearRect(0,0,w,h);const pad={l:44,r:10,t:16,b:23};const cw=w-pad.l-pad.r,ch=h-pad.t-pad.b;
 if(history.length<2)return;
 const values=history.flatMap(v=>[v[1],v[2]]).filter(Number.isFinite);if(!values.length)return;
 let lo=Math.min(...values),hi=Math.max(...values);const margin=Math.max((hi-lo)*.12,7);lo-=margin;hi+=margin;
 const x0=history[0][0],x1=history[history.length-1][0];
 const xp=t=>pad.l+(t-x0)/Math.max(1,x1-x0)*cw,yp=v=>pad.t+(hi-v)/(hi-lo)*ch;
 ctx.lineWidth=1;ctx.strokeStyle='#355064';ctx.fillStyle='#9aadc4';ctx.font='11px system-ui';
 for(let i=0;i<=4;i++){const v=lo+(hi-lo)*i/4,y=yp(v);ctx.beginPath();ctx.moveTo(pad.l,y);ctx.lineTo(w-pad.r,y);ctx.stroke();ctx.fillText(v.toFixed(0)+'°',3,y+4)}
 [[1,'#6bdfff'],[2,'#ffbc73']].forEach(([idx,color])=>{
 ctx.lineWidth=2;ctx.strokeStyle=color;ctx.beginPath();history.forEach((s,i)=>{let x=xp(s[0]),y=yp(s[idx]);i?ctx.lineTo(x,y):ctx.moveTo(x,y)});ctx.stroke();
 });
 ctx.fillStyle='#90a9be';ctx.fillText((x1-x0)/1000+' sec',pad.l,h-5)
}
async function loadHistory(){try{const r=await fetch('/history',{cache:'no-store'});if(r.ok){history=await r.json();graph()}}catch(e){}}
function set(k,v){el(k).textContent=v}
async function update(){try{
 const response=await fetch('/api',{cache:'no-store'});if(!response.ok)throw Error(response.status);
 const d=await response.json();set('connected','Live • '+Math.round(d.ms/1000)+'s');el('connected').className='small good';
 set('yaw',Number(d.gyro).toFixed(2)+'°');
 for(const k of ['target','error','rate','bias'])set(k,Number(d[k]).toFixed(k==='bias'?4:2)+(k==='rate'||k==='bias'?' °/s':'°'));
 for(const k of ['accel_x','accel_y','accel_z','accel_norm'])set(k,Number(d[k]).toFixed(3));
 for(const k of ['roll','pitch'])set(k,Number(d[k]).toFixed(2)+'°');
 set('pwm',d.pwm);set('state',d.state);set('cube_phase',d.cube_phase);set('park_stage',d.park_stage);
 set('front_a',d.front_a>0?d.front_a+' mm':'--');set('front_b',d.front_b>0?d.front_b+' mm':'--');set('front_left',d.front_left>0?d.front_left+' mm':'--');set('front_right',d.front_right>0?d.front_right+' mm':'--');set('side',d.side>0?d.side+' mm':'--');
 set('last_camera',d.last_camera);set('camera_steer','S steering '+d.camera_steer.toFixed(3)+' | bypass frames '+d.follow_seen);set('camera_age',d.camera_age+' ms');set('turns',d.turns);set('ticks','Encoder: '+d.ticks+' ticks');
 set('event',d.event);set('fault',d.fault);el('error').className='value '+(Math.abs(d.error)>8?'warn':'good');
 if(d.ms>latest){history.push([d.ms,d.gyro,d.target,d.rate,d.pwm,0]);latest=d.ms;const cutoff=d.ms-48000;while(history.length&&history[0][0]<cutoff)history.shift();graph()}
 }catch(e){set('connected','Disconnected • '+String(e.message||e));el('connected').className='small bad'}
}
let polling=false;
async function poll(){if(!polling){polling=true;try{await update()}finally{polling=false}}setTimeout(poll,350)}
loadHistory().then(poll);
</script></body></html>
)HLRHTML";

void telemetry_setup() {
  if (activeMode != MODE_FINAL) return;
  WiFi.mode(WIFI_AP);
  if (!WiFi.softAP("HyperLine", "HyperLine-FE")) {
    Serial.println("TELEMETRY: Wi-Fi AP failed (robot can still run)");
    return;
  }
  telemetryServer.on("/", HTTP_GET, []() {
    telemetryServer.send_P(200, "text/html; charset=utf-8", TELEMETRY_HTML);
  });
  telemetryServer.on("/api", HTTP_GET, []() {
    telemetryServer.send(200, "application/json", telemetry_json());
  });
  telemetryServer.on("/history", HTTP_GET, []() {
    telemetry_send_history();
  });
  telemetryServer.begin();
  telemetryReady = true;
  Serial.print("TELEMETRY: HyperLine / HyperLine-FE at http://");
  Serial.println(WiFi.softAPIP());
}

void telemetry_tick() {
  if (!telemetryReady) return;
  imu_read_acceleration();
  if (telemetryFrontTestStopped && millis() - telemetryLastFrontMs >= 120UL) {
    telemetryLastFrontMs = millis();
    tof_poll_front_only();
    telemetry_front_cells(tof_zone_mm(FRONT_DIR, 2, 3),
                          tof_zone_mm(FRONT_DIR, 3, 3));
    int l2=tof_zone_mm(FRONT_DIR,2,2),l3=tof_zone_mm(FRONT_DIR,3,2);
    int r2=tof_zone_mm(FRONT_DIR,2,4),r3=tof_zone_mm(FRONT_DIR,3,4);
    telemetryFrontLeft=(l2>0 && l3>0) ? (l2+l3)/2 : -1;
    telemetryFrontRight=(r2>0 && r3>0) ? (r2+r3)/2 : -1;
  }
  telemetry_record_sample();
  const uint32_t now = millis();
  if ((uint32_t)(now - telemetryLastTickMs) <
      (telemetryMotorPwm != 0 ? 80UL : 55UL)) return;
  telemetryLastTickMs = now;
  static bool busy = false;
  if (busy) return;
  busy = true;
  telemetryServer.handleClient();
  busy = false;
}
