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
#include <QRegularExpression>

#include <Qsci/qsciscintilla.h>

#include "../sqriptor.h"
#include "lexer/journal.h"


namespace Style {
    enum Type {
        Default = 0, Date, Host, Process, Number, Error, Comment = Date
    };
};

QsciLexerJournal::QsciLexerJournal(QObject *parent) : QsciLexerCustom(parent)
{
}

void QsciLexerJournal::updateColors()
{
    QColor bg = Sqriptor::config.color.bg;
    QColor fg = Sqriptor::config.color.fg;
    setPaper(bg);
    setColor(fg);
    setColor(fg, Style::Default);
    setColor(Sqriptor::config.color.comment, Style::Date);
    setColor(Sqriptor::config.color.operateur, Style::Process);
    setColor(Sqriptor::config.color.number, Style::Number);
    setColor(Sqriptor::config.color.error, Style::Error);
    setColor(Sqriptor::config.color.string, Style::Host);
}

static inline void testToken(char byte, int *indicator, const char *lower, const char *upper, int length)
{
    int idx = *indicator;
    if (idx >= length)
        return;
    if (byte == lower[idx] || byte == upper[idx])
        ++(*indicator);
    else
        *indicator = 0;
}

void QsciLexerJournal::styleText (int start, int end)
{
    int length = 0;
    bool newline = true, comment = false;
    int error = 0, crash = 0, warning = 0, failed = 0;

    QByteArray text = editor()->bytes(start, end);
    startStyling(start);
    for (int i = 0; i < end - start; ++i) {
        ++length;
        if (text.at(i) == '\n') {
            Style::Type style = comment ? Style::Comment : Style::Default;
            if (error > 4 || crash > 4 || warning > 6 || failed > 5)
                style = Style::Error;
            setStyling(length, style);
            length = 0;
            error = crash = warning = failed = 0;
            newline = true; comment = false;
            continue;
        }
        if (!newline) {
            testToken(text.at(i), &error, "error", "ERROR", 5);
            testToken(text.at(i), &crash, "crash", "CRASH", 5);
            testToken(text.at(i), &warning, "warning", "WARNING", 7);
            testToken(text.at(i), &failed, "failed", "FAILED", 6);
        /*
            switch (error) {
                case 0: if (text.at(i) == 'e' || text.at(i) == 'E') ++error;
                    break;
                case 1: case 2: case 4:
                    (text.at(i) == 'r' || text.at(i) == 'R') ? ++error : error = 0;
                    break;
                case 3: (text.at(i) == 'o' || text.at(i) == 'O') ? ++error : error = 0;
                    break;
                default: break;
            }
            switch (crash) {
                case 0: if (text.at(i) == 'c' || text.at(i) == 'C') ++crash;
                    break;
                case 1: (text.at(i) == 'r' || text.at(i) == 'R') ? ++crash : crash = 0;
                    break;
                case 2: (text.at(i) == 'a' || text.at(i) == 'A') ? ++crash : crash = 0;
                    break;
                case 3: (text.at(i) == 's' || text.at(i) == 'S') ? ++crash : crash = 0;
                    break;
                case 4: (text.at(i) == 'h' || text.at(i) == 'H') ? ++crash : crash = 0;
                    break;
                default: break;
            }
            switch (warning) {
                case 0: if (text.at(i) == 'w' || text.at(i) == 'W') ++warning;
                    break;
                case 1: (text.at(i) == 'a' || text.at(i) == 'A') ? ++warning : warning = 0;
                    break;
                case 2: (text.at(i) == 'r' || text.at(i) == 'R') ? ++warning : warning = 0;
                    break;
                case 3: case 5: (text.at(i) == 'n' || text.at(i) == 'N') ? ++warning : warning = 0;
                    break;
                case 4: (text.at(i) == 'i' || text.at(i) == 'I') ? ++warning : warning = 0;
                    break;
                case 6: (text.at(i) == 'g' || text.at(i) == 'G') ? ++warning : warning = 0;
                    break;
                default: break;
            }
            */
            continue;
        }
        if (text.at(i) == '-' || text.at(i) == -30) { // ░
            newline = false;
            comment = true;
            continue;
        }
        
        auto nextWord = [&]() {
            while (i < end - start && text.at(i) != ' ') {
                if (text.at(i) == '\n') { // error, jump to next line
//                    qDebug() << "unexpected newline" << i;
                    --i; --length;
                    return false;
                }
                ++i;
                if (i >= end - start) { // error and out
                    qDebug() << "error > size" << i << int(end - start);
                    setStyling(length-1, Style::Error);
                    length = 0;
                    return false;
                }
                ++length;
            }
            if (i < end - start) {
                ++length; ++i;
            }
            return true;
        };
        
        newline = false;
        
        --length;
        if (!nextWord())
            continue;
        if (!nextWord())
            continue;
        if (!nextWord())
            continue;

        setStyling(length, Style::Comment);
        length = 0;
        if (!nextWord())
            continue;
        setStyling(length, Style::Host);
        length = 0;
        int pstart = i;
        if (!nextWord())
            continue;
        int pend = i;
        if (text.at(i-3) == ']') {
            i -= 3; length -= 3;
            while (i >= pstart && text.at(i) != '[') {
                --i; --length;
            }
        } else if (text.at(i-2) == ':') {
            length -= 2;
            i -= 2;
            pend = i;
        }
        setStyling(length, Style::Process);
        
        if (i != pend) {
            setStyling(pend - i - 2, Style::Number);
            i = pend - 2;
        }
        length = 1;
        if (i < end - start && text.at(i) == '\n') {
            --i; --length;
        }
    }
//    if (length)
//        setStyling(length, comment ? Style::Comment : Style::Default);
}
