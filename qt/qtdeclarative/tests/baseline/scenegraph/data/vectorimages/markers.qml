import QtQuick
import QtQuick.VectorImage

Rectangle {
    id: topLevelItem
    width: 600
    height: 300

    ListModel {
        id: files
        ListElement { src: "../shared/svg/extended_features/marker.svg" }
        ListElement { src: "../shared/svg/extended_features/marker2.svg" }
    }

    Grid {
        columns: 4
        anchors.fill: parent
        spacing: 20
        Repeater {
            model: files
            VectorImage {
                source: src
                preferredRendererType: VectorImage.CurveRenderer
                width: 300
                fillMode: VectorImage.PreserveAspectFit
            }
        }
    }
}
