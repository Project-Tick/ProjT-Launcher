import QtQuick
import QtQuick.Shapes

Rectangle {
    width: 320
    height: 240
    color: "lightgray"

    ListModel {
        id: renderers
        ListElement { renderer: Shape.GeometryRenderer }
        ListElement { renderer: Shape.CurveRenderer }
    }

    LinearGradient {
        id: grad1
        x2: 60; y2: 60
        spread: ShapeGradient.RepeatSpread
        GradientStop { position: 0; color: "black" }
        GradientStop { position: 1; color: "red" }
    }

    RadialGradient {
        id: grad2
        centerX: 30; centerY: 30
        focalX: centerX; focalY: centerY
        centerRadius: 30
        spread: ShapeGradient.RepeatSpread
        GradientStop { position: 0; color: "yellow" }
        GradientStop { position: .5; color: "black" }
        GradientStop { position: 1; color: "yellow" }
    }

    Row {
        padding: 10
        spacing: 20
        Repeater {
            model: renderers
            Shape {
                width: 140
                preferredRendererType: renderer

                ShapePath {
                    id: c1
                    strokeWidth: 10
                    strokeColor: "cyan"
                    fillColor: "black"

                    PathRectangle {
                        id: rc1
                        width: 60; height: 60
                    }
                }

                ShapePath {
                    id: c2
                    strokeWidth: 10
                    strokeColor: "cyan"
                    fillColor: "black"

                    PathRectangle {
                        id: rc2
                        x: 80
                        width: 60; height: 60
                    }
                }

                ShapePath {
                    id: g1
                    strokeWidth: 10
                    strokeColor: "cyan"
                    fillColor: "black"
                    strokeGradient: grad1

                    PathRectangle {
                        id: rg1
                        y: 80
                        width: 60; height: 60
                    }
                }

                ShapePath {
                    id: g2
                    strokeWidth: 10
                    strokeColor: "cyan"
                    fillColor: "black"
                    strokeGradient: grad1

                    PathRectangle {
                        id: rg2
                        x: 80
                        y: 80
                        width: 60; height: 60
                    }
                }

                ShapePath {
                    id: g3
                    strokeWidth: 10
                    strokeColor: "cyan"
                    fillColor: "black"
                    strokeGradient: grad1

                    PathRectangle {
                        id: rg3                    
                        y: 2 * 80
                        width: 60; height: 60
                    }
                }

                ShapePath {
                    id: g4
                    strokeWidth: 10
                    strokeColor: "cyan"
                    fillColor: "black"
                    strokeGradient: grad1

                    PathRectangle {
                        id: rg4
                        x: 80
                        y: 2 * 80
                        width: 60; height: 60
                    }
                }

                Timer {
                    running: true
                    interval: 150 // <200ms needed for scenegrabber; disable for manual testing
                    onTriggered: {
                        c1.strokeGradient = grad1
                        g1.strokeGradient = null
                        g2.strokeGradient = null
                        g3.strokeGradient = grad2
                    }
                }
            }
        }
    }
}

