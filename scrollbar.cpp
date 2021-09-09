/*
 *   Sqriptor text editor for Qt5
 *   Copyright 2021 by Thomas Lübking <thomas.luebking@gmail.com>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License version 2
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include <QPaintEvent>
#include <QPainter>
#include <QRect>
#include <QStyle>
#include <QStyleOptionSlider>
#include "scrollbar.h"

#include <QDebug>

ScrollBar::ScrollBar() : QScrollBar() {}

void ScrollBar::paintEvent(QPaintEvent *event)
{
    QScrollBar::paintEvent(event);
    if (maximum() < 1)
        return;
    QPainter p(this);
    p.setPen(Qt::white);
    p.setCompositionMode(QPainter::RasterOp_SourceXorDestination);
    QStyleOptionSlider opt;
    initStyleOption(&opt);
    opt.subControls = QStyle::SC_All;
    QRect groove = style()->subControlRect(QStyle::CC_ScrollBar, &opt, QStyle::SC_ScrollBarGroove, this);
    groove.adjust(groove.width()/3, 0, -groove.width()/3, 0);
    foreach (int marker, m_markers) {
        int y = groove.y() + groove.height()*marker/maximum();
        p.drawLine(groove.left(), y, groove.right(), y);
    }
}