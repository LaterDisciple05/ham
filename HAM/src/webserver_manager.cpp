#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

#include "webserver_manager.h"
#include "relay_manager.h"
#include "config.h"

WebServer server(80);

// prevents false interrupt after web control
extern unsigned long ignoreInterruptUntil;

static bool isValidChannel() {

  if(!server.hasArg("ch")) {
    return true;
  }

  String ch = server.arg("ch");

  if(ch.length() == 0) {
    return false;
  }

  for(size_t i = 0; i < ch.length(); i++) {

    if(!isDigit(ch[i])) {
      return false;
    }
  }

  return ch.toInt() > 0;
}

// =====================================================
// HTML
// =====================================================
String getHTML() {

return R"rawliteral(

<!DOCTYPE html>
<html>

<head>

<meta name="viewport" content="width=device-width, initial-scale=1">

<title>Home Automation Dashboard</title>

<style>

:root{
  --paper:#fbf9f5;
  --grid:#ece6dc;
  --card:#ffffff;
  --border:#d8d1c6;
  --ink:#1f2933;
  --muted:#6b7280;
  --accent:#0f766e;
  --accent2:#1e40af;
  --ok:#15803d;
  --off:#b91c1c;
}

*{
  box-sizing:border-box;
}

body{
  margin:0;
  min-height:100vh;
  display:flex;
  align-items:center;
  justify-content:center;
  padding:20px;
  color:var(--ink);
  font-family:"Trebuchet MS","Verdana","Tahoma",sans-serif;
  background:
    linear-gradient(90deg, var(--grid) 1px, transparent 1px),
    linear-gradient(var(--grid) 1px, transparent 1px),
    var(--paper);
  background-size:28px 28px;
}

@keyframes rise{
  from{ opacity:0; transform:translateY(6px); }
  to{ opacity:1; transform:translateY(0); }
}

@keyframes fade{
  from{ opacity:0; }
  to{ opacity:1; }
}

.app{
  width:min(980px, 100%);
  background:var(--card);
  border:1px solid var(--border);
  border-radius:14px;
  padding:20px;
  box-shadow:0 10px 24px rgba(0,0,0,0.08);
  animation:rise 0.2s ease;
}

.top{
  display:flex;
  align-items:center;
  justify-content:space-between;
  gap:12px;
  margin-bottom:18px;
}

h1{
  font-size:22px;
  margin:0;
}

.sub{
  margin:4px 0 0 0;
  font-size:12px;
  color:var(--muted);
}

.panel{
  border:1px solid var(--border);
  border-radius:12px;
  padding:14px;
  margin-bottom:16px;
}

.panel-title{
  font-weight:700;
  margin-bottom:10px;
}

.form{
  display:grid;
  grid-template-columns:1.3fr 0.6fr 1.1fr;
  gap:12px;
  align-items:end;
}

.field label{
  display:block;
  font-size:12px;
  color:var(--muted);
  margin-bottom:6px;
}

.field.buttons{
  display:flex;
  gap:8px;
  flex-wrap:wrap;
}

input{
  padding:10px;
  border:1px solid var(--border);
  border-radius:8px;
  font-size:14px;
  width:100%;
}

button{
  padding:10px 12px;
  border:1px solid var(--border);
  border-radius:8px;
  font-size:13px;
  font-weight:700;
  cursor:pointer;
  background:#fff;
  transition:background 0.2s ease;
}

.primary{
  background:var(--accent);
  color:#fff;
  border-color:var(--accent);
}

.ghost{
  background:#fff;
  color:var(--ink);
}

.message{
  margin-top:8px;
  font-size:12px;
  color:var(--muted);
}

.list-header{
  display:flex;
  align-items:center;
  justify-content:space-between;
  margin-bottom:8px;
  font-weight:700;
}

.table-head,
.device-row{
  display:grid;
  grid-template-columns:1.4fr 0.6fr 0.7fr 1.8fr 0.5fr;
  gap:10px;
  align-items:center;
}

.table-head{
  font-size:12px;
  color:var(--muted);
  padding-bottom:6px;
  border-bottom:1px solid var(--border);
}

.device-row{
  padding:10px 0;
  border-bottom:1px dashed var(--border);
  animation:fade 0.2s ease;
}

.device-row:last-child{
  border-bottom:0;
}

.status-pill{
  display:inline-block;
  padding:4px 8px;
  border-radius:999px;
  font-size:12px;
  border:1px solid var(--border);
  background:#fff;
}

.status-on{
  color:var(--ok);
  border-color:rgba(21,128,61,0.35);
}

.status-off{
  color:var(--off);
  border-color:rgba(185,28,28,0.35);
}

.online{
  color:var(--ok);
  border-color:rgba(21,128,61,0.35);
}

.offline{
  color:var(--off);
  border-color:rgba(185,28,28,0.35);
}

.actions{
  display:flex;
  flex-wrap:wrap;
  gap:6px;
}

.btn-on{
  border-color:rgba(21,128,61,0.4);
  color:var(--ok);
}

.btn-off{
  border-color:rgba(185,28,28,0.4);
  color:var(--off);
}

.btn-toggle{
  border-color:rgba(15,118,110,0.4);
  color:var(--accent);
}

.btn-refresh{
  border-color:rgba(30,64,175,0.35);
  color:var(--accent2);
}

.btn-remove{
  border-color:rgba(0,0,0,0.2);
  color:var(--muted);
}

.switch{
  position:relative;
  display:inline-block;
  width:44px;
  height:22px;
}

.switch input{
  opacity:0;
  width:0;
  height:0;
}

.slider{
  position:absolute;
  cursor:pointer;
  top:0;
  left:0;
  right:0;
  bottom:0;
  background:#e7e4dc;
  border:1px solid var(--border);
  border-radius:999px;
  transition:background 0.2s ease;
}

.slider:before{
  content:"";
  position:absolute;
  height:16px;
  width:16px;
  left:3px;
  top:2px;
  background:#fff;
  border:1px solid var(--border);
  border-radius:50%;
  transition:transform 0.2s ease;
}

.switch input:checked + .slider{
  background:rgba(21,128,61,0.2);
  border-color:rgba(21,128,61,0.35);
}

.switch input:checked + .slider:before{
  transform:translateX(20px);
}

.footer{
  display:flex;
  justify-content:space-between;
  font-size:12px;
  color:var(--muted);
}

.empty{
  padding:12px 0;
  color:var(--muted);
  font-size:13px;
}

@media (max-width:720px){
  .form{
    grid-template-columns:1fr;
  }

  .field.buttons{
    justify-content:flex-start;
  }

  .table-head{
    display:none;
  }

  .device-row{
    grid-template-columns:1fr;
    gap:6px;
  }
}

</style>

</head>

<body>

  <div class="app">

    <div class="top">

      <div>
        <h1>Home Automation Dashboard</h1>
        <div class="sub">Add devices and control switches</div>
      </div>

      <div class="status-pill offline" id="connection">Offline</div>

    </div>

    <div class="panel">

      <div class="panel-title">Add device</div>

      <div class="form">

        <div class="field">
          <label for="deviceName">Device name</label>
          <input id="deviceName" type="text" placeholder="e.g. Living Room Light" maxlength="24">
        </div>

        <div class="field">
          <label for="deviceChannel">Channel</label>
          <input id="deviceChannel" type="number" min="1" value="1">
        </div>

        <div class="field buttons">
          <button class="primary" id="addBtn">Add device</button>
          <button class="ghost" id="resetBtn">Reset list</button>
        </div>

      </div>

      <div class="message" id="message">Device list is stored in this browser.</div>

    </div>

    <div class="panel">

      <div class="list-header">
        <div>Devices</div>
        <button class="ghost" id="refreshAll">Refresh all</button>
      </div>

      <div class="table-head">
        <div>Name</div>
        <div>Channel</div>
        <div>Status</div>
        <div>Controls</div>
        <div></div>
      </div>

      <div id="deviceList"></div>

    </div>

    <div class="footer">
      <div id="lastUpdate">Last update: --</div>
      <div>Auto refresh: 1s</div>
    </div>

  </div>

<script>

const storageKey = "ha-devices";

const defaults = [
  { id: 1, name: "Relay 1", ch: 1 }
];

const deviceList = document.getElementById("deviceList");

const connectionEl = document.getElementById("connection");

const lastUpdateEl = document.getElementById("lastUpdate");

const messageEl = document.getElementById("message");

const nameInput = document.getElementById("deviceName");

const channelInput = document.getElementById("deviceChannel");

const addBtn = document.getElementById("addBtn");

const resetBtn = document.getElementById("resetBtn");

const refreshAllBtn = document.getElementById("refreshAll");

function escapeHtml(value) {
  return value.replace(/[&<>"']/g, match => ({
    "&": "&amp;",
    "<": "&lt;",
    ">": "&gt;",
    "\"": "&quot;",
    "'": "&#39;"
  }[match]));
}

function loadDevices() {
  const raw = localStorage.getItem(storageKey);
  if(!raw) {
    return defaults.slice();
  }
  try {
    const parsed = JSON.parse(raw);
    if(Array.isArray(parsed) && parsed.length > 0) {
      return parsed;
    }
  }
  catch(err) {
    return defaults.slice();
  }
  return defaults.slice();
}

function saveDevices() {
  localStorage.setItem(storageKey, JSON.stringify(devices));
}

let devices = loadDevices();

function setMessage(text) {
  messageEl.textContent = text;
}

function setConnection(online) {
  connectionEl.textContent = online ? "Online" : "Offline";
  connectionEl.classList.remove("online", "offline");
  connectionEl.classList.add(online ? "online" : "offline");
}

function renderDevices() {
  deviceList.innerHTML = "";

  if(devices.length === 0) {
    const empty = document.createElement("div");
    empty.className = "empty";
    empty.textContent = "No devices. Add one above.";
    deviceList.appendChild(empty);
    return;
  }

  devices.forEach(device => {
    const row = document.createElement("div");
    row.className = "device-row";
    row.dataset.id = device.id;
    row.innerHTML = `
      <div class="device-name">${escapeHtml(device.name)}</div>
      <div>CH ${device.ch}</div>
      <div><span class="status-pill" id="status-${device.id}">...</span></div>
      <div class="actions">
        <label class="switch">
          <input type="checkbox" id="toggle-${device.id}" onchange="setRelay(${device.id}, this.checked ? 1 : 0)">
          <span class="slider"></span>
        </label>
        <button class="btn-on" onclick="setRelay(${device.id}, 1)">ON</button>
        <button class="btn-off" onclick="setRelay(${device.id}, 0)">OFF</button>
        <button class="btn-refresh" onclick="updateStatus(${device.id})">Refresh</button>
      </div>
      <div>
        <button class="btn-remove" onclick="removeDevice(${device.id})">Remove</button>
      </div>
    `;
    deviceList.appendChild(row);
  });
}

function findDevice(id) {
  return devices.find(d => d.id === id);
}

function setStatusUI(id, isOn) {
  const statusEl = document.getElementById(`status-${id}`);
  const toggleEl = document.getElementById(`toggle-${id}`);
  if(!statusEl) {
    return;
  }
  statusEl.className = "status-pill " + (isOn ? "status-on" : "status-off");
  statusEl.textContent = isOn ? "ON" : "OFF";
  if(toggleEl) {
    toggleEl.checked = isOn;
  }
}

async function callEndpoint(url) {
  try {
    const response = await fetch(url);
    await response.text();
    setConnection(true);
  }
  catch(err) {
    setConnection(false);
  }
}

async function setRelay(id, state) {
  const device = findDevice(id);
  if(!device) {
    return;
  }
  await callEndpoint(`/relay?ch=${device.ch}&state=${state}`);
  updateStatus(id);
}

async function updateStatus(id) {
  const device = findDevice(id);
  if(!device) {
    return;
  }
  try {
    const response = await fetch(`/status?ch=${device.ch}`);
    const data = await response.text();
    setConnection(true);
    setStatusUI(id, data.trim() === "1");
    const now = new Date();
    lastUpdateEl.textContent = "Last update: " + now.toLocaleTimeString();
  }
  catch(err) {
    setConnection(false);
  }
}

function refreshAll() {
  devices.forEach(device => updateStatus(device.id));
}

function addDevice() {
  const name = nameInput.value.trim();
  const channel = parseInt(channelInput.value, 10);
  if(!name) {
    setMessage("Enter a device name.");
    return;
  }
  if(Number.isNaN(channel) || channel < 1) {
    setMessage("Channel must be 1 or higher.");
    return;
  }
  const exists = devices.some(device => device.name.toLowerCase() === name.toLowerCase() && device.ch === channel);
  if(exists) {
    setMessage("That device already exists.");
    return;
  }
  const id = Date.now();
  devices.push({ id, name, ch: channel });
  saveDevices();
  renderDevices();
  updateStatus(id);
  setMessage("Device added.");
  nameInput.value = "";
}

function removeDevice(id) {
  devices = devices.filter(device => device.id !== id);
  saveDevices();
  renderDevices();
  setMessage("Device removed.");
}

function resetDevices() {
  devices = defaults.slice();
  saveDevices();
  renderDevices();
  refreshAll();
  setMessage("List reset to default.");
}

addBtn.addEventListener("click", addDevice);

resetBtn.addEventListener("click", resetDevices);

refreshAllBtn.addEventListener("click", refreshAll);

nameInput.addEventListener("keydown", event => {
  if(event.key === "Enter") {
    addDevice();
  }
});

channelInput.addEventListener("keydown", event => {
  if(event.key === "Enter") {
    addDevice();
  }
});

renderDevices();

refreshAll();

setInterval(refreshAll, 1000);

</script>

</body>
</html>

)rawliteral";

}

// =====================================================
// ROUTES
// =====================================================

void handleRoot() {

    server.send(200, "text/html", getHTML());
}

void handleRelayRoute() {

  if(!isValidChannel()) {

    server.send(400, "text/plain", "Invalid channel");

    return;
  }

    if(server.hasArg("state")) {

        String state = server.arg("state");

        if(state == "1") {
            setRelay(true);
        }
        else {
            setRelay(false);
        }

        ignoreInterruptUntil = millis() + 500;
    }

    server.send(200, "text/plain", "OK");
}

  void handleToggleRoute() {

    if(!isValidChannel()) {

      server.send(400, "text/plain", "Invalid channel");

      return;
    }

    toggleRelay();

    ignoreInterruptUntil = millis() + 500;

    server.send(200, "text/plain", "OK");
  }

void handleStatus() {

  if(!isValidChannel()) {

    server.send(400, "text/plain", "Invalid channel");

    return;
  }

    server.send(
        200,
        "text/plain",
        getRelayState() ? "1" : "0"
    );
}

// =====================================================
// INIT
// =====================================================

void initWebServer() {

  WiFi.mode(WIFI_AP_STA);

    WiFi.softAP(AP_SSID, AP_PASSWORD);

    Serial.println("AP Started");

    Serial.println(WiFi.softAPIP());

    server.on("/", handleRoot);

    server.on("/relay", handleRelayRoute);

    server.on("/toggle", handleToggleRoute);

    server.on("/status", handleStatus);

    server.begin();

    Serial.println("WebServer Started");
}

// =====================================================
// LOOP
// =====================================================

void handleWebServer() {

    server.handleClient();
}