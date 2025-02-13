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
#include <QActionGroup>
#include <QApplication>
#include <QLineEdit>
#include <QMenu>
#include <QDebug>

#include <Qsci/qsciscintilla.h>

#include <Qsci/qscilexer.h>
#include <Qsci/qscilexeravs.h>
#include <Qsci/qscilexerbash.h>
#include <Qsci/qscilexerbatch.h>
#include <Qsci/qscilexercmake.h>
#include <Qsci/qscilexercoffeescript.h>
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
//#include <Qsci/qscilexermarkdown.h>
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

#include "lexer/awk.h"
#include "lexer/cppqt.h"
#include "lexer/fontconfig.h"
#include "lexer/journal.h"
#include "lexer/lisp.h"
#include "lexer/markdown2.h"
#include "lexer/nim.h"
#include "lexer/pkgbuild.h"
#include "lexer/qtpro.h"
#include "lexer/xorg.h"
#include "lexer/xorglog.h"


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
//#include "colors/markdown.cpp"
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

static void setColorsAWK(QsciLexerAWK *lexer) { setColorsBash(lexer); }
static void setColorsFontConfig(QsciLexerFontConfig *lexer) { setColorsHTML(lexer); }
static void setColorsJournal(QsciLexerJournal *lexer) { lexer->updateColors(); }
static void setColorsLISP(QsciLexerLISP *lexer) { lexer->updateColors(); }
static void setColorsMarkdown2(QsciLexerMarkdown2 *lexer) { lexer->updateColors(); }
static void setColorsNIM(QsciLexerNIM *lexer) { setColorsPython(lexer); }
static void setColorsPkgBuild(QsciLexerPkgBuild *lexer) { setColorsBash(lexer); }
static void setColorsQtPro(QsciLexerQtPro *lexer) { setColorsBash(lexer); }
static void setColorsXorg(QsciLexerXorg *lexer) { lexer->updateColors(); }
static void setColorsXorgLog(QsciLexerXorgLog *lexer) { lexer->updateColors(); }

static void resetColors(QsciScintilla *document, Syntax::Lexer syntax) {
    document->setFont(Sqriptor::config.font);
    QColor bg = COLOR_BACKGROUND;
    QColor fg = COLOR_FOREGROUND;
    document->setPaper(bg);
    document->setColor(fg);
    // document->setCaretForegroundColor(fg);
    document->setCaretLineVisible(true);
    document->SendScintilla(QsciScintilla::SCI_SETCARETLINEVISIBLEALWAYS, true);
    document->setMatchedBraceBackgroundColor(bg.value() < fg.value() ? COLOR_OPERATOR.darker(200) : COLOR_OPERATOR.lighter(200));
    document->setMatchedBraceForegroundColor(COLOR_OPERATOR);
    document->setUnmatchedBraceForegroundColor(COLOR_OPERATOR);
    // accentuate background color
    bg = bg.value() < fg.value() ? bg.darker(120) : bg.lighter(120);
    document->setMarginsBackgroundColor(bg);
    document->setCaretLineBackgroundColor(bg);
    document->setFoldMarginColors(bg, bg);
    fg = QColor((bg.red()+fg.red())/2, (bg.green()+fg.green())/2, (bg.green()+fg.green())/2);
    document->setMarginsForegroundColor(fg);
    document->setCaretForegroundColor(fg);
    document->setCaretWidth(qMax(1,qRound(document->physicalDpiX()/96.0f)));
    fg = QColor((2*bg.red()+fg.red())/3, (2*bg.green()+fg.green())/3, (2*bg.green()+fg.green())/3);
    document->setIndentationGuidesBackgroundColor(fg);
    document->setIndentationGuidesForegroundColor(fg);
    document->setEdgeColor(fg);
    document->setWhitespaceForegroundColor(fg);
    QsciScintilla::EdgeMode mode = QsciScintilla::EdgeNone;
    if (Sqriptor::config.wrap.indicator) {
        mode = QsciScintilla::EdgeLine;
        if (syntax == Syntax::Markdown2)
            mode = QsciScintilla::EdgeBackground;
        else if (syntax == Syntax::Fortran77) {
            mode = QsciScintilla::EdgeMultipleLines;
            document->clearEdgeColumns();
            document->addEdgeColumn(6, fg);
            fg = COLOR_ERROR;
            fg = QColor((bg.red()+fg.red())/2, (bg.green()+fg.green())/2, (bg.green()+fg.green())/2);
            document->addEdgeColumn(72, fg);
        }
        if (mode != QsciScintilla::EdgeMultipleLines)
            document->setEdgeColumn(Sqriptor::config.wrap.indicatorPos);
    }
    document->setEdgeMode(mode);
    document->setMarginsFont(Sqriptor::config.font);

    int asc = 0, dsc = 0;
    if (syntax == Syntax::Markdown2 && syntaxDict[syntax]) {
        QFont fnt = syntaxDict[syntax]->font(0);
        QFontMetrics fm(fnt);
        asc = fm.ascent();
        dsc = fm.descent();
        fnt.setPointSize(fnt.pointSize()*1.4);
        fm = QFontMetrics(fnt);
        asc -= fm.ascent();
        asc = 3*asc/2;
        dsc -= fm.descent();
        dsc = 3*dsc/2;
    }
    document->setExtraAscent(asc);
    document->setExtraDescent(dsc);
}

#define TRICK_QSCINTILLA_DETACHLEXER document->setLexer(nullptr); resetColors(document, syntax); document->SendScintilla(QsciScintilla::SCI_STYLECLEARALL);

void Sqriptor::setSyntax(Syntax syntax, QsciScintilla *document, bool updateColorsOnly)
{
    if (!document && !updateColorsOnly)
        document = textEdit();

    if (syntax == Syntax::Auto && !updateColorsOnly) {
        if (!document->lexer()) { // don't use the filename to override an explicit/content detected lexer
            const QString filename = document->property("sqriptor_filename").toString();
            syntax = Sqriptor::syntax(filename);
            if (syntax == Syntax::Properties && filename.contains("xorg.conf"))
                syntax = Syntax::Xorg;
        }
        if (syntax == Syntax::Auto) {
            QString shebang = document->text(0).section("\n",0,0);
            if (shebang.startsWith("#!") || shebang.contains("mime/"))
                syntax = Sqriptor::syntax(shebang);
            else if (shebang.startsWith("diff --git"))
                syntax = Syntax::Diff;
            else if (shebang.startsWith("-- Journal begins at") ||
                        shebang.contains(" kernel: Linux version ") ||
                        shebang.contains(" kernel: microcode: updated early: "))
                syntax = Syntax::Journal;
            else if (shebang.startsWith("<?xml")) {
                syntax = Syntax::XML;
                if (shebang.contains("<!DOCTYPE fontconfig")) {
                    syntax = Syntax::FontConfig;
                } else {
                    shebang = document->text(1).section("\n",0,0);
                    if (shebang.startsWith("<!DOCTYPE fontconfig"))
                        syntax = Syntax::FontConfig;
                }
            } else if (shebang.contains("/var/log/Xorg.")) {
                syntax = Syntax::XorgLog;
            }
        }
        if (syntax == Syntax::Auto) {
            QString shebang = document->text(1).section("\n",0,0);
            if (shebang.contains("X.Org X Server"))
                syntax = Syntax::XorgLog;
        }
        if (syntax == Syntax::Auto)
            return;
    }

    if ((syntaxDict[syntax] || syntax == Syntax::None) && !updateColorsOnly) {
        QsciLexer *oldLexer = document->lexer();
        TRICK_QSCINTILLA_DETACHLEXER
        document->setLexer(syntaxDict[syntax]);
        document->setProperty("sqriptor_syntax", syntax);
        if (!syntaxDict[syntax] && oldLexer) {
            document->SendScintilla(QsciScintilla::SCI_STYLESETFORE, QsciScintilla::STYLE_DEFAULT, COLOR_FOREGROUND);
            document->SendScintilla(QsciScintilla::SCI_STYLECLEARALL);
        }
        resetColors(document, syntax);
        if (document == textEdit())
            indicateCurrentSyntax();
        return;
    }

#define MAKE_LEXER_2(_TYPE_,_CLASS_) case Syntax::_TYPE_ : \
            if (!hook) syntaxDict[syntax] = new QsciLexer##_CLASS_(this); \
            syntaxDict[syntax]->setDefaultFont(config.font);\
            syntaxDict[syntax]->setFont(config.font);\
            syntaxDict[syntax]->setDefaultPaper(COLOR_BACKGROUND);\
            syntaxDict[syntax]->setPaper(COLOR_BACKGROUND);\
            syntaxDict[syntax]->setDefaultColor(COLOR_FOREGROUND);\
            syntaxDict[syntax]->setColor(COLOR_FOREGROUND);\
            setColors##_TYPE_(static_cast<QsciLexer##_TYPE_ * >(syntaxDict[syntax])); \
            break;

#define MAKE_LEXER(_TYPE_) MAKE_LEXER_2(_TYPE_,_TYPE_)
    
    QsciLexer *hook = syntaxDict[syntax];
    if (updateColorsOnly && !hook)
        return; // don't pre-fab lexers...
    switch (syntax) {
        case Syntax::None:
            break;
        MAKE_LEXER(AVS)
        MAKE_LEXER(AWK)
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
        MAKE_LEXER_2(CPP,CPPQt)
        MAKE_LEXER(CSS)
        MAKE_LEXER(D)
        MAKE_LEXER(Diff)
        MAKE_LEXER(EDIFACT)
        MAKE_LEXER(FontConfig)

        case Syntax::Fortran:
            if (!hook) hook = syntaxDict[syntax] = new QsciLexerFortran(this);
            [[fallthrough]];
        MAKE_LEXER(Fortran77)
        
        case Syntax::XML:
            if (!hook) hook = syntaxDict[syntax] = new QsciLexerHTML(this);
            [[fallthrough]];
            MAKE_LEXER(HTML)

        MAKE_LEXER(Journal)
        MAKE_LEXER(JSON)
        MAKE_LEXER(LISP)
        MAKE_LEXER(Lua)
        MAKE_LEXER(Makefile)
        MAKE_LEXER(Markdown2)
            
        MAKE_LEXER(NIM)

        case Syntax::Octave:
            if (!hook) hook = syntaxDict[syntax] = new QsciLexerMatlab(this);
            [[fallthrough]];
        MAKE_LEXER(Matlab)
            
        MAKE_LEXER(Pascal)
        MAKE_LEXER(Perl)
        MAKE_LEXER(PkgBuild)
        MAKE_LEXER(PO)
        MAKE_LEXER(PostScript)
        MAKE_LEXER(POV)
        MAKE_LEXER(Properties)
        MAKE_LEXER(Python)
        MAKE_LEXER(QtPro)
        MAKE_LEXER(Ruby)
        MAKE_LEXER(Spice)
        MAKE_LEXER(SQL)
        MAKE_LEXER(TCL)
        MAKE_LEXER(TeX)
        MAKE_LEXER(Verilog)
        MAKE_LEXER(VHDL)
        MAKE_LEXER(Xorg)
        MAKE_LEXER(XorgLog)
        MAKE_LEXER(YAML)
            break;
        default:
            break;
    }
    if (syntax == Syntax::YAML)
        document->setIndentationsUseTabs(false); // they're illegal
    if (updateColorsOnly)
        return; // we're not touching the document here at all
    TRICK_QSCINTILLA_DETACHLEXER
    document->setLexer(syntaxDict[syntax]);
    document->setProperty("sqriptor_syntax", syntax);
    if (document == textEdit()) {
        indicateCurrentSyntax();
        m_filterLine->setText(m_filterLine->text());
    }
    resetColors(document, syntax);
}

void Sqriptor::indicateCurrentSyntax()
{
    const QsciLexer *lexer = textEdit()->lexer();
    QString name = "None";
    if (lexer) {
        name = lexer->metaObject()->className();
        name.remove("QsciLexer");
    }
    foreach (QAction *act, m_syntaxActions->actions()) {
        if (act->data().toString() == name) {
            act->setChecked(true);
            return;
        }
    }
}

// php: # = //, /**/ - todo: the html lexer does php, but we need different brace treatment
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
    bool isHtml = false;
    if (name == "CPPQt" || name == "CSharp" || name == "Java" ||  name == "CSS" ||
        name == "JavaScript" || name == "D" || // D also supports /++/
        (isHtml = (name == "HTML")) || name == "XML" || name == "Pascal" || name == "FontConfig") {
        if (isHtml) {
//            QString suffix = doc->property("sqriptor_filename").toString().section('.', -1);
//            isHtml = !(suffix.startsWith("php") || suffix == "phtml");
            // And what do we do with hybrids that are mostly html with some php code inside?
        }
        QString head = "/*", tail = "*/";
        if (isHtml || name == "XML" || name == "FontConfig") {
            head = "<!--"; tail = "-->";
        } else if (name == "Pascal") {
            head = "{"; tail = "}";
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

        if (isHtml || name == "CSS" || name == "XML" || name == "FontConfig") {
            // only supports /**/, <!-- --> syntax
            doc->insertAt(tail, line, text.length()-1);
            doc->insertAt(head, line, 0);
            return true;
        }

        // Not "pascal", but delphi supports this style
        if (QStringView(text).mid(index, 2).toString() == "//") {
            doc->setSelection(line, index, line, index + 2);
            doc->removeSelectedText();
        } else if (QStringView(text).mid(0, 2).toString() == "//") {
            doc->setSelection(line, 0, line, 2);
            doc->removeSelectedText();
            doc->setCursorPosition(line, index - 2);
        } else {
            doc->insertAt("//", line, 0);
        }
        return true;
    }

    if (name == "Bash" || name == "NIM" || name == "Python" || name == "Ruby" || name == "Perl" ||
        name == "Makefile" || name == "CMake" || name.startsWith("Fortran") || name.contains("LISP") ||
        name == "TCL" || name == "AWK" || name == "Xorg" || name == "PkgBuild" || name == "YAML" ||
        name == "Properties" || name == "QtPro") {
        // tcltk might or not require ";#" inline
        const QChar bang = name.startsWith("Fortran") ? '!' : name.contains("LISP") ? ';' : '#';
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
        if (QStringView(text).mid(index, 1).toString() == bang) {
            doc->setSelection(line, index, line, index + 1);
            doc->removeSelectedText();
        } else if (QStringView(text).mid(0, 1).toString() == bang) {
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
