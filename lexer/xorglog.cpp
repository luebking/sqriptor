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
#include "lexer/xorglog.h"

namespace Style {
    enum Type {
        Default = 0, Probed, Config, ServerDefault, Commandline, Notice,
                     Info, Warning, Error, Unimplemented, Unknown, Count
    };
};

QsciLexerXorgLog::QsciLexerXorgLog(QObject *parent) : QsciLexerCustom(parent)
{
}

void QsciLexerXorgLog::updateColors()
{
    setPaper(Sqriptor::config.color.bg);
    setColor(Sqriptor::config.color.comment);
    setColor(Sqriptor::config.color.comment, Style::Default);
    setColor(Sqriptor::config.color.string, Style::Config);
    setColor(Sqriptor::config.color.number, Style::Probed);
    setColor(Sqriptor::config.color.preproc, Style::ServerDefault);
    setColor(Sqriptor::config.color.character, Style::Commandline);
    setColor(Sqriptor::config.color.operateur, Style::Notice);
    setColor(Sqriptor::config.color.fg, Style::Info);
    setColor(Sqriptor::config.color.operateur, Style::Warning);
    setColor(Sqriptor::config.color.error, Style::Error);
    setColor(Sqriptor::config.color.error, Style::Unimplemented);
    setColor(Sqriptor::config.color.error, Style::Unknown);
    // keyword
}

static inline void testToken(char byte, int *indicator, const char *lower)
{
    int idx = *indicator;
    if (idx >= 4)
        return;
    if (byte == lower[idx])
        ++(*indicator);
    else
        *indicator = 0;
}

void QsciLexerXorgLog::styleText (int start, int end)
{
    int length = 0;
    int type[Style::Count] = {0};

    QByteArray text = editor()->bytes(start, end);
    startStyling(start);
    for (int i = 0; i < end - start; ++i) {
        ++length;
        if (text.at(i) == '\n') {
            Style::Type style = Style::Default;
            for (int i = 1; i < Style::Count; ++i) {
                if (type[i] >= 4) {
                    if (style != Style::Default) {
                        style = Style::Default;
                        break; //
                    }
                    style = static_cast<Style::Type>(i);
                }
            }
            setStyling(length, style);
            length = 0;
            for (int i = 0; i < Style::Count; ++i) type[i] = 0;
            continue;
        }
        testToken(text.at(i), &type[Style::Probed],         "(--)");
        testToken(text.at(i), &type[Style::Config],         "(**)");
        testToken(text.at(i), &type[Style::ServerDefault],  "(==)");
        testToken(text.at(i), &type[Style::Commandline],    "(++)");
        testToken(text.at(i), &type[Style::Notice],         "(!!)");
        testToken(text.at(i), &type[Style::Info],           "(II)");
        testToken(text.at(i), &type[Style::Warning],        "(WW)");
        testToken(text.at(i), &type[Style::Error],          "(EE)");
        testToken(text.at(i), &type[Style::Unimplemented],  "(NI)");
        testToken(text.at(i), &type[Style::Unknown],        "(??)");
    }
}
