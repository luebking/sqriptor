/*
 *   Sqriptor text editor for Qt5
 *   Copyright 2022 by Thomas Lübking <thomas.luebking@gmail.com>
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

#ifndef QSCILEXERLISP_H
#define QSCILEXERLISP_H

// class QStringList;

#include <Qsci/qscilexercustom.h>

class QsciLexerLISP : public QsciLexerCustom {
    Q_OBJECT
public:
    QsciLexerLISP(QObject *parent);
    void updateColors();
    void setKeywords(const char *keywords);
    virtual void styleText (int start, int end);
    virtual const char *language () const { return "LISP"; }
    virtual QString description(int style) const {Q_UNUSED(style); return "LISP"; }
//    virtual int blockLookback() const
//    virtual const char *blockStartKeyword(int *style = 0) const { return "("; }
//    virtual const char *blockStart(int *style = 0) const { return "("; }
//    virtual const char *blockEnd(int *style = 0) const { return ")"; }
private:
    QStringList m_keywords;
};

#endif // QSCILEXERMARKDOWN2_H