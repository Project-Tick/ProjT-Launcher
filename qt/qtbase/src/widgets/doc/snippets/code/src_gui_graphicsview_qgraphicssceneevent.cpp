// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include <QGraphicsSceneDragDropEvent>

class GraphicsSceneEvent : public QGraphicsSceneDragDropEvent
{
public:
    void exampleFunction() {
        //! [0]
        setDropAction(proposedAction());
        //! [0]
    }
};
