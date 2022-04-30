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


#include <QDebug>
#include <QtMath>
#include <QRegularExpression>

#include <Qsci/qsciscintilla.h>

#include "../sqriptor.h"
#include "lexer/lisp.h"


namespace Style {
    enum Type {
        Default = 0, Operator, Number, Keyword, Literal, QuasiLiteral, String, Comment, Error
    };
};

QsciLexerLISP::QsciLexerLISP(QObject *parent) : QsciLexerCustom(parent)
{
//    setAutoIndentStyle(QsciScintilla::AiOpening|QsciScintilla::AiClosing); // this somehow does nothing...
    updateColors();
    setKeywords("not defun + - * / = < > <= >= princ eval apply funcall quote identity function "
                "complement backquote lambda set setq setf defun defmacro gensym make symbol "
                "intern symbol name symbol value symbol plist get getf putprop remprop hash make "
                "array aref car cdr caar cadr cdar cddr caaar caadr cadar caddr cdaar cdadr cddar "
                "cdddr caaaar caaadr caadar caaddr cadaar cadadr caddar cadddr cdaaar cdaadr "
                "cdadar cdaddr cddaar cddadr cdddar cddddr cons list append reverse last nth "
                "nthcdr member assoc subst sublis nsubst  nsublis remove length list length "
                "mapc mapcar mapl maplist mapcan mapcon rplaca rplacd nconc delete atom symbolp "
                "numberp boundp null listp consp minusp zerop plusp evenp oddp eq eql equal cond "
                "case and or let l if print prog prog1 prog2 progn go return do dolist dotimes catch "
                "throw error cerror break continue errset baktrace evalhook truncate float rem min "
                "max abs sin cos tan expt exp sqrt random logand logior logxor lognot bignums "
                "logeqv lognand lognor logorc2 logtest logbitp logcount integer length nil");
}

void QsciLexerLISP::setKeywords(const char *keywords)
{
    m_keywords = (QString(keywords).split(" ", Qt::SkipEmptyParts));
}

void QsciLexerLISP::updateColors()
{
    QColor bg = Sqriptor::config.color.bg;
    QColor fg = Sqriptor::config.color.fg;
    setPaper(bg);
    setColor(fg);
    setColor(fg, Style::Default);
    setColor(Sqriptor::config.color.number, Style::Number);
    setColor(Sqriptor::config.color.operateur, Style::Operator);
    setColor(Sqriptor::config.color.keyword, Style::Keyword);
    setColor(Sqriptor::config.color.character, Style::Literal);
    setColor(Sqriptor::config.color.preproc, Style::QuasiLiteral);
    setColor(Sqriptor::config.color.string, Style::String);
    setColor(Sqriptor::config.color.comment, Style::Comment);
    setColor(Sqriptor::config.color.error, Style::Error);
    setFont(Sqriptor::config.font);
    setFont(Sqriptor::config.font, Style::Default);
}

static bool operator==(const QStringView&view, const char* string) {
    return !view.compare(QLatin1String(string));
}

void QsciLexerLISP::styleText(int start, int end)
{
    static const QRegularExpression tokenizer(";|\\(|\\)|\\\"|\\s+|['`]*[A-Za-z\\d_#\\+@$%\\.\\^&=\\*-?\\!<>]+|'|`|\\W");
    static const QRegularExpression linebreak("\\s*(\\n|\\r)+\\s*");

    QString text = editor()->text(start, end);
    QRegularExpressionMatchIterator i = tokenizer.globalMatch(text/*, 0, QRegularExpression::PartialPreferFirstMatch*/);

    bool comment = false,
         string  = (start > 0) ? editor()->SendScintilla(QsciScintillaBase::SCI_GETSTYLEAT, start - 1) == Style::String : false,
         isNum;

    int length = 0;
    auto finishStyle = [&]() {
        setStyling(length, Style::Default);
        length = 0;
    };
    startStyling(start);
    QRegularExpressionMatch match, prev;
    while (i.hasNext()) {
        prev = match;
        match = i.next();
        QStringView token = match.capturedView(0);
        // qDebug() << token;
        if (comment) {
            length += token.toUtf8().length(); // match.capturedLength(0); - this sucks
            if (linebreak.match(token).hasMatch()) {
                setStyling(length, Style::Comment); length = 0; comment = false;
            }
            continue;
        }
        if (token == "\"") {
            if (string) {
                setStyling(length + 1, Style::String); string = false; length = 0;
            } else {
                finishStyle(); string = true; length = 1;
            }
            continue;
        }
        if (string) {
            length += token.toUtf8().length(); // match.capturedLength(0); - this sucks
            if (linebreak.match(token).hasMatch()) {
                setStyling(length, Style::String); length = 0; // we assume that the string continues in the next line
            }
            continue;
        }
        if (token == ";") {
            finishStyle(); length = 1; comment = true;
            continue;
        }
        if (token == "(" || token == ")") {
            finishStyle(); setStyling(1, Style::Operator);
            continue;
        }
        if (token.startsWith('\'')) {
            finishStyle(); setStyling(token.toUtf8().length(), Style::Literal);// match.capturedLength(0); - this sucks
            continue;
        }
        if (token.startsWith('`')) {
            finishStyle(); setStyling(token.toUtf8().length(), Style::QuasiLiteral);// match.capturedLength(0); - this sucks
            continue;
        }
        if (m_keywords.contains(token, Qt::CaseInsensitive)) {
            finishStyle(); setStyling(token.toUtf8().length(), Style::Keyword);// match.capturedLength(0); - this sucks
            continue;
        }
        Q_UNUSED(token.toInt(&isNum));
        if (!isNum)
            Q_UNUSED(token.toDouble(&isNum));
        if (isNum) {
            finishStyle(); setStyling(token.toUtf8().length(), Style::Number);// match.capturedLength(0); - this sucks
            continue;
        }
        length += token.toUtf8().length(); // match.capturedLength(0); - this sucks
    }
    if (length) {
        setStyling(length, comment ? Style::Comment : (string ? Style::Error : Style::Default));
        length = 0;
    }
}