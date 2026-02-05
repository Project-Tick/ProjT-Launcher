import QtQuick

Item {
    id: self
    property Item a: Item { id: a } // qmllint disable id-shadows-member
    Component.onCompleted: self.data = [ a ]
}
