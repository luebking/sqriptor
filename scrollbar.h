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

#ifndef SCROLLBAR_H
#define SCROLLBAR_H

#include <QList>
#include <QScrollBar>

class QPaintEvent;

class ScrollBar : public QScrollBar
{
    Q_OBJECT

public:
    ScrollBar();
    void setMarkers(const QList<int> &markers) {
        m_markers = markers;
        update();
        }
protected:
    void paintEvent(QPaintEvent *event);
private:
    QList<int> m_markers;
};

#endif // SCROLLBAR_H
