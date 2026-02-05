import QtQuick

Item {
    id: root
    property int i: 1
    Item {
        property bool b: i === 1
    }

    function f() {
        1, 1
        if (i = 2) {
        }
    }
}
