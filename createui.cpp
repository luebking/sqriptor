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
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLineEdit>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPainter>
#include <QPainterPath>
#include <QScrollBar>
#include <QSpinBox>
#include <QTextEdit>
#include <QToolButton>


#include <QDebug>

#include <Qsci/qsciscintilla.h>
#include <Qsci/qscicommandset.h>
#include <Qsci/qscicommand.h>

#include "sqriptor.h"

#define ADD_ACT menu->addAction(act); addAction(act);


class NavHelper : public QObject {
    public:
        NavHelper(Sqriptor *parent) : QObject(parent) {}
protected:
    bool eventFilter(QObject *, QEvent *ev) {
        if (ev->type() == QEvent::KeyPress || ev->type() == QEvent::KeyRelease) {
            QKeyEvent *kev = static_cast<QKeyEvent*>(ev);
            switch (kev->key()) {
                case Qt::Key_Home:
                case Qt::Key_End:
                    if (!(kev->modifiers() & Qt::ControlModifier))
                        return false;
                    [[fallthrough]];
                case Qt::Key_Up:
                case Qt::Key_Down:
                case Qt::Key_PageUp:
                case Qt::Key_PageDown:
                    QCoreApplication::sendEvent(static_cast<Sqriptor*>(parent())->textEdit(), ev);
                    return true;
                default:
                    return false;
            }
        }
        return false;
    }
};

void Sqriptor::createUI()
{
    QPixmap pix(64,64);
    renderIcon(pix);
    setWindowIcon(pix);
    QMenu *menu;
    QAction *act;

    act = new QAction(this);
    act->setShortcut(tr("Ctrl+0"));
    connect(act, &QAction::triggered, [=](){
        textEdit()->zoomTo(0);
    });
    addAction(act);

    menu = menuBar()->addMenu("Ξ"); // ◎ (·) ⬤ Ξ • · ◉ ▣ ■ □ ▢ ◯ ◆ ★ •  
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

    NavHelper *navHelper = new NavHelper(this);

    QLineEdit *findLine = new QLineEdit(searchBar);
    findLine->installEventFilter(navHelper);
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
    replaceLine->installEventFilter(navHelper);
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
    
    QLineEdit *filterLine = new QLineEdit(searchBar);
    filterLine->installEventFilter(navHelper);
    
    auto l_filter = [=]() {
        QsciScintilla *doc = textEdit();
        const QString filter = filterLine->text();
        if (filter.isEmpty()) {
            for (int i = 0; i < doc->lines(); ++i)
                doc->SendScintilla(QsciScintillaBase::SCI_SHOWLINES, i, i);
        } else if (searchRegExp->isChecked()) {
            const QRegularExpression rx(filter, searchCaseSens->isChecked() ?
                                                QRegularExpression::NoPatternOption :
                                                QRegularExpression::CaseInsensitiveOption);
            for (int i = 0; i < doc->lines(); ++i) {
                QString line = doc->text(i);
                if (line.contains(rx, nullptr))
                    doc->SendScintilla(QsciScintillaBase::SCI_SHOWLINES, i, i);
                else
                    doc->SendScintilla(QsciScintillaBase::SCI_HIDELINES, i, i);
            }
        } else {
            Qt::CaseSensitivity cs = searchCaseSens->isChecked() ?
                                            Qt::CaseSensitive : Qt::CaseInsensitive;
            for (int i = 0; i < doc->lines(); ++i) {
                QString line = doc->text(i);
                if (line.contains(filter, cs))
                    doc->SendScintilla(QsciScintillaBase::SCI_SHOWLINES, i, i);
                else
                    doc->SendScintilla(QsciScintillaBase::SCI_HIDELINES, i, i);
            }
        }
    };
    
    connect(filterLine, &QLineEdit::returnPressed, [=]() { l_filter(); });
    connect(filterLine, &QLineEdit::textEdited, [=]() { l_filter(); });

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
    layout->addWidget(filterLine);
    gotoLine->hide();
    replaceLine->hide();
    filterLine->hide();

    menuBar()->setCornerWidget(searchBar);
    
    menu->addSeparator();
    
#define HIDE_STUFF  searchBar->hide(); gotoLine->hide(); findLine->hide(); filterLine->hide();\
                    replaceLine->hide(); btn->hide(); replaceAll->setVisible(false);
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

    act = new QAction(tr("Fi&lter"), this);
    act->setShortcut(tr("Ctrl+L"));
    connect(act, &QAction::triggered, [=]() {
        HIDE_STUFF
        btn->setText(tr("Filter:"));
        btn->show();
        filterLine->show();
        filterLine->setFocus();
        filterLine->selectAll();
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
/*
    act = new QAction(tr("Show &Tip"), this);
    act->setShortcut(tr("Ctrl+?"));
    connect(act, &QAction::triggered, [=](){ textEdit()->setCallTipsStyle(QsciScintilla::CallTipsContext); textEdit()->callTip(); });
    ADD_ACT
*/
    m_wrapped = new QAction(tr("&Wrap text"), this);
    m_wrapped->setShortcut(tr("F10"));
    m_wrapped->setCheckable(true);
    connect(m_wrapped, &QAction::triggered, [=](bool checked){
        textEdit()->setWrapMode(checked ? QsciScintilla::WrapWhitespace : QsciScintilla::WrapNone);
        });
    menu->addAction(m_wrapped);
    
    m_folds = new QAction(tr("&Folding"), this);
    m_folds->setShortcut(tr("F11"));
    m_folds->setCheckable(true);
    connect(m_folds, &QAction::triggered, [=](bool checked){
        textEdit()->setFolding(checked ? QsciScintilla::PlainFoldStyle : QsciScintilla::NoFoldStyle);
        });
    menu->addAction(m_folds);

    m_EolVis = new QAction(tr("&EOL"), this);
    m_EolVis->setCheckable(true);
    connect(m_EolVis, &QAction::triggered, [=](bool checked){
        textEdit()->setEolVisibility(checked);
        });
    menu->addAction(m_EolVis);

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
    
    menu->addSeparator();

    act = new QAction(tr("&Flip colors"), this);
    connect(act, &QAction::triggered, [=]() {
        QColor c = config.color.bg;
        config.color.bg = config.color.fg;
        config.color.fg = c;
        updatePalette();
    });
    menu->addAction(act);
    
    QMenu *syntaxMenu = menuBar()->addMenu(tr("&Syntax"));
    m_syntaxActions = new QActionGroup(this);
#define ADD_SYNTAX2(_SYN_, _COM_) act = m_syntaxActions->addAction(menu->addAction(#_SYN_ _COM_, [=](){setSyntax(Syntax::_SYN_);}));\
        act->setCheckable(true); act->setData(#_SYN_)
#define ADD_SYNTAX(_SYN_) ADD_SYNTAX2(_SYN_, "")
    syntaxMenu->addAction(tr("Auto"), [=](){setSyntax(Syntax::Auto);});
    syntaxMenu->addSeparator();
    m_syntaxActions->addAction(syntaxMenu->addAction(tr("None"), [=](){setSyntax(Syntax::None);}))->setCheckable(true);
    syntaxMenu->addSeparator();
    
    menu = syntaxMenu->addMenu(tr("&Code"));
    ADD_SYNTAX(CPP); ADD_SYNTAX(CSharp); ADD_SYNTAX(D); ADD_SYNTAX(Fortran);
    ADD_SYNTAX(Fortran77); ADD_SYNTAX(Java); ADD_SYNTAX(Pascal);
    ADD_SYNTAX(Python);
    
    menu = syntaxMenu->addMenu(tr("&Script"));
    ADD_SYNTAX(AVS); ADD_SYNTAX(AWK); ADD_SYNTAX(Bash); ADD_SYNTAX(Batch); ADD_SYNTAX(CoffeeScript);
    ADD_SYNTAX(JavaScript); ADD_SYNTAX(Lua); ADD_SYNTAX(Perl);
    QAction *htmlSyntax = ADD_SYNTAX2(HTML, "\tid. PHP, JSP, ASP"); ADD_SYNTAX(PostScript);
    ADD_SYNTAX(Ruby); ADD_SYNTAX(TCL);
    
    menu = syntaxMenu->addMenu(tr("Mark&up"));
    ADD_SYNTAX(CSS);  ADD_SYNTAX(Markdown2); act->setText("Markdown");
    menu->addAction(htmlSyntax);
    ADD_SYNTAX(TeX); ADD_SYNTAX(YAML);
    
    menu = syntaxMenu->addMenu(tr("&Math"));
    ADD_SYNTAX2(IDL, "\tInteractive Data Language"); ADD_SYNTAX(Matlab); ADD_SYNTAX(Octave);
    
    menu = syntaxMenu->addMenu(tr("&Buildsystem"));
    ADD_SYNTAX(CMake); ADD_SYNTAX(Makefile); ADD_SYNTAX(PkgBuild);
    
    menu = syntaxMenu->addMenu(tr("&Config/Data"));
    ADD_SYNTAX(Diff); ADD_SYNTAX2(Journal, "\tSystemD"); ADD_SYNTAX(JSON);
    ADD_SYNTAX2(PO, "\teg. gettext"); ADD_SYNTAX(POV);
    ADD_SYNTAX2(Properties, "\teg. ini syntax"); ADD_SYNTAX(SQL);
    ADD_SYNTAX(XML); ADD_SYNTAX(Xorg);
    
    menu = syntaxMenu->addMenu(tr("&Obscure && ancient stuff"));
    ADD_SYNTAX2(EDIFACT, "\tUN Data Interchange");
    ADD_SYNTAX2(Spice, "\tElectronic circuit simulator");
    ADD_SYNTAX2(Verilog, "\tHardware description");
    ADD_SYNTAX2(VHDL, "\tHardware description");


    menuBar()->addSeparator();

    menu = menuBar()->addMenu(tr("&Help"));

    act = new QAction(tr("&About"), this);
    connect(act, &QAction::triggered, [=](){
        QString brag = tr(  "<html><h2>Sqriptor</h2><h4>Greatest&nbsp;and&nbsp;best&nbsp;TextEditor"
                            "&nbsp;in&nbsp;the&nbsp;world.</h4><h4 align=right>Tribute.</h4>"
                            "<hr><a href='https://github.com/luebking/sqriptor'>Code on github</a><br>"
                            "<br><a href='https://github.com/luebking/sqriptor/issues'>"
                            "Report, what you falsely assume to be a bug</a>"
                            "<hr><h4>Facilitating</h4>"
                            "<a href=https://www.riverbankcomputing.com/software/qscintilla/intro'>"
                            "QScintilla</a><br><br><a href='https://www.scintilla.org/'>Scintilla</a></html>");
        QMessageBox *about = new QMessageBox(QMessageBox::Information, tr("About Sqriptor"), brag,
                                             QMessageBox::Ok, nullptr, Qt::Dialog);
        about->setAttribute(Qt::WA_DeleteOnClose);
        about->setTextInteractionFlags(Qt::LinksAccessibleByMouse|Qt::LinksAccessibleByKeyboard);
        QPixmap icon(256,256);
        renderIcon(icon);
        about->setIconPixmap(icon);
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

void Sqriptor::renderIcon(QPixmap &pix)
{
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    const int w = pix.width(), h = pix.height();
    const qreal gm = 1.618f;
    p.setPen(Qt::NoPen);
    p.setBrush(qApp->palette().color(QPalette::Active, QPalette::Highlight));
    p.drawEllipse(0, h-h/gm, w/gm, h/gm);
    QPainterPath path;
    path.moveTo(w/1.83, 0);
    path.lineTo(w, h - h/1.83);
    path.lineTo(w-w/gm, h/gm);
    path.closeSubpath();
    p.fillPath(path, QColor(0x666666));
    p.end();
}