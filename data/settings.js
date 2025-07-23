const keys = ["logLevel", "dataSaveInterval", "keepData", "timezoneOffset", "dayPhaseStart", "nightPhaseStart"];

const inputs = {};
for (const key of keys) {
    inputs[key] = document.getElementById(key);
}

let curData = {};
request("/api/settings").then((json) => {
    inputs["logLevel"].value = json.logLevel;
    inputs["dataSaveInterval"].value = json.dataSaveInterval;
    inputs["keepData"].value = json.keepData / 60 / 60;
    inputs["timezoneOffset"].value = `${json.timezoneOffset > 0 ? "+" : ""}${toTimeString(Math.abs(json.timezoneOffset))}`;
    inputs["dayPhaseStart"].value = toTimeString(json.dayPhaseStart);
    inputs["nightPhaseStart"].value = toTimeString(json.nightPhaseStart);
    curData = json;
}).catch((e) => {
    alert(e.message);
    console.error(e);
});

function saveSettings(event) {
    event.target.disabled = true;
    event.target.innerText = "Saving . . .";
    const payload = getPayload();
    request("/api/settings", "POST", payload).then(() => {
        location.reload();
    }).catch((e) => {
        event.target.disabled = false;
        event.target.innerText = "Save";
        alert(e.message);
        console.error(e);
    });
}

async function request(path, method = "GET", body = null) {
    const options = {
        method: method,
        headers: { "Content-Type": "application/json" },
    };
    if(body) options.body = JSON.stringify(body);

    let response = await fetch("http://192.168.0.102" + path, options).catch(() => {
        throw new Error("Failed to fetch");
    });
    const json = await response.json();
    if (!response.ok) throw new Error(json.detail);
    return json;
}

function getPayload(){
    const full = {
        logLevel: +inputs["logLevel"].value,
        dataSaveInterval: +inputs["dataSaveInterval"].value,
        keepData: inputs["keepData"].value * 60 * 60,
        timezoneOffset: fromTimeString(inputs["timezoneOffset"].value),
        dayPhaseStart: fromTimeString(inputs["dayPhaseStart"].value),
        nightPhaseStart: fromTimeString(inputs["nightPhaseStart"].value),
    };
    if(full.nightPhaseStart < 0 || full.nightPhaseStart > 24*60*60) full.nightPhaseStart = NaN;
    if(full.dayPhaseStart < 0 || full.dayPhaseStart > 24*60*60) full.dayPhaseStart = NaN;

    const res = {};
    keys.forEach((key)=>{
        if(isNaN(full[key])) throw new Error("Invalid value for " + key);
        if(full[key] !== curData[key]) res[key] = full[key];
    });
    return res;
}

function toTimeString(sec){
    const minutes = Math.floor(sec / 60) % 60;
    const hours = Math.floor(sec / 60 / 60);
    return `${hours.toString().padStart(2, "0")}:${minutes.toString().padStart(2, "0")}`;
}

function fromTimeString(time){
    const [hours, minutes] = time.split(":");
    return parseInt(hours) * 60 * 60 + parseInt(minutes) * 60;
}
