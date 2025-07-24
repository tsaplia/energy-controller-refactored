function createSocket(path, onMessage) {
    const socket = new WebSocket(`ws://${location.host}${path}`);
    socket.addEventListener("message", onMessage);
    socket.addEventListener("error", (event) => {
        console.error("WebSocket error: ", event);
        alert("Failed to connect to server. Check connection.");
    });
    socket.addEventListener("open", () => console.log("WebSocket connected"));
    socket.addEventListener("close", () => console.log("WebSocket disconnected"));
    return socket;
}

async function request(path, method = "GET", body = null) {
    const options = {
        method: method,
        headers: { "Content-Type": "application/json" },
    };
    if (body) options.body = JSON.stringify(body);

    let response = await fetch(path, options).catch(() => {
        throw new Error("Failed to fetch");
    });
    const json = await response.json();
    if (!response.ok) throw new Error(json.detail);
    return json;
}

async function requestFile(path) {
    const res = await fetch(path);
    if(!res.ok) throw new Error(res.statusText);
    return res.text();
}

function dataDateFormat(date) {
    const datePart = date.toLocaleDateString("en-GB").slice(0, 5);
    const timePart = date.toLocaleTimeString("en-GB", {
        hour: "2-digit",
        minute: "2-digit",
        second: "2-digit",
    });
    return `${datePart} ${timePart}`;
}