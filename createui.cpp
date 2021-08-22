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
#include <QFileDialog>
#include <QLineEdit>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QScrollBar>
#include <QSpinBox>
#include <QTextEdit>
#include <QToolButton>
#include <QHBoxLayout>

#include <QDebug>

#include <Qsci/qsciscintilla.h>
#include <Qsci/qscicommandset.h>
#include <Qsci/qscicommand.h>

#include "sqriptor.h"

#define ADD_ACT menu->addAction(act); addAction(act);

void Sqriptor::createUI()
{
    QMenu *menu;
    QAction *act;

    menu = menuBar()->addMenu("(·)");
    m_tabMenu = menu->menuAction();
    m_tabMenu->setVisible(false);
    connect(menu, &QMenu::aboutToShow, [=]() {
        menu->clear();
        QAction *a;
        for (int i = 0; i < m_documents->count(); ++i) {
            a = new QAction(m_documents->tabText(i), menu);
            connect(a, &QAction::triggered, [=](){
                m_documents->setCurrentIndex(i);
            });
            menu->addAction(a);
        }
    });
    
    // FILE
    menu = menuBar()->addMenu(tr("&File"));

#define CREATE_EOL(_TYPE_) act->setCheckable(true); \
    act->setData(QsciScintilla::_TYPE_); \
    eolGroup->addAction(act);\
    connect(act, &QAction::triggered, [=](){ \
        if (textEdit()->eolMode() != QsciScintilla::_TYPE_) { \
            textEdit()->convertEols(QsciScintilla::_TYPE_); \
            textEdit()->setEolMode(QsciScintilla::_TYPE_); \
        } \
    }); \
    m_eolMenu->addAction(act)

    m_eolMenu = menu->addMenu(tr("&EOL"));
    QActionGroup *eolGroup = new QActionGroup(m_eolMenu);
    act = new QAction(tr("LF (\\n) \t The sane choice"), m_eolMenu);
    CREATE_EOL(EolUnix);

    act = new QAction(tr("CR (\\r) \t Woz didn't know better"), m_eolMenu);
    CREATE_EOL(EolMac);

    act = new QAction(tr("CRLF (\\r\\n) \t The Windows abomination"), m_eolMenu);
    CREATE_EOL(EolWindows);

    menu->addSeparator();

    act = new QAction(tr("&New"), this);
    act->setShortcut(tr("Ctrl+N"));
    connect(act, &QAction::triggered, [=](){ newFile(); });
    ADD_ACT

    act = new QAction(tr("&Open..."), this);
    act->setShortcut(tr("Ctrl+O"));
    connect(act, &QAction::triggered, [=](){
        QString path = textEdit()->property("sqriptor_filename").toString();
        path = QFileInfo(path).absolutePath();
        foreach (QString name, QFileDialog::getOpenFileNames(this, QString(), path))
            open(name);
    });
    ADD_ACT
    
    act = new QAction(tr("&Close..."), this);
    act->setShortcut(tr("Ctrl+W"));
    connect(act, SIGNAL(triggered()), SLOT(closeTab()));
    ADD_ACT

    act = new QAction(tr("&Save"), this);
    act->setShortcut(tr("Ctrl+S"));
    connect(act, SIGNAL(triggered()), SLOT(save()));
    ADD_ACT

    act = new QAction(tr("Save &As..."), this);
    connect(act, SIGNAL(triggered()), SLOT(saveAs()));
    ADD_ACT

    menu->addSeparator();
    
    act = new QAction(tr("E&xit"), this);
    act->setShortcut(tr("Ctrl+Q"));
    connect(act, SIGNAL(triggered()), SLOT(close()));
    ADD_ACT

    // EDIT
    menu = menuBar()->addMenu(tr("&Edit"));

    act = new QAction(tr("Cu&t"), this);
    act->setShortcut(tr("Ctrl+X"));
    connect(act, &QAction::triggered, [=](){textEdit()->cut();});
    connect(this, SIGNAL(copyAvailable(bool)), act, SLOT(setEnabled(bool)));
    act->setEnabled(false);
    ADD_ACT

    act = new QAction(tr("&Copy"), this);
    act->setShortcut(tr("Ctrl+C"));
    connect(act, &QAction::triggered, [=](){textEdit()->copy();});
    connect(this, SIGNAL(copyAvailable(bool)), act, SLOT(setEnabled(bool)));
    act->setEnabled(false);
    ADD_ACT

    act = new QAction(tr("&Paste"), this);
    act->setShortcut(tr("Ctrl+V"));
    connect(act, &QAction::triggered, [=](){textEdit()->paste();});
    ADD_ACT

    menuBar()->addSeparator();
    
    QWidget *searchBar = new QWidget(menuBar());
    QToolButton *btn = new QToolButton(searchBar);
    btn->setPopupMode(QToolButton::InstantPopup);
    btn->setText(tr("Find:"));
    QMenu *tbmenu = new QMenu(btn);
    QAction *searchRegExp = new QAction(tr("Regular Expression"), searchBar);
    searchRegExp->setCheckable(true);
    tbmenu->addAction(searchRegExp);
    QAction *searchCaseSens = new QAction(tr("Case Sensitive"), searchBar);
    searchCaseSens->setCheckable(true);
    tbmenu->addAction(searchCaseSens);
    QAction *searchWord = new QAction(tr("Find Words Only"), searchBar);
    searchWord->setCheckable(true);
    tbmenu->addAction(searchWord);
    QAction *searchForward = new QAction(tr("Forward"), searchBar);
    searchForward->setCheckable(true);
    searchForward->setChecked(true);
    tbmenu->addAction(searchForward);
    tbmenu->addSeparator();
//    QAction *filterFind = new QAction(tr("Find all"), searchBar);
//    tbmenu->addAction(filterFind);
    QAction *replaceAll = new QAction(tr("Replace all"), searchBar);
    tbmenu->addAction(replaceAll);
    replaceAll->setVisible(false);
    btn->setMenu(tbmenu);
    
    static bool menuWasVisible = true;
    act = new QAction(searchBar);
    act->setShortcut(Qt::Key_Escape);
    act->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    connect(act, &QAction::triggered, [=]() {
        textEdit()->setFocus();
        if (!menuWasVisible)
            menuBar()->hide();
    });
    searchBar->addAction(act);
    
    QLineEdit *findLine = new QLineEdit(searchBar);
    auto l_search = [=](bool jump) {
        int line, index, dummy;
        jump = jump ? searchForward->isChecked() : !searchForward->isChecked();
        if (jump)
            textEdit()->getSelection(&line, &index, &dummy, &dummy);
        else
            textEdit()->getSelection(&dummy, &dummy, &line, &index);
        bool found = textEdit()->findFirst( findLine->text(),
                                            searchRegExp->isChecked(),
                                            searchCaseSens->isChecked(),
                                            searchWord->isChecked(),
                                            true,
                                            searchForward->isChecked(),
                                            line, index);
        if (found || findLine->text().isEmpty()) {
            findLine->setPalette(QPalette());
        } else {
            QPalette pal = findLine->palette();
            pal.setColor(findLine->foregroundRole(), config.color.error);
            findLine->setPalette(pal);
        }
    };

    connect(findLine, &QLineEdit::returnPressed, [=]() { l_search(false); });
    connect(findLine, &QLineEdit::textEdited, [=]() { l_search(true); });

    QLineEdit *replaceLine = new QLineEdit(searchBar);
    connect(replaceLine, &QLineEdit::returnPressed, [=]() {
        if (textEdit()->selectedText().isEmpty())
            textEdit()->cancelFind(); // user clicked forward, voids the replace, we search on
        textEdit()->replace(replaceLine->text());
        l_search(false);
    });
    
    connect(replaceAll, &QAction::triggered, [=]() {
        const QString text = findLine->text();
        if (text.isEmpty())
            return;
        const bool re = searchRegExp->isChecked();
        const bool cs = searchCaseSens->isChecked();
        const bool wo = searchWord->isChecked();
        int la, ia, lb, ib;
        textEdit()->getSelection(&la, &ia, &lb, &ib);
        if (ib != ia) {
            // if the user selected more than one line, that's explicit and
            // we take it as hint to limit the replace to that area
            // in-line selections happen for the find, but what's worse
            // is that we need to restore the selection after every find/replace
            // and the originally selected text can shrink due to that, eg.
            // |111|111, s/11/2/g;
            // |111|111 => |211|11 => |221|1
            // Chances of the user planning sth. like that are much lower if
            // s/he selected multiple lines
            while (textEdit()->findFirstInSelection(text, re, cs, wo)) {
                textEdit()->replace(replaceLine->text());
                textEdit()->setSelection(la, ia, lb, ib);
            }
        } else {
            while (textEdit()->findFirst(text, re, cs, wo, true))
                textEdit()->replace(replaceLine->text());
        }
    });

    QSpinBox *gotoLine = new QSpinBox(searchBar);
    connect(qApp, &QApplication::focusChanged, [=]() {
        if (gotoLine->hasFocus())
            gotoLine->setRange(1, textEdit()->lines());
    });
    connect(gotoLine, QOverload<int>::of(&QSpinBox::valueChanged), [=](int i) {
        textEdit()->setCursorPosition(i-1, 0); // humans start counting at 0...
        QScrollBar *sb;
        if ((sb = textEdit()->verticalScrollBar())) {
            sb->setValue(i-1+(sb->maximum()-textEdit()->lines())/2);
        }
    });
    connect(gotoLine, &QSpinBox::editingFinished, [=]() {
        textEdit()->setFocus();
    });
    
    QHBoxLayout *layout = new QHBoxLayout(searchBar);
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(gotoLine);
    layout->addWidget(btn);
    layout->addWidget(findLine);
    layout->addWidget(replaceLine);
    gotoLine->hide();
    replaceLine->hide();

    menuBar()->setCornerWidget(searchBar);
    
    menu->addSeparator();
    
#define HIDE_STUFF searchBar->hide(); gotoLine->hide(); findLine->hide(); replaceLine->hide(); btn->hide(); replaceAll->setVisible(false);
#define SHOW_STUFF menuWasVisible = menuBar()->isVisible(); searchBar->show(); menuBar()->show();
    
    act = new QAction(tr("&Find"), this);
    act->setShortcut(tr("Ctrl+F"));
    connect(act, &QAction::triggered, [=]() {
        HIDE_STUFF
        btn->setText(tr("Find:"));
        btn->show();
        findLine->show();
        QString text = textEdit()->selectedText();
        if (!text.isEmpty())
            findLine->setText(text);
        findLine->setFocus();
        findLine->selectAll();
        SHOW_STUFF
    });
    ADD_ACT
    
    act = new QAction(tr("&Replace"), this);
    act->setShortcut(tr("Ctrl+R"));
    connect(act, &QAction::triggered, [=]() {
        HIDE_STUFF
        btn->setText(tr("Replace:"));
        btn->show();
        findLine->show();
        replaceLine->show();
        replaceAll->setVisible(true);
        textEdit()->cancelFind(); // clear previous finds to no replace them
        QString text = textEdit()->selectedText();
        if (!text.isEmpty()) {
            findLine->setText(text);
            l_search(true); // initialize find results
        }
        findLine->setFocus();
        findLine->selectAll();
        SHOW_STUFF
    });
    ADD_ACT
    
    act = new QAction(tr("Find &Next"), this);
    act->setShortcut(tr("F3"));
    connect(act, &QAction::triggered, [=]() {
        textEdit()->findFirst(  findLine->text(),
                                searchRegExp->isChecked(),
                                searchCaseSens->isChecked(),
                                searchWord->isChecked(),
                                true,
                                true /*forward*/);
    });
    menu->addAction(act);

    act = new QAction(tr("Find &Prev"), this);
    act->setShortcut(tr("Ctrl+F3"));
    connect(act, &QAction::triggered, [=]() {
        int line, index, dummy;
        textEdit()->getSelection(&line, &index, &dummy, &dummy);
        textEdit()->findFirst(  findLine->text(),
                                searchRegExp->isChecked(),
                                searchCaseSens->isChecked(),
                                searchWord->isChecked(),
                                true,
                                false /*backward*/,
                                line, index);
    });
    menu->addAction(act);

    act = new QAction(tr("&Goto Line"), this);
    act->setShortcut(tr("Ctrl+G"));
    connect(act, &QAction::triggered, [=]() {
        HIDE_STUFF
        gotoLine->show();
        gotoLine->setFocus();
        gotoLine->selectAll();
        SHOW_STUFF
    });
    ADD_ACT
    
    menu->addSeparator();
    
    act = new QAction(tr("Toggle &Bookmark"), this);
    act->setShortcut(tr("Ctrl+B"));
    connect(act, SIGNAL(triggered()), SLOT(toggleBookmark()));
    ADD_ACT
    
    act = new QAction(tr("Goto &Next Bookmark"), this);
    act->setShortcut(tr("Alt+PgDown"));
    connect(act, SIGNAL(triggered()), SLOT(nextBookmark()));
    ADD_ACT
    
    act = new QAction(tr("Goto &Previous Bookmark"), this);
    act->setShortcut(tr("Alt+PgUp"));
    connect(act, SIGNAL(triggered()), SLOT(prevBookmark()));
    ADD_ACT
    
    menu->addSeparator();
    
    act = new QAction(tr("Toggle &Comment"), this);
    act->setShortcut(tr("Ctrl+D"));
    connect(act, SIGNAL(triggered()), SLOT(toggleComment()));
    ADD_ACT
    
    menu = menuBar()->addMenu(tr("&View"));
    act = new QAction(tr("&Wrap text"), this);
    act->setCheckable(true);
    connect(act, &QAction::triggered, [=](bool checked){
        textEdit()->setWrapMode(checked ? QsciScintilla::WrapWhitespace : QsciScintilla::WrapNone);
        });
    menu->addAction(act);

    act = new QAction(tr("&EOL"), this);
    act->setCheckable(true);
    connect(act, &QAction::triggered, [=](bool checked){
        textEdit()->setEolVisibility(checked);
        });
    menu->addAction(act);

//    void 	setEolMode (EolMode mode)

    act = new QAction(tr("Show &Menu"), this);
    act->setCheckable(true);
    act->setChecked(true);
    connect(act, &QAction::triggered, [=](bool checked){
        menuBar()->QWidget::setVisible(checked);
        });
    act->setShortcut(tr("Ctrl+M"));
    ADD_ACT

    menu->addSeparator();

    act = new QAction(tr("&Toggle 0x9-Indentation"), this);
    act->setShortcut(tr("Alt+T"));
    connect(act, &QAction::triggered, [=](){
        textEdit()->setIndentationsUseTabs(!textEdit()->indentationsUseTabs());
    });
    ADD_ACT

    menu->addSeparator();

    act = new QAction(tr("&Settings..."), this);
    connect(act, SIGNAL(triggered()), SLOT(showSettings()));
    menu->addAction(act);
    
    menu = m_syntaxMenu = menuBar()->addMenu(tr("&Syntax"));
    QActionGroup *group = new QActionGroup(this);
#define ADD_SYNTAX(_SYN_) act = group->addAction(menu->addAction(#_SYN_, [=](){setSyntax(Syntax::_SYN_);}));\
        act->setCheckable(true); act->setData("QsciLexer"#_SYN_)
    menu->addAction(tr("Auto"), [=](){setSyntax(Syntax::Auto);});
    menu->addSeparator();
    group->addAction(menu->addAction(tr("None"), [=](){setSyntax(Syntax::None);}))->setCheckable(true);
    menu->addSeparator();
    ADD_SYNTAX(AVS); ADD_SYNTAX(Bash); ADD_SYNTAX(Batch); ADD_SYNTAX(CMake);
    ADD_SYNTAX(CoffeeScript); ADD_SYNTAX(CPP); ADD_SYNTAX(CSharp); ADD_SYNTAX(CSS);
    ADD_SYNTAX(D); ADD_SYNTAX(Diff); ADD_SYNTAX(EDIFACT); ADD_SYNTAX(Fortran);
    ADD_SYNTAX(Fortran77); ADD_SYNTAX(HTML); ADD_SYNTAX(IDL); ADD_SYNTAX(Java);
    ADD_SYNTAX(JavaScript); ADD_SYNTAX(JSON); ADD_SYNTAX(Lua); ADD_SYNTAX(Makefile);
    ADD_SYNTAX(Markdown); ADD_SYNTAX(Matlab); ADD_SYNTAX(Octave); ADD_SYNTAX(Pascal);
    ADD_SYNTAX(Perl); ADD_SYNTAX(PO); ADD_SYNTAX(PostScript); ADD_SYNTAX(POV);
    ADD_SYNTAX(Properties); ADD_SYNTAX(Python); ADD_SYNTAX(Ruby); ADD_SYNTAX(Spice);
    ADD_SYNTAX(SQL); ADD_SYNTAX(TCL); ADD_SYNTAX(TeX); ADD_SYNTAX(Verilog);
    ADD_SYNTAX(VHDL); ADD_SYNTAX(XML); ADD_SYNTAX(YAML);

    menuBar()->addSeparator();

    menu = menuBar()->addMenu(tr("&Help"));

    act = new QAction(tr("&About"), this);
    connect(act, &QAction::triggered, [=](){
        QString brag = tr(  "<html><h2>Sqriptor</h2><h3>Greatest&nbsp;and&nbsp;best&nbsp;TextEditor"
                            "&nbsp;in&nbsp;the&nbsp;world.</h3><h4 align=right>Tribute.</h4>"
                            "<hr><a href='https://github.com/luebking/sqriptor'>Code on github</a><br>"
                            "<br><a href='https://github.com/luebking/sqriptor/issues'>"
                            "Report, what you falsely assume to be a bug</a></html>");
        QMessageBox *about = new QMessageBox(QMessageBox::Information, tr("About Sqriptor"), brag,
                                             QMessageBox::Ok, nullptr, Qt::Dialog);
        about->setAttribute(Qt::WA_DeleteOnClose);
        about->setTextInteractionFlags(Qt::LinksAccessibleByMouse|Qt::LinksAccessibleByKeyboard);
        about->show();
    });
    menu->addAction(act);

    static QDialog *shortcutHelp = nullptr;
    act = new QAction(tr("Scintilla &Shortcuts"), this);
    connect(act, &QAction::triggered, [=](){
        if (shortcutHelp) {
            shortcutHelp->show();
            return;
        }
        QsciScintilla *doc = textEdit();
        QString text("<html><table>");
        foreach(QsciCommand *cmd, doc->standardCommands()->commands()) {
            if (!cmd->key())
                continue;
            text += "<tr><td align=center style='font-weight:bold;'>" + QKeySequence(cmd->key()).toString() + 
                    "</td><td align=left>" + cmd->description() + "</td></tr>";
        }
        text += "</table></html>";
        shortcutHelp = new QDialog(this);
        shortcutHelp->setWindowTitle(tr("Scintilla Shortcuts"));
        QHBoxLayout *layout = new QHBoxLayout(shortcutHelp);
        shortcutHelp->setLayout(layout);
        QTextEdit *display = new QTextEdit(shortcutHelp);
        display->setReadOnly(true);
        display->setText(text);
        layout->addWidget(display);
        shortcutHelp->resize(640,480);
        shortcutHelp->show();
    });
    menu->addAction(act);

/*     act = new QAction(tr("About &Qt"), this);
    act->setStatusTip(tr("Show the Qt library's About box"));
    connect(act, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    menu->addAction(act); */
}

void Sqriptor::indicateCurrentEOL()
{
    foreach (QAction *act, m_eolMenu->actions()) {
        if (act->data().toInt() == textEdit()->eolMode()) {
            act->setChecked(true);
            return;
        }
    }
}
