// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#define QT_USE_QSTRINGBUILDER

#include <qtest.h>
#include <QPainterPath>
#include <QSGNode>
#include <private/qquadpath_p.h>
#include <private/qquickshapecurverenderer_p.h>
#include <private/qsgcurveprocessor_p.h>

class tst_CurveRenderer : public QObject
{
    Q_OBJECT

public:
    tst_CurveRenderer();

private slots:
    void initTestCase_data();

    void toQuadPath();
    void solveIntersections();
    void addCurvature();

    void render_data();
    void render();
};

tst_CurveRenderer::tst_CurveRenderer()
{
}

QPainterPath createRegularPolygon(int n, qreal offsetFraction = 0.0)
{
    const QPointF center;
    const qreal radius = 100;
    const qreal offset = offsetFraction * radius;

    QPainterPath path;

    if (n < 3) {
        return path; // Invalid polygon
    }

    // Calculate the angle between each vertex
    qreal angleStep = 2.0 * M_PI / n;

    // Start at the top (angle = -π/2 to point upward)
    qreal startAngle = -M_PI / 2.0;

    // Calculate first vertex (with offset added)
    QPointF firstPoint(
            center.x() + (radius + offset) * std::cos(startAngle),
            center.y() + (radius + offset) * std::sin(startAngle)
            );

    path.moveTo(firstPoint);

    // Add remaining vertices, alternating offset
    for (int i = 1; i < n; ++i) {
        qreal angle = startAngle + i * angleStep;
        // Alternate between subtracting and adding offset
        qreal currentRadius = (i % 2 == 0) ? radius + offset : radius - offset;

        QPointF point(
                center.x() + currentRadius * std::cos(angle),
                center.y() + currentRadius * std::sin(angle)
                );
        path.lineTo(point);
    }

    // Close the path
    path.closeSubpath();

    return path;
}

void tst_CurveRenderer::initTestCase_data()
{
    QTest::addColumn<QPainterPath>("path");

    {
        QPainterPath path;
        path.moveTo(100, 400);
        path.lineTo(200, 400);
        path.quadTo(220, 500, 700, 700);
        path.cubicTo(600, 600, 800, 200, 200, 50);
        path.lineTo(50, 750);
        path.cubicTo(600, 700, 300, 200, 750, 50);
        path.cubicTo(800, 200, 300, 800, 300, 400);
        QTest::newRow("figure1") << path;
    }

    for (int i = 4; i <= 1024; i *= 2)
        QTest::newRow(QByteArray::number(i).prepend("poly")) << createRegularPolygon(i, 0.5);
}

void tst_CurveRenderer::toQuadPath()
{
    QFETCH_GLOBAL(QPainterPath, path);

    QBENCHMARK {
        (void)QQuadPath::fromPainterPath(path);
    }
}

void tst_CurveRenderer::solveIntersections()
{
    QFETCH_GLOBAL(QPainterPath, path);

    QQuadPath qPath = QQuadPath::fromPainterPath(path);
    qPath = qPath.subPathsClosed();

    QBENCHMARK {
        QQuadPath aPath(qPath);
        (void)QSGCurveProcessor::solveIntersections(aPath);
    }
}

void tst_CurveRenderer::addCurvature()
{
    QFETCH_GLOBAL(QPainterPath, path);

    QQuadPath qPath = QQuadPath::fromPainterPath(path);
    qPath = qPath.subPathsClosed();
    QSGCurveProcessor::solveIntersections(qPath);

    QBENCHMARK {
        QQuadPath aPath(qPath);
        aPath.addCurvatureData();
    }
}

void tst_CurveRenderer::render_data()
{
    QTest::addColumn<bool>("hasFill");
    QTest::addColumn<int>("strokeWidth");
    // tbd: dashed, gradient fill etc.

    QTest::newRow("onlyfill") << true << 0;
    QTest::newRow("onlystroke") << false << 10;
    QTest::newRow("strokeandfill") << true << 10;
}

void tst_CurveRenderer::render()
{
    QFETCH(bool, hasFill);
    QFETCH(int, strokeWidth);
    QFETCH_GLOBAL(QPainterPath, path);

    QSGNode dummyNode;
    QQuickShapeCurveRenderer renderer(nullptr);
    renderer.setRootNode(&dummyNode);
    renderer.beginSync(1, nullptr); // Just sets the number of path items to 1; needs no endsync
    renderer.setFillColor(0, hasFill ? Qt::yellow : Qt::transparent);
    renderer.setStrokeColor(0, Qt::black);
    renderer.setStrokeWidth(0, strokeWidth);

    QBENCHMARK {
        renderer.beginSync(1, nullptr);
        renderer.setPath(0, path);
        renderer.endSync(false);
        renderer.updateNode();
    }
}

QTEST_MAIN(tst_CurveRenderer)
#include "tst_bench_curverenderer.moc"
