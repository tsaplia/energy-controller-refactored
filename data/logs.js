const logBox = document.getElementById("log-box");
const socket = new WebSocket("ws://" + location.hostname + "/ws/logs");

function addLog(log) {""
    const logEl = document.createElement("div");
    logEl.innerText = log.trim();
    logBox.appendChild(logEl);
}

socket.addEventListener("message", (event) => {
    const data = JSON.parse(event.data);
    if (data.type === "history") {
        const logs = data.log.split(";");
        logs.forEach((log) => addLog(log));
    } else if (data.type === "log") {
        addLog(data.log);
    } else {
        console.log("Unknown message type: " + data.type);
    }
});

socket.addEventListener("error", (event) => {
    console.log("WebSocket error: ", event);
    alert("Failed to connect to server. Check connection.");
});
socket.addEventListener("open", () => console.log("WebSocket connected"));
socket.addEventListener("close", () => console.log("WebSocket disconnected"));
