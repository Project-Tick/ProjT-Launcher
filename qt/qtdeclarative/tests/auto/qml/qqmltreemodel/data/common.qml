// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import QtQuick
import Qt.labs.qmlmodels

Item {
    id: root
    width: 200
    height: 200

    property alias testModel: testModel
    property alias treeView: treeView

    function appendNodeToRoot() {
        testModel.appendRow({
                            checked: true,
                            amount: 1,
                            fruitType: "Pear",
                            fruitName: "Williams",
                            fruitPrice: 1.50,
                            color: "green"
                            })
    }

    function appendSubTreeToRoot() {
        testModel.appendRow({
                            checked: false,
                            amount: 4,
                            fruitType: "Peach",
                            fruitName: "Princess Peach",
                            fruitPrice: 1.45,
                            color: "yellow",
                            rows: [
                                {
                                    checked: true,
                                    amount: 5,
                                    fruitType: "Strawberry",
                                    fruitName: "Perry the Berry",
                                    fruitPrice: 3.80,
                                    color: "red"
                                },
                                {
                                    checked: false,
                                    amount: 6,
                                    fruitType: "Pear",
                                    fruitName: "Bear Pear",
                                    fruitPrice: 1.50,
                                    color: "green"
                                }
                            ]
                        })
    }

    function appendToRootInvalid1() {
        testModel.appendRow(123)
    }

    function appendToRootInvalid2() {
        testModel.appendRow({
                            checked: true,
                            amount: 1,
                            fruitType: "Pear",
                            fruitName: [],
                            fruitPrice: 1.50,
                            color: "green"
                            })
    }

    function appendToRootInvalid3() {
        testModel.appendRow([{
                            checked: true,
                            amount: 1,
                            fruitType: "Pear",
                            fruitName: [],
                            fruitPrice: 1.50,
                            color: "green"
                            }])
    }

    function appendNodeToRootWithExtraData() {
        testModel.appendRow({
                            checked: true,
                            amount: 1,
                            fruitType: "Pear",
                            fruitName: "Williams",
                            fruitPrice: 1.50,
                            color: "green",
                            extradata: "extradata"
                            })
    }

    function appendToNegativeIndex() {
        var index = testModel.index([0,1,-1], 0)

        testModel.appendRow(index, {
                            checked: true,
                            amount: 1,
                            fruitType: "Pear",
                            fruitName: "Williams",
                            fruitPrice: 1.50,
                            color: "green"
                            })
    }

    function appendToInvalidIndex() {
        var index = testModel.index([0,12,1], 0)

        testModel.appendRow(index, {
                            checked: true,
                            amount: 1,
                            fruitType: "Pear",
                            fruitName: "Williams",
                            fruitPrice: 1.50,
                            color: "green"
                            })
    }

    function appendInvalidNode1() {
        var index = testModel.index([0,1,1], 0)
        testModel.appendRow(index, 150)
    }

    function appendInvalidNode2() {
        var index = testModel.index([0,1,1], 0)

        testModel.appendRow(index, {
                            checked: false,
                            amount: 4,
                            fruitType: "Peach",
                            fruitName: "Princess Peach",
                            fruitPrice: 1.45,
                            color: "yellow",
                            rows: [
                                {
                                    checked: true,
                                    amount: 5,
                                    fruitType: "Strawberry",
                                    fruitName: "Perry the Berry",
                                    fruitPrice: 3.80,
                                    color: "red",
                                },
                                {
                                    checked: false,
                                    amount: 6,
                                    fruitType:[],
                                    fruitName: "Bear Pear",
                                    fruitPrice: 1.50,
                                    color: "green",
                                }
                            ]
                        })
    }

    function appendInvalidNode3() {
        var index = testModel.index([0,1,1], 0)

        testModel.appendRow(index, [{
                            checked: true,
                            amount: 1,
                            fruitType: "Pear",
                            fruitName: "Bear Pear",
                            fruitPrice: 1.50,
                            color: "green"
                            }])
    }

    function appendToLeaf() {
        var index = testModel.index([1,0], 0)

        testModel.appendRow(index, {
                            checked: true,
                            amount: 1,
                            fruitType: "Pear",
                            fruitName: "Bear Pear",
                            fruitPrice: 1.50,
                            color: "green"
                            })
    }

    function appendToMiddle() {
        var index = testModel.index([0,0], 0)

        testModel.appendRow(index, {
                            checked: true,
                            amount: 5,
                            fruitType: "Strawberry",
                            fruitName: "Perry the Berry",
                            fruitPrice: 3.80,
                            color: "red"
                            })
    }


    function appendToNodeWithExtraData() {
        var index = testModel.index([1], 0)

        testModel.appendRow(index, {
                            checked: true,
                            amount: 1,
                            fruitType: "Pear",
                            fruitName: "Williams",
                            fruitPrice: 1.50,
                            color: "green",
                            extradata: "extradata"
                            })
    }

    function setWithNegativeIndex() {
        var index = testModel.index([0,1,-1], 0)

        testModel.setRow(index, {
                        checked: true,
                        amount: 1,
                        fruitType: "Pear",
                        fruitName: "Williams",
                        fruitPrice: 1.50,
                        color: "green"
                        })
    }

    function setWithInvalidIndex() {
        var index = testModel.index([3,0,1], 0)

        testModel.setRow(index, {
                        checked: true,
                        amount: 1,
                        fruitType: "Pear",
                        fruitName: "Williams",
                        fruitPrice: 1.50,
                        color: "green"
                        })
    }

    function setInvalidData1() {
        var index = testModel.index([0,1,1], 0)
        testModel.setRow(index, 150)
    }

    function setInvalidData2() {
        var index = testModel.index([0,1,1], 0)

        testModel.setRow(index, {
                        checked: false,
                        amount: 4,
                        fruitType: "Peach",
                        fruitName: "Princess Peach",
                        fruitPrice: 1.45,
                        color: "yellow",
                        rows: [
                            {
                                checked: true,
                                amount: 5,
                                fruitType: "Strawberry",
                                fruitName: "Perry the Berry",
                                fruitPrice: 3.80,
                                color: "red",
                            },
                            {
                                checked: false,
                                amount: 6,
                                fruitType: "Pear",
                                fruitName: "Bear Pear",
                                fruitPrice: 1.50,
                                color: "green",
                            }
                        ]
                    })
    }

    function setInvalidData3() {
        var index = testModel.index([0,1,1], 0)

        testModel.setRow(index, [{
                        checked: true,
                        amount: 1,
                        fruitType: "Pear",
                        fruitName: "Bear Pear",
                        fruitPrice: 1.50,
                        color: "green"
                        }])
    }

    function setLeaf() {
        var index = testModel.index([0,1,1], 0)

        testModel.setRow(index, {
                        checked: true,
                        amount: 1,
                        fruitType: "Pear",
                        fruitName: "Bear Pear",
                        fruitPrice: 1.50,
                        color: "green"
                        })
    }

    function setNodeWithExtraData() {
        var index = testModel.index([1,0], 0)

        testModel.setRow(index, {
                        checked: true,
                        amount: 4,
                        fruitType: "Orange",
                        fruitName: "Navel",
                        fruitPrice: 2.50,
                        color: "orange",
                        extradata: "extradata"
                        })
    }

    function insertRowTopLevel() {
        testModel.insertRow(0, {
                        checked: true,
                        amount: 42,
                        fruitType: "InsertedOrange",
                        fruitName: "InsertedNavel",
                        fruitPrice: 2.50,
                        color: "orange"
                        })
    }

    function insertRowTop() {
        var index = testModel.index([0], 0)

        testModel.insertRow(0, index, {
                        checked: true,
                        amount: 420,
                        fruitType: "InsertedOrange2",
                        fruitName: "InsertedNavel2",
                        fruitPrice: 2.50,
                        color: "red"
                        })
    }

    function insertRow() {
        var index = testModel.index([0], 0)

        testModel.insertRow(1, index, {
                        checked: false,
                        amount: 4200,
                        fruitType: "InsertedOrange3",
                        fruitName: "InsertedNavel3",
                        fruitPrice: 2.50,
                        color: "blue"
                        })
    }

    function insertRowAtEnd() {
        testModel.insertRow(3, {
                        checked: false,
                        amount: 100,
                        fruitType: "InsertedOrangeEnd",
                        fruitName: "InsertedNavelEnd",
                        fruitPrice: 2.50,
                        color: "black"
                        })
    }

    function insertWithChildren() {
        var index = testModel.index([0], 0)

        testModel.insertRow(0, index, {
                        checked: false,
                        amount: 1,
                        fruitType: "ParentFruit",
                        fruitName: "ParentFruit",
                        fruitPrice: 2.50,
                        color: "green",
                        rows: [
                            {
                                checked: true,
                                amount: 5,
                                fruitType: "BabyFruit",
                                fruitName: "BabyFruit",
                                fruitPrice: 2.50,
                                color: "green",
                            },
                            {
                                checked: false,
                                amount: 6,
                                fruitType: "BabyFruit1",
                                fruitName: "BabyFruit1",
                                fruitPrice: 2.50,
                                color: "green",
                            }
                        ]
                        })
    }

    function insertRowNonExisingIndex() {
        var index = testModel.index([0], 0)

        testModel.insertRow(13, index, {
                        checked: false,
                        amount: 4200,
                        fruitType: "InsertedOrange",
                        fruitName: "InsertedNavel",
                        fruitPrice: 2.50,
                        color: "blue"
                        })
    }

    function insertInvalidData() {
        testModel.insertRow(0, {
                        house: 12,
                        tree: false
                        })
    }

    function insertInvalidData2() {
        testModel.insertRow(0, {
                        house: 12,
                        tree: false,
                        bird: "eagle",
                        fourth: 4,
                        fifth: 5
                        })
    }

    TreeView {
        id: treeView
        anchors.fill: parent
        model: TestModel {
            id: testModel
        }
        delegate: Text {
            text: model.display
        }
    }
}
