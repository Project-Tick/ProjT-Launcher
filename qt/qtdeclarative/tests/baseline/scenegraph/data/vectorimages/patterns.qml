import QtQuick
import QtQuick.VectorImage

Rectangle {
    id: topLevelItem
    width: 900
    height: 600

    ListModel {
        id: files
        ListElement { src: "../shared/svg/extended_features/pattern_objectboundingbox.svg" }
        ListElement { src: "../shared/svg/extended_features/pattern_userspace.svg" }
        ListElement { src: "../shared/svg/extended_features/pattern_xy.svg" }
        ListElement { src: "../shared/svg/extended_features/pattern_xy2.svg" }
        ListElement { src: "../shared/svg/extended_features/pattern_transform.svg" }
        ListElement { src: "../shared/svg/extended_features/patternWithImplicitSize.svg" }
    }

    Grid {
        columns: 3
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
