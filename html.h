String htmlDocument = R"rawliteral(<!DOCTYPE html>
<html>
    <head>
        <meta name="viewport" content="width=device-width, initial-scale=1.0, user-scalable=no">
        <title>LED Control</title>
        <style>
            body,.group{
                display: flex;
                flex-direction: column;
            }
            body{
                gap: 15px;
            }
            #temp{
                font-size: 34px;
            }

            .countdown {
                /* display: grid; */
                /* margin: 1em auto; */
                width: 1.5em;
                height: 1.5em;
            }
            .countdown::after {
                /* grid-column: 1; */
                /* grid-row: 1; */
                /* place-self: center; */
                font: 1em/2 ubuntu mono, consolas, monaco, monospace;
                animation: num 10s steps(1) infinite;
                content: "" counter(s, decimal-leading-zero);
            }

            @keyframes num {
            0% {
                counter-reset: s 10;
            }
            10% {
                counter-reset: s 9;
            }
            20% {
                counter-reset: s 8;
            }
            30% {
                counter-reset: s 7;
            }
            40% {
                counter-reset: s 6;
            }
            50% {
                counter-reset: s 5;
            }
            60% {
                counter-reset: s 4;
            }
            70% {
                counter-reset: s 3;
            }
            80% {
                counter-reset: s 2;
            }
            90% {
                counter-reset: s 1;
            }
            100% {
                counter-reset: s 0;
            }
            }
            svg {
                position: relative;
                left: -.2em;
                top: 2em;
            }

            [r] {
                fill: none;
                stroke: silver;
            }
            [r] + [r] {
                transform: rotate(-90deg);
                stroke-linecap: round;
                stroke: #8a9b0f;
                animation: arc 10s linear infinite;
                animation-name: arc, col;
            }

            @keyframes arc {
            0% {
                stroke-dashoffset: 0px;
            }
            }
            @keyframes col {
            0% {
                stroke: #8a9b0f;
            }
            25% {
                stroke: #f8ca00;
            }
            50% {
                stroke: #e97f02;
            }
            75% {
                stroke: #bd1550;
            }
            100% {
                stroke: #490a3d;
            }
            }

            #groupTemp{
                display: flex;
                flex-direction: row;
            }
            canvas{
                border: 1px solid black;
            }
            #xwidth{
                width: 15%;
            }
        </style>
    </head>
    <body>
        <h1>ESP8266 Web Server</h1>
        <span id="host">localhost</span>

        <div id="groupTemp">
            <p id="temp">No temperature</p>
        <div class="countdown">
            <svg viewBox="-50 -50 100 100" stroke-width="10">
                <circle r="45"></circle>
                <circle r="45" stroke-dasharray="282.7433388230814" stroke-dashoffset="282.7433388230814px"></circle>
            </svg>
        </div>
        </div>
        <select name="xwidth" id="xwidth">
            <option value="6">1 minut</option>
            <option value="60">10 minutes</option>
            <option value="180">30 minutes</option>
            <option value="360">1 hour</option>
        </select>
        <canvas width="600%" id="canvas"></canvas>

        <div class="group">
            <label for="devName">Device name</label>
            <input type="text" id="devName">
        </div>

        <div class="group">
            <label for="ssid">WiFi name</label>
            <input type="text" id="ssid">
        </div>

        <div class="group">
            <label for="pass">WiFi password</label>
            <input type="text" id="pass">
        </div>

        <button id="set">Set params</button>
        <button id="reset">Reset device</button>

        <p id="err"></p>

        <script>
            const ssid = document.getElementById('ssid');
            const pass = document.getElementById('pass');
            const temp = document.getElementById('temp');
            const btn = document.getElementById('set');
            const rst = document.getElementById('reset');
            const err = document.getElementById('err');
            const host = document.getElementById('host');
            const name = document.getElementById('devName');
            const canvas = document.getElementById('canvas');
            const combo = document.getElementById('xwidth');
            const ctx = canvas.getContext('2d');
            const devIP = window.location.host;

            

            let minTemp = 0;
            let maxTemp = 0;
            let prevTemp = minTemp;
            let xWidth = combo.value;
            let points = [];

            ctx.fillStyle = "black";
            ctx.lineWidth = 1.0;

            const addPoint = (val) => {
                if(points.length>=xWidth)points = points.slice(1, xWidth);
                points.push(val);
                if (val > maxTemp) maxTemp = val;
                if (val < minTemp) minTemp = val;
                update();
            }

            const calcY = (val) => {
                const dt = maxTemp - minTemp;
                const yStep = Math.floor(canvas.height / (dt ? dt : 2));
                return dt ? (dt - (val - minTemp)) * yStep : yStep;
            }

            const update = () =>{
                ctx.clearRect(0, 0, canvas.width, canvas.height);
                let dx = canvas.width / xWidth;
                ctx.beginPath();
                points.forEach((point,i) => {
                    ctx.lineTo(i * dx+30, calcY(point));
                    ctx.moveTo(i*dx + 30, calcY(point));
                });
                ctx.fillText(maxTemp + "", 1, calcY(maxTemp) + 10);
                ctx.fillText(minTemp + "", 1, calcY(minTemp));
                ctx.stroke();
            }

            window.addEventListener('load', ()=>{
                host.innerText = devIP;//`http://${devIP}/`;
                fetch(`http://${devIP}/params`)
                .then(response => response.json())
                .then(data => {
                    ssid.value = data.ssid;
                    pass.value = data.password;
                    temp.innerText = parseFloat(data.temperature);
                    name.value = data.name;
                    minTemp = parseFloat(data.temperature);
                    maxTemp = parseFloat(data.temperature);
                    addPoint(parseFloat(data.temperature));
                })
                .catch(error => {err.innerText = error.message;});
            });
            setInterval(() => {
                fetch(`http://${devIP}/temp`)
                    .then(response => response.json())
                    .then(data => {
                        temp.innerText = data.temperature;
                        addPoint(data.temperature);
                    })
                    .catch(error => { err.innerText = error.message; });
            },10000);
            btn.addEventListener('click', ()=>{
                fetch(`http://${devIP}/params?ssid=${ssid.value}&password=${pass.value}&name=${name.value}`,{
                    method:'PUT'
                })
                    .then(response => response.json())
                    .then(data => {
                        if(ssid.value === data.ssid && pass.value === data.password) {
                            err.innerText = 'Updated! Reset device.';
                        }else{
                            err.innerText = 'Fail. Try again.';
                        }
                        clearInfo();
                    })
                    .catch(error => { err.innerText = error.message; });
            });
            rst.addEventListener('click', () => {
                    fetch(`http://${devIP}/reset`);
            });

            const clearInfo = () => setTimeout(()=>{err.innerText = ''}, 3000);

            combo.addEventListener('change', () => { xWidth = combo.value; update(); });
            
        </script>
    </body>
</html>
  )rawliteral";