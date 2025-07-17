const valueEls = document.querySelectorAll(".value");
const valueNames = ["voltage", "current", "power", "energy", "frequency", "pf"];

createSocket("/ws/data", (event) => {
    const json = JSON.parse(event.data);
    console.log(json);
    for(let i=0; i<6; i++) {
        valueEls[i].textContent = json.data[valueNames[i]];
    }
});