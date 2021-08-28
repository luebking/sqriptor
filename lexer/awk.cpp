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


#include "lexer/awk.h"


QsciLexerAWK::QsciLexerAWK(QObject *parent) : QsciLexerBash(parent)
{
}


const char *QsciLexerAWK::keywords(int set) const
{
    if (set == 1)
        return  "break case continue default do else exit function for if in "
                "next return switch while @include delete nextfile print printf "
                "BEGIN BEGINFILE END atan2 cos exp int log rand sin sqrt srand "
                "asort asorti gensub gsub index length match patsplit split "
                "sprintf strtonum sub substr tolower toupper close fflush "
                "system mktime strftime systime and compl lshift rshift xor "
                "isarray bindtextdomain dcgettext dcngettext" //;
//    if (set == 2)
                " ARGC ARGIND ARGV FILENAME FNR FS NF NR OFMT OFS ORS RLENGTH RS"
                "RSTART SUBSEP TEXTDOMAIN BINMODE CONVFMT FIELDWIDTHS FPAT "
                "IGNORECASE LINT TEXTDOMAIN ENVIRON ERRNO PROCINFO RT";
    return nullptr;
}
