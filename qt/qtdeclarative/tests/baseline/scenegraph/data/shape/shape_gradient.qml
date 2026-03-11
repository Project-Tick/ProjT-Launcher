import QtQuick
import QtQuick.Shapes

Item {
    width: 1280
    height: 480

    ListModel {
        id: renderers
        ListElement { renderer: Shape.GeometryRenderer; cosmetic: false; reverseStroke: true }
        ListElement { renderer: Shape.CurveRenderer; cosmetic: false; reverseStroke: true }
        ListElement { renderer: Shape.GeometryRenderer; cosmetic: true; reverseStroke: true }
        ListElement { renderer: Shape.CurveRenderer; cosmetic: true; reverseStroke: true }
        ListElement { renderer: Shape.GeometryRenderer; cosmetic: false; reverseStroke: false }
        ListElement { renderer: Shape.CurveRenderer; cosmetic: false; reverseStroke: false }
        ListElement { renderer: Shape.GeometryRenderer; cosmetic: true; reverseStroke: false }
        ListElement { renderer: Shape.CurveRenderer; cosmetic: true; reverseStroke: false }
    }

    Row {
        Repeater {
            model: renderers
            Column {
                Shape {
                    preferredRendererType: renderer
                    width: 160
                    height: 150

                    ShapePath {
                        cosmeticStroke: cosmetic
                        strokeWidth: 10
                        fillGradient: LinearGradient {
                            y1: 50; y2: 80
                            GradientStop { position: 0; color: "red" }
                            GradientStop { position: 1; color: "cyan" }
                        }
                        strokeGradient: LinearGradient {
                            y1: 50; y2: 80
                            GradientStop { position: reverseStroke ? 1 : 0; color: "red" }
                            GradientStop { position: reverseStroke ? 0 : 1; color: "cyan" }
                        }

                        startX: 10; startY: 10
                        PathLine { relativeX: 140; relativeY: 0 }
                        PathLine { relativeX: 0; relativeY: 100 }
                        PathLine { relativeX: -140; relativeY: 0 }
                        PathLine { relativeX: 0; relativeY: -100 }
                    }

                    ShapePath {
                        cosmeticStroke: cosmetic
                        strokeWidth: 10
                        fillGradient: RadialGradient {
                            centerX: 80
                            centerY: 75 + 1 * 140
                            centerRadius: centerY
                            focalX: centerX
                            focalY: centerY
                            GradientStop { position: 0; color: "red" }
                            GradientStop { position: 0.5; color: "cyan" }
                            GradientStop { position: 1; color: "red" }
                        }
                        strokeGradient: RadialGradient {
                            centerX: 80
                            centerY: 75 + 1 * 140
                            centerRadius: centerY
                            focalX: centerX
                            focalY: centerY
                            GradientStop { position: 0; color: reverseStroke ? "cyan" : "red" }
                            GradientStop { position: .5; color: reverseStroke ? "red" : "cyan" }
                            GradientStop { position: 1; color: reverseStroke ? "cyan" : "red" }
                        }

                        startX: 10; startY: 10 + 1 * 140
                        PathLine { relativeX: 140; relativeY: 0 }
                        PathLine { relativeX: 0; relativeY: 100 }
                        PathLine { relativeX: -140; relativeY: 0 }
                        PathLine { relativeX: 0; relativeY: -100 }
                    }

                    ShapePath {
                        cosmeticStroke: cosmetic
                        strokeWidth: 10
                        fillGradient: ConicalGradient {
                            centerX: 80
                            centerY: 75 + 2 * 140
                            GradientStop { position: 0; color: "red" }
                            GradientStop { position: .5; color: "cyan" }
                            GradientStop { position: 1; color: "red" }
                        }
                        strokeGradient: ConicalGradient {
                            centerX: 80
                            centerY: 75 + 2 * 140
                            GradientStop { position: 0; color: reverseStroke ? "cyan" : "red" }
                            GradientStop { position: .5; color: reverseStroke ? "red" : "cyan" }
                            GradientStop { position: 1; color: reverseStroke ? "cyan" : "red" }
                        }

                        startX: 10; startY: 10 + 2 * 140
                        PathLine { relativeX: 140; relativeY: 0 }
                        PathLine { relativeX: 0; relativeY: 100 }
                        PathLine { relativeX: -140; relativeY: 0 }
                        PathLine { relativeX: 0; relativeY: -100 }
                    }
                }
            }
        }
    }
}
