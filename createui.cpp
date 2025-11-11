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
#include <QComboBox>
#include <QElapsedTimer>
#include <QFileDialog>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPainter>
#include <QPainterPath>
#include <QProcess>
#include <QPushButton>
#include <QScrollBar>
#include <QSpinBox>
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <QStringConverter>
#else
#include <QTextCodec>
#endif
#include <QTextBrowser>
#include <QToolButton>


#include <QDebug>

#include <Qsci/qsciscintilla.h>
#include <Qsci/qscicommandset.h>
#include <Qsci/qscicommand.h>

#include "scrollbar.h"
#include "sqriptor.h"

#define ADD_ACT menu->addAction(act); addAction(act);


class NavHelper : public QObject {
    public:
        NavHelper(Sqriptor *parent) : QObject(parent) {}
        bool allowTab = false;
protected:
    bool eventFilter(QObject *obj, QEvent *ev) {
        if (ev->type() == QEvent::KeyPress || ev->type() == QEvent::KeyRelease) {
            QKeyEvent *kev = static_cast<QKeyEvent*>(ev);
            switch (kev->key()) {
                case Qt::Key_Home:
                case Qt::Key_End:
                    if (!(kev->modifiers() & Qt::ControlModifier))
                        return false;
                    [[fallthrough]];
                case Qt::Key_Tab:
                    if (allowTab && ev->type() == QEvent::KeyPress) {
                        QWidget *w = qobject_cast<QWidget*>(obj);
                        if (w && !(qobject_cast<QLineEdit*>(w->nextInFocusChain()) &&
                                                w->nextInFocusChain()->isVisible()) &&
                                  qobject_cast<QLineEdit*>(w->previousInFocusChain())) {
                            w->previousInFocusChain()->setFocus();
                            return true;
                        }
                        return false;
                    }
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

class StatusMenuBackground : public QObject {
    public:
        StatusMenuBackground(QMenuBar *parent, const Sqriptor *s) : QObject(parent), m_sqriptor(s) {}
    protected:
        bool eventFilter(QObject *obj, QEvent *ev) {
            if (ev->type() != QEvent::Paint)
                return false;
            QMenuBar *bar = static_cast<QMenuBar*>(obj);
            QRect rect = bar->cornerWidget()->geometry();
            int right = rect.left();
            rect.setLeft(bar->actionGeometry(bar->actions().constLast()).right());
            rect.setRight(right);

            const int align = Qt::AlignVCenter|QStyle::visualAlignment(bar->layoutDirection(), Qt::AlignRight);
            QsciScintilla *doc = m_sqriptor->textEdit();
            QColor c;
            if (doc->isModified()) {
                c = m_sqriptor->config.color.error;
            } else {
                const QColor bg = bar->palette().color(bar->backgroundRole()),
                             fg = bar->palette().color(bar->foregroundRole());
                c = QColor((2*bg.red()+fg.red())/3, (2*bg.green()+fg.green())/3, (2*bg.green()+fg.green())/3);
            }
            QPainter p(bar);
            p.setPen(c);
            QString text = doc->property("sqriptor_filepath").toString();
            text = p.fontMetrics().elidedText(text, Qt::ElideLeft, rect.width());
            p.drawText(rect, align|Qt::TextSingleLine, text);
            p.end();
            return false;
        }
    private:
        const Sqriptor *m_sqriptor;
};

class PreView : public QTextBrowser
{
public:
    PreView(QWidget *parent) : QTextBrowser(parent) {
        setFrameShape(QFrame::NoFrame);
        setOpenLinks(true);
        setOpenExternalLinks(true);
        parent->installEventFilter(this);
    }
protected:
    bool eventFilter(QObject *obj, QEvent *ev) {
        if (ev->type() == QEvent::Resize && obj == parentWidget()) {
            setGeometry(parentWidget()->rect());
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
    
    QMenu *lastWeek = menu->addMenu(tr("&Last Night on Sqriptor"));
    connect(lastWeek, &QMenu::aboutToShow, [=]() {
        lastWeek->clear();
        if (config.recentFiles.isEmpty())
            return;
        QAction *act2;
        for (QString &recent : config.recentFiles) {
            act2 = lastWeek->addAction(QFileInfo(recent).fileName());
            act2->setData(recent);
            connect(act2, &QAction::triggered, [=]() { open(act2->data().toString()); });
        }
        lastWeek->addSeparator();
        act2 = lastWeek->addAction(tr("That's not my porn library in the history!"));
        connect(act2, &QAction::triggered, [=]() { config.recentFiles.clear(); });
    });
    connect(menu, &QMenu::aboutToShow, [=]() { lastWeek->setEnabled(!config.recentFiles.isEmpty()); });
    
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
    QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));

    menu = editMenu->addMenu(tr("&Copy && Paste"));
    act = new QAction(tr("Cu&t"), this);
    act->setShortcut(tr("Ctrl+X"));
    connect(act, &QAction::triggered, [=](){textEdit()->cut();});
    connect(this, SIGNAL(copyAvailable(bool)), act, SLOT(setEnabled(bool)));
    act->setEnabled(false);
    ADD_ACT

    act = new QAction(tr("&Copy"), this);
    act->setShortcut(tr("Ctrl+C"));
    connect(act, &QAction::triggered, [=](){copy();});
    connect(this, SIGNAL(copyAvailable(bool)), act, SLOT(setEnabled(bool)));
    act->setEnabled(false);
    ADD_ACT

    act = new QAction(tr("&Paste"), this);
    act->setShortcut(tr("Ctrl+V"));
    connect(act, &QAction::triggered, [=](){textEdit()->paste();});
    ADD_ACT

    QWidget *searchBar = new QWidget(menuBar());
    QToolButton *btn = new QToolButton(searchBar);
    btn->setAutoRaise(true);
    btn->setPopupMode(QToolButton::InstantPopup);
    btn->setText(tr("Find:"));
    QMenu *tbmenu = new QMenu(btn);
    QAction *filterInvert = new QAction(tr("Invert"), searchBar);
    filterInvert->setCheckable(true);
    tbmenu->addAction(filterInvert);
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
    QAction *findAllAct = new QAction(tr("Find all\tReturn"), searchBar);
    tbmenu->addAction(findAllAct);
    findAllAct->setVisible(false);
    QAction *replaceAll = new QAction(tr("Replace all"), searchBar);
    tbmenu->addAction(replaceAll);
    replaceAll->setVisible(false);
    QAction *showFilterContext = new QAction(tr("Show Context"), searchBar);
    tbmenu->addAction(showFilterContext);
    showFilterContext->setCheckable(true);
    showFilterContext->setChecked(false);
    showFilterContext->setVisible(false);
    m_filterBookmarks = new QAction(tr("Filter Bookmarks"), searchBar);
    tbmenu->addAction(m_filterBookmarks);
    m_filterBookmarks->setCheckable(true);
    m_filterBookmarks->setChecked(false);
    m_filterBookmarks->setVisible(false);
    m_filterBookmarks->setShortcut(tr("Alt+B"));
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
    
    connect(findLine, &QLineEdit::returnPressed, [=]() {
        l_search(false);
        findAll(findLine->text(), searchRegExp->isChecked(), searchCaseSens->isChecked(), searchWord->isChecked());
        });
    connect(findLine, &QLineEdit::textEdited, [=]() {
        l_search(true);
        findAll(QString(), false, false, false);
        });
    connect(findAllAct, &QAction::triggered, [=]() {
        findAll(findLine->text(), searchRegExp->isChecked(), searchCaseSens->isChecked(), searchWord->isChecked());
        });

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
        QsciScintilla *doc = textEdit();
        doc->getSelection(&la, &ia, &lb, &ib);
        doc->beginUndoAction();
        if (lb != la) {
            // if the user selected more than one line, that's explicit and
            // we take it as hint to limit the replace to that area
            // in-line selections happen for the find, but what's worse
            // is that we need to restore the selection after every find/replace
            // and the originally selected text can shrink due to that, eg.
            // |111|111, s/11/2/g;
            // |111|111 => |211|11 => |221|1
            // Chances of the user planning sth. like that are much lower if
            // s/he selected multiple lines
            if (doc->findFirstInSelection(text, re, cs, wo)) {
                do textEdit()->replace(replaceLine->text());
                while (doc->findNext());
            }
        } else {
            if (textEdit()->findFirst(text, re, cs, wo, false, true, 0, 0)) {
                do textEdit()->replace(replaceLine->text());
                while (doc->findNext());
            }
        }
        doc->endUndoAction();
    });
    
    m_filterLine = new QLineEdit(searchBar);
    m_filterLine->installEventFilter(navHelper);
    
    /*  "cooperative multithreeading" effort
        filtering a looooooong document (100.000 lines etc.) can take a while
        and we need to manage that
        This is all GUI stuff and idk. whether QsciScintilla, let alone Scintilla
        are sufficiently re-entrant. So we manage that here.
    */
    QTimer *filterTimer = new QTimer(this);
    filterTimer->setSingleShot(true);
    filterTimer->setInterval(200);
    /*  The average typing speed is 1 char / 300ms - source: internet.
        At the same time, delays > 250ms are perceived as stalls. source: idem.
        So we go for 200ms to allow faster users multiple key strokes while not
        looking too laggy.
        But if the filtering takes 500ms, that doesn't help...
    */
#define PROCESS_EVENTS  if (profiler.hasExpired(66)) { \
                            QCoreApplication::processEvents(); \
                            filterTimerWasActive = filterTimer->isActive(); \
                            filterTimer->stop(); \
                            profiler.restart(); \
                        }
    /** So in addition we profile the filtering and flush the event queue every
        66ms, maintaining 15fps (it's not an ego-shooter or some animation)
        In that, we catch an active timer and stall it until the filtering is done
        so we don't stumble over ourselves - this could still happen if the user
        also presses Enter for no reason... I should maybe @todo test that...
    */

#define FILTER_TEXT(_MATCH_) \
    int lastMatch = 0; /* skip first line because scintilla won't hide it */ \
    for (int i = 1; i < doc->lines(); ++i) { \
        if ((grow || doc->SendScintilla(QsciScintillaBase::SCI_GETLINEVISIBLE, i)) && _MATCH_) { \
            if (lastMatch + context < i - 1 - context) { \
                doc->SendScintilla(QsciScintillaBase::SCI_HIDELINES, lastMatch + 1 + context, i - 1 - context); \
                if (context) \
                    doc->markerAdd(lastMatch + context, s_filtersplitter); \
            } \
            context = showContext ? 5 : 0; \
            doc->SendScintilla(QsciScintillaBase::SCI_SHOWLINES, i - context, i + context); \
            lastMatch = i; \
        } \
        PROCESS_EVENTS /* allow interaction if this is slow */ \
        if (filter != m_filterLine->text()) { \
            lastMatch = doc->lines(); /* we don't want to hide the "tail" here */ \
            break; /* user altered pattern, abort current filter */ \
        } \
    } \
    if (lastMatch + 1 + context < doc->lines()) /* hide tail */ \
        doc->SendScintilla(QsciScintillaBase::SCI_HIDELINES, lastMatch + 1 + context, doc->lines() - 1)

    static QString lastFilter;
    static bool hadContext = showFilterContext->isChecked() || m_filterBookmarks->isChecked();
    static bool wasCaseSens = searchCaseSens->isChecked();
    static bool wasInverted = filterInvert->isChecked();
    static bool wasRegExp = searchRegExp->isChecked();
    connect(m_documents, &QTabWidget::currentChanged, [=](int idx) { lastFilter.clear(); });
    auto l_filter = [=]() {
        bool filterTimerWasActive = false;
        QsciScintilla *doc = textEdit();
        const QString filter = m_filterLine->text();

        int context = 0;
        const bool showContext = showFilterContext->isChecked() || m_filterBookmarks->isChecked();
        static const int s_filtersplitter = 31;
        doc->markerDefine(QsciScintilla::Underline, s_filtersplitter);
        QColor bg = config.color.bg, fg = config.color.fg;
        QColor mid = QColor((2*bg.red()+fg.red())/3, (2*bg.green()+fg.green())/3, (2*bg.green()+fg.green())/3);
        doc->setMarkerBackgroundColor(mid, s_filtersplitter);

        /** In addition to the above, also shortcut the filtering if we know that
            the filter got narrower... this could be a problem w/ regexp
            @todo test that, too...
        */
        const bool grow =   searchRegExp->isChecked() || lastFilter.isEmpty() ||
                            !filter.contains(lastFilter) || wasInverted != filterInvert->isChecked() ||
                            (showContext && !hadContext) || (wasCaseSens && !searchCaseSens->isChecked()) ||
                            (wasRegExp != searchRegExp->isChecked());
        if ((grow && showContext) || (!showContext && hadContext))
            doc->markerDeleteAll(s_filtersplitter);
        lastFilter = filter;
        hadContext = showContext;
        wasCaseSens = searchCaseSens->isChecked();
        wasInverted = filterInvert->isChecked();
        wasRegExp = searchRegExp->isChecked();

        QElapsedTimer profiler;
        profiler.start();

        if (m_filterBookmarks->isChecked()) {
            FILTER_TEXT((doc->markersAtLine(i) & 2));
        } else if (filter.isEmpty()) {
            doc->SendScintilla(QsciScintillaBase::SCI_SHOWLINES, 0, doc->lines() - 1);
            doc->ensureCursorVisible();
        } else if (searchRegExp->isChecked()) {
            const QRegularExpression rx(filter, searchCaseSens->isChecked() ?
                                                QRegularExpression::NoPatternOption :
                                                QRegularExpression::CaseInsensitiveOption);
            if (filterInvert->isChecked()) {
                FILTER_TEXT(!doc->text(i).contains(rx));
            } else {
                FILTER_TEXT(doc->text(i).contains(rx));
            }
        } else {
            Qt::CaseSensitivity cs = searchCaseSens->isChecked() ?
                                            Qt::CaseSensitive : Qt::CaseInsensitive;
            if (filterInvert->isChecked()) {
                FILTER_TEXT(!doc->text(i).contains(filter, cs));
            } else {
                FILTER_TEXT(doc->text(i).contains(filter, cs));
            }
        }
        if (filterTimerWasActive)
            filterTimer->start();
    };

    connect(filterTimer, &QTimer::timeout, [=]() { l_filter(); });
    connect(m_filterLine, &QLineEdit::returnPressed, [=]() { filterTimer->stop(); l_filter(); });
    connect(m_filterLine, &QLineEdit::textEdited, [=]() { filterTimer->start(); });
    connect(m_filterBookmarks, &QAction::toggled, [=]() { filterTimer->stop(); l_filter(); });
    connect(filterInvert, &QAction::toggled, [=]() { filterTimer->stop(); l_filter(); });
    connect(searchRegExp, &QAction::toggled, [=]() { if (m_filterLine->isVisible()) { filterTimer->stop(); l_filter(); } });
    connect(searchCaseSens, &QAction::toggled, [=]() { if (m_filterLine->isVisible()) { filterTimer->stop(); l_filter(); } });

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
    layout->addWidget(m_filterLine);
    gotoLine->hide();
    replaceLine->hide();
    m_filterLine->hide();

    menuBar()->setCornerWidget(searchBar);
    
#define HIDE_STUFF  searchBar->hide(); gotoLine->hide(); findLine->hide(); m_filterLine->hide();\
                    replaceLine->hide(); btn->hide(); findAllAct->setVisible(false); \
                    replaceAll->setVisible(false); showFilterContext->setVisible(false); \
                    filterInvert->setVisible(false); searchWord->setVisible(false); \
                    searchForward->setVisible(false); navHelper->allowTab = false; \
                    m_filterBookmarks->setVisible(false);
#define SHOW_STUFF menuWasVisible = menuBar()->isVisible(); searchBar->show(); menuBar()->show();
    
    menu = editMenu->addMenu(tr("&Search and replace"));
    act = new QAction(tr("&Find"), this);
    act->setShortcut(tr("Ctrl+F"));
    connect(act, &QAction::triggered, [=]() {
        HIDE_STUFF
        btn->setText(tr("Find:"));
        btn->show();
        findLine->show();
        findAllAct->setVisible(true);
        searchWord->setVisible(true);
        searchForward->setVisible(true);
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
        searchWord->setVisible(true);
        searchForward->setVisible(true);
        textEdit()->cancelFind(); // clear previous finds to no replace them
        navHelper->allowTab = true;
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
        m_filterLine->show();
        showFilterContext->setVisible(true);
        m_filterBookmarks->setVisible(true);
        filterInvert->setVisible(true);
        m_filterLine->setFocus();
        m_filterLine->selectAll();
        SHOW_STUFF
    });
    ADD_ACT

    menu = editMenu->addMenu(tr("&Bookmarks"));
    
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
    
    menu = editMenu;
    
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
    m_wrapped->setChecked(config.wrap.words);
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
    act = syntaxMenu->addAction(tr("None"), [=](){setSyntax(Syntax::None);});
    act->setData("None");
    act->setCheckable(true);
    m_syntaxActions->addAction(act);
    syntaxMenu->addSeparator();
    
    menu = syntaxMenu->addMenu(tr("&Code"));
    ADD_SYNTAX(CPP); ADD_SYNTAX(CSharp); ADD_SYNTAX(D); ADD_SYNTAX(Fortran);
    ADD_SYNTAX(Fortran77); ADD_SYNTAX(GLSL); ADD_SYNTAX(Java); ADD_SYNTAX(LISP); ADD_SYNTAX(NIM);
    ADD_SYNTAX(Pascal); ADD_SYNTAX(Python); ADD_SYNTAX(Rust);
    
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
    ADD_SYNTAX(CMake); ADD_SYNTAX(Makefile); ADD_SYNTAX(QtPro); ADD_SYNTAX(PkgBuild);
    
    menu = syntaxMenu->addMenu(tr("&Config/Data"));
    ADD_SYNTAX(Diff); ADD_SYNTAX(FontConfig); ADD_SYNTAX2(Journal, "\tSystemD"); ADD_SYNTAX(JSON);
    ADD_SYNTAX2(PO, "\teg. gettext"); ADD_SYNTAX(POV);
    ADD_SYNTAX2(Properties, "\teg. ini syntax"); ADD_SYNTAX(SQL);
    ADD_SYNTAX(XML); ADD_SYNTAX(Xorg); ADD_SYNTAX(XorgLog);
    
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
    
    act = new QAction(tr("&Syntax detection"), this);
    connect(act, &QAction::triggered, [=](){
        QString help =
        tr("<html><h2>Syntax Detection</h2>"
        "The automatic syntax selection is primarily based on the files <b>suffix</b>, "
        "ie. <i>file.<b>pl</b></i> will be detected to be Perl.<hr>"
        "If your file doesn't have a suffix, the same suffix is matched against its "
        "<b>shebang</b>, ie. if <i>/bin/foo</i> has the first line <b>#!/bin/perl</b> it will "
        "be detected to be perl as well.<hr>"
        "As last resort, you can indicate the type w/ a comment (document syntax) "
        "in the first line, eg. <b>; mime/ini</b> will apply the ini syntax, even "
        "if the filename is <i>~/.config/foorc</i> - ';' indicates an ini comment.<hr>"
        "If you think some suffix is missing, you can file a <s>bug</s> "
        "<a href='https://github.com/luebking/sqriptor/issues'>feature request</a>"
        "</html>");
        QMessageBox *syntaxHelp = new QMessageBox(QMessageBox::Information, tr("Syntax detection"),
                                                help, QMessageBox::Ok, nullptr, Qt::Dialog);
        syntaxHelp->setAttribute(Qt::WA_DeleteOnClose);
        syntaxHelp->setTextInteractionFlags(Qt::LinksAccessibleByMouse|Qt::LinksAccessibleByKeyboard);
        syntaxHelp->show();
    });
    menu->addAction(act);
    
    act = new QAction(tr("&Block selection"), this);
    connect(act, &QAction::triggered, [=](){
        QString help =
        tr("<html><h2>Block selection</h2>"
        "Hold <b>Alt+Shift</b> while using the <b>arrow keys</b> or press "
        "<b>Ctrl</b> while selecting text with the <b>mouse</b>.<br>"
        "(Only) text cut or copied this way will be pasted in block mode.<br>"
        "<br>You're welcome."
        "</html>");
        QMessageBox *blockHelp = new QMessageBox(QMessageBox::Information, tr("Block selection"),
                                                help, QMessageBox::Ok, nullptr, Qt::Dialog);
        blockHelp->setAttribute(Qt::WA_DeleteOnClose);
        blockHelp->show();
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

    menuBar()->addSeparator();

    m_previewAction = act = menuBar()->addAction(tr("Toggle Preview"));
    connect(act, &QAction::triggered, [=]() {togglePreview();} );
    act->setShortcut(tr("Ctrl+P"));
    act->setToolTip(tr("Ctrl+P"));
    act->setVisible(false);

    StatusMenuBackground *smb = new StatusMenuBackground(menuBar(), this);
    menuBar()->installEventFilter(smb);

/*     act = new QAction(tr("About &Qt"), this);
    act->setStatusTip(tr("Show the Qt library's About box"));
    connect(act, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    menu->addAction(act); */
}

void Sqriptor::findAll(QString text, bool rx, bool cs, bool wo)
{
    QList<int> markers;
    QsciScintilla *doc = textEdit();
    ScrollBar *sb = static_cast<ScrollBar*>(doc->verticalScrollBar());
    if (text.isEmpty()) {
        sb->setMarkers(markers);
        return;
    }

    QElapsedTimer profiler;
    profiler.start();
    if (rx) {
        const QRegularExpression qrx(text, cs ? QRegularExpression::NoPatternOption :
                                                 QRegularExpression::CaseInsensitiveOption);
        for (int i = 0; i < doc->lines(); ++i) {
            if (doc->text(i).contains(qrx, nullptr))
                markers << i*sb->maximum()/doc->lines();
            if (profiler.hasExpired(66)) {
                sb->setMarkers(markers);
                QCoreApplication::processEvents();
                profiler.restart();
            }
        }
    } else {
        Qt::CaseSensitivity qcs = cs ? Qt::CaseSensitive : Qt::CaseInsensitive;
        for (int i = 0; i < doc->lines(); ++i) {
            if (doc->text(i).contains(text, qcs))
                markers << i*sb->maximum()/doc->lines();
            if (profiler.hasExpired(66)) {
                sb->setMarkers(markers);
                QCoreApplication::processEvents();
                profiler.restart();
            }
        }
    }
    sb->setMarkers(markers);
}

void Sqriptor::copy()
{
    QsciScintilla *doc = textEdit();
    if (m_filterLine->text().isEmpty() && !m_filterBookmarks->isChecked()) {
        doc->SendScintilla(QsciScintilla::SCI_COPY);
        return;
    }
    int l1, i1, l2, i2;
    doc->getSelection(&l1, &i1, &l2, &i2);
    if (l1 == l2) {
        doc->SendScintilla(QsciScintilla::SCI_COPY);
        return;
    }
    // user tries to copy multiple lines out of a filter. So filter them.
    QString buffer = doc->text(l1).mid(i1);
    for (int i = l1+1; i < l2; ++i) {
        if (doc->SendScintilla(QsciScintillaBase::SCI_GETLINEVISIBLE, i))
            buffer += doc->text(i);
    }
    buffer += doc->text(l2).left(i2);
    QByteArray utf8 = buffer.toUtf8();
    doc->SendScintilla(QsciScintilla::SCI_COPYTEXT, utf8.size(), utf8.constData());
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

QString Sqriptor::ask4Codec(const QString &codec, const QString &fileName)
{
    QDialog *warning = new QDialog(this);
    warning->setWindowTitle(tr("Sqriptor"));
    QGridLayout *gld = new QGridLayout(warning);
    warning->setLayout(gld);
    QLabel *lbl;
    QIcon icon = style()->standardIcon(QStyle::SP_MessageBoxWarning, 0, warning);
    if (!icon.isNull()) {
        lbl = new QLabel(warning);
        int iconSize = style()->pixelMetric(QStyle::PM_MessageBoxIconSize, 0, warning);
        lbl->setPixmap(icon.pixmap(warning->windowHandle(), QSize(iconSize, iconSize)));
        gld->addWidget(lbl, 0, 0, Qt::AlignTop|Qt::AlignHCenter);
    }
    lbl = new QLabel(warning);
    lbl->setText(tr("<html><h2>Anachronism Alert!</h2>"
                    "<i>%1</i> seems to be encoded in<h3 align=\"center\">%2</h3>"
                    "<b>Notice!</b> that it will be <b>saved as UTF-8!</b><hr>"
                    "</html>").arg(fileName).arg(codec));
    gld->addWidget(lbl, 0, icon.isNull() ? 0 : 1, 1, -1, Qt::AlignCenter);
    lbl = new QLabel(tr("Interpret text as"), warning);
    gld->addWidget(lbl, 1, 0);
    QComboBox *codecs = new QComboBox(warning);
    codecs->setEditable(true);
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
    QStringList codecNames = QStringConverter::availableCodecs();
#else
    QStringList codecNames;
    for (const QByteArray &ba : QTextCodec::availableCodecs())
        codecNames << ba;
#endif
    codecNames.removeDuplicates();
    codecNames.sort(Qt::CaseInsensitive);
    codecs->addItems(codecNames);
    codecs->setInsertPolicy(QComboBox::NoInsert);
    codecs->setCurrentText(codec);
    gld->addWidget(codecs, 1, 1);
    QPushButton *btn = new QPushButton(tr("Ok"), warning);
    connect(btn, SIGNAL(clicked()), warning, SLOT(accept()));
    gld->addWidget(btn, 1, 2);
    warning->exec();
    QString ret = codecs->currentText();
    delete warning;
    return ret;
}

void Sqriptor::togglePreview(int idx, Qt::CheckState state)
{
    QsciScintilla *doc = textEdit(idx);
    QTextBrowser *preview = doc->findChild<QTextBrowser*>(QAnyStringView());
    if (!preview && state == Qt::Unchecked)
        return;
    if (!preview) {
        preview = new PreView(doc);
        preview->setGeometry(doc->rect());
        preview->hide();
    }
    if (state == Qt::PartiallyChecked) {
        if (preview->isVisible())
            state = Qt::Unchecked;
        else
            state = Qt::Checked;
    }
    if (state == Qt::Unchecked) {
        preview->hide();
        doc->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        doc->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        return;
    }
    const int syntax = doc->property("sqriptor_syntax").toInt();
    if (syntax == Syntax::Markdown2) {
        if (!config.markdownProcessor.isEmpty()) {
            QProcess mdProc;
            mdProc.startCommand(config.markdownProcessor);
            mdProc.waitForStarted();
            mdProc.write(doc->text().toLocal8Bit());
            mdProc.closeWriteChannel();
            mdProc.waitForFinished();
            preview->document()->setDefaultStyleSheet(config.previewCSS);
            preview->setHtml(QString::fromLocal8Bit(mdProc.readAllStandardOutput()));
        } else {
            preview->setMarkdown(doc->text());
        }
    } else if (syntax == Syntax::HTML) {
        preview->document()->setDefaultStyleSheet(config.previewCSS);
        preview->setHtml(doc->text());
    }
    doc->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    doc->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    preview->show();
    preview->raise();
}