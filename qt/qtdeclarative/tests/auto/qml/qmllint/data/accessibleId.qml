import QtQml

QtObject {
    id: a // qmllint disable id-shadows-member
    property Component c: Component {
        QtObject {
            id: a // qmllint disable id-shadows-member
            property QtObject o: QtObject {
                property int a: 5
                objectName: a.objectName
            }
        }
    }
}
