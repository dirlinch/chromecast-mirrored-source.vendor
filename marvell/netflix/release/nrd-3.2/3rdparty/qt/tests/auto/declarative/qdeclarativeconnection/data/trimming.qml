import Qt 4.7

Item {
    id: screen; width: 50

    property string tested
    signal testMe(int param1, string param2)

    Connections { target: screen; onTestMe: screen.tested = param2 + param1 }
}
