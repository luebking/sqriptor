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

#include <QDebug>
#include "lexer/fontconfig.h"


QsciLexerFontConfig::QsciLexerFontConfig(QObject *parent) : QsciLexerXML(parent)
{
}


const char *QsciLexerFontConfig::keywords(int set) const
{
    // qDebug() << "queried set " << set;
    if (set == 1)
        return  "fontconfig dir cachedir include match alias config blank rescan selectfont "
                "acceptfont rejectfont glob pattern patelt match test edit int double string bool "
                "matrix range charset langset name const or and plus minus times divide "
                "eq not_eq less less_eq more more_eq contains not_contains not if family prefer "
                "accept default xml description" // elements
                "family familylang style stylelang fullname fullnamelang slant weight size width "
                "aspect pixelsize spacing foundry antialias hinting hintstyle verticallayout "
                "autohint globaladvance file index ftface rasterizer outline scalable scale "
                "dpi rgba lcdfilter minspace charset lang fontversion capability embolden "
                "fontfeatures prgname target mode qual binding compare ignore-blanks version"; // attributes
    if (set == 2) /// JS keyword abuse, @todo doesn't work…
        return  "thin extralight ultralight light book regular normal medium demibold semibold "
                "bold extrabold black heavy roman italic oblique ultracondensed extracondensed "
                "condensed semicondensed normal semiexpanded expanded extraexpanded ultraexpanded "
                "proportional dual mono charcell unknown rgb bgr vrgb vbgr none lcdnone lcddefault "
                "lcdlight lcdlegacy hintnone hintslight hintmedium hintfull"; // consts
    if (set == 6)
        return QsciLexerXML::keywords(set);
    return nullptr;
}
