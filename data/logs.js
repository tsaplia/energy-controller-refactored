const logBox = document.getElementById("log-box");

createSocket("/ws/logs", (event) => {
    const json = JSON.parse(event.data);
    if (json.type === "history") {
        const logs = json.data.split(";");
        logs.forEach((log) => addLog(log));
    } else if (json.type === "log") {
        addLog(json.data);
    } else {
        console.error("Unknown message type: " + json.type);
    }
});

function addLog(log) {
    const logEl = document.createElement("div");
    logEl.innerText = log.trim();
    logBox.appendChild(logEl);
    if (logBox.scrollHeight - logBox.clientHeight - logBox.scrollTop < 40) {
        logBox.scrollTop = logBox.scrollHeight;
    }
}
