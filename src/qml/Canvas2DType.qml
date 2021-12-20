import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.2

Item {
    id: mainview
    width: parent.width
    height: parent.height
    visible: true
    property var challengeShapes: []
    property var barrierShapes: []
    property var shapes: []
    //! Holds the simulator to UI scaling. Responsible to enlarge the simulation values.
    property var scaling: 1
    property var peepsOffset: 1
    property var peepRadius: 1
    property var gradientStart: Qt.point(0, 0) 
    property var gradientEnd: Qt.point(0, 0) 
    property var gradientColor: "white"


    signal updateMousePos(var mousePos)
    signal getUiData()

    function clear() {
        for(var i = 0; i < shapes.length; i++){
            shapes[i].destroy()
        }
        for(var i = 0; i < challengeShapes.length; i++){
            challengeShapes[i].destroy()
        }
        for(var i = 0; i < barrierShapes.length; i++){
            barrierShapes[i].destroy()
        }
        shapes = []
        challengeShapes = []
        barrierShapes = []
    }

    function createPeeps(peepsPositions, peepsColors) {
        var radius = peepRadius * scaling / 2
        peepsOffset = radius
        for(var i = 0; i < peepsPositions.length; i++){
            var position = peepsPositions[i]
            // Left and top side the peeps are cut in half, the radius offset fixing this.
            position.x = position.x * scaling + radius
            position.y = position.y * scaling + radius
            shapes.push(peep.createObject(mainview, {"position": position, "fillColor": peepsColors[i], "radius": radius}))
        }
    }

    function createRectChallengeItem(rectangle, color) {
        rectangle.x = rectangle.x * scaling
        rectangle.y = rectangle.y * scaling
        rectangle.width = rectangle.width * scaling
        rectangle.height = rectangle.height * scaling
        challengeShapes.push(rect.createObject(mainview, {"rect": rectangle, "fillColor": color}))
    }

    function createRectBarrierItem(rectangle, color) {
        rectangle.x = rectangle.x * scaling
        rectangle.y = rectangle.y * scaling
        rectangle.width = rectangle.width * scaling
        rectangle.height = rectangle.height * scaling
        barrierShapes.push(rect.createObject(mainview, {"rect": rectangle, "fillColor": color}))
    }

    function createBorders(borders, borderLength, color) {
        for(var i = 0; i < borders.length; i++){
            var rectangle = Qt.rect
            if (borders[i] == 0){
                challengeShapes.push(rect.createObject(mainview, {"rect": Qt.rect(0, 0, borderLength, height), "fillColor": color}))
            } else if (borders[i] == 1) {
                challengeShapes.push(rect.createObject(mainview, {"rect": Qt.rect(0, 0, width, borderLength), "fillColor": color}))
            } else if (borders[i] == 2) {
                challengeShapes.push(rect.createObject(mainview, {"rect": Qt.rect(width - borderLength, 0, borderLength, height), "fillColor": color}))
            } else if (borders[i] == 3) {
                challengeShapes.push(rect.createObject(mainview, {"rect": Qt.rect(0, height - borderLength, width, borderLength), "fillColor": color}))
            }
        }
    }
    
    function createCircleChallengeItem(center, color, radius) {
        center.x = center.x * scaling + peepsOffset
        center.y = center.y * scaling + peepsOffset
        radius = radius * scaling
        challengeShapes.push(circle.createObject(mainview, {"position": center, "fillColor": color, "radius": radius}))
    }

    function createCircleBarrierItem(center, color, radius) {
        center.x = center.x * scaling + peepsOffset
        center.y = center.y * scaling + peepsOffset
        radius = radius * scaling
        barrierShapes.push(circle.createObject(mainview, {"position": center, "fillColor": color, "radius": radius}))
    }

    function setCanvasGradient(border, color, distance) {
        if (border == 0) {
            gradientStart.x = 0
            gradientStart.y = height / 2
            gradientEnd.x = distance * scaling
            gradientEnd.y = height / 2
        } else if (border == 1) {
            gradientStart.x = width / 2
            gradientStart.y = 0
            gradientEnd.x = width / 2
            gradientEnd.y = distance * scaling
        } else if (border == 2) {
            gradientStart.x = width
            gradientStart.y = height / 2
            gradientEnd.x = width - distance * scaling
            gradientEnd.y = height / 2
        } else if (border == 3) {
            gradientStart.x = width / 2
            gradientStart.y = height
            gradientEnd.x = width / 2
            gradientEnd.y = height - distance * scaling
        }
        gradientColor = color
    }

    Peep {
        id: peep
    }

    Circle {
        id: circle
    }

    
    Rect {
        id: rect
    }

    Canvas {
        id: canvas
        anchors.fill: parent
        focus: true

        MouseArea {
            id: mouseArea

            anchors.fill: parent
            property var mouseData

            function updateMouse()
            {
                updateMousePos(Qt.point(mouseData.x - width / 2, mouseData.y - height / 2))
            }

            Timer {
                id: pressAndHoldTimer
                interval: 1 / 20 * 1000 // 20 Hz
                repeat: true
                running: false
                onTriggered: {
                    mouseArea.updateMouse()
                }
            }

            Timer {
                id: repaintTimer
                interval: 1 / 30 * 1000 // 30 Hz
                repeat: true
                running: true
                onTriggered: {
                    canvas.requestPaint()
                    getUiData()
                }
            }

            onPressAndHold: {
                if ((mouse.button == Qt.LeftButton)){
                    pressAndHoldTimer.running = true
                    mouseData = mouse
                }
            }

            onReleased:
            {
                if ((mouse.button == Qt.LeftButton)){
                    pressAndHoldTimer.running = false
                }
            }
        }

        onPaint: {
            var context = getContext("2d");
            context.beginPath();
            var gradient = context.createLinearGradient(gradientStart.x, gradientStart.y, gradientEnd.x, gradientEnd.y)
            gradient.addColorStop(0.01, gradientColor)
            gradient.addColorStop(0.95, "white")
            context.fillStyle = gradient
            context.fillRect(0, 0, width, height)
            context.fill();
            for(var i = 0; i < mainview.shapes.length; i++){
                shapes[i].draw(context)
            }

            for(var i = 0; i < mainview.challengeShapes.length; i++){
                challengeShapes[i].draw(context)
            }

            for(var i = 0; i < mainview.barrierShapes.length; i++){
                console.log(barrierShapes[i].position, barrierShapes[i].radius)
                barrierShapes[i].draw(context)
            }
        }
    }
}
