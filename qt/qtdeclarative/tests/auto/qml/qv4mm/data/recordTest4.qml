import QtQml

QtObject {
    id: derivedRoot
    property int extraProperty: 999
    property string derivedName: "derived"

    property RecordTestBase child: RecordTestBase {}
}
