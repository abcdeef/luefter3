var gateway = `ws://${window.location.hostname}:81/`;
var websocket;

function debug(msg) {
    console.log(msg);
    document.getElementById("debug").append(msg);
}
function docReady(fn) {
    if (document.readyState === "complete" || document.readyState === "interactive") {
        setTimeout(fn, 1);
    } else {
        document.addEventListener("DOMContentLoaded", fn);
    }
}

function connect() {
    websocket = new WebSocket(gateway);
    websocket.binaryType = "arraybuffer";

    websocket.onmessage = onMessage;

    websocket.onopen = onOpen;

    websocket.onclose = function (event) {
        debug("close");
        setTimeout(function () {
            debug("reconnect");
            connect();
        }, 500);
    };
    websocket.onerror = function (event) {
        debug("error");
        websocket.close();
    }
}
docReady(function () {
    connect();
});
