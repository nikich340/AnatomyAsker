/* Copyright (c) 2014-2019, The Qt Company. All rights reserved.
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

#pragma once
#include <QGraphicsView>
#include <QGraphicsPixmapItem>

class GraphicsView : public QGraphicsView
{
    Q_OBJECT

public:
    GraphicsView(QGraphicsScene *scene = nullptr, QGraphicsPixmapItem *pixItem = nullptr, QWidget *parent = nullptr);
    bool viewportEvent(QEvent *event) override;
    void setPix(QPixmap pix);

private:
    QPixmap source;
    QGraphicsPixmapItem *m_pPixItem;
    qreal totalScaleFactor;
};
