import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.2


//! 2D Canvas to display the world of Peeps.
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

    //! Signals the current mouse press event.
    signal updateMousePos(var mousePos)
    //! Signals the new data update request.
    signal requestUiData()

    //! Clears the canvas of all data.
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

    //! Creates all the peeps requested.
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

    //! Creates rectangular challenge element.
    function createRectChallengeItem(rectangle, color) {
        rectangle.x = rectangle.x * scaling
        rectangle.y = rectangle.y * scaling
        rectangle.width = rectangle.width * scaling
        rectangle.height = rectangle.height * scaling
        challengeShapes.push(rect.createObject(mainview, {"rect": rectangle, "fillColor": color}))
    }

    //! Create border challenge elements.
    function createBorderChallengeItems(borders, borderLength, color) {
        for(var i = 0; i < borders.length; i++){
            var rectangle = Qt.rect
            // Left border
            if (borders[i] == 0){
                challengeShapes.push(rect.createObject(mainview, {"rect": Qt.rect(0, 0, borderLength, height), "fillColor": color}))
            // Top border
            } else if (borders[i] == 1) {
                challengeShapes.push(rect.createObject(mainview, {"rect": Qt.rect(0, 0, width, borderLength), "fillColor": color}))
            // Right border
            } else if (borders[i] == 2) {
                challengeShapes.push(rect.createObject(mainview, {"rect": Qt.rect(width - borderLength, 0, borderLength, height), "fillColor": color}))
            // Bottom border
            } else if (borders[i] == 3) {
                challengeShapes.push(rect.createObject(mainview, {"rect": Qt.rect(0, height - borderLength, width, borderLength), "fillColor": color}))
            }
        }
    }
    
    //! Creates a circular challaenge element.
    function createCircleChallengeItem(center, color, radius) {
        center.x = center.x * scaling + peepsOffset
        center.y = center.y * scaling + peepsOffset
        radius = radius * scaling
        challengeShapes.push(circle.createObject(mainview, {"position": center, "fillColor": color, "radius": radius}))
    }

    //! Creates rectangular barrier element.
    function createRectBarrierItem(rectangle, color) {
        rectangle.x = rectangle.x * scaling
        rectangle.y = rectangle.y * scaling
        rectangle.width = rectangle.width * scaling
        rectangle.height = rectangle.height * scaling
        barrierShapes.push(rect.createObject(mainview, {"rect": rectangle, "fillColor": color}))
    }

    //! Creates a circle barrier element.
    function createCircleBarrierItem(center, color, radius) {
        center.x = center.x * scaling + peepsOffset
        center.y = center.y * scaling + peepsOffset
        radius = radius * scaling
        barrierShapes.push(circle.createObject(mainview, {"position": center, "fillColor": color, "radius": radius}))
    }

    //! Sets canvas gradient challenge item.
    function setCanvasGradient(border, color, distance) {
        switch (border)
        {
            // Left gradient
            case 0:
                gradientStart.x = 0
                gradientStart.y = height / 2
                gradientEnd.x = distance * scaling
                gradientEnd.y = height / 2
                break
            // Top gradient
            case 1:
                gradientStart.x = width / 2
                gradientStart.y = 0
                gradientEnd.x = width / 2
                gradientEnd.y = distance * scaling
                break
            // Right gradient
            case 2:
                gradientStart.x = width
                gradientStart.y = height / 2
                gradientEnd.x = width - distance * scaling
                gradientEnd.y = height / 2
                break;
            // Bottom gradient
            case 3:
                gradientStart.x = width / 2
                gradientStart.y = height
                gradientEnd.x = width / 2
                gradientEnd.y = height - distance * scaling
                break
            default:
                break
        }
        gradientColor = color
    }

    //! Peep class to instantiate objects from.
    Peep {
        id: peep
    }

    //! Peep class to instantiate objects from.
    Circle {
        id: circle
    }

    //! Rect class to instantiate obejects from.
    Rect {
        id: rect
    }

    //! Canvas to paint elements and handle user interaction.
    Canvas {
        id: canvas
        anchors.fill: parent
        focus: true

        MouseArea {
            id: mouseArea

            anchors.fill: parent
            property var mouseData

            //! Updates and signals new mouse press position.
            function updateMouse()
            {
                updateMousePos(Qt.point(mouseData.x - width / 2, mouseData.y - height / 2))
            }

            //! Timer to check mouse pressing
            Timer {
                id: pressAndHoldTimer
                interval: 1 / 20 * 1000 // 20 Hz
                repeat: true
                running: false
                onTriggered: {
                    mouseArea.updateMouse()
                }
            }

            //! Canvas repaint timer.
            Timer {
                id: repaintTimer
                interval: 1 / 30 * 1000 // 30 Hz
                repeat: true
                running: true
                onTriggered: {
                    canvas.requestPaint()
                    requestUiData()
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

        //! Paints all created elements.
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
                barrierShapes[i].draw(context)
            }
        }
    }
}
