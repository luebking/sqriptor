/*
 *   Sqriptor text editor for Qt5
 *   Copyright 2025 by Thomas Lübking <thomas.luebking@gmail.com>
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
#include "lexer/cppqt.h"


QsciLexerCPPQt::QsciLexerCPPQt(QObject *parent) : QsciLexerCPP(parent)
{
}

/*
https://en.cppreference.com/w/cpp/keyword

alignas (C++11)
alignof (C++11)
char8_t (C++20)
char16_t (C++11)
char32_t (C++11)
concept (C++20)
consteval (C++20) (5)
constexpr (C++11) (3)
constinit (C++20)
co_await (C++20)
co_return (C++20)
co_yield (C++20)
decltype (C++11)
noexcept (C++11)
nullptr (C++11)
requires (C++20)
static_assert (C++11)
thread_local (C++11)

In addition to keywords, there are identifiers with special meaning, which may be used as names 
of objects or functions, but have special meaning in certain contexts.
----------------------------
final (C++11)
override (C++11)
import (C++20)
module (C++20)
*/


const char *QsciLexerCPPQt::keywords(int set) const
{
    if (set == 1) {
        return // extended from QSciScintilla - not eager to sprintf here
            "and and_eq asm auto bitand bitor bool break case "
            "catch char class compl const const_cast continue "
            "default delete do double dynamic_cast else enum "
            "explicit export extern false float for friend goto if "
            "inline int long mutable namespace new not not_eq "
            "operator or or_eq private protected public register "
            "reinterpret_cast short signed sizeof static"
            "static_cast struct switch template this throw true "
            "try typedef typeid typename union unsigned using "
            "virtual void volatile wchar_t while xor xor_eq"
            // extra stuff
            "char8_t char16_t char32_t constexpr decltype noexcept override final "
            // Qt stuff
            "foreach "
            "qint8 qint16 qint32 qint64 qint128 qintptr qlonglong "
            "qptrdiff qreal qsizetype quint8 quint16 quint32 quint64 "
            "quint128 quintptr qulonglong uchar uint ulong ushort";
    }
    if (set == 2) {
//        qDebug() << QsciLexerCPP::keywords(set);
        return 
            "return nullptr alignas alignof import module "
            "connect disconnect emit SIGNAL signals SLOT slots tr "
            "qobject_cast Q_ARG Q_ENUM Q_OBJECT Q_RETURN_ARG Q_FLAG Q_UNUSED qDebug";
    }
    return QsciLexerCPP::keywords(set);
}
