/* Copyright (c) 2014-2018, The Qt Company. All rights reserved.
 * Copyright (c) 2019, Nikita Grebenyuk
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "GraphicsView.h"
#include <QTouchEvent>
#include <QWheelEvent>

GraphicsView::GraphicsView(QGraphicsScene *scene, QGraphicsPixmapItem *pixItem, QWidget *parent)
    : QGraphicsView(scene, parent), m_pPixItem(pixItem), totalScaleFactor(1.0)
{
    if (this->scene() == nullptr) {
        QGraphicsScene *newScene = new QGraphicsScene;
        this->setScene(newScene);
    }
    if (m_pPixItem == nullptr) {
        m_pPixItem = new QGraphicsPixmapItem;
        m_pPixItem->setTransformationMode(Qt::SmoothTransformation);
        this->scene()->addItem(m_pPixItem);
    }
    this->viewport()->setAttribute(Qt::WA_AcceptTouchEvents);
    this->setDragMode(ScrollHandDrag);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

bool GraphicsView::viewportEvent(QEvent *event)
{
    switch (event->type()) {
    case QEvent::TouchBegin:
    case QEvent::TouchUpdate:
    case QEvent::TouchEnd:
    {
        QTouchEvent *touchEvent = static_cast<QTouchEvent*>(event);
        QList<QTouchEvent::TouchPoint> touchPoints = touchEvent->touchPoints();
        if (touchPoints.count() == 2) {
            // determine scale factor
            const QTouchEvent::TouchPoint &touchPoint0 = touchPoints.first();
            const QTouchEvent::TouchPoint &touchPoint1 = touchPoints.last();
            qreal currentScaleFactor =
                    QLineF(touchPoint0.pos(), touchPoint1.pos()).length()
                    / QLineF(touchPoint0.startPos(), touchPoint1.startPos()).length();
            if (touchEvent->touchPointStates() & Qt::TouchPointReleased) {
                // if one of the fingers is released, remember the current scale
                // factor so that adding another finger later will continue zooming
                // by adding new scale factor to the existing remembered value.
                totalScaleFactor *= currentScaleFactor;
                currentScaleFactor = 1;
            }
            setTransform(QTransform().scale(totalScaleFactor * currentScaleFactor, totalScaleFactor * currentScaleFactor));
            return true;
        }
        break;
    }
    case QEvent::Wheel:
    {
        QWheelEvent *wheelEvent = static_cast<QWheelEvent*>(event);
        if (wheelEvent->orientation() == Qt::Vertical) {
            qreal angle = wheelEvent->angleDelta().y();
            int numSteps = angle / 120;
            qreal currentScaleFactor = 1.0;
            for (int i = 1; i <= numSteps; ++i) {
                currentScaleFactor *= 1.05;
            }
            for (int i = -1; i >= numSteps; --i) {
                currentScaleFactor /= 1.05;
            }
            totalScaleFactor *= currentScaleFactor;
            currentScaleFactor = 1;
            setTransform(QTransform().scale(totalScaleFactor, totalScaleFactor));
            return true;
        }
        break;
    }
    default:
        break;
    }
    return QGraphicsView::viewportEvent(event);
}
void GraphicsView::setPix(QPixmap pix) {
    m_pPixItem->setPixmap(pix);
    this->scene()->setSceneRect(QRect(0, 0, pix.width(), pix.height()));
    totalScaleFactor = qMin((qreal)this->width() / (qreal)pix.width(), (qreal)this->height() / (qreal)pix.height());
    this->setTransform(QTransform().scale(totalScaleFactor, totalScaleFactor));
}
