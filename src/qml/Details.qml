import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQml.Models 2.2

//! Contains simulation details and config options.
Item {
    property var generationText: ""     ///< Generation count text to display
    property var simStepText: ""        ///< Simulation step count text to display
    property var maxPopulationText: ""  ///< Max population count text to display
    property var challengeIndex: 0      ///< Current challenge index

    //! Singals the new challenge selection.
    signal selectChallenge(var id)

    //! Sets the ui details. For example selectable sensors, actions, challenges.
    function setData()
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

    //! Details info frame
    Frame {
        anchors.left: parent.right
        anchors.top: parent.top
        anchors.leftMargin: 50
        anchors.topMargin: 20
        id: detailsFrame
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
                text: generationText
                font.pointSize: 12
            }
            Text {
                id: simulationStep
                text: simStepText
                font.pointSize: 12
            }
            Text {
                id: maxPopulation
                text: maxPopulationText
                font.pointSize: 12
            }
        }
    }

    //! Sensors/actions settings frame
    //! Settings is in effect only if Update is clicked
    Frame {
        id: settingsFrame
        anchors.left: detailsFrame.right
        anchors.top: parent.top
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
            
            //! Sensors multi select list
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
                                    // Change selection state of clicked item.
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
            
            //! Actions multi select list
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
                        model: ListModel {}

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
                                    // Change selection state of clicked item.
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

            //! Update button. Settings selections take effect only after "Update" pressed
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
    
    //! Challenges selector.
    Frame {
        id: challenges
        anchors.left: settingsFrame.right
        anchors.top: parent.top
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
                currentIndex: challengeIndex
                onActivated: {
                    backendInterface.SetChallengeId(challengeComboBox.currentIndex)
                    selectChallenge(challengeComboBox.currentIndex)
                }
            }
        }
    }
}
