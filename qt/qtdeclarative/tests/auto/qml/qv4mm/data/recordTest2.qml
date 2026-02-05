import QtQml

QtObject {
    id: root
    property int differentValue: 100

    property QtObject child: QtObject {
        property string childName: "child"
    }
}
