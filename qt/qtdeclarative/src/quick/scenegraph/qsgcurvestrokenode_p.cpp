// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// Qt-Security score:significant reason:default

#include "qsgcurvestrokenode_p_p.h"
#include "qsgcurvestrokenode_p.h"

#include <private/qsgtexture_p.h>

QT_BEGIN_NAMESPACE

void QSGCurveStrokeMaterialShader::updateSampledImage(RenderState &state, int binding, QSGTexture **texture,
                                                      QSGMaterial *newMaterial, QSGMaterial *oldMaterial)
{
    Q_UNUSED(oldMaterial);
    QSGCurveStrokeMaterial *m = static_cast<QSGCurveStrokeMaterial *>(newMaterial);
    const QSGCurveStrokeNode *node = m->node();
    if (binding != 1 || node->gradientType() == QGradient::NoGradient)
        return;

    if (node->gradientType() != QGradient::NoGradient) {
        const QSGGradientCacheKey cacheKey(node->strokeGradient()->stops,
                                           node->strokeGradient()->spread);
        QSGTexture *t = QSGGradientCache::cacheForRhi(state.rhi())->get(cacheKey);
        t->commitTextureOperations(state.rhi(), state.resourceUpdateBatch());
        *texture = t;
    }
}


bool QSGCurveStrokeMaterialShader::updateUniformData(RenderState &state, QSGMaterial *newEffect, QSGMaterial *oldEffect)
{
    bool changed = false;
    QByteArray *buf = state.uniformData();
    Q_ASSERT(buf->size() >= 64);
    const int matrixCount = qMin(state.projectionMatrixCount(), newEffect->viewCount());

    auto *newMaterial = static_cast<QSGCurveStrokeMaterial *>(newEffect);
    auto *oldMaterial = static_cast<QSGCurveStrokeMaterial *>(oldEffect);

    auto *newNode = newMaterial != nullptr ? newMaterial->node() : nullptr;
    auto *oldNode = oldMaterial != nullptr ? oldMaterial->node() : nullptr;

    if (state.isMatrixDirty()) {
        float localScale = newNode != nullptr ? newNode->localScale() : 1.0f;
        for (int viewIndex = 0; viewIndex < matrixCount; ++viewIndex) {
            QMatrix4x4 m = state.combinedMatrix(viewIndex);
            m.scale(localScale);
            memcpy(buf->data() + viewIndex * 64, m.constData(), 64);
        }
        // determinant is xscale * yscale, as long as Item.transform does not include shearing or rotation
        float matrixScale = qSqrt(qAbs(state.determinant())) * state.devicePixelRatio() * localScale;
        memcpy(buf->data() + matrixCount * 64, &matrixScale, 4);
        float dpr = state.devicePixelRatio();
        memcpy(buf->data() + matrixCount * 64 + 8, &dpr, 4);
        changed = true;
    }

    if (state.isOpacityDirty()) {
        const float opacity = state.opacity();
        memcpy(buf->data() + matrixCount * 64 + 4, &opacity, 4);
        changed = true;
    }

    if (oldNode == nullptr || newNode->strokeWidth() != oldNode->strokeWidth()) {
        float w = newNode->strokeWidth();
        memcpy(buf->data() + matrixCount * 64 + 12, &w, 4);
        changed = true;
    }

    int offset = matrixCount * 64 + 16;
    if (newNode == nullptr)
        return changed;

    if (newNode->gradientType() == QGradient::NoGradient) {
        QVector4D newStrokeColor(newNode->color().redF(),
                                 newNode->color().greenF(),
                                 newNode->color().blueF(),
                                 newNode->color().alphaF());
        QVector4D oldStrokeColor = oldNode != nullptr
                                       ? QVector4D(oldNode->color().redF(),
                                                   oldNode->color().greenF(),
                                                   oldNode->color().blueF(),
                                                   oldNode->color().alphaF())
                                       : QVector4D{};
        if (oldNode == nullptr || oldStrokeColor != newStrokeColor) {
            memcpy(buf->data() + offset, &newStrokeColor, 16);
            changed = true;
        }
        offset += 16;
    } else if (newNode->gradientType() == QGradient::LinearGradient) {
        Q_ASSERT(buf->size() >= offset + 8 + 8);

        QVector2D newGradientStart = QVector2D(newNode->strokeGradient()->a);
        QVector2D oldGradientStart = oldNode != nullptr
                                         ? QVector2D(oldNode->strokeGradient()->a)
                                         : QVector2D{};

        if (newGradientStart != oldGradientStart || oldEffect == nullptr) {
            memcpy(buf->data() + offset, &newGradientStart, 8);
            changed = true;
        }
        offset += 8;

        QVector2D newGradientEnd = QVector2D(newNode->strokeGradient()->b);
        QVector2D oldGradientEnd = oldNode!= nullptr
                                       ? QVector2D(oldNode->strokeGradient()->b)
                                       : QVector2D{};

        if (newGradientEnd != oldGradientEnd || oldEffect == nullptr) {
            memcpy(buf->data() + offset, &newGradientEnd, 8);
            changed = true;
        }

        offset += 8;
    } else if (newNode->gradientType() == QGradient::RadialGradient) {
        Q_ASSERT(buf->size() >= offset + 8 + 8 + 4 + 4 + 4);

        QVector2D newFocalPoint = QVector2D(newNode->strokeGradient()->b);
        QVector2D oldFocalPoint = oldNode != nullptr
                                      ? QVector2D(oldNode->strokeGradient()->b)
                                      : QVector2D{};
        if (oldNode == nullptr || newFocalPoint != oldFocalPoint) {
            memcpy(buf->data() + offset, &newFocalPoint, 8);
            changed = true;
        }
        offset += 8;

        QVector2D newCenterPoint = QVector2D(newNode->strokeGradient()->a);
        QVector2D oldCenterPoint = oldNode != nullptr
                                       ? QVector2D(oldNode->strokeGradient()->a)
                                       : QVector2D{};

        QVector2D newCenterToFocal = newCenterPoint - newFocalPoint;
        QVector2D oldCenterToFocal = oldCenterPoint - oldFocalPoint;
        if (oldNode == nullptr || newCenterToFocal != oldCenterToFocal) {
            memcpy(buf->data() + offset, &newCenterToFocal, 8);
            changed = true;
        }
        offset += 8;

        float newCenterRadius = newNode->strokeGradient()->v0;
        float oldCenterRadius = oldNode != nullptr
                                    ? oldNode->strokeGradient()->v0
                                    : 0.0f;
        if (oldNode == nullptr || !qFuzzyCompare(newCenterRadius, oldCenterRadius)) {
            memcpy(buf->data() + offset, &newCenterRadius, 4);
            changed = true;
        }
        offset += 4;

        float newFocalRadius = newNode->strokeGradient()->v1;
        float oldFocalRadius = oldNode != nullptr
                                   ? oldNode->strokeGradient()->v1
                                   : 0.0f;
        if (oldNode == nullptr || !qFuzzyCompare(newFocalRadius, oldFocalRadius)) {
            memcpy(buf->data() + offset, &newFocalRadius, 4);
            changed = true;
        }
        offset += 4;

        // reserved
        offset += 8;

    } else if (newNode->gradientType() == QGradient::ConicalGradient) {
        Q_ASSERT(buf->size() >= offset + 8 + 4 + 4);

        QVector2D newFocalPoint = QVector2D(newNode->strokeGradient()->a);
        QVector2D oldFocalPoint = oldNode != nullptr
                                      ? QVector2D(oldNode->strokeGradient()->a)
                                      : QVector2D{};
        if (oldNode == nullptr || newFocalPoint != oldFocalPoint) {
            memcpy(buf->data() + offset, &newFocalPoint, 8);
            changed = true;
        }
        offset += 8;

        float newAngle = newNode->strokeGradient()->v0;
        float oldAngle = oldNode != nullptr
                             ? oldNode->strokeGradient()->v0
                             : 0.0f;
        if (oldNode == nullptr || !qFuzzyCompare(newAngle, oldAngle)) {
            newAngle = -qDegreesToRadians(newAngle);
            memcpy(buf->data() + offset, &newAngle, 4);
            changed = true;
        }
        offset += 4;

        // Reserved
        offset += 4;
    }

    if (oldNode == nullptr || newNode->debug() != oldNode->debug()) {
        float w = newNode->debug();
        memcpy(buf->data() + offset, &w, 4);
        changed = true;
    }

    return changed;
}

QSGMaterialType *QSGCurveStrokeMaterial::type() const
{
    static QSGMaterialType types[8];
    uint type = node()->gradientType();
    Q_ASSERT((type & ~3) == 0); // Only two first bits for gradient type

    uchar expanding = m_strokeExpanding ? 1 : 0;

    uint index = (type << 1) | expanding;
    Q_ASSERT(index < 8);

    return &types[index];
}

int QSGCurveStrokeMaterial::compare(const QSGMaterial *other) const
{
    const QSGCurveStrokeMaterial *otherMaterial = static_cast<const QSGCurveStrokeMaterial *>(other);

    QSGCurveStrokeNode *a = node();
    QSGCurveStrokeNode *b = otherMaterial->node();
    if (a == b)
        return 0;

    int typeDif = type() - other->type();
    if (!typeDif) {
        if (a->gradientType() == QGradient::NoGradient && a->color() != b->color())
            return a->color().rgb() < b->color().rgb() ? -1 : 1;

        if (a->gradientType() != QGradient::NoGradient) {
            const QSGGradientCache::GradientDesc &ga = *a->strokeGradient();
            const QSGGradientCache::GradientDesc &gb = *b->strokeGradient();

            if (int d = ga.compare(gb))
                return d;
        }

        if (a->strokeWidth() != b->strokeWidth())
            return a->strokeWidth() < b->strokeWidth() ? -1 : 1;
    }
    return typeDif;
}

QSGMaterialShader *QSGCurveStrokeMaterial::createShader(QSGRendererInterface::RenderMode renderMode) const
{
    Q_UNUSED(renderMode);
    int variant = int(QSGCurveStrokeMaterialShader::Variant::Default);
    if (m_strokeExpanding)
        variant |= int(QSGCurveStrokeMaterialShader::Variant::Expanding);
    if (node()->useStandardDerivatives())
        variant |= int(QSGCurveStrokeMaterialShader::Variant::Derivatives);
    switch (node()->gradientType()) {
    case QGradient::LinearGradient:
        variant |= int(QSGCurveStrokeMaterialShader::Variant::LinearGradient);
        break;
    case QGradient::RadialGradient:
        variant |= int(QSGCurveStrokeMaterialShader::Variant::RadialGradient);
        break;
    case QGradient::ConicalGradient:
        variant |= int(QSGCurveStrokeMaterialShader::Variant::ConicalGradient);
        break;
    default:
        break;
    }

    return new QSGCurveStrokeMaterialShader(variant, viewCount());
}

QT_END_NAMESPACE
