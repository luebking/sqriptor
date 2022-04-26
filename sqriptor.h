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

#ifndef SQRIPTOR_H
#define SQRIPTOR_H

#include <QMainWindow>

class QAction;
class QActionGroup;
class QLineEdit;
class QMenu;
class QTabWidget;
class QsciScintilla;

namespace Syntax {
    enum Lexer {
        None = 0, Auto, AVS, AWK, Bash, Batch, CMake, CoffeeScript, CPP, CSS,
        D, Diff, EDIFACT, Fortran77, HTML, JSON, Lua, Makefile,
        Markdown2, Matlab, Pascal, Perl, PO, PostScript, POV, Properties,
        Python, Ruby, Spice, SQL, TCL, TeX, Verilog, VHDL, YAML, 
        CSharp, IDL, Java, JavaScript, Fortran, XML, Octave, Xorg, Journal,
        XorgLog, PkgBuild, NIM, Count
    };
};

namespace Size { enum Mode { Relative = 0, Absolute, Remember }; };

typedef struct Config
{
    Size::Mode sizeMode;
    QSize size;
    QFont font;
    struct wrap {
        bool words;
        bool indicator;
        int indicatorPos;
    } wrap;
    struct tab {
        bool isTab;
        int width;
    } tab;
    struct color {
        QColor  bg, fg, string, character, comment, preproc,
                number, operateur, keyword, error;
    } color;
    bool changed;
    QStringList recentFiles;
} Config;


class Sqriptor : public QMainWindow
{
    Q_OBJECT

public:
    Sqriptor();
    void open(QString fileName, bool forceNewTab = false);
    QsciScintilla *textEdit(int idx = -1) const;
    void newFile(QString name = QString(), bool forceNewTab = false);
    void readStdin();
    typedef Syntax::Lexer Syntax;
    static Config config;

protected:
    void closeEvent(QCloseEvent *event);

signals:
    void copyAvailable(bool);

private slots:
    bool save();
    bool saveAs();
    void toggleBookmark();
    void nextBookmark();
    void prevBookmark();
    void setSyntax(Syntax syntax = Syntax::Auto, QsciScintilla *document = nullptr, bool updateColorsOnly = false);
    int addTab();
    bool closeTab(int idx = -1);
    bool toggleComment();
    void showSettings();
    void checkTimestamp();
    void analyzeSyntaxOnce();
    void copy();

private:
    void createUI();
    void readSettings();
    void writeSettings();
    bool maybeSave(int idx = -1);
    void loadFile(const QString &fileName);
    bool saveFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);
    Syntax syntax(QString suffix) const;
    void indicateCurrentSyntax();
    void indicateCurrentEOL();
    void updateTimestamp(QObject *o);
    void updatePalette();
    void renderIcon(QPixmap &pix);
    void findAll(QString text, bool rx, bool cs, bool wo);
    QString ask4Codec(const QString &codec, const QString &fileName);

    QTabWidget *m_documents;
    QActionGroup *m_syntaxActions;
    QMenu *m_eolMenu;
    QAction *m_tabMenu;
    QAction *m_EolVis, *m_wrapped, *m_folds;
    QLineEdit *m_filterLine;
};

#endif
