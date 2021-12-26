import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.2
import QtQuick.Window 2.3
import QtQml.Models 2.2

import backendGuiInterface 1.0

Rectangle {
    id: mainPage
    width: Screen.width
    height: Screen.height
    color: "darkgrey"
    property var cTitleAreaWidth: 40

    property var backendEngine

    function updateCanvasSize(){
        var scaledPeepRadius = simulatorCanvas.peepRadius * backendInterface.UiScale()
        canvasRectangle.width = backendEngine.GetFrameSize().width * backendInterface.UiScale() + scaledPeepRadius
        canvasRectangle.height = backendEngine.GetFrameSize().height * backendInterface.UiScale() + scaledPeepRadius
        simulatorCanvas.scaling = backendInterface.UiScale()
    }

    function setChallengeItems(challengeId)
    {
        if (challengeId == Challenge.Altruism) {
            var setup = backendInterface.GetAltruismSetup()
            simulatorCanvas.createCircleChallengeItem(setup.altruismCenter, setup.altruismColor, setup.altruismRadius)
            simulatorCanvas.createCircleChallengeItem(setup.sacrificeCenter, setup.sacrificeColor, setup.sacrificeRadius)
        } else if (challengeId == Challenge.AltruismSacrifice) {
            var setup = backendInterface.GetAltruismSacrificeSetup()
            simulatorCanvas.createCircleChallengeItem(setup.altruismSacrificeCenter, setup.altruismSacrificeColor, setup.altruismSacrificeRadius)
        } else if (challengeId == Challenge.Circle) {
            var setup = backendInterface.GetCircleSetup()
            simulatorCanvas.createCircleChallengeItem(setup.center, setup.color, setup.radius)
        } else if (challengeId == Challenge.RightHalf) {
            var setup = backendInterface.GetRightHalfSetup()
            simulatorCanvas.createRectChallengeItem(setup.rect, setup.rectColor)
        } else if (challengeId == Challenge.RightQuarter) {
            var setup = backendInterface.GetRightQuarterSetup()
            simulatorCanvas.createRectChallengeItem(setup.rect, setup.rectColor)
        } else if (challengeId == Challenge.LeftEighth) {
            var setup = backendInterface.GetLeftEighthSetup()
            simulatorCanvas.createRectChallengeItem(setup.rect, setup.rectColor)
        } else if (challengeId == Challenge.NeighborCount) {
            var setup = backendInterface.GetNeighborCountSetup()
            simulatorCanvas.createBorders(setup.neighborCountBorders, 10, setup.neighborCountColor)
        } else if (challengeId == Challenge.CenterWeighted) {
            var setup = backendInterface.GetCenterWeightedSetup()
            simulatorCanvas.createCircleChallengeItem(setup.center, setup.color, setup.radius)
        } else if (challengeId == Challenge.CenterUnweighted) {
            var setup = backendInterface.GetCenterUnweightedSetup()
            simulatorCanvas.createCircleChallengeItem(setup.center, setup.color, setup.radius)
        } else if (challengeId == Challenge.CenterSparsed) {
            var setup = backendInterface.GetCenterSparsedSetup()
            simulatorCanvas.createCircleChallengeItem(setup.centerSparsedCenter, setup.centerSparsedColor, setup.centerSparsedRadius)
        } else if (challengeId == Challenge.Corner) {
            var setup = backendInterface.GetCornerSetup()
            for(var i = 0; i < setup.cornerCenters.length; i++){
                simulatorCanvas.createCircleChallengeItem(setup.cornerCenters[i], setup.cornerColor, setup.cornerRadius)
            }
        } else if (challengeId == Challenge.CornerWeighted) {
            var setup = backendInterface.GetCornerSetup()
            for(var i = 0; i < setup.cornerCenters.length; i++){
                simulatorCanvas.createCircleChallengeItem(setup.cornerCenters[i], setup.cornerColor, setup.cornerRadius)
            }
        } else if (challengeId == Challenge.RadioActiveWalls) {
            var setup = backendInterface.GetRadioactiveWallSetup()
            simulatorCanvas.setCanvasGradient(setup.border, setup.radioactiveColor, setup.distance)
        } else if (challengeId == Challenge.TouchAnyWall) {
            var setup = backendInterface.GetTouchAnyWallSetup()
            simulatorCanvas.createBorders(setup.anyWallBorders, 10, setup.anyWallColor)
        } else if (challengeId == Challenge.AgainstAnyWall) {
            var setup = backendInterface.GetAgainstAnyWallSetup()
            simulatorCanvas.createBorders(setup.anyWallBorders, 10, setup.anyWallColor)
        } else if (challengeId == Challenge.EastWestEighths) {
            var setup = backendInterface.GetEastWestEighthsSetup()
            simulatorCanvas.createRectChallengeItem(setup.rectLeft, setup.doubleRectColor)
            simulatorCanvas.createRectChallengeItem(setup.rectRight, setup.doubleRectColor)
        } else if (challengeId == Challenge.Pairs) {
            var setup = backendInterface.GetPairsSetup()
            simulatorCanvas.createBorders(setup.pairsBorders, 10, setup.pairsColor)
        }
    }

    function setBarriers(barrierType)
    {
        if (barrierType == Barrier.ThreeFloatingIslands ||
            barrierType == Barrier.SpotsSpecified) {
            var setup = backendInterface.GetCircleBarriers()
            for(var i = 0; i < setup.circleBarriers.length; i++){
                simulatorCanvas.createCircleBarrierItem(setup.circleBarriers[i], setup.circleBarrierColor, setup.circleBarrierRadius)
            }
        } else if (barrierType == Barrier.VerticalBarConstantLoc ||
                   barrierType == Barrier.VerticalBarRandomLoc ||
                   barrierType == Barrier.FiveBlocksStaggered ||
                   barrierType == Barrier.HorizontalBarConstantLoc) {
            var setup = backendInterface.GetRectBarriers()
            for(var i = 0; i < setup.rectBarriers.length; i++){
                simulatorCanvas.createRectBarrierItem(setup.rectBarriers[i], setup.rectBarrierColor)
            }
        }
    }

    function setUI()
    {
        var names = backendInterface.GetSensorNames()
        for(var i = 0; i < names.length; i++){
            sensorModel.model.append({name:names[i], selected: false})
        }
        sensorsScroll.contentHeight = sensorModel.model.count * sensorModel.itemHeight

        var names = backendInterface.GetActionNames()
        for(var i = 0; i < names.length; i++){
            actionModel.model.append({name:names[i], selected: false})
        }
        actionsScroll.contentHeight = actionModel.model.count * actionModel.itemHeight

        var names = backendInterface.GetChallengeNames()
        for(var i = 0; i < names.length; i++){
            challengeComboBox.model.append({name:names[i]})
        }
    }

    function updateUiData() {
        simulatorCanvas.clear();
        var challenge = backendInterface.GetChallengeId()
        var barrierType = backendInterface.GetBarrierType()
        var imageData = backendInterface.GetImageFrameData()
        simulatorCanvas.createPeeps(imageData.peepsPositions, imageData.peepsColors)
        setChallengeItems(challenge)
        challengeComboBox.currentIndex = challenge
        setBarriers(barrierType)

        generation.text = "Generation " + imageData.generation
        simulationStep.text = "Sim step " + imageData.simStep
        maxPopulation.text = "Max population " + imageData.maxPopulation
        
    }

    QMLInterface{
        id: backendInterface

        Component.onCompleted: {
            mainPage.backendEngine = StartBackend()
            mainPage.backendEngine.ParametersUpdated.connect(mainPage.updateCanvasSize)
            simulatorCanvas.getUiData.connect(mainPage.updateUiData)
        }
    }

    Text {
        id: titleText
        text: "Game of Evolution"
        y: 30
        anchors.horizontalCenter: mainPage.horizontalCenter
        font.pointSize: 24; font.bold: true
    }

    Rectangle {
        id: simulatorWindow
        y: titleText.y + titleText.font.pointSize + cTitleAreaWidth
        width: mainPage.width; height: mainPage.height - y
        color: "lightgray"

        Frame {
            id: canvasRectangle
            anchors.left: parent.left

            Canvas2DType{
                id: simulatorCanvas
            }
        }

        Frame {
            anchors.top: canvasRectangle.bottom
            Row {
                padding: 5.0
                spacing: 20
                Button {
                    id: resetButton
                    text: "Reset"
                    background: Rectangle {
                        color: "darkgrey"
                    }

                    onClicked: {
                        backendInterface.ResetSim()
                    }
                }

                Button {
                    id: startButton
                    text: "Start"
                    background: Rectangle {
                        color: "darkgrey"
                    }

                    onClicked: {
                        backendInterface.StartSim()
                    }
                }

                Button {
                    id: stopButton
                    text: "Stop"
                    background: Rectangle {
                        color: "darkgrey"
                    }

                    onClicked: {
                        backendInterface.StopSim()
                    }
                }
            }
        }

        Frame {
            id: detailsRectangle
            anchors.left: canvasRectangle.right
            anchors.top: simulatorWindow.top
            anchors.leftMargin: 50
            anchors.topMargin: 20
            Column {
                spacing: 5
                Text {
                    id: details
                    text: "Details:"
                    font.bold: true
                    font.pointSize: 16
                }
                Text {
                    id: generation
                    text: "Generation"
                    font.pointSize: 12
                }
                Text {
                    id: simulationStep
                    text: "Sim step "
                    font.pointSize: 12
                }
                Text {
                    id: maxPopulation
                    text: "Max population "
                    font.pointSize: 12
                }
            }
        }

        Frame {
            id: settingsFrame
            anchors.left: detailsRectangle.right
            anchors.top: simulatorWindow.top
            anchors.leftMargin: 50
            anchors.topMargin: 20
            Column {
                spacing: 5
                Text {
                    id: settings
                    text: "Settings:"
                    font.bold: true
                    font.pointSize: 16
                }

                Text {
                    id: sensors
                    text: "Sensors"
                    font.italic: true
                    font.pointSize: 14
                }
                
                Rectangle {
                    width: 400; height: 200
                    
                    ScrollView {
                        id: sensorsScroll
                        anchors.fill: parent
                        ScrollBar.vertical.policy: ScrollBar.AlwaysOn
                        ScrollBar.vertical.interactive: true
                        clip: true
                        
                        DelegateModel {
                            property var itemHeight: 20
                            property var itemsSelectedArray: []
                            id: sensorModel
                            model: ListModel {}

                            groups: [
                                DelegateModelGroup { name: "selected" }
                            ]

                            delegate: Rectangle {
                                id: item
                                height: sensorModel.itemHeight
                                width: parent.width
                                radius: 5
                                border.color: {
                                    if (selected)
                                        return "black"
                                    return "white"
                                }
                                color: {
                                  if (selected)
                                      return "lightsteelblue"
                                  return "white"
                                }
                                Text {
                                    font.pointSize: 12
                                    font.bold: true
                                    text: {
                                        return name;
                                    }
                                }
                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: {
                                        selectAllCheckBox.checked = false
                                        sensorModel.model.get(index).selected = !sensorModel.model.get(index).selected
                                    }
                                }
                            }
                        }

                        ListView {
                            anchors.fill: parent
                            model: sensorModel
                        }
                    }
                }
                Text {
                    id: actions
                    text: "Actions"
                    font.italic: true
                    font.pointSize: 14
                }
                
                Rectangle {
                    width: 400; height: 200
                    
                    ScrollView {
                        id: actionsScroll
                        anchors.fill: parent
                        ScrollBar.vertical.policy: ScrollBar.AlwaysOn
                        ScrollBar.vertical.interactive: true
                        clip: true
                        
                        DelegateModel {
                            property var itemHeight: 20
                            id: actionModel
                            model: ListModel { id: actionModelList }

                            groups: [
                                DelegateModelGroup { name: "selected" }
                            ]

                            delegate: Rectangle {
                                id: item
                                height: actionModel.itemHeight
                                width: parent.width
                                radius: 5
                                border.color: {
                                    if (selected)
                                        return "black"
                                    return "white"
                                }
                                color: {
                                  if (selected)
                                      return "lightsteelblue"
                                  return "white"
                                }
                                Text {
                                    font.pointSize: 12
                                    font.bold: true
                                    text: {
                                        return name;
                                    }
                                }
                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: {
                                        selectAllCheckBox.checked = false
                                        actionModel.model.get(index).selected = !actionModel.model.get(index).selected
                                    }
                                }
                            }
                        }

                        ListView {
                            anchors.fill: parent
                            model: actionModel
                        }
                    }
                }
                Row {
                    spacing: 80
                    padding: 10.0
                    Button {
                        id: updateButton
                        text: "Update"
                        background: Rectangle {
                            color: "darkgrey"
                        }

                        onClicked: {
                            var sensorSelectedArray = []
                            for(var i = 0; i < sensorModel.count; i++){
                                sensorSelectedArray.push(sensorModel.model.get(i).selected)
                            }
                            var actionSelectedArray = []
                            for(var i = 0; i < actionModel.count; i++){
                                actionSelectedArray.push(actionModel.model.get(i).selected)
                            }
                            backendInterface.UpdateSensorsActions(sensorSelectedArray, actionSelectedArray)
                        }
                    }
                    Row {
                      spacing: 10

                      Text {
                          text: "Select All"
                          font.pointSize: 12
                      }

                      CheckBox {
                          id: selectAllCheckBox
                          onClicked: {
                              for(var i = 0; i < sensorModel.count; i++){
                                  sensorModel.model.get(i).selected = selectAllCheckBox.checked
                              }

                              for(var i = 0; i < actionModel.count; i++){
                                  actionModel.model.get(i).selected = selectAllCheckBox.checked
                              }
                          }
                      }
                    }
                }
            }
        }
        Frame {
            id: challenges
            anchors.left: settingsFrame.right
            anchors.top: simulatorWindow.top
            anchors.leftMargin: 50
            anchors.topMargin: 20
            Column {
                Text {
                    text: "Challenges:"
                    font.pointSize: 16
                    font.bold: true
                }

                ComboBox {
                  id: challengeComboBox
                  model: ListModel {}
                  onActivated: {
                      backendInterface.SetChallengeId(challengeComboBox.currentIndex)
                      setChallengeItems(challengeComboBox.currentIndex)
                  }
                }
            }
        }
    }

    Button {
        id: quitButton
        text: "Quit"

        anchors.right: mainPage.right
        anchors.top: mainPage.top

        onClicked: {
            backendInterface.Quit()
            Qt.quit()
        }
    }

    Component.onCompleted: {
        mainPage.setUI()
    }
}