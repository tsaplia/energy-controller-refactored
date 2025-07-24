const settingKeys = ["logLevel", "dataSaveInterval", "keepData", "timezoneOffset", "dayPhaseStart", "nightPhaseStart"];

const settingInputs = {};
for (const key of settingKeys) {
    settingInputs[key] = document.getElementById(key);
}

const ssidInput = document.getElementById("wifiName");
const passInput = document.getElementById("wifiPass");

let curData = {};
request("/api/settings")
    .then((json) => {
        settingInputs["logLevel"].value = json.logLevel;
        settingInputs["dataSaveInterval"].value = json.dataSaveInterval;
        settingInputs["keepData"].value = json.keepData / 60 / 60;
        settingInputs["timezoneOffset"].value = `${json.timezoneOffset > 0 ? "+" : ""}${secToTimeString(
            Math.abs(json.timezoneOffset)
        )}`;
        settingInputs["dayPhaseStart"].value = secToTimeString(json.dayPhaseStart);
        settingInputs["nightPhaseStart"].value = secToTimeString(json.nightPhaseStart);
        curData = json;
    })
    .catch((e) => {
        alert(e.message);
        console.error(e);
    });

async function saveSettings(event) {
    processingWrapper(event, async () => {
        const payload = getPayload();
        await request("/api/settings", "POST", payload);
        location.reload();
    });
}

async function connectToWifi(event) {
    processingWrapper(event, async () => {
        if (!ssidInput.value || !passInput.value) return;
        const payload = { ssid: ssidInput.value, password: passInput.value };
        await request("/api/connect-wifi", "POST", payload);
        alert("Connection request sent. Please wait a few seconds.");
    });
}

async function resetEnergy(event) {
    processingWrapper(event, async () => {
        if (!confirm("Are you sure you want to reset energy?")) return;
        await request("/api/reset-energy", "PUT");
        alert("Energy has been reset.");
    });
}

async function processingWrapper(event, func) {
    try {
        event.target.disabled = true;
        await func(event);
    } catch (e) {
        alert(e.message);
        console.error(e);
    } finally {
        event.target.disabled = false;
    }
}

async function getEnergyStatsFile(event) {
    processingWrapper(event, async () => {
        const raw = await requestFile("/energy-stats.csv");
        const values = raw
            .slice(0, -1) // to remove last \n
            .split("\n")
            .map((line) => line.split(";")); // [date, phase, energy][]
        values.sort((a, b) => {
            if (a[0] === b[0]) return a[1] < b[1] ? -1 : 1; // since D < N
            return a[0] < b[0] ? -1 : 1; // since date has YYYY-MM-DD format
        });

        let res = "Date;Day;Night;Total\n";
        for (let i = 1; i < values.length; i++) {
            const curValue = (values[i][2] - values[i - 1][2]).toFixed(1);
            if (values[i][1] === "D") {
                res += `${values[i][0]};${curValue};`;
            } else {
                if (i === 1) {
                    res += `${values[i][0]};-;${curValue};-\n`;
                    continue;
                }
                const totalValue = (values[i][2] - values[i - 2][2]).toFixed(1);
                res += `${curValue};${totalValue};\n`;
            }
        }
        saveToFile(res, "energy-stats.csv");
    });
}

async function getSensorDataFile(event) {
    processingWrapper(event, async () => {
        const text = await requestFile("/sensor-data.csv");
        const formattedText = text
            .slice(0, -1) // to remove last \n
            .split("\n")
            .map((line, ind) => {
                if (ind === 0) return line;
                const timestamp = line.slice(0, line.indexOf(";"));
                return line.replace(timestamp, dataDateFormat(new Date(timestamp * 1000)));
            })
            .join("\n");
        saveToFile(formattedText, "sensor-data.csv");
    });
}

function getPayload() {
    const full = {
        logLevel: +settingInputs["logLevel"].value,
        dataSaveInterval: +settingInputs["dataSaveInterval"].value,
        keepData: settingInputs["keepData"].value * 60 * 60,
        timezoneOffset: secFromTimeString(settingInputs["timezoneOffset"].value),
        dayPhaseStart: secFromTimeString(settingInputs["dayPhaseStart"].value),
        nightPhaseStart: secFromTimeString(settingInputs["nightPhaseStart"].value),
    };
    if (full.nightPhaseStart < 0 || full.nightPhaseStart > 24 * 60 * 60) full.nightPhaseStart = NaN;
    if (full.dayPhaseStart < 0 || full.dayPhaseStart > 24 * 60 * 60) full.dayPhaseStart = NaN;

    const res = {};
    settingKeys.forEach((key) => {
        if (isNaN(full[key])) throw new Error("Invalid value for " + key);
        if (full[key] !== curData[key]) res[key] = full[key];
    });
    return res;
}

function secToTimeString(sec) {
    const minutes = Math.floor(sec / 60) % 60;
    const hours = Math.floor(sec / 60 / 60);
    return `${hours.toString().padStart(2, "0")}:${minutes.toString().padStart(2, "0")}`;
}

function secFromTimeString(time) {
    const [hours, minutes] = time.split(":");
    return parseInt(hours) * 60 * 60 + parseInt(minutes) * 60;
}

function saveToFile(text, filename) {
    const blob = new Blob([text], { type: "text/plain" });
    const url = URL.createObjectURL(blob);
    const link = document.createElement("a");
    link.href = url;
    link.download = filename;
    document.body.appendChild(link);
    link.click();
    link.remove();
    URL.revokeObjectURL(url);
}
