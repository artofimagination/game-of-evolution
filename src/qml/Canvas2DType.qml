import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.2

Item {
    id: mainview
    width: parent.width
    height: parent.height
    visible: true
    property var shapes: []
    property var peepsPositions: []
    property var peepsColors: []
    //! Holds the simulator to UI scaling. Responsible to enlarge the simulation values.
    property var scaling: 1

    signal updateMousePos(var mousePos)
    signal getUiData()

    Peep {
        id: peep
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
                interval: 1 / 40 * 1000 // 20 Hz
                repeat: true
                running: true
                onTriggered: {
                    for(var i = 0; i < mainview.shapes.length; i++){
                        mainview.shapes[i].destroy()
                    }
                    mainview.shapes = []
                    for(var i = 0; i < peepsPositions.length; i++){
                        var position = peepsPositions[i]
                        position.x = position.x * mainview.scaling
                        position.y = position.y * mainview.scaling
                        mainview.shapes.push(peep.createObject(mainview, {"position": position, "fillColor": peepsColors[i], "radius": 1}))
                        mainview.shapes[mainview.shapes.length - 1].radius *= mainview.scaling / 2 
                    }
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
            var gradient = context.createLinearGradient(100,0,50,200)
            gradient.addColorStop(0.01, "white")
            gradient.addColorStop(0.95, "white")
            context.fillStyle = gradient
            context.fillRect(0, 0, width, height)
            context.fill();
            for(var i = 0; i < mainview.shapes.length; i++){
                shapes[i].draw(context)
            }
        }
    }
}
