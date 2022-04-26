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


#include "lexer/nim.h"


QsciLexerNIM::QsciLexerNIM(QObject *parent) : QsciLexerPython(parent)
{
}


const char *QsciLexerNIM::keywords(int set) const
{
    if (set == 1)
        return  "addr and as asm bind block break case cast concept"
                "const continue converter defer discard distinct div do elif else end enum "
                "except export finally for from func if import in include interface is "
                "isnot iterator let macro method mixin mod nil not notin object of or out "
                "proc ptr raise ref return shl shr static template try tuple type "
                "using var when while xor yield";
    return nullptr;
}
