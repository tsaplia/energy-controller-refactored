google.charts.load("current", { packages: ["corechart"] });
google.charts.setOnLoadCallback(drawCharts);

const valueEls = document.querySelectorAll(".value");
const valueNames = ["voltage", "current", "power", "energy", "frequency", "pf"];

createSocket("/ws/data", (event) => {
    const json = JSON.parse(event.data);
    console.log(json);
    for (let i = 0; i < 6; i++) {
        valueEls[i].textContent = json.data[valueNames[i]];
    }
});

async function drawCharts(){
    try {
        const text = await requestFile("/sensor-data.csv");
        const fullRows = text
            .split("\n")
            .slice(1, -1)
            .map((line) => line.split(";"));
        const voltageRows = fullRows.map((row) => [new Date(row[0] * 1000), +row[1]]);
        const powerRows = fullRows.map((row) => [new Date(row[0] * 1000), +row[3]]);
        drawChart(voltageRows, "voltageChart");
        drawChart(powerRows, "powerChart");
    } catch (e) {
        console.error(e);
        alert(e.message);
    }
}

function drawChart(dataRows, id) {
    if(!dataRows.length) return;
    const fullRows = dataRows.map((row) => [...row, getTooltip(row)]);
    const data = google.visualization.arrayToDataTable([
        [
            { id: "date", type: "datetime" },
            { id: "value", type: "number" },
            { type: "string", role: "tooltip", p: { html: true } },
        ],
        ...fullRows,
    ]);
    const chart = new google.visualization.LineChart(document.getElementById(id));
    chart.draw(data, {
        legend: {
            position: "none",
        },
        hAxis: {
            format: "HH:mm",
            viewWindow: {
                min: new Date(dataRows[dataRows.length - 1][0].getTime() - 24 * 60 * 60 * 1000),
                max: dataRows[dataRows.length - 1][0],
            },
        },
        chartArea: {
            left: "14%",
            top: "5%",
            width: "80%",
            height: "80%",
        },
        tooltip: { isHtml: true },
        explorer: {
            axis: "horizontal",
            maxZoomIn: 1 / 24,
            maxZoomOut: 1, 
            zoomDelta: 1.3
        },
    });
}

function getTooltip(row) {
    return `<div>${dataDateFormat(row[0])} <b>${row[1]}</b></div>`;
}
