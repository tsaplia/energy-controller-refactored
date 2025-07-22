function createSocket(path, onMessage) {
    const socket = new WebSocket(`ws://${location.host}${path}`);
    socket.addEventListener("message", onMessage);
    socket.addEventListener("error", (event) => {
        console.log("WebSocket error: ", event);
        alert("Failed to connect to server. Check connection.");
    });
    socket.addEventListener("open", () => console.log("WebSocket connected"));
    socket.addEventListener("close", () => console.log("WebSocket disconnected"));
    return socket;
}
