function Val2String(asd) {
    return asd.toString() + "%";
}
function onMessage(event) {
    var tmp;
    try {
        tmp = JSON.parse(event.data);
    } catch (e) {
        console.log(e);
        console.log(event.data);
    }
    if (tmp.PWM) {
        document.getElementById(tmp.PWM.pin).value = tmp.PWM.val;
        document.getElementById(tmp.PWM.pin).parentElement.nextSibling.innerHTML = Val2String(tmp.PWM.val);
    }
    if (tmp.PWMS) {
        var elem = document.getElementById("pwms");
        elem.innerHTML = "";
        for (let i = 0; i < tmp.PWMS.length; i++) {
            var b1 = document.createElement("div");
            b1.innerHTML = tmp.PWMS[i].name;
            elem.appendChild(b1);

            var slider = document.createElement("div");
            slider.className = "slidecontainer";
            var sl = document.createElement("input");
            sl.className = "slider";
            sl.type = "range";
            sl.min = "0";
            sl.step = "10";
            sl.max = "100";
            sl.value = tmp.PWMS[i].val;
            sl.id = tmp.PWMS[i].pin;

            let b3 = document.createElement("div");
            sl.oninput = function () {
                websocket.send(new Uint8Array([1, this.id, this.value]).buffer);
                b3.innerHTML = Val2String(this.value);
            };
            slider.appendChild(sl);
            elem.appendChild(slider);

            b3.innerHTML = Val2String(tmp.PWMS[i].val);
            elem.appendChild(b3);
        }
        var slider = document.createElement("div");
        slider.className = "slidecontainer";
        slider.style = "grid-column:2"
        var sl = document.createElement("input");
        sl.className = "slider";
        sl.type = "range";
        sl.min = "0";
        sl.step = "10";
        sl.max = "100";
        sl.value = 0;
        sl.id = "all";
        slider.appendChild(sl);
        elem.appendChild(slider);
    
    }
    if (tmp.VERSION) {
        var element = document.getElementById("version");
        element.innerHTML = "Version: " + tmp.VERSION;
    }
    if (tmp.TEMP) {
        var element = document.getElementById("temp");
        element.innerHTML = "Wassertemperatur: " + tmp.TEMP;
    }
}

function onOpen() {
    websocket.send(new Uint8Array([0]).buffer);
}
