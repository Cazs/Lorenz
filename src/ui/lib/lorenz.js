class LorenzWSClientConfig {
    socket;
    run = true;
    camPanSpeed = 10.1;
    actorSize = 5;
    renderRate = 20;
    hpBarWidth = 100;
    hpBarHeight = 10;
    playerStatsStartPosX = 20;
    playerStatsStartPosY = 20;
    bullets = {}; // as LorenzWSClientBullet
    validBulletDirections = ['t', 'b', 'l', 'r', 'tr', 'tl', 'br', 'bl'];
    animate = {
        hpFontSize: 15,
        xpFontSize: 15,
        hitCountFontSize: 15,
        killCountFontSize: 15,
        camX: 0,
        camY: 0,
        camW: 400,
        camH: 400,
        camShake: 0,
        worldOffsetX: 0,
        worldOffsetY: 0,
        bulletUpdateInterval: 150,
        bulletFireRate: 350,
    };
    controls = {
        speed: Number(0.1),
        xSpeed: 0,
        ySpeed: 0,
        bulletSpeed: Number(7), // TODO: Compensate for network latency
        lastBulletUpdate: '',
        lastKnownDirection: 'r',
    
        wKey: {
            lastActive: new Date().getTime(),
            lastActiveDiff: 0,
            active: false,
            speed: Number(0),
        },
        sKey: {
            lastActive: new Date().getTime(),
            lastActiveDiff: 0,
            active: false,
            speed: Number(0),
        },
        aKey: {
            lastActive: new Date().getTime(),
            lastActiveDiff: 0,
            active: false,
            speed: Number(0),
        },
        dKey: {
            lastActive: new Date().getTime(),
            lastActiveDiff: 0,
            active: false,
            speed: Number(0),
        },
        spaceKey: {
            lastActive: new Date().getTime(),
            lastActiveDiff: 0,
            active: false,
            speed: Number(0),
        },
        up: {
            lastActive: new Date().getTime(),
            lastActiveDiff: 0,
            active: false,
        },
        down: {
            lastActive: new Date().getTime(),
            lastActiveDiff: 0,
            active: false,
        },
        left: {
            lastActive: new Date().getTime(),
            lastActiveDiff: 0,
            active: false,
        },
        right: {
            lastActive: new Date().getTime(),
            lastActiveDiff: 0,
            active: false,
        },
    };
};

class LorenzWSClient {
    id = 0;
    x = Number(30);
    y = Number(30);
    hp = 100;
    xp = 0;
    level = 0;
    hitCount = 0;
    killCount = 0;
    closestBuilding = {};
    closestBuildingDistance = 0;
    bullets = {}; // as LorenzWSClientBullet
    worldObjectCollision = {}; // as LorenzWSWorldObject
};

class LorenzWSClientBullet {
    id = 0;
    x = 0;
    y = 0;
    hasImpacted = false;
};

class LorenzWSWorldObject {
    id = 0;
    x = 0;
    y = 0;
    w = 0;
    h = 0;
    z = 0;
    type = '';
};

class Lorenz {
    MAX_WIDTH; // : Number
    MAX_HEIGHT; // : Number
    window;
    document;
    canvas; // : HTML5Element
    ctx; // : HTML5Element
    appConfig; // : LorenzWSClientConfig;
    localClient; // : LorenzWSClient;
    remoteClient; // : LorenzWSClient;
    worldMap = []; // : LorenzWSWorldObject[]

    x = 200;
    y = 200;
    r = 50;
    pos = {
        x: 200,
        y: 200,
        pozX: 200,
        pozY: 200,
    }

    constructor(window, document, canvasId) {
        this.window = window;
        this.document = document;
        this.MAX_WIDTH = 1366; // TODO
        this.MAX_HEIGHT = 720; // TODO
        
        this.canvas = this.document.getElementById(canvasId);
        this.canvas.width = this.MAX_WIDTH - 30;
        this.canvas.height = this.MAX_HEIGHT - 30;
        this.ctx = this.canvas.getContext('2d');

        this.appConfig = new LorenzWSClientConfig();
        this.localClient = new LorenzWSClient();
        this.remoteClient = new LorenzWSClient();

        // TODO: default to map constants?
        this.localClient.closestBuildingDistance = -20000; // TODO: default to map constants?
        this.localClient.closestBuildingDistance = {
            x: -20000,
            y: -20000,
            w: 0,
            h: 0,
        }
        // this.wsServerIp = wsIp;

        this.pos = {
            x: 200,
            y: 200,
            pozX: 200,
            pozY: 200,
        }

        this.worldMap['8'] = {
            id: 8,
            x: 100,
            y: 300,
            w: 50,
            h: 50,
            z: 25,
            type: 1,
        }
    };

    getRandomArbitrary(min, max) {
        return Math.random() * (max - min) + min;
    };

    drawDistanceLine(obj1, obj2) {
        this.ctx.strokeStyle = "cyan";
        this.ctx.fillStyle = "cyan";
        
        this.ctx.beginPath();

        this.ctx.moveTo(obj2.x, obj2.y);
        this.ctx.lineTo(obj1.x, obj1.y);

        this.ctx.closePath();

        this.ctx.fill();
        this.ctx.stroke();
    }

    isCollide(worldObjectPos, localPlayerPos) {
        switch(worldObjectPos.type) {
            case 0:
                return this.isCubeCollide(worldObjectPos, localPlayerPos)
            case 1:
                return this.isCylinderCollide(worldObjectPos, localPlayerPos);
            default:
                return undefined;
        }
    };
    
    isCubeCollide(a, b) {
        return !(
            ((a.y + a.h) < (b.y)) ||
            (a.y > (b.y + b.h)) ||
            ((a.x + a.w) < b.x) ||
            (a.x > (b.x + b.w))
        );
    };

    isCylinderCollide(worldObject, localPlayer) {
        /*var p1x = a.x, p1y = a.y, r1 = a.w, p2x = b.x, p2y = b.y, r2 = b.w;
        var a;
        var x;
        var y;
      
        a = r1 + r2;
        x = p1x - p2x;
        y = p1y - p2y;*/
      
        /*if (a > Math.sqrt((x * x) + (y * y))) {
          return true;
        } else {
          return false;
        }*/

        // return (a > Math.sqrt((x * x) + (y * y)));
        // const p2oColl = this.intersect(a, b);
        // console.log('>>>> isCylinderCollide: ', p2oColl);
        // return p2oColl;

        var lp = {
            x: this.localClient.x + this.appConfig.animate.worldOffsetX,
            y: this.localClient.y + this.appConfig.animate.worldOffsetY,
            w: this.appConfig.actorSize,
            h: this.appConfig.actorSize
        };
        var o = {
            x: Number(worldObject.x) + this.appConfig.animate.worldOffsetX,
            y: Number(worldObject.y) + this.appConfig.animate.worldOffsetY,
            w: Number(worldObject.w),
            h: Number(worldObject.h),
        };
        const worldObjectCenter = {
            id: worldObject.id,
            x: Number(worldObject.x) + (Number(worldObject.w) / 2) + this.appConfig.animate.worldOffsetX,
            y: Number(worldObject.y) + (Number(worldObject.h) / 2) + this.appConfig.animate.worldOffsetY,
            w: Number(worldObject.w),
            h: Number(worldObject.h),
        };

        // var playerToObjectDistance = this.getDistance({...this.localClient, x: this.localClient.x + this.appConfig.animate.worldOffsetX, y: this.localClient.y + this.appConfig.animate.worldOffsetY }, worldObjectCenter);
        // var playerToObjectDistance = this.getDistance(lp, worldObjectCenter);
        console.log('>>>> isCylinderCollide localPlayer: ', localPlayer);
        console.log('>>>> isCylinderCollide worldObject: ', worldObject);
        // var playerToObjectDistance = this.getDistance(localPlayer, worldObject);
        var playerToObjectDistance = 0;
        var worldObjectX = 0;
        var worldObjectY = 0;
        var localPlayerX = 0;
        var localPlayerY = 0;

        // X
        if(localPlayer.x > worldObject.x) {
            worldObjectX = worldObject.x + worldObject.w;
            localPlayerX = localPlayer.x;
        } else {
            worldObjectX = worldObject.x;
            localPlayerX = localPlayer.x + localPlayer.w;
        }
        // Y
        if(localPlayer.y > worldObject.y) {
            worldObjectY = worldObject.y + worldObject.h;
            localPlayerY = localPlayer.y; 
        } else {
            worldObjectY = worldObject.y;
            localPlayerY = localPlayer.y + localPlayer.h;
        } 
  
        // playerToObjectDistance = this.getDistance(localPlayer, { ...worldObject, x: worldObjectX, y: worldObjectY });
        playerToObjectDistance = this.getDistance({ ...localPlayer, x: localPlayerX, y: localPlayerY }, { ...worldObject, x: worldObjectX, y: worldObjectY });

        console.log('>>>> isCylinderCollide playerToObjectDistance: ', playerToObjectDistance);
        // var d1 = this.getDistance(lp, o);
        // console.log('>>>> isCylinderCollide Dist1: ', Math.abs(Math.floor(d1 - o.w/2)));
        // var d2 = this.getDistance(lp, worldObject);
        // console.log('>>>> isCylinderCollide Dist2: ', d2);
        // var p2oColl = Math.abs(Math.floor(d1)) <= 0;
        // var p2oColl = Math.abs(Math.floor(d1 - o.w/2)) <= 0;
        var p2oColl = Math.floor(playerToObjectDistance) <= 2;
        console.log('>>>> isCylinderCollide: ', p2oColl);
        return p2oColl;
    }
      
    intersect(localPlayer, worldCylinderObject) {
        var productX = localPlayer.x - worldCylinderObject.x;
        var productY = localPlayer.y - worldCylinderObject.y;
        var distance = Math.sqrt(productX * productX + productY * productY);
        if (distance <= (localPlayer.r + worldCylinderObject.r)) {
          return true;
        } else {
          return false;
        }
    }

    getCollisionSide(a, b) {
        if((Math.abs(a.x) + a.w) - (Math.abs(b.x) + b.w) >= 0) {
            return 'l';
        }

        if((Math.abs(a.x) + a.w) - (Math.abs(b.x) + b.w) <= 0) {
            return 'r';
        }
        
        if((Math.abs(a.y) + a.h) - (Math.abs(b.y) + b.h) >= 0) {
            return 't';
        }

        if((Math.abs(a.y) + a.h) - (Math.abs(b.y) + b.h) <= 0) {
            return 'b';
        }

        return side;
    }
    
    getDistance(obj1, obj2) {
        // return Math.sqrt(((Number(obj1.x || 0) - Number(obj2.x || 0)) * 2) + ((Number(obj1.y || 0) - Number(obj2.y || 0)) * 2));
        return Math.sqrt(((Math.abs(obj1.x || 0) - Math.abs(obj2.x || 0)) * 2) + ((Math.abs(obj1.y || 0) - Math.abs(obj2.y || 0)) * 2));
    }

    renderWorldBackground() {
        for(var i = 0; i < 20; i++) {
            this.ctx.fillStyle = "white";
            this.ctx.fillText('*', this.getRandomArbitrary(0, this.MAX_WIDTH), this.getRandomArbitrary(0, this.MAX_HEIGHT));

            if(this.appConfig.animate.camShake > 0) {
                this.appConfig.animate.camShake -= 1;
            }

            if(this.appConfig.animate.camShake > 0) {
                this.ctx.strokeStyle = "red";
                this.ctx.lineWidth = this.appConfig.animate.camShake;
                this.ctx.strokeRect(this.appConfig.animate.camX, this.appConfig.animate.camY, this.canvas.width - this.appConfig.animate.camShake, this.canvas.height - this.appConfig.animate.camShake + 20);
            }
        }
    };

    drawCube(objectCoordX, objectCoordY, objectLength, objectWidth, objectHeight) {
        const localPlayerCoords = {
            ...this.localClient,
            x: this.localClient.x + this.appConfig.animate.worldOffsetX,
            y: this.localClient.y + this.appConfig.animate.worldOffsetY,
            w: this.appConfig.actorSize,
            h: this.appConfig.actorSize,
        };

        const lpToObjX = ((objectCoordX - localPlayerCoords.x) / objectLength) * objectHeight; // 10; // TODO: diff limit? object height/z?
        const lpToObjY = ((objectCoordY - localPlayerCoords.y) / objectWidth) * objectHeight; // 10; // TODO: diff limit? object height/z?

        // BOTTOM FACE
        this.drawBottomFace(objectCoordX, objectCoordY, objectLength, objectWidth);

        if(lpToObjX > 0) {
            // LEFT FACE
            this.drawLeftFace(objectCoordX, objectCoordY, objectLength, objectWidth, lpToObjX, lpToObjY);
        } else {
            // RIGHT FACE
            this.drawRightFace(objectCoordX, objectCoordY, objectLength, objectWidth, lpToObjX, lpToObjY);
        }

        if(lpToObjY > 0) {
            // NORTH FACE
            this.drawNorthFace(objectCoordX, objectCoordY, objectLength, objectWidth, lpToObjX, lpToObjY);
        } else {
            // SOUTH FACE
            this.drawSouthFace(objectCoordX, objectCoordY, objectLength, objectWidth, lpToObjX, lpToObjY);
        }

        // TOP FACE
        this.drawTopFace(objectCoordX, objectCoordY, objectLength, objectWidth, lpToObjX, lpToObjY);

        // DISTANCE LINE
        /*this.ctx.fillStyle = "magenta";
        this.ctx.strokeStyle = "magenta";
        const x = objectCoordX + (objectLength / 2) + offsetX;
        const y = objectCoordY + (objectWidth / 2) + offsetY;

        this.ctx.beginPath();
        this.ctx.moveTo(x, y);
        this.ctx.lineTo(this.localClient.x + this.appConfig.animate.worldOffsetX, this.localClient.y + this.appConfig.animate.worldOffsetY);
        this.ctx.closePath();
        this.ctx.stroke();
        this.ctx.arc(x, y, 10, 0, 2 * Math.PI);*/
    }

    drawBottomFace(xCoord, yCoord, actorWidth, actorHeight) {
        // rear square face stroke
        this.ctx.beginPath();
        // move to top1 left
        this.ctx.moveTo(xCoord, yCoord);
        // diagonal line to top2 right
        this.ctx.lineTo(xCoord + actorWidth, yCoord);
        // vertical line to bottom2 right
        this.ctx.lineTo(xCoord + actorWidth, yCoord + actorHeight);
        // diagonal line to bottom1 left
        this.ctx.lineTo(xCoord, yCoord + actorHeight);
        // vertical to top1 left
        this.ctx.closePath();

        // rear square face fill
        this.ctx.fillStyle = "#3F4042"; // "red";
        this.ctx.strokeStyle = "#3F4042"; // "black";
        this.ctx.fill();
        this.ctx.lineWidth = 1.0;
        this.ctx.lineCap = "round";
        this.ctx.lineJoin = "round";
        this.ctx.stroke();
    }

    drawRightFace(xCoord, yCoord, actorWidth, actorHeight, offsetX, offsetY) {
        // right trapezium face stroke
        this.ctx.beginPath();
        // move to top1 left
        this.ctx.moveTo(xCoord + actorWidth, yCoord);
        // diagonal line to top2 right
        this.ctx.lineTo(xCoord + actorWidth + offsetX, yCoord + offsetY);
        // vertical line to bottom2 right
        this.ctx.lineTo(xCoord + actorWidth + offsetX, yCoord + offsetY + actorHeight);
        // diagonal line to bottom1 left
        this.ctx.lineTo(xCoord + actorWidth, yCoord + actorHeight);
        // vertical to top1 left
        this.ctx.closePath();

        // right trapezium face fill
        this.ctx.fillStyle = "#929498"; // "rgba(180, 60, 30, .7)";
        this.ctx.strokeStyle = "#929498"; // "black";
        this.ctx.fill();
        this.ctx.lineWidth = 1.0;
        this.ctx.lineCap = "round";
        this.ctx.lineJoin = "round";
        this.ctx.stroke();
    }

    drawLeftFace(xCoord, yCoord, actorWidth, actorHeight, offsetX, offsetY) {
        // left trapezium face stroke
        this.ctx.beginPath();
        // move to top1 left
        this. ctx.moveTo(xCoord, yCoord);
        // diagonal line to top2 right
        this.ctx.lineTo(xCoord + offsetX, yCoord + offsetY);
        // vertical line to bottom2 right
        this.ctx.lineTo(xCoord + offsetX, yCoord + offsetY + actorHeight);
        // diagonal line to bottom1 left
        this.ctx.lineTo(xCoord, yCoord + actorHeight);
        // vertical to top1 left
        this.ctx.closePath();

        // left trapezium face fill
        this.ctx.fillStyle = "#929498"; // "rgba(50, 255, 30, .7)";
        this.ctx.strokeStyle = "#929498"; // "black";
        this.ctx.fill();
        this.ctx.lineWidth = 1.0;
        this.ctx.lineCap = "round";
        this.ctx.lineJoin = "round";
        this.ctx.stroke();
    }

    drawSouthFace(xCoord, yCoord, actorWidth, actorHeight, offsetX, offsetY) { 
        // north trapezium face stroke
        this.ctx.beginPath();
        this. ctx.moveTo(xCoord, yCoord + actorHeight);
        this.ctx.lineTo(xCoord + offsetX, yCoord + actorHeight + offsetY);
        this.ctx.lineTo(xCoord + offsetX + actorWidth, yCoord + actorHeight + offsetY);
        this.ctx.lineTo(xCoord + actorWidth, yCoord + actorHeight);
        this.ctx.closePath();

        // north trapezium face fill
        this.ctx.fillStyle = "#525254"; // "blue"; // "rgba(50, 255, 30, .7)"; // "#929498";
        this.ctx.strokeStyle = "#525254"; // "blue"; // "rgba(50, 255, 30, .7)"; // "black"; // "#929498";
        this.ctx.fill();
        this.ctx.lineWidth = 1.0;
        this.ctx.lineCap = "round";
        this.ctx.lineJoin = "round";
        this.ctx.stroke(); 
    }
    
    drawNorthFace(xCoord, yCoord, actorWidth, actorHeight, offsetX, offsetY) { 
        // north trapezium face stroke
        this.ctx.beginPath();
        // move to top1 left
        // r
        this. ctx.moveTo(xCoord, yCoord);
        // horizontal line to bottom1 right
        // ltr
        this.ctx.lineTo(xCoord + actorWidth, yCoord);
        // rtb
        this.ctx.lineTo(xCoord + actorWidth + offsetX, yCoord + offsetY);
        // rtl
        this.ctx.lineTo(xCoord + offsetX, yCoord + offsetY);
        // ltt
        this.ctx.closePath();

        // north trapezium face fill
        this.ctx.fillStyle = "#525254"; // "lime"; // "rgba(50, 255, 30, .7)"; // "#929498";
        this.ctx.strokeStyle = "#525254"; // "lime"; // "rgba(50, 255, 30, .7)"; // "black"; // "#929498";
        this.ctx.fill();
        this.ctx.lineWidth = 1.0;
        this.ctx.lineCap = "round";
        this.ctx.lineJoin = "round";
        this.ctx.stroke(); 
    }

    drawTopFace(xCoord, yCoord, actorWidth, actorHeight, offsetX, offsetY) {
        // front square face stroke
        this.ctx.beginPath();
        // move to top1 left
        this.ctx.moveTo(xCoord + offsetX, yCoord + offsetY);
        // diagonal line to top2 right
        this.ctx.lineTo(xCoord + offsetX + actorWidth, yCoord + offsetY);
        // vertical line to bottom2 right
        this.ctx.lineTo(xCoord + offsetX + actorWidth, yCoord + offsetY + actorHeight);
        // diagonal line to bottom1 left
        this.ctx.lineTo(xCoord + offsetX, yCoord + offsetY + actorHeight);
        // vertical to top1 left
        this.ctx.closePath();

        // front square face fill
        this.ctx.fillStyle = "#E7EAF0"; // "blue";
        this.ctx.strokeStyle =  "#E7EAF0"; // "black";
        this.ctx.fill();
        this.ctx.lineWidth = 1.0;
        this.ctx.lineCap = "round";
        this.ctx.lineJoin = "round";
        this.ctx.stroke();
    }
    
    drawCylinderRearFace(xCoord, yCoord, objectSize, colour) {
        this.ctx.beginPath();
        this.ctx.lineWidth = 1.0;
        this.ctx.strokeStyle = "red";
        this.ctx.fillStyle = colour; // "#3F4042"; // "blue";
        this.ctx.arc(xCoord + this.appConfig.animate.worldOffsetX, yCoord + this.appConfig.animate.worldOffsetY, objectSize, 0, 2 * Math.PI);
        this.ctx.fill();
        this.ctx.stroke();
        this.ctx.closePath();
    }

    drawCylinderFace(xCoord, yCoord, objectWidth, objectLength, objectHeight) {
        // const ptoDistance = this.getDistance(this.localClient, {x: xCoord, y: yCoord});
        var ox = 0;
        var oy = 0;

        var lp = {
            x: this.localClient.x + this.appConfig.animate.worldOffsetX,
            y: this.localClient.y + this.appConfig.animate.worldOffsetY,
            w: this.appConfig.actorSize,
            h: this.appConfig.actorSize
        };
        var o = {
            x: xCoord + this.appConfig.animate.worldOffsetX,
            y: yCoord + this.appConfig.animate.worldOffsetY,
            w: objectWidth,
            h: objectLength,
        };

        if(lp.x - o.x < 0) {
            // console.log('player is left of cylinder');
            ox = -1 * (((lp.x - o.x) / o.w) * (o.w / 10));
        } else if(lp.x - o.x > 0) {
            // console.log('player is right of cylinder');
            ox = -1 * (((lp.x - o.x) / o.w) * (o.w / 10));
        } else {
            ox = 0;
        }
        
        if(lp.y - o.y < 0) {
            // console.log('player is above cylinder');
            oy = -1 * (((lp.y - o.y) / o.h) * (o.h / 10));
        } else if(lp.y - o.y > 0) {
            // console.log('player is below cylinder');
            oy = -1 * (((lp.y - o.y) / o.h) * (o.h / 10));
        } else {
            oy = 0;
        }

        if(Math.abs(ox) > objectHeight) {
            ox = ox >= 0 ? objectHeight : -1 * objectHeight;
        }
        if(Math.abs(oy) > objectHeight) {
            oy = oy >= 0 ? objectHeight : -1 * objectHeight;
        }

        var dy = lp.y - o.y;
        var dx = lp.x - o.x;
        var theta = Math.atan2(dy, dx); // range (-PI, PI]
        // console.log('playerAngle1: ', theta);
        // theta *= 180 / Math.PI; // rads to degs, range (-180, 180]
        // console.log('playerAngle2: ', theta);
    
        if(theta <= 0) {
            this.drawCylinderRearFace(xCoord, yCoord, objectWidth, "#929498");
            this.ctx.beginPath();
            this.ctx.lineWidth = 1.0;
            this.ctx.strokeStyle = "red";
            this.ctx.fillStyle = "#3F4042";
            this.ctx.arc(o.x, o.y, objectWidth, (theta/2) + 0.9, (theta/2) - 1.9);
            this.ctx.fill();
            this.ctx.closePath();
        } else {
            this.drawCylinderRearFace(xCoord, yCoord, objectWidth, "#3F4042");
            this.ctx.beginPath();
            this.ctx.lineWidth = 1.0;
            this.ctx.strokeStyle = "red";
            this.ctx.fillStyle = "#929498";
            this.ctx.arc(o.x, o.y, objectWidth, (theta/2) - 0.9, (theta/2) + 1.9);
            this.ctx.fill();
            this.ctx.closePath();
        }

        this.ctx.beginPath();
        this.ctx.lineWidth = 1.0;
        this.ctx.strokeStyle = "blue";
        this.ctx.fillStyle = "#E7EAF0";
        this.ctx.arc(xCoord + this.appConfig.animate.worldOffsetX + ox, yCoord + this.appConfig.animate.worldOffsetY + oy, objectWidth - 15, 0, 2 * Math.PI);
        this.ctx.fill();
        this.ctx.closePath();

        // this.ctx.beginPath();
        // this.ctx.lineWidth = 1.0;
        // this.ctx.strokeStyle = "cyan";
        // this.ctx.moveTo(lp.x, lp.y);
        // this.ctx.lineTo(o.x, o.y);
        // this.ctx.stroke();
        // this.ctx.closePath();
    }

    degreeToRadians(value) {
        return (value / 360) * 2 * Math.PI;
    };

    calcAngleDegrees(x, y) {
        return (Math.atan2(y, x) * 180) / Math.PI;
    }

    calcAngle(cx, cy, ex, ey) {
        var dy = ey - cy;
        var dx = ex - cx;
        var theta = Math.atan2(dy, dx); // range (-PI, PI]
        theta *= 180 / Math.PI; // rads to degs, range (-180, 180]
        //if (theta < 0) theta = 360 + theta; // range [0, 360)
        return theta;
    }
    
    renderWorld() {
        if(this.ctx) {
            var maxOffset = 20;

            const mapCenterX = this.canvas.width / 2;
            const mapCenterY = this.canvas.height / 2;

            this.appConfig.animate.worldOffsetX = mapCenterX - this.localClient.x;
            this.appConfig.animate.worldOffsetY = mapCenterY - this.localClient.y;

            var playerToObjectDistance = this.localClient.closestBuildingDistance;
            this.localClient.worldObjectCollision = {};

            for (const [key, value] of Object.entries(this.worldMap)) {
                var worldObject = value;

                const worldObjectCenter = {
                    id: worldObject.id,
                    x: worldObject.x + (worldObject.w / 2) + this.appConfig.animate.worldOffsetX,
                    y: worldObject.y + (worldObject.h / 2) + this.appConfig.animate.worldOffsetY,
                    w: worldObject.w,
                    h: worldObject.h,
                };
                
                const worldObjectCoords = {
                    id: worldObject.id,
                    x: worldObject.x + this.appConfig.animate.worldOffsetX,
                    y: worldObject.y + this.appConfig.animate.worldOffsetY,
                    w: worldObject.w,
                    h: worldObject.h,
                    type: worldObject.type,
                };

                playerToObjectDistance = this.getDistance({...this.localClient, x: this.localClient.x + this.appConfig.animate.worldOffsetX, y: this.localClient.y + this.appConfig.animate.worldOffsetY }, worldObjectCenter);
                // const playerAngle = this.calcAngle(worldObjectCenter.x, worldObjectCenter.y, this.localClient.x, this.localClient.y);
             
                var offsetX = 0;
                var offsetY = 0;
                var hDiff = Math.abs(this.localClient.x - worldObjectCenter.x + this.appConfig.animate.worldOffsetX);
                var vDiff = Math.abs(this.localClient.y - worldObjectCenter.y + this.appConfig.animate.worldOffsetY);
                
                // client left of worldObjectCenter
                if(this.localClient.x - worldObjectCenter.x + this.appConfig.animate.worldOffsetX > (-1 * maxOffset)) {
                    offsetX = hDiff <= maxOffset ? -1 * hDiff : -1 * maxOffset;
                }
                
                // client right of worldObjectCenter
                if(this.localClient.x - worldObjectCenter.x + this.appConfig.animate.worldOffsetX <= maxOffset) {
                    offsetX = hDiff <= maxOffset ? hDiff : maxOffset;
                }
                
                // client above worldObjectCenter
                if(this.localClient.y - worldObjectCenter.y + this.appConfig.animate.worldOffsetY > (-1 * maxOffset)) {
                    offsetY = vDiff <= maxOffset ? -1 * vDiff :  -1 * maxOffset; 
                }
                
                // client below worldObjectCenter
                if(this.localClient.y - worldObjectCenter.y + this.appConfig.animate.worldOffsetY <= maxOffset) {
                    offsetY = vDiff <= maxOffset ? vDiff : maxOffset; 
                }

                if(this.localClient.closestBuildingDistance >= playerToObjectDistance) {
                    this.localClient.closestBuildingDistance = playerToObjectDistance;
                    this.localClient.closestBuilding = worldObjectCenter;
                }

                if(worldObject.type === 0) {
                    this.drawCube(worldObject.x + this.appConfig.animate.worldOffsetX, worldObject.y + this.appConfig.animate.worldOffsetY, worldObject.w, worldObject.h, worldObject.z);
                    if(this.isCollide(worldObjectCoords, {...this.localClient, x: this.localClient.x + this.appConfig.animate.worldOffsetX, y: this.localClient.y + this.appConfig.animate.worldOffsetY, w: this.appConfig.actorSize, h: this.appConfig.actorSize})) {
                        this.localClient.worldObjectCollision = {
                            objectInstance: worldObjectCoords,
                            collisionTime: new Date().getTime(),
                        };
                    }
                } else if(worldObject.type === 1) {
                    // this.drawCylinderFace(100, 300, 50, lpToObjX, lpToObjY);
                    // this.drawCylinderFace(objectCoordX + lpToObjX, objectCoordY + lpToObjY, objectLength, objectWidth, objectHeight);
                    // this.drawCylinderFace(100, 300, objectLength, objectWidth, objectHeight);
                    // this.drawCylinderFace(worldObject.x, worldObject.y, worldObject.w, worldObject.h, worldObject.z);
                    // this.drawCylinderFace(worldObject.x + this.appConfig.animate.worldOffsetX, worldObject.y + this.appConfig.animate.worldOffsetY, worldObject.w, worldObject.h, worldObject.z);
                    this.drawCylinderFace(worldObject.x, worldObject.y, worldObject.w, worldObject.h, worldObject.z);
                    // if(this.isCollide(worldObject, this.localClient)) {
                        // if(this.isCollide(worldObject, {...this.localClient, x: this.localClient.x + this.appConfig.animate.worldOffsetX, y: this.localClient.y + this.appConfig.animate.worldOffsetY, w: this.appConfig.actorSize, h: this.appConfig.actorSize})) {
                    if(this.isCollide(worldObjectCoords, {...this.localClient, x: this.localClient.x + this.appConfig.animate.worldOffsetX, y: this.localClient.y + this.appConfig.animate.worldOffsetY, w: this.appConfig.actorSize, h: this.appConfig.actorSize})) {
                        this.localClient.worldObjectCollision = {
                            objectInstance: worldObjectCoords,
                            collisionTime: new Date().getTime(),
                        };
                    }
                }

                // if(this.isCollide(worldObjectCoords, {...this.localClient, x: this.localClient.x + this.appConfig.animate.worldOffsetX, y: this.localClient.y + this.appConfig.animate.worldOffsetY, w: this.appConfig.actorSize, h: this.appConfig.actorSize})) {
                //     this.localClient.worldObjectCollision = {
                //         objectInstance: worldObjectCoords,
                //         collisionTime: new Date().getTime(),
                //     };
                // }

                // update btow objs
                for (const [key, value] of Object.entries(this.appConfig.bullets)) {
                    var bullet = value;

                    const bToWCollsion = this.isCollide(
                        worldObjectCoords,
                        {
                            ...bullet,
                            x: Number(bullet.x) + this.appConfig.animate.worldOffsetX,
                            y: Number(bullet.y) + this.appConfig.animate.worldOffsetY,
                            w: this.appConfig.actorSize,
                            h: this.appConfig.actorSize,
                        });

                    // console.log('1. bullet age: ', new Date().getTime() - bullet.lastUpdate);

                    if(bToWCollsion || (new Date().getTime() - bullet.lastUpdate > this.appConfig.animate.bulletUpdateInterval)) { // TODO: Bullet TTL?
                        // console.log('bToWCollision: ', bullet, ' => ', worldObject);
                        delete this.appConfig.bullets[bullet.id];
                        this.redraw();
                    };
                };

                // this.drawCylinderBottomFace(100, 300, 50, offsetX, offsetY);
                // this.drawCylinderBottomFace(100, 300, 50);
                // this.drawCylinderTopFace(100, 300, 50, offsetX, offsetY);
            }
        }

        // this.window.requestAnimationFrame(this.renderWorld);
    };
    
    renderPlayerStats() {
        // local player's HP
        if(this.ctx) {
            // draw HP bar
            this.ctx.fillStyle = 'white';
            this.ctx.fillRect(this.appConfig.playerStatsStartPosX + 30, this.appConfig.playerStatsStartPosY, this.appConfig.hpBarWidth, this.appConfig.hpBarHeight);
            this.ctx.fillStyle = 'blue';
            this.ctx.fillRect(this.appConfig.playerStatsStartPosX + 31, this.appConfig.playerStatsStartPosY + 1, ((this.localClient.hp/this.appConfig.hpBarWidth) * 100) - 2, this.appConfig.hpBarHeight - 2);
            
            this.ctx.strokeStyle = 'white';
            this.ctx.lineWidth = 1;
            this.ctx.fillStyle = 'magenta';
            
            var hc = this.localClient.hitCount % 10; // TODO: replace 10 with hit multiplier
            var hcTotal = Math.floor(this.localClient.hitCount / 10); // TODO: replace 10 with hit multiplier
            
            var text = 'HP:';
            this.ctx.font = `${this.appConfig.animate.hpFontSize}px Sans-serif`;
            this.ctx.strokeText(text, this.appConfig.playerStatsStartPosX, this.appConfig.playerStatsStartPosY + 10);
            this.ctx.fillText(text, this.appConfig.playerStatsStartPosX, this.appConfig.playerStatsStartPosY + 10);
            
            text = `XP: ${this.localClient.xp}`;
            this.ctx.font = `${this.appConfig.animate.xpFontSize}px Sans-serif`;
            this.ctx.strokeText(text, this.appConfig.playerStatsStartPosX, this.appConfig.playerStatsStartPosY + 25);
            this.ctx.fillText(text, this.appConfig.playerStatsStartPosX, this.appConfig.playerStatsStartPosY + 25);

            text = `Hit Count: ${hcTotal} x${hc}`;
            this.ctx.font = `${this.appConfig.animate.hitCountFontSize}px Sans-serif`;
            this.ctx.strokeText(text, this.appConfig.playerStatsStartPosX, this.appConfig.playerStatsStartPosY + 40);
            this.ctx.fillText(text, this.appConfig.playerStatsStartPosX, this.appConfig.playerStatsStartPosY + 40);

            text = `Kill Count: ${this.localClient.killCount}`;
            this.ctx.font = `${this.appConfig.animate.killCountFontSize}px Sans-serif`;
            this.ctx.strokeText(text, this.appConfig.playerStatsStartPosX, this.appConfig.playerStatsStartPosY + 55);
            this.ctx.fillText(text, this.appConfig.playerStatsStartPosX, this.appConfig.playerStatsStartPosY + 55);

            // this.redraw();
            // this.window.requestAnimationFrame(this.renderPlayerStats);

            if(this.appConfig.animate.hitCountFontSize > 15) {
                this.appConfig.animate.hitCountFontSize -= 1;
            }
            if(this.appConfig.animate.killCountFontSize > 15) {
                this.appConfig.animate.killCountFontSize -= 1;
            }
            if(this.appConfig.animate.hpFontSize > 15) {
                this.appConfig.animate.hpFontSize -= 1;
            }
            if(this.appConfig.animate.xpFontSize > 15) {
                this.appConfig.animate.xpFontSize -= 1;
            }
        }
    };

    updateControls(key, active) {
        if (key) {
            this.appConfig.controls[key].active = active;
        } else {
            console.error(`Error: Invalid key: ${key}`);
        }
    };

    redraw() {
        // paint bg
        this.ctx.fillStyle = 'black';
        this.ctx.fill();
        this.ctx.fillRect(this.appConfig.animate.camX, this.appConfig.animate.camY, this.MAX_WIDTH, this.MAX_HEIGHT);

        this.renderWorldBackground();
        this.renderPlayerStats();
        this.renderWorld();

        if(this.remoteClient && this.remoteClient.id) {
            this.ctx.beginPath();
            this.ctx.arc(this.remoteClient.x + this.appConfig.animate.worldOffsetX, this.remoteClient.y + this.appConfig.animate.worldOffsetY, this.appConfig.actorSize, 0, 2 * Math.PI);
            this.ctx.closePath();

            this.ctx.fillStyle = "red";
            this.ctx.strokeStyle = "white";

            this.ctx.fill();
            this.ctx.fillText(`P${this.remoteClient.id}`, this.remoteClient.x - 70 + this.appConfig.animate.worldOffsetX, this.remoteClient.y - 17 + this.appConfig.animate.worldOffsetY);
            this.ctx.fillStyle = 'white';
            this.ctx.fillRect(this.remoteClient.x - 50 + this.appConfig.animate.worldOffsetX, this.remoteClient.y - 25 + this.appConfig.animate.worldOffsetY, this.appConfig.hpBarWidth, this.appConfig.hpBarHeight);
            this.ctx.fillStyle = 'red';
            this.ctx.fillRect(this.remoteClient.x - 49 + this.appConfig.animate.worldOffsetX, this.remoteClient.y - 24 + this.appConfig.animate.worldOffsetY, ((this.remoteClient.hp/this.appConfig.hpBarWidth) * 100) - 2, this.appConfig.hpBarHeight - 2);

            this.ctx.stroke();
        }

        // Render bullets
        for (const [key, value] of Object.entries(this.appConfig.bullets)) {
            let bullet = value; // as LorenzWSClientBullet

            if(!Number.isNaN(bullet.id)) {
                this.ctx.beginPath();
                this.ctx.arc(Number(bullet.x) + this.appConfig.animate.worldOffsetX, Number(bullet.y) + this.appConfig.animate.worldOffsetY, this.appConfig.actorSize, 0, 2 * Math.PI);
                this.ctx.closePath();

                this.ctx.fillStyle = Number(bullet.source) === this.localClient.id ? "lime" : "red";
                
                this.ctx.fill();
                this.ctx.stroke();
            }

            if(Boolean(bullet.hasImpacted)) {
                this.ctx.fillText(`You've been hit!`, bullet.x - 6, bullet.y - 20);
            }
        }

        this.ctx.beginPath();
        this.ctx.arc(this.localClient.x + this.appConfig.animate.worldOffsetX, this.localClient.y + this.appConfig.animate.worldOffsetY, this.appConfig.actorSize, 0, 2 * Math.PI);
        this.ctx.closePath();

        this.ctx.fillStyle = "blue";
        this.ctx.strokeStyle = "white";

        this.ctx.fill();
        this.ctx.fillText(`P${this.localClient.id}`, this.localClient.x - 70 + this.appConfig.animate.worldOffsetX, this.localClient.y - 17 + this.appConfig.animate.worldOffsetY);
        this.ctx.fillStyle = 'white';
        this.ctx.fillRect(this.localClient.x - 50 + this.appConfig.animate.worldOffsetX, this.localClient.y - 25 + this.appConfig.animate.worldOffsetY, this.appConfig.hpBarWidth, this.appConfig.hpBarHeight);
        this.ctx.fillStyle = 'blue';
        this.ctx.fillRect(this.localClient.x - 49 + this.appConfig.animate.worldOffsetX, this.localClient.y - 24 + this.appConfig.animate.worldOffsetY, ((this.localClient.hp/this.appConfig.hpBarWidth) * 100) - 2, this.appConfig.hpBarHeight - 2);

        // dist line
        // if(this.localClient && this.localClient.closestBuilding) {
        //     this.drawDistanceLine({...this.localClient, x: this.localClient.x + this.appConfig.animate.worldOffsetX, y: this.localClient.y + this.appConfig.animate.worldOffsetY }, this.localClient.closestBuilding);
        // }

        // this.draw();
        // CanvasRenderingContext2D.prototype.drawCylinder(200, 500, 100, 100);
        // this.drawCylinderBottomFace(100, 300, 50);
        // this.drawCylinderBottomFace(100, 300, 50);

        this.ctx.stroke();
    };

    initKeyHandlers(dcmnt) {
        dcmnt.addEventListener(
            "keydown",
            (event) => {
                const keyName = event.key;

                switch(keyName) {
                    case "w":
                    case "W":
                        this.updateControls("wKey", true);
                        break;
                    case "s":
                    case "S":
                        this.updateControls("sKey", true);
                        break;
                    case "a":
                    case "A":
                        this.updateControls("aKey", true);
                        break;
                    case "d":
                    case "D":
                        this.updateControls("dKey", true);
                        break;
                    case " ":
                        this.updateControls("spaceKey", true);
                        break;
                    case "ArrowUp":
                        this.updateControls("up", true);
                        break;
                    case "ArrowDown":
                        this.updateControls("down", true);
                        break;
                    case "ArrowLeft":
                        this.updateControls("left", true);
                        break;
                    case "ArrowRight":
                        this.updateControls("right", true);
                        break;
                };
            },
            false,
        );

        dcmnt.addEventListener(
            "keyup",
            (event) => {
                const keyName = event.key;

                switch(keyName) {
                    case "w":
                    case "W":
                        this.updateControls("wKey", false);
                    break;
                    case "s":
                    case "S":
                        this.updateControls("sKey", false);
                    break;
                    case "a":
                    case "A":
                        this.updateControls("aKey", false);
                    break;
                    case "d":
                    case "D":
                        this.updateControls("dKey", false);
                    break;
                    case "ArrowUp":
                        this.updateControls("up", false);
                    break;
                    case "ArrowDown":
                        this.updateControls("down", false);
                    break;
                    case "ArrowLeft":
                        this.updateControls("left", false);
                    break;
                    case "ArrowRight":
                        this.updateControls("right", false);
                        break;
                    case " ":
                        this.updateControls("spaceKey", false);
                        break;
                };
            },
            false,
        );
    };

    initSocketHandlers(wsServerIp) {
        // Create WebSocket connection.
        console.log('Sending server connection request...');
        this.appConfig.socket = new WebSocket(wsServerIp);
        
        this.appConfig.socket.addEventListener("open", (event) => {
            console.log('Server connection established...');
            this.appConfig.socket.send('ClientInit: *');
        });

        this.appConfig.socket.onmessage = (event) => {
            var serverResponseData;
            var remoteClientRawData;
            var remoteClientData;
            var remoteClientBulletData;
            var remoteClientBulletCoordData;
            var remoteClientBulletImpactData;
            var remoteClientBulletCoord;
            var remoteClientBulletImpact;

            if(this.appConfig.run) {
                try {
                    serverResponseData = new String(event.data).valueOf();

                    if(serverResponseData.startsWith('ACK_CLIENT_INIT:')) {
                        var localClientInitResponse = serverResponseData.split('ACK_CLIENT_INIT:')[1];
                        var localClientPosData = localClientInitResponse.split('|')[0].split(',');
                        this.localClient.id = Number(localClientPosData[0]);
                        this.localClient.x = Number(localClientPosData[1]);
                        this.localClient.y = Number(localClientPosData[2]);
                        
                        var localClientMapData = localClientInitResponse.split('|')[1];
                        var localClientMapObjects = localClientMapData.split(';');

                        for(var localClientMapObjectPos in localClientMapObjects) {
                            var localClientMapObject = localClientMapObjects[localClientMapObjectPos];
                            var localClientMapObjectId = localClientMapObject.split(',')[0];
                            if(localClientMapObjectId.toString().trim().length > 0) {
                                var localClientMapObjectX = localClientMapObject.split(',')[1];
                                var localClientMapObjectY = localClientMapObject.split(',')[2];
                                var localClientMapObjectL = localClientMapObject.split(',')[3];
                                var localClientMapObjectW = localClientMapObject.split(',')[4];
                                var localClientMapObjectH = localClientMapObject.split(',')[5];
                                var localClientMapObjectType = localClientMapObject.split(',')[6];
                                
                                this.worldMap[localClientMapObjectId] = {
                                    id: Number(localClientMapObjectId),
                                    x: Number(localClientMapObjectX),
                                    y: Number(localClientMapObjectY),
                                    w: Number(localClientMapObjectL),
                                    h: Number(localClientMapObjectW),
                                    z: Number(localClientMapObjectH),
                                    type: Number(localClientMapObjectType),
                                }
                            }
                        }
                    }

                    if(serverResponseData.startsWith('ACK_CLIENT_MOVETO:')) {
                        var rcId = 0;
                        var rcX = 0;
                        var rcY = 0;

                        remoteClientRawData = serverResponseData.split('ACK_CLIENT_MOVETO:')[1];
                        
                        remoteClientData = remoteClientRawData.split('#')[0].split(',');

                        rcId = Number(remoteClientData[0]);
                        rcX = Number(remoteClientData[1]);
                        rcY = Number(remoteClientData[2]);
                        
                        // ignore local client movements
                        if(rcId !== this.localClient.id) {

                            this.remoteClient.id = rcId;
                            this.remoteClient.x = Number(rcX.toFixed(2));
                            this.remoteClient.y = Number(rcY.toFixed(2));
                        }

                        this.redraw();
                        // window.requestAnimationFrame(this.redraw);
                    }

                    if(serverResponseData.startsWith('ACK_BULLET_MOVETO:')) {
                        var rcbId = 0;
                        var rcbSid = 0;
                        var rcbDir = '';
                        var rcbX = 0;
                        var rcbY = 0;

                        var rcbiData = [];
                        var rcbiId = 0;
                        var rcbiDir = '';

                        var rcbiSrcStats = [];
                        var rcbiSrcId = 0;
                        var rcbiSrcHp = 0;
                        var rcbiSrcXp = 0;
                        var rcbiSrcHits = 0;
                        var rcbiSrcKills = 0;

                        var rcbiTarStats = [];
                        var rcbiTarId = 0;
                        var rcbiTarHp = 0;
                        var rcbiTarXp = 0;
                        var rcbiTarHits = 0;
                        var rcbiTarKills = 0;

                        remoteClientBulletData = serverResponseData.split('ACK_BULLET_MOVETO:')[1]; // TODO: check if exists
                        console.log('[Client Message] Received ACK_BULLET_MOVETO from server - remoteClientBulletData: ', remoteClientBulletData);

                        remoteClientBulletCoordData = remoteClientBulletData.split('|')[0]; // TODO: check if exists
                        console.log('[Client Message] Received ACK_BULLET_MOVETO from server - remoteClientBulletCoordData: ', remoteClientBulletCoordData);
                        
                        remoteClientBulletImpactData = remoteClientBulletData.split('|')[1]; // TODO: check if exists
                        console.log('[Client Message] Received ACK_BULLET_MOVETO from server - remoteClientBulletImpactData: ', remoteClientBulletImpactData);

                        remoteClientBulletCoord = remoteClientBulletCoordData.split(','); // TODO: check if len > 4
                        rcbId = Number(remoteClientBulletCoord[0]);
                        rcbSid = Number(remoteClientBulletCoord[1]);
                        rcbDir = remoteClientBulletCoord[2];
                        rcbX = Number(Number(remoteClientBulletCoord[3]).toFixed(2));
                        rcbY = Number(Number(remoteClientBulletCoord[4]).toFixed(2));
                        
                        if(rcbId > 0 && rcbDir.length > 0 && !Number.isNaN(rcbX) && !Number.isNaN(rcbY)) {

                            if(!this.appConfig.bullets[rcbId]) {
                                this.appConfig.bullets[rcbId] = {
                                    id: rcbId, 
                                    x: rcbX,
                                    y: rcbY,
                                    direction: rcbDir,
                                    source: rcbSid,
                                    hasImpacted: false,
                                    pending: false,
                                    lastUpdate: new Date().getTime(),
                                } // as LorenzWSClientBullet;
                            } else {
                                this.appConfig.bullets[rcbId].x = rcbX;
                                this.appConfig.bullets[rcbId].y = rcbY;
                                this.appConfig.bullets[rcbId].direction = rcbDir;
                                this.appConfig.bullets[rcbId].source = rcbSid;
                                this.appConfig.bullets[rcbId].hasImpacted = false;
                                this.appConfig.bullets[rcbId].pending = false;
                                this.appConfig.bullets[rcbId].lastUpdate = new Date().getTime();
                            }

                            this.redraw();
                        }

                        if(remoteClientBulletImpactData.includes(',')) {
                            remoteClientBulletImpact = remoteClientBulletImpactData.split(',');
                            console.log('Remote Client Bullet Impact: ', remoteClientBulletImpact);
                            
                            if(remoteClientBulletImpact.length > 0 && remoteClientBulletImpact[0].includes('@')) {
                                rcbiData = remoteClientBulletImpact[0].split('@');
                                console.log('Remote Client Bullet Impact Data: ', rcbiData);

                                if(rcbiData.length > 1) {
                                    rcbiId = rcbId;
                                    rcbiDir = rcbDir;
                                    
                                    // Source Data
                                    rcbiSrcStats = rcbiData[0]?.split('+');
                                    rcbiSrcId = Number(rcbiSrcStats[0]);
                                    rcbiSrcHp = Number(Number(rcbiSrcStats[1]).toFixed(2));
                                    rcbiSrcXp = Number(Number(rcbiSrcStats[2]).toFixed(2));
                                    rcbiSrcHits = Number(rcbiSrcStats[3]);
                                    rcbiSrcKills = Number(rcbiSrcStats[4]);
                                    
                                    // Target Data
                                    rcbiTarStats = rcbiData[1]?.split('+');
                                    rcbiTarId = Number(rcbiTarStats[0]);
                                    rcbiTarHp = Number(Number(rcbiTarStats[1]).toFixed(2));
                                    rcbiTarXp = Number(Number(rcbiTarStats[2]).toFixed(2));
                                    rcbiTarHits = Number(rcbiTarStats[3]);
                                    rcbiTarKills = Number(rcbiTarStats[4]);

                                    if(!this.appConfig.bullets[rcbiId]) {
                                        this.appConfig.bullets[rcbiId] = {
                                            id: Number(rcbiId),
                                            x: rcbX,
                                            y: rcbY,
                                            direction: rcbiDir,
                                            source: rcbiSrcId,
                                            target: rcbiTarId,
                                            hasImpacted: true,
                                            pending: false,
                                            lastUpdate: new Date().getTime(),
                                        } // as LorenzWSClientBullet;
                                    } else {
                                        this.appConfig.bullets[rcbiId].x = rcbX;
                                        this.appConfig.bullets[rcbiId].y = rcbY;
                                        this.appConfig.bullets[rcbiId].direction = rcbiDir;
                                        this.appConfig.bullets[rcbiId].source = rcbiSrcId;
                                        this.appConfig.bullets[rcbiId].target = rcbiTarId;
                                        this.appConfig.bullets[rcbiId].hasImpacted = true;
                                        this.appConfig.bullets[rcbiId].pending = false;
                                        this.appConfig.bullets[rcbiId].lastUpdate = new Date().getTime();
                                    }

                                    // can you get hit by your own projectile?
                                    if(rcbiSrcId === this.localClient.id) {
                                        console.log('Fragged: ', rcbiTarId);
                                        this.localClient.hp = rcbiSrcHp;
                                        this.localClient.xp = rcbiSrcXp;
                                        this.localClient.hitCount = rcbiSrcHits;
                                        this.localClient.killCount = rcbiSrcKills;

                                        this.remoteClient.hp = rcbiTarHp;
                                        this.remoteClient.xp = rcbiTarXp;
                                        this.remoteClient.hitCount = rcbiTarHits;
                                        this.remoteClient.killCount = rcbiTarKills;

                                        if(this.remoteClient.hp > 0) {
                                            this.appConfig.animate.xpFontSize = 20; // ?
                                            this.appConfig.animate.hitCountFontSize = 20; // ?
                                            this.appConfig.animate.killCountFontSize = 20; // ?
                                            this.animateHit(this.remoteClient);
                                        } else {
                                            this.appConfig.animate.killCountFontSize = 20;
                                            this.appConfig.animate.xpFontSize = 20;
                                            this.animateKill(this.remoteClient);
                                        }
                                        this.redraw();
                                        // window.requestAnimationFrame(this.redraw);
                                    }

                                    if(rcbiTarId === this.localClient.id) {
                                        console.log('Got Fragged by: ', rcbiSrcId);
                                        this.localClient.hp = rcbiTarHp;
                                        this.localClient.xp = rcbiTarXp;
                                        this.localClient.hitCount = rcbiTarHits;
                                        this.localClient.killCount = rcbiTarKills;

                                        this.remoteClient.hp = rcbiSrcHp;
                                        this.remoteClient.xp = rcbiSrcXp;
                                        this.remoteClient.hitCount = rcbiSrcHits;
                                        this.remoteClient.killCount = rcbiSrcKills;
                                        
                                        this.appConfig.animate.camShake = 50;

                                        if(this.localClient.hp > 0) {
                                            this.appConfig.animate.hpFontSize = 20;
                                            this.appConfig.animate.xpFontSize = 20; // ?
                                            this.appConfig.animate.hitCountFontSize = 20; // ?
                                            this.appConfig.animate.killCountFontSize = 20; // ?
                                            // TODO: health bar shake animation
                                            this.animateHit(this.localClient);
                                        } else {
                                            this.appConfig.animate.hpFontSize = 20; // ?
                                            // TODO: health bar shake animation
                                            this.animateKill(this.localClient);
                                        }

                                        this.redraw();
                                        // window.requestAnimationFrame(this.redraw);
                                    }

                                    this.redraw();
                                    // window.requestAnimationFrame(this.redraw);
                                }

                                this.redraw();
                                // window.requestAnimationFrame(this.redraw);
                            }

                            this.redraw();
                            // window.requestAnimationFrame(this.redraw);
                        }

                        this.redraw();
                        // window.requestAnimationFrame(this.redraw);
                    }
                    
                    this.redraw();
                    // window.requestAnimationFrame(this.redraw);
                } catch(e) {
                    console.error('Exception: ', e);
                }
            }
        };

        this.appConfig.socket.onclose = (event) => {
            if (event.wasClean) {
                console.log(`[onclose] Connection closed cleanly, code=${event.code} reason=${event.reason}`);
            } else {
                // e.g. server process killed or network down
                console.log('[onclose] Connection died, event data: ', event);
            }
            this.appConfig.run = false;
        };

        this.appConfig.socket.onerror = (error) => {
            console.log('[onerror] ws error: ', error);
            this.appConfig.run = false;
        };
    };

    degreeToRadians(value) {
        return (value / 360) * 2 * Math.PI;
    };

    animateHit(client) {
        setTimeout(() => {
            for(var i = 0; i < 25; i++) {
                this.ctx.fillStyle = "white";
                this.ctx.arc(client.x + this.appConfig.animate.worldOffsetX, client.y + this.appConfig.animate.worldOffsetY, 30, 0, Math.PI * 2, false);  // FX #6
                this.ctx.fill();

                this.ctx.fillStyle = "magenta";
                this.ctx.arc(client.x + this.appConfig.animate.worldOffsetX, client.y + this.appConfig.animate.worldOffsetY, this.getRandomArbitrary(i, i + 5), 0, Math.PI * 2, false);  // FX #6

                this.ctx.fill();
            }

            // requestAnimationFrame(this.animateHit); // ?
        }, this.appConfig.renderRate);
    };
    
    animateKill(client) {
        setTimeout(() => {
            for(var i = 0; i < 25; i++) {
                this.ctx.fillStyle = "white";
                this.ctx.arc(client.x + this.appConfig.animate.worldOffsetX, client.y + this.appConfig.animate.worldOffsetY, 30, 0, Math.PI * 2, false);  // FX #6
                this.ctx.fill();

                this.ctx.fillStyle = "magenta";
                this.ctx.arc(client.x + this.appConfig.animate.worldOffsetX, client.y + this.appConfig.animate.worldOffsetY, this.getRandomArbitrary(0, 30), 0, Math.PI * 2, false);
                
                this.ctx.fill();
            }

            // requestAnimationFrame(this.animateKill); // ?
        }, this.appConfig.renderRate + 100);
    };

    collDir(player, enemy) {
        // Most of this stuff would probably be good to keep stored inside the player
        // along side their x and y position. That way it doesn't have to be recalculated
        // every collision check
        var playerHalfW = player.w/2
        var playerHalfH = player.h/2
        var enemyHalfW = enemy.w/2
        var enemyHalfH = enemy.h/2
        var playerCenterX = player.x + player.w/2
        var playerCenterY = player.y + player.h/2
        var enemyCenterX = enemy.x + enemy.w/2
        var enemyCenterY = enemy.y + enemy.h/2
    
        // Calculate the distance between centers
        var diffX = playerCenterX - enemyCenterX
        var diffY = playerCenterY - enemyCenterY
    
        // Calculate the minimum distance to separate along X and Y
        var minXDist = playerHalfW + enemyHalfW
        var minYDist = playerHalfH + enemyHalfH
    
        // Calculate the depth of collision for both the X and Y axis
        var depthX = diffX > 0 ? minXDist - diffX : -minXDist - diffX
        var depthY = diffY > 0 ? minYDist - diffY : -minYDist - diffY
    
        // Now that you have the depth, you can pick the smaller depth and move
        // along that axis.
        if(depthX != 0 && depthY != 0) {
            if(Math.abs(depthX) < Math.abs(depthY)){ 
                // Collision along the X axis. React accordingly
                if(depthX > 0) {
                    // Left side collision
                    return 'left';
                } else {
                    // Right side collision
                    return 'right';
                }
            } else {
                // Collision along the Y axis.
                if(depthY > 0) {
                    // Top side collision
                    return 'top';
                } else{
                    // Bottom side collision
                    return 'bottom';
                }
            }
        }
    }

    init(wsServerIp, wndw, dcmnt) {
        console.log("Init Script...");

        this.initKeyHandlers(dcmnt);
        this.initSocketHandlers(wsServerIp);

        const collisionOffset = 5;
        var worldObjectCollisions;

        // render world
        wndw.setInterval(async () => {
            var isKeyActive = false;
            var isMoving = false;
            var hasCollided = false;

            if(this.appConfig.run && this.appConfig.socket && this.appConfig.socket.readyState === WebSocket.OPEN) {
                isKeyActive = false;
                var minDist = -1;
                var minDistObj;
                let localX = Number(this.localClient.x);
                let localY = Number(this.localClient.y);

                if(this.appConfig.controls.wKey.active) {
                    isKeyActive = true;
                    this.appConfig.controls.wKey.speed += this.appConfig.controls.speed;
                } else {
                    this.appConfig.controls.wKey.speed = this.appConfig.controls.wKey.speed - this.appConfig.controls.speed < 0 ? 0 : this.appConfig.controls.wKey.speed - this.appConfig.controls.speed;
                }
                this.appConfig.controls.wKey.speed = Number(this.appConfig.controls.wKey.speed.toFixed(2));
                localY = Number((localY - this.appConfig.controls.wKey.speed).toFixed(2));

                if(this.appConfig.controls.sKey.active) {
                    isKeyActive = true;
                    this.appConfig.controls.sKey.speed += this.appConfig.controls.speed;
                } else {
                    this.appConfig.controls.sKey.speed = this.appConfig.controls.sKey.speed - this.appConfig.controls.speed < 0 ? 0 : this.appConfig.controls.sKey.speed - this.appConfig.controls.speed;
                }
                this.appConfig.controls.sKey.speed = Number(this.appConfig.controls.sKey.speed.toFixed(2));
                localY = Number((localY + this.appConfig.controls.sKey.speed).toFixed(2));

                if(this.appConfig.controls.aKey.active) {
                    isKeyActive = true;
                    this.appConfig.controls.aKey.speed += this.appConfig.controls.speed;
                } else {
                    this.appConfig.controls.aKey.speed = this.appConfig.controls.aKey.speed - this.appConfig.controls.speed < 0 ? 0 : this.appConfig.controls.aKey.speed - this.appConfig.controls.speed;
                }
                this.appConfig.controls.aKey.speed = Number(this.appConfig.controls.aKey.speed.toFixed(2));
                localX = Number((localX - this.appConfig.controls.aKey.speed).toFixed(2));
                
                if(this.appConfig.controls.dKey.active) {
                    isKeyActive = true;
                    this.appConfig.controls.dKey.speed += this.appConfig.controls.speed;
                } else {
                    this.appConfig.controls.dKey.speed = this.appConfig.controls.dKey.speed - this.appConfig.controls.speed < 0 ? 0 : this.appConfig.controls.dKey.speed - this.appConfig.controls.speed;
                }
                this.appConfig.controls.dKey.speed = Number(this.appConfig.controls.dKey.speed.toFixed(2));
                localX = Number((localX + this.appConfig.controls.dKey.speed).toFixed(2));
                
                // ****
                if(this.localClient.worldObjectCollision.objectInstance
                    // && (new Date().getTime() - this.localClient.worldObjectCollision.collisionTime <= 30)
                ) {

                    var isCollision =
                    this.localClient.worldObjectCollision.objectInstance.type === 0
                        ? this.isCollide(this.localClient.worldObjectCollision.objectInstance, {...this.localClient, x: this.localClient.x + this.appConfig.animate.worldOffsetX, y: this.localClient.y + this.appConfig.animate.worldOffsetY, w: this.appConfig.actorSize, h: this.appConfig.actorSize})
                        : this.isCollide(this.localClient.worldObjectCollision.objectInstance, {...this.localClient, x: this.localClient.x + this.appConfig.animate.worldOffsetX, y: this.localClient.y + this.appConfig.animate.worldOffsetY, w: this.appConfig.actorSize, h: this.appConfig.actorSize});

                    if(isCollision) {
                        switch(this.collDir({...this.localClient, x: this.localClient.x + this.appConfig.animate.worldOffsetX, y: this.localClient.y + this.appConfig.animate.worldOffsetY, w: this.appConfig.actorSize, h: this.appConfig.actorSize }, this.localClient.worldObjectCollision.objectInstance)) {
                            case 'top':
                                if(localY >= this.localClient.y) {
                                    this.localClient.y = localY;
                                } else {
                                    this.appConfig.controls.sKey.speed = this.appConfig.controls.wKey.speed;
                                    this.appConfig.controls.wKey.speed = 0;
                                }
                                // break;
                            case 'bottom':
                                if(localY <= this.localClient.y) {
                                    this.localClient.y = localY;
                                } else {
                                    this.appConfig.controls.wKey.speed = this.appConfig.controls.sKey.speed;
                                    this.appConfig.controls.sKey.speed = 0;
                                }
                                // break;
                            case 'left':
                                if(localX >= this.localClient.x) {
                                    this.localClient.x = localX;
                                } else {
                                    this.appConfig.controls.dKey.speed = this.appConfig.controls.aKey.speed;
                                    this.appConfig.controls.aKey.speed = 0;
                                }
                                // break;
                            case 'right':
                                if(localX <= this.localClient.x) {
                                    this.localClient.x = localX;
                                } else {
                                    this.appConfig.controls.aKey.speed = this.appConfig.controls.dKey.speed;
                                    this.appConfig.controls.dKey.speed = 0;
                                }
                                // break;
                            default:
                                this.localClient.x = localX;
                                this.localClient.y = localY;
                                // break;
                        }
                    } else {
                        this.localClient.x = localX;
                        this.localClient.y = localY;
                    }
                } else {
                    this.localClient.x = localX;
                    this.localClient.y = localY;
                }
                // ****

                await Object.entries(this.worldMap)
                    .forEach(([worldObjectKey, worldObjectInstance]) => {
                        const wo = {
                            ...worldObjectInstance,
                            x: worldObjectInstance.x + (worldObjectInstance.w / 2),
                            y: worldObjectInstance.y + (worldObjectInstance.h / 2),
                        };

                        this.ctx.beginPath();
                        this.ctx.arc(wo.x, wo.y, this.appConfig.actorSize, 0, 2 * Math.PI);
                        this.ctx.closePath();

                        const ptwDistance = this.getDistance({...this.localClient, x: this.localClient.x + this.appConfig.animate.worldOffsetX, y: this.localClient.y + this.appConfig.animate.worldOffsetY}, wo);

                        if(minDist >= 0) {
                            if(minDist < ptwDistance) {
                                minDist = ptwDistance;
                                minDistObj = wo;
                            }
                        } else {
                            minDist = ptwDistance;
                            minDistObj = wo;
                        }

                        this.localClient.closestBuildingDistance = minDist;
                        this.localClient.closestBuilding = minDistObj;
                    });
                // ****

                isMoving = this.appConfig.controls.wKey.speed || this.appConfig.controls.sKey.speed || this.appConfig.controls.aKey.speed || this.appConfig.controls.dKey.speed;
                
                if((isKeyActive || isMoving) && !hasCollided) {
                    this.appConfig.controls.lastKnownDirection = '';

                    if(this.appConfig.controls.wKey.speed > 0) {
                        this.appConfig.controls.lastKnownDirection += 't';
                    }

                    if(this.appConfig.controls.sKey.speed > 0) {
                        this.appConfig.controls.lastKnownDirection += 'b';
                    }

                    if(this.appConfig.controls.aKey.speed > 0) {
                        this.appConfig.controls.lastKnownDirection += 'l';
                    }

                    if(this.appConfig.controls.dKey.speed > 0) {
                        this.appConfig.controls.lastKnownDirection += 'r';
                    }

                    this.appConfig.socket.send(`ClientMoveTo: ${this.localClient.x},${this.localClient.y};`);

                    this.redraw();
                    // window.requestAnimationFrame(this.redraw);
                }

                let bId = new Date().getTime();

                // add new bullet after 250ms of key active
                if(this.appConfig.controls.spaceKey.active && Math.abs(bId - this.appConfig.controls.spaceKey.lastActive) >= this.appConfig.animate.bulletFireRate) { // 190) { // 250) { // TODO: Compensate for network latency
                    let localClientBullets = Object.entries(this.appConfig.bullets).filter(([key, value]) => value.source === this.localClient.id);

                    // TODO: crashes server if >= 10 bullets for some reason
                    if(localClientBullets.length < 9 && this.appConfig.validBulletDirections.includes(this.appConfig.controls.lastKnownDirection)) { // TODO: Compensate for network latency
                        this.appConfig.bullets[bId] = {
                            id: bId,
                            direction: this.appConfig.controls.lastKnownDirection,
                            x: this.localClient.x, // + this.appConfig.animate.worldOffsetX,
                            y: this.localClient.y, // + this.appConfig.animate.worldOffsetY,
                            source: this.localClient.id,
                            pending: false,
                            hasImpacted: false,
                            lastUpdate: new Date().getTime(),
                        };

                        this.appConfig.controls.spaceKey.lastActive = bId;
                    }
                }

                for (const [key, value] of Object.entries(this.appConfig.bullets)) {
                    let bullet = value; // as LorenzWSClientBullet

                    if(new Date().getTime() - bullet.lastUpdate > this.appConfig.animate.bulletUpdateInterval) {
                        delete this.appConfig.bullets[bullet.id];
                        this.redraw();
                        continue;
                    };

                    bullet.x = Number(bullet.x);
                    bullet.y = Number(bullet.y);

                    if(Boolean(bullet.hasImpacted) || new Date().getTime() - bullet.lastUpdate > this.appConfig.animate.bulletUpdateInterval) {
                        delete this.appConfig.bullets[bullet.id];
                        this.redraw();
                        // window.requestAnimationFrame(this.redraw);
                    } else {
                        if((Number(bullet.id) > 0) && (Number(bullet.source) === this.localClient.id)) {
                            let currentX = bullet.x;
                            let currentY = bullet.y;
                            let nogo = false;

                            switch(bullet.direction) {
                                case 't':
                                    bullet.y = Number(bullet.y) - this.appConfig.controls.bulletSpeed > 0 ? Number((Number(bullet.y) - this.appConfig.controls.bulletSpeed).toFixed(2)) : Number(bullet.y);
                                    break;
                                case 'b':
                                    bullet.y = Number(bullet.y) + this.appConfig.controls.bulletSpeed < this.MAX_HEIGHT ? Number((Number(bullet.y) + this.appConfig.controls.bulletSpeed).toFixed(2)) : Number(bullet.y);
                                    break;
                                case 'l':
                                    bullet.x = bullet.x - this.appConfig.controls.bulletSpeed > 0 ? Number((bullet.x - this.appConfig.controls.bulletSpeed).toFixed(2)) : bullet.x;
                                    break;
                                case 'r':
                                    bullet.x = bullet.x + this.appConfig.controls.bulletSpeed < this.MAX_WIDTH ? Number((bullet.x + this.appConfig.controls.bulletSpeed).toFixed(2)) : bullet.x;
                                    break;
                                case 'tr':
                                    bullet.x = bullet.x + this.appConfig.controls.bulletSpeed < this.MAX_WIDTH ? Number((bullet.x + this.appConfig.controls.bulletSpeed).toFixed(2)) : bullet.x;
                                    bullet.y = bullet.y - this.appConfig.controls.bulletSpeed > 0 ? Number((bullet.y - this.appConfig.controls.bulletSpeed).toFixed(2)) : bullet.y;
                                    break;
                                case 'tl':
                                    bullet.x = bullet.x - this.appConfig.controls.bulletSpeed > 0 ? Number((bullet.x - this.appConfig.controls.bulletSpeed).toFixed(2)) : bullet.x;
                                    bullet.y = bullet.y - this.appConfig.controls.bulletSpeed > 0 ? Number((bullet.y - this.appConfig.controls.bulletSpeed).toFixed(2)) : bullet.y;
                                    break;
                                case 'br':
                                    bullet.x = bullet.x + this.appConfig.controls.bulletSpeed < this.MAX_WIDTH ? Number((bullet.x + this.appConfig.controls.bulletSpeed).toFixed(2)) : bullet.x;
                                    bullet.y = bullet.y + this.appConfig.controls.bulletSpeed < this.MAX_HEIGHT ? Number((bullet.y + this.appConfig.controls.bulletSpeed).toFixed(2)) : bullet.y;
                                    break;
                                case 'bl':
                                    bullet.x = bullet.x - this.appConfig.controls.bulletSpeed > 0 ? Number((bullet.x - this.appConfig.controls.bulletSpeed).toFixed(2)) : bullet.x;
                                    bullet.y = bullet.y + this.appConfig.controls.bulletSpeed < this.MAX_HEIGHT ? Number((bullet.y + this.appConfig.controls.bulletSpeed).toFixed(2)) : bullet.y;
                                    break;
                                default:
                                    nogo = true;
                                    break;
                            }

                            var bulletUpdate = `BulletMoveTo: ${bullet.id},${bullet.direction},${bullet.x},${bullet.y};`;

                            if(!Boolean(nogo)
                                && !Boolean(bullet.pending)
                                && !Boolean(bullet.hasImpacted)
                                && !((Number(currentX) === bullet.x) && (Number(currentY) === bullet.y))
                                && (Number(bullet.source) === this.localClient.id)
                                && (((new Date().getTime()) - Number(bullet.lastUpdate)) >= 30)
                                && !(this.appConfig.controls.lastBulletUpdate.includes(bulletUpdate))
                            ) {
                                bullet.lastUpdate = new Date().getTime();
                                bullet.pending = true;
                                this.appConfig.controls.lastBulletUpdate = bulletUpdate;
                                
                                console.log(`Sending ${bulletUpdate}`);
                                this.appConfig.socket.send(bulletUpdate);
                                this.redraw();
                                // window.requestAnimationFrame(this.redraw);
                            }
                        }
                    }
                }

                this.redraw();
                // window.requestAnimationFrame(this.redraw);
            }
        }, this.appConfig.renderRate);
    };
}

// export default Lorenz;