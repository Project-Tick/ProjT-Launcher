// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include <QtWidgets>
#include <QPrinter>

void examples()
{
    {
        //! [0]
        QGraphicsScene scene;
        scene.addText("Hello, world!");

        QGraphicsView view(&scene);
        view.show();
        //! [0]
    }

    {
        //! [1]
        QGraphicsScene scene;
        scene.addRect(QRectF(-10, -10, 20, 20));

        QGraphicsView view(&scene);
        view.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
        view.show();
        //! [1]
    }

    {
        //! [2]
        QGraphicsView view;
        view.setBackgroundBrush(QImage(":/images/backgroundtile.png"));
        view.setCacheMode(QGraphicsView::CacheBackground);
        //! [2]
    }

    {
        //! [4]
        QGraphicsScene scene;
        scene.addItem(someItem);
        //...

        QGraphicsView view(&scene);
        view.show();
        //...

        QPrinter printer(QPrinter::HighResolution);
        printer.setDocName("Example");
        QPainter painter(&printer);

        // print, fitting the viewport contents into a full page
        view.render(&painter);

        // print the upper half of the viewport into the lower.
        // half of the page.
        QRect viewport = view.viewport()->rect();
        view.render(&painter,
                    QRectF(0, printer.height() / 2,
                        printer.width(), printer.height() / 2),
                    viewport.adjusted(0, 0, 0, -viewport.height() / 2));

        //! [4]
    }

    {
        //! [7]
        QGraphicsScene scene;
        scene.addText("GraphicsView rotated clockwise");

        QGraphicsView view(&scene);
        view.rotate(90); // the text is rendered with a 90 degree clockwise rotation
        view.show();
        //! [7]
    }
}

class CustomView : public QGraphicsView
{
public:
    void mousePressEvent(QMouseEvent *event) override;
};

//! [5_6_declaration]
void CustomView::mousePressEvent(QMouseEvent *event)
{
//! [5_6_declaration]

    {
    //! [5]
        qDebug() << "There are" << items(event->pos()).size()
                 << "items at position" << mapToScene(event->pos());
    //! [5]
    }

    {
    //! [6]
        if (QGraphicsItem *item = itemAt(event->pos())) {
            qDebug() << "You clicked on item" << item;
        } else {
            qDebug("You didn't click on an item.");
        }
    //! [6]
    }

//! [5_6_end]
}
//! [5_6_end]
