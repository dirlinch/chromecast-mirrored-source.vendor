import Qt 4.7

Rectangle {
    width: 200
    height: 200
    Text {
        Behavior on anchors.verticalCenterOffset { NumberAnimation { duration: 300; } }
        text: "Hello World"
    }
}
