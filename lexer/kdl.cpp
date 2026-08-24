/*
 *   Sqriptor text editor for Qt6
 *   Copyright 2026 by Thomas Lübking <thomas.luebking@gmail.com>
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
#include "lexer/kdl.h"

QsciLexerKDL::QsciLexerKDL(QObject *parent) : QsciLexerCPP(parent)
{
    // KDL doesn't have a preprocessor
    setFoldPreprocessor(false);
    setStylePreprocessor(true);
}

const char *QsciLexerKDL::keywords(int set) const
{
    if (set == 1)
        return "true false off null"; // KDL does not have any keywords but hey…
    return "";
}

const char *QsciLexerKDL::wordCharacters() const
{
    // add the dash. "+" is legit used for eg. shortcuts, so nice to separate
    // unfortunately this does not impact the highlighting … :(
    return "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_#-";
}
