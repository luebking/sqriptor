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


#include <QApplication>
#include <QCloseEvent>
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>
#include <QMessageBox>
#include <QPoint>
#include <QScreen>
#include <QScrollBar>
#include <QSettings>
#include <QSize>
#include <QSocketNotifier>
#include <QStatusBar>
#include <QTabBar>
#include <QTextStream>

#include <Qsci/qsciscintilla.h>

#include "sqriptor.h"

#include <QtDebug>

Config Sqriptor::config;

class EventPicker : public QObject {
protected:
    bool eventFilter(QObject *o, QEvent *ev) {
        if (ev->type() == QEvent::MouseButtonPress)
        if (QWidget *w = qobject_cast<QWidget*>(o)) {
            qDebug() << w << w->geometry();
            while (w = w->parentWidget())
                qDebug() << w << w->geometry();
            qDebug() << "=========================";
        }
        return false;
    }
};

Sqriptor::Sqriptor()
{
    readSettings();

    QPalette pal = qApp->palette();
    pal.setColor(QPalette::Window, config.color.bg);
    pal.setColor(QPalette::Base, config.color.bg);
    pal.setColor(QPalette::Button, config.color.bg);
    pal.setColor(QPalette::WindowText, config.color.fg);
    pal.setColor(QPalette::Text, config.color.fg);
    pal.setColor(QPalette::ButtonText, config.color.fg);
    setPalette(pal);
    //qApp->installEventFilter(new EventPicker);
    m_documents = new QTabWidget;
    m_documents->setDocumentMode(true);
    m_documents->setTabsClosable(true);
    m_documents->setContentsMargins(0, 0, 0, 0);
    connect(m_documents, SIGNAL(tabCloseRequested(int)), SLOT(closeTab(int)));
    connect(m_documents, &QTabWidget::currentChanged, [=](int idx) {
        setWindowModified(textEdit()->isModified());
        setWindowTitle(tr("%1[*]").arg(m_documents->tabText(idx)));
        indicateCurrentSyntax();
    });
    m_documents->setTabPosition(QTabWidget::West);
    m_documents->setTabBarAutoHide(true);
    setCentralWidget(m_documents);

    createUI();
    addTab();

    setCurrentFile("");
    statusBar()->showMessage(tr("Ready"));
}

QsciScintilla *Sqriptor::textEdit(int idx) const
{
    Q_ASSERT(idx < m_documents->count());
    return qobject_cast<QsciScintilla*>(idx < 0 ? m_documents->currentWidget() : m_documents->widget(idx));
}

void Sqriptor::closeEvent(QCloseEvent *event)
{
    for (int i = m_documents->count() - 1; i > -1; --i) {
        if (!closeTab(i)) {
            event->ignore();
            return;
        }
    }
    writeSettings();
    event->accept();
}

void Sqriptor::newFile(QString name, bool forceNewTab)
{
    QsciScintilla *doc = textEdit();
    if (forceNewTab || doc->length() > 0 || doc->isModified()) {
        m_documents->setCurrentIndex(addTab());
        setCurrentFile(name);
    } else {
        textEdit()->clear();
        setCurrentFile(name);
    }
}

void Sqriptor::open(QString fileName, bool forceNewTab)
{
    if (fileName.isEmpty())
        return;

    QsciScintilla *doc = textEdit();
    if (forceNewTab || doc->length() > 0 || doc->isModified())
        m_documents->setCurrentIndex(addTab());
    loadFile(fileName);
}

int Sqriptor::addTab()
{
    QsciScintilla *doc = new QsciScintilla;
    doc->setFrameShape(QFrame::NoFrame);
    doc->setFont(config.font);
    
    doc->setBraceMatching(QsciScintilla::StrictBraceMatch);
    doc->setWhitespaceVisibility(QsciScintilla::WsVisibleOnlyInIndent);
    doc->setEdgeMode(QsciScintilla::EdgeLine);
    doc->setEdgeColumn(80);
    
    doc->markerDefine(QsciScintilla::Bookmark, 1);
    doc->setMarkerForegroundColor(config.color.bg, 1);
    doc->setMarkerBackgroundColor(palette().color(QPalette::Link), 1);
    doc->setWrapVisualFlags(QsciScintilla::WrapFlagNone, QsciScintilla::WrapFlagInMargin, 0);
    doc->setMarginWidth(0, 16);
    doc->setMarginWidth(1, 16);
    doc->setMarginWidth(0, 24);
    
    doc->setAutoCompletionSource(QsciScintilla::AcsAll);
    doc->setAutoCompletionThreshold(3);
//    doc->setAutoCompletionUseSingle(AcusNever);
    doc->setAutoCompletionReplaceWord(true);
    
    doc->setIndentationGuides(true);
    doc->setAutoIndent(true);
    doc->setTabIndents(true);
    doc->setBackspaceUnindents(true);
    doc->setIndentationsUseTabs(config.tab.isTab);
    doc->setTabWidth(config.tab.width);
    
    connect(doc, SIGNAL(copyAvailable(bool)), SIGNAL(copyAvailable(bool)));
    connect(doc, &QsciScintilla::modificationChanged, [=](){
        m_documents->tabBar()->setTabTextColor(m_documents->indexOf(doc),
                                               doc->isModified() ? config.color.error : QColor());
        if (doc == m_documents->currentWidget())
            setWindowModified(doc->isModified());
    });

    setSyntax(Syntax::None, doc);
    doc->setFocus();

    return m_documents->addTab(doc, "");
}

bool Sqriptor::closeTab(int idx)
{
    if (idx < m_documents->count())
        idx = m_documents->currentIndex();
    if (maybeSave(idx)) {
        if (m_documents->count() > 1) {
            delete m_documents->widget(idx);
        } else {
            textEdit()->clear();
            setCurrentFile("");
        }
        return true;
    }
    return false;
}

bool Sqriptor::save()
{
    QString fileName = textEdit()->property("sqriptor_filename").toString();
    if (fileName.isEmpty()) {
        return saveAs();
    } else {
        return saveFile(fileName);
    }
}

bool Sqriptor::saveAs()
{
    QString path = textEdit()->property("sqriptor_filename").toString();
    path = QFileInfo(path).absolutePath();
    QString fileName = QFileDialog::getSaveFileName(this, QString(), path);
    if (fileName.isEmpty())
        return false;

    return saveFile(fileName);
}

void Sqriptor::about()
{
   QMessageBox::about(this, tr("About Application"),
            tr("The <b>Application</b> example demonstrates how to "
               "write modern GUI applications using Qt, with a menu bar, "
               "toolbars, and a status bar."));
}



void Sqriptor::toggleBookmark()
{
    int line, index;
    textEdit()->getCursorPosition(&line, &index);
    if (textEdit()->markersAtLine(line) == 2)
        textEdit()->markerDelete(line, 1);
    else
        textEdit()->markerAdd(line, 1);
}

void Sqriptor::nextBookmark()
{
    int line, index;
    textEdit()->getCursorPosition(&line, &index);
    if (textEdit()->markersAtLine(line) == 2)
        ++line;
    textEdit()->setCursorPosition(textEdit()->markerFindNext(line, 2), 0);
}

void Sqriptor::prevBookmark()
{
    int line, index;
    textEdit()->getCursorPosition(&line, &index);
    if (textEdit()->markersAtLine(line) == 2)
        --line;
    textEdit()->setCursorPosition(textEdit()->markerFindPrevious(line, 2), 0);
}

void Sqriptor::readSettings()
{
    QSettings settings("sqriptor");
    QRect geo = QGuiApplication::screenAt(pos())->availableGeometry();
    resize(settings.value("size", QSize(qRound(geo.width()/1.618f),
                                        qRound(geo.height()/1.618f))).toSize());
    settings.beginGroup("font");
    config.font.setFamily(settings.value("family", "monospace").toString());
    config.font.setPointSize(settings.value("size", 9).toInt());
    settings.endGroup();
    settings.beginGroup("tab");
    config.tab.isTab = (settings.value("isTab", false).toBool());
    config.tab.width = (settings.value("width", 4).toInt());
    settings.endGroup();
    settings.beginGroup("color");
    #define COLOR(_TYPE_,_DEFAULT_) settings.value(_TYPE_, QColor(_DEFAULT_)).value<QColor>()
    config.color.bg = COLOR("background", 0x2E2E2E);
    config.color.fg = COLOR("foreground", 0xfafafa);
    config.color.string = COLOR("string", 0x8CD000);
    config.color.character = COLOR("char", 0x00A513);
    config.color.comment = COLOR("comment", 0x999999);
    config.color.preproc = COLOR("preproc", 0xFF3DB5);
    config.color.number = COLOR("number", 0x1793D0);
    config.color.operateur = COLOR("operator", 0xFF8F00);
    config.color.keyword = COLOR("keyword", 0x6A7FCC);
    config.color.error = COLOR("error", 0xE00000);
    settings.endGroup();
}

void Sqriptor::writeSettings()
{
    QSettings settings("sqriptor");
    settings.beginGroup("font");
    settings.setValue("family", config.font.family());
    settings.setValue("size", config.font.pointSize());
    settings.endGroup();
    settings.beginGroup("tab");
    settings.setValue("isTab", config.tab.isTab);
    settings.setValue("width", config.tab.width);
    settings.endGroup();
    settings.beginGroup("color");
    settings.setValue("background", config.color.bg);
    settings.setValue("foreground", config.color.fg);
    settings.setValue("string", config.color.string);
    settings.setValue("char", config.color.character);
    settings.setValue("comment", config.color.comment);
    settings.setValue("preproc", config.color.preproc);
    settings.setValue("number", config.color.number);
    settings.setValue("operator", config.color.operateur);
    settings.setValue("keyword", config.color.keyword);
    settings.setValue("error", config.color.error);
    settings.endGroup();
}

bool Sqriptor::maybeSave(int idx)
{
    if (textEdit(idx)->isModified()) {
        int ret = QMessageBox::warning(this, tr("Application"),
                     tr("The document has been modified.\n"
                        "Do you want to save your changes?"),
                     QMessageBox::Yes | QMessageBox::Default,
                     QMessageBox::No,
                     QMessageBox::Cancel | QMessageBox::Escape);
        if (ret == QMessageBox::Yes)
            return save();
        else if (ret == QMessageBox::Cancel)
            return false;
    }
    return true;
}

void Sqriptor::loadFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return;
    }

    QTextStream in(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    textEdit()->setText(in.readAll());
    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File loaded"), 2000);
}

bool Sqriptor::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    QTextStream out(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    out << textEdit()->text();
    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File saved"), 2000);
    return true;
}

void Sqriptor::setCurrentFile(const QString &fileName)
{
    textEdit()->setProperty("sqriptor_filename", fileName);
    textEdit()->setModified(false);
    setWindowModified(false);

    QString shownName;
    if (fileName.isEmpty())
        shownName = "untitled.txt";
    else
        shownName = QFileInfo(fileName).fileName();

    m_documents->setTabText(m_documents->currentIndex(), shownName);
    setWindowTitle(tr("%1[*]").arg(shownName));
    setSyntax(Syntax::Auto);
}

void Sqriptor::readStdin()
{
    QFile *input = new QFile;
    if (input->open(stdin, QFile::ReadOnly)) {
        QSocketNotifier *snr = new QSocketNotifier(input->handle(), QSocketNotifier::Read, input);
        connect (snr, &QSocketNotifier::activated, [=](){
            if (!(textEdit()->length() || textEdit()->isModified())) {
                textEdit()->setText(QString::fromLocal8Bit(input->readAll()));
            }
            input->close();
            delete input;
        });
    } else {
        delete input;
        input = nullptr;
    }
}
// ===========================================

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    Sqriptor sqriptor;
    sqriptor.show();
    bool getStdin = true;
    int counter = 0;
    for (int i = 1; i < argc; ++i) {
        QString arg(argv[i]);
        if (arg.startsWith('-')) {
            continue;
        }
        if (QFile::exists(arg)) {
            sqriptor.open(arg, counter > 0);
            getStdin = false;
            ++counter;
        } else {
            sqriptor.newFile(arg, counter > 0);
            ++counter;
        }
    }
    if (getStdin)
        sqriptor.readStdin();
    QMetaObject::invokeMethod(sqriptor.textEdit(), "setFocus", Qt::QueuedConnection);
    return app.exec();
}