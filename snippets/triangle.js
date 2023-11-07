<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Rotating Cube</title>
    <style>
        body {
            margin: 0;
        }
        canvas {
            display: block;
        }
    </style>
</head>
<body>
<script>
    function getRandomArbitrary(min, max) {
        return Math.random() * (max - min) + min;
    }

    // constants
    const COLOR_BG = "black";
    const COLOR_CUBE = "white";
    const SPEED_X = 0; // 0.05; // rps
    const SPEED_Y = 0.05; // 0.15; // rps
    const SPEED_Z = 0; // 0.10; // rps
    const POINT3D = function(x, y, z) { this.x = x; this.y = y; this.z = z; };

    // set up the canvas and context
    var canvas = document.createElement("canvas");
    document.body.appendChild(canvas);
    var ctx = canvas.getContext("2d");

    // dimensions
    var h = document.documentElement.clientHeight;
    var w = document.documentElement.clientWidth;
    canvas.height = h;
    canvas.width = w;

    // colours and lines
    // ctx.fillStyle = `rgba(${getRandomArbitrary(0, 255)}, ${getRandomArbitrary(0, 255)}, ${getRandomArbitrary(0, 255)}, ${getRandomArbitrary(0, 1)})`; // COLOR_BG;
    ctx.fillStyle = COLOR_BG;
    ctx.strokeStyle = COLOR_CUBE;
    ctx.lineWidth = w / 100;
    ctx.lineCap = "round";

    // cube parameters
    var cx = w / 2;
    var cy = h / 2;
    var cz = 50;
    var size = h / 4;
    var vertices = [
        new POINT3D(cx - size, cy - size, cz - size),
        new POINT3D(cx + size, cy - size, cz - size),
        new POINT3D(cx + size, cy + size, cz - size),
        new POINT3D(cx - size, cy + size, cz - size),
        new POINT3D(cx - size, cy - size, cz + size),
        new POINT3D(cx + size, cy - size, cz + size),
        new POINT3D(cx + size, cy + size, cz + size),
        new POINT3D(cx - size, cy + size, cz + size)
    ];
    var edges = [
        [0, 1], [1, 2], [2, 3], [3, 0], // back face
        [4, 5], [5, 6], [6, 7], [7, 4], // front face
        [0, 4], [1, 5], [2, 6], [3, 7] // connecting sides
    ];
    
    // set up the animation loop
    var timeDelta, timeLast = 0;
    requestAnimationFrame(loop);

    function loop(timeNow) {

        // calculate the time difference
        timeDelta = timeNow - timeLast;
        timeLast = timeNow;

        // background
        ctx.fillStyle = COLOR_BG;
        ctx.fillRect(0, 0, w, h);

        // rotate the cube along the z axis
        let angle = timeDelta * 0.0001 * SPEED_Z * Math.PI * 2;
        for (let v of vertices) {
            let dx = v.x - cx;
            let dy = v.y - cy;
            let x = dx * Math.cos(angle) - dy * Math.sin(angle);
            let y = dx * Math.sin(angle) + dy * Math.cos(angle);
            v.x = x + cx;
            v.y = y + cy;
        }

        // rotate the cube along the x axis
        angle = timeDelta * 0.001 * SPEED_X * Math.PI * 2;
        for (let v of vertices) {
            let dy = v.y - cy;
            let dz = v.z - cz;
            let y = dy * Math.cos(angle) - dz * Math.sin(angle);
            let z = dy * Math.sin(angle) + dz * Math.cos(angle);
            v.y = y + cy;
            v.z = z + cz;
        }

        // rotate the cube along the y axis
        angle = timeDelta * 0.001 * SPEED_Y * Math.PI * 2;
        for (let v of vertices) {
            let dx = v.x - cx;
            let dz = v.z - cz;
            let x = dz * Math.sin(angle) + dx * Math.cos(angle);
            let z = dz * Math.cos(angle) - dx * Math.sin(angle);
            v.x = x + cx;
            v.z = z + cz;
        }

        // right trapezium face stroke
        /*ctx.beginPath();
        // move to top1 left
        ctx.moveTo(vertices[edges[0]].x, vertices[edges[0]].y);
        ctx.lineTo(vertices[edge[1]].x, vertices[edge[1]].y);
        // diagonal line to top2 right
        ctx.lineTo(xCoord + actorWidth + offsetX, yCoord + offsetY);
        // vertical line to bottom2 right
        ctx.lineTo(xCoord + actorWidth + offsetX, yCoord + offsetY + actorHeight);
        // diagonal line to bottom1 left
        ctx.lineTo(xCoord + actorWidth, yCoord + actorHeight);
        // vertical to top1 left
        ctx.closePath();

        // right trapezium face fill
        ctx.fillStyle = "rgba(180, 60, 30, .7)";
        ctx.strokeStyle = "black";
        ctx.fill();
        ctx.lineWidth = 1.0;
        ctx.lineCap = "round";
        ctx.lineJoin = "round";
        ctx.stroke();*/

        ctx.beginPath();
        // horizontal-top
        ctx.moveTo(vertices[edges[0][0]].x, vertices[edges[0][0]].y);
        ctx.lineTo(vertices[edges[0][1]].x, vertices[edges[0][1]].y);
        ctx.lineTo(vertices[edges[1][1]].x, vertices[edges[1][1]].y);
        // horizontal-lower
        // ctx.moveTo(vertices[edges[0][1]].x, vertices[edges[0][1]].y);
        // ctx.lineTo(vertices[edges[1][0]].x, vertices[edges[1][0]].y);
        // vertical-right
        // ctx.moveTo(vertices[edges[1][0]].x, vertices[edges[1][0]].y);
        // ctx.lineTo(vertices[edges[1][1]].x, vertices[edges[1][1]].y);

        ctx.closePath();
        ctx.strokeStyle = "white";
        // ctx.fillStyle = "white";
        ctx.lineWidth = 1.0;
        ctx.stroke();

        // call the next frame
        requestAnimationFrame(loop);
    }
</script>
</body>
</html>