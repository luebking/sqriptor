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

#include <QAction>
#include <QApplication>
#include <QMenu>
#include <QDebug>

#include <Qsci/qsciscintilla.h>

#include <Qsci/qscilexer.h>
#include <Qsci/qscilexeravs.h>
#include <Qsci/qscilexerbash.h>
#include <Qsci/qscilexerbatch.h>
#include <Qsci/qscilexercmake.h>
#include <Qsci/qscilexercoffeescript.h>
#include <Qsci/qscilexercpp.h>
#include <Qsci/qscilexercsharp.h>
#include <Qsci/qscilexercss.h>
#include <Qsci/qscilexercustom.h>
#include <Qsci/qscilexerd.h>
#include <Qsci/qscilexerdiff.h>
#include <Qsci/qscilexeredifact.h>
#include <Qsci/qscilexerfortran.h>
#include <Qsci/qscilexerfortran77.h>
#include <Qsci/qscilexerhtml.h>
#include <Qsci/qscilexeridl.h>
#include <Qsci/qscilexerjava.h>
#include <Qsci/qscilexerjavascript.h>
#include <Qsci/qscilexerjson.h>
#include <Qsci/qscilexerlua.h>
#include <Qsci/qscilexermakefile.h>
#include <Qsci/qscilexermarkdown.h>
#include <Qsci/qscilexermatlab.h>
#include <Qsci/qscilexeroctave.h>
#include <Qsci/qscilexerpascal.h>
#include <Qsci/qscilexerperl.h>
#include <Qsci/qscilexerpo.h>
#include <Qsci/qscilexerpostscript.h>
#include <Qsci/qscilexerpov.h>
#include <Qsci/qscilexerproperties.h>
#include <Qsci/qscilexerpython.h>
#include <Qsci/qscilexerruby.h>
#include <Qsci/qscilexerspice.h>
#include <Qsci/qscilexersql.h>
#include <Qsci/qscilexertcl.h>
#include <Qsci/qscilexertex.h>
#include <Qsci/qscilexerverilog.h>
#include <Qsci/qscilexervhdl.h>
#include <Qsci/qscilexerxml.h>
#include <Qsci/qscilexeryaml.h>


#include "sqriptor.h"

static QsciLexer *syntaxDict[Syntax::Count] = {nullptr};

#define COLOR_BACKGROUND Sqriptor::config.color.bg
#define COLOR_FOREGROUND Sqriptor::config.color.fg
#define COLOR_STRING Sqriptor::config.color.string
#define COLOR_CHAR Sqriptor::config.color.character
#define COLOR_COMMENT Sqriptor::config.color.comment
#define COLOR_PREPROC Sqriptor::config.color.preproc
#define COLOR_NUMBER Sqriptor::config.color.number
#define COLOR_OPERATOR Sqriptor::config.color.operateur
#define COLOR_KEYWORD Sqriptor::config.color.keyword
#define COLOR_ERROR Sqriptor::config.color.error

#include "colors/avs.cpp"
#include "colors/bash.cpp"
#include "colors/batch.cpp"
#include "colors/cmake.cpp"
#include "colors/coffeescript.cpp"
#include "colors/cpp.cpp"
#include "colors/css.cpp"
#include "colors/d.cpp"
#include "colors/diff.cpp"
#include "colors/edifact.cpp"
#include "colors/fortran77.cpp"
#include "colors/html.cpp"
#include "colors/json.cpp"
#include "colors/lua.cpp"
#include "colors/makefile.cpp"
#include "colors/markdown.cpp"
#include "colors/matlab.cpp"
#include "colors/pascal.cpp"
#include "colors/perl.cpp"
#include "colors/po.cpp"
#include "colors/postscript.cpp"
#include "colors/pov.cpp"
#include "colors/properties.cpp"
#include "colors/python.cpp"
#include "colors/ruby.cpp"
#include "colors/spice.cpp"
#include "colors/sql.cpp"
#include "colors/tcl.cpp"
#include "colors/tex.cpp"
#include "colors/verilog.cpp"
#include "colors/vhdl.cpp"
#include "colors/yaml.cpp"


static void resetColors(QsciScintilla *document) {
    QColor bg = COLOR_BACKGROUND;
    QColor fg = COLOR_FOREGROUND;
    document->setPaper(bg);
    document->setColor(fg);
    document->setCaretForegroundColor(fg);
    document->setCaretLineVisible(true);
    document->setMatchedBraceBackgroundColor(COLOR_OPERATOR);
    document->setMatchedBraceForegroundColor(bg);
    // accentuate background color
    bg = bg.value() < fg.value() ? bg.darker(120) : bg.lighter(120);
    document->setMarginsBackgroundColor(bg);
    fg = QColor((bg.red()+fg.red())/2, (bg.green()+fg.green())/2, (bg.green()+fg.green())/2);
    document->setMarginsForegroundColor(fg);
    document->setCaretLineBackgroundColor(bg);
}

void Sqriptor::setSyntax(Syntax syntax, QsciScintilla *document, bool updateColorsOnly)
{
    if (!document && !updateColorsOnly)
        document = textEdit();

    if (syntax == Syntax::Auto && !updateColorsOnly) {
        syntax = Sqriptor::syntax(document->property("sqriptor_filename").toString());
        if (syntax == Syntax::Auto) {
            QString shebang = document->text(0).section("\n",0,0);
            if (shebang.startsWith("#!"))
                syntax = Sqriptor::syntax(shebang);
        }
        if (syntax == Syntax::Auto)
            return;
    }

    if (syntaxDict[syntax] && !updateColorsOnly) {
        document->setLexer(syntaxDict[syntax]);
        document->setProperty("sqriptor_syntax", syntax);
        resetColors(document);
        return;
    }

#define MAKE_LEXER(_TYPE_) case Syntax::_TYPE_ : \
            if (!hook) syntaxDict[syntax] = new QsciLexer##_TYPE_(this); \
            syntaxDict[syntax]->setFont(QFont("monospace", 9));\
            syntaxDict[syntax]->setDefaultPaper(COLOR_BACKGROUND);\
            syntaxDict[syntax]->setPaper(COLOR_BACKGROUND);\
            syntaxDict[syntax]->setDefaultColor(COLOR_FOREGROUND);\
            syntaxDict[syntax]->setColor(COLOR_FOREGROUND);\
            setColors##_TYPE_(static_cast<QsciLexer##_TYPE_ * >(syntaxDict[syntax])); \
            break;
    
    QsciLexer *hook = syntaxDict[syntax];
    switch (syntax) {
        case Syntax::None:
            document->setFont(config.font);
            break;
        MAKE_LEXER(AVS)
        MAKE_LEXER(Bash)
        MAKE_LEXER(Batch)
        MAKE_LEXER(CMake)
        MAKE_LEXER(CoffeeScript)

        case Syntax::CSharp:
            if (!hook) hook = syntaxDict[syntax] = new QsciLexerCSharp(this);
            [[fallthrough]];
        case Syntax::IDL:
            if (!hook) hook = syntaxDict[syntax] = new QsciLexerIDL(this);
            [[fallthrough]];
        case Syntax::Java:
            if (!hook) hook = syntaxDict[syntax] = new QsciLexerJava(this);
            [[fallthrough]];
        case Syntax::JavaScript:
            if (!hook) hook = syntaxDict[syntax] = new QsciLexerJavaScript(this);
            [[fallthrough]];
        MAKE_LEXER(CPP)
        MAKE_LEXER(CSS)
        MAKE_LEXER(D)
        MAKE_LEXER(Diff)
        MAKE_LEXER(EDIFACT)

        case Syntax::Fortran:
            if (!hook) hook = syntaxDict[syntax] = new QsciLexerFortran(this);
            [[fallthrough]];
        MAKE_LEXER(Fortran77)
        
        case Syntax::XML:
            if (!hook) hook = syntaxDict[syntax] = new QsciLexerHTML(this);
            [[fallthrough]];
            MAKE_LEXER(HTML)

        MAKE_LEXER(JSON)
        MAKE_LEXER(Lua)
        MAKE_LEXER(Makefile)
        MAKE_LEXER(Markdown)

        case Syntax::Octave:
            if (!hook) hook = syntaxDict[syntax] = new QsciLexerMatlab(this);
            [[fallthrough]];
        MAKE_LEXER(Matlab)
            
        MAKE_LEXER(Pascal)
        MAKE_LEXER(Perl)
        MAKE_LEXER(PO)
        MAKE_LEXER(PostScript)
        MAKE_LEXER(POV)
        MAKE_LEXER(Properties)
        MAKE_LEXER(Python)
        MAKE_LEXER(Ruby)
        MAKE_LEXER(Spice)
        MAKE_LEXER(SQL)
        MAKE_LEXER(TCL)
        MAKE_LEXER(TeX)
        MAKE_LEXER(Verilog)
        MAKE_LEXER(VHDL)
        MAKE_LEXER(YAML)
            break;
        default:
            break;
    }
    if (updateColorsOnly)
        return; // we're not touching the document here at all
    document->setLexer(syntaxDict[syntax]);
    document->setProperty("sqriptor_syntax", syntax);
    if (document == textEdit())
        indicateCurrentSyntax();
    resetColors(document);
}

void Sqriptor::indicateCurrentSyntax()
{
    const QsciLexer *lexer = textEdit()->lexer();
    QString name = "None";
    if (lexer) {
        name = lexer->metaObject()->className();
        name.remove("QsciLexer");
    }
    foreach (QAction *act, m_syntaxMenu->actions()) {
        if (act->text() == name) {
            act->setChecked(true);
            return;
        }
    }
}

// php: # = //, /**/
// cmake: # and #[[ ]]
// pascal: //, {}, (**) - lol… boobies

bool Sqriptor::toggleComment()
{
    QsciScintilla *doc = textEdit();
    const QsciLexer *lexer = doc->lexer();
    if (!lexer)
        return false;
    QString name = lexer->metaObject()->className();
    name.remove("QsciLexer");
    if (name == "CPP" || name == "CSharp" || name == "Java" ||  name == "CSS" ||
        name == "JavaScript" || name == "D" || name == "HTML" || name == "XML") { // D also supports /++/
        QString head = "/*", tail = "*/";
        if (name == "HTML" || name == "XML") {
            head = "<!--"; tail = "-->";
        }
#define COMMENT_SEGMENT head.length(), text.length() - (head.length() + tail.length())
        QString text = doc->selectedText();
        if (!text.isEmpty()) {
            if (text.startsWith(head) && text.endsWith(tail))
                doc->replaceSelectedText(text.mid(COMMENT_SEGMENT).trimmed());
            else
                doc->replaceSelectedText(head + " " + text + " " + tail);
            return true;
        }
        int line, index;
        doc->getCursorPosition(&line, &index);
        text = doc->text(line);
        int begin, end;
        if ((begin = text.lastIndexOf(head, index)) > -1 && (end = text.indexOf(tail, index)) > -1) {
            doc->setSelection(line, begin, line, end + tail.length());
            text = doc->selectedText();
            text = text.mid(COMMENT_SEGMENT);
            if (begin > 0) // clumsy line comment, only option for css, html & xml
                text = text.trimmed(); // unconditionally, we'd likely nuke indention
            doc->replaceSelectedText(text);
            return true;
        }

        if (name == "CSS" || name == "HTML" || name == "XML") {
            // only supports /**/, <!-- --> syntax
            doc->insertAt(tail, line, text.length()-1);
            doc->insertAt(head, line, 0);
            return true;
        }

        if (text.midRef(index, 2) == "//") {
            doc->setSelection(line, index, line, index + 2);
            doc->removeSelectedText();
        } else if (text.midRef(0, 2) == "//") {
            doc->setSelection(line, 0, line, 2);
            doc->removeSelectedText();
            doc->setCursorPosition(line, index - 2);
        } else {
            doc->insertAt("//", line, 0);
        }
        return true;
    }

    if (name == "Bash" || name == "Python" || name == "Ruby" || name == "Perl" || 
        name == "Makefile" || name == "CMake" || name.startsWith("Fortran") ||
        name == "TCL") { // tcltk might or not require ";#" inline
        const QChar bang = name.startsWith("Fortran") ? '!' : '#';
        QString text = doc->selectedText();
        if (!text.isEmpty()) {
            if (text.startsWith(bang))
                doc->replaceSelectedText(text.mid(1,text.length()-1));
            else
                doc->replaceSelectedText(bang + text);
            return true;
        }
        int line, index;
        doc->getCursorPosition(&line, &index);
        text = doc->text(line);
        int begin = text.lastIndexOf(bang, index >= text.size() ? -1 : index);
        if (begin > 2) // full line has better cursor management
            index = begin; // caught next line
        if (text.midRef(index, 1) == bang) {
            doc->setSelection(line, index, line, index + 1);
            doc->removeSelectedText();
        } else if (text.midRef(0, 1) == bang) {
            doc->setSelection(line, 0, line, 1);
            doc->removeSelectedText();
            doc->setCursorPosition(line, index - 1);
        } else {
            doc->insertAt(bang, line, 0);
        }
        return true;
    }

    return false;
}
