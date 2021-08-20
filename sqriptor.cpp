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
#include <QCloseEvent>
#include <QDateTime>
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
#include <QTabBar>
#include <QTextStream>

#include <Qsci/qsciscintilla.h>
#include <Qsci/qscicommandset.h>
#include <Qsci/qscicommand.h>

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
    setStatusBar(nullptr); // useless waste of screen estate
    readSettings();
    config.changed = false;

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
        checkTimestamp();
    });
    m_documents->setTabPosition(QTabWidget::West);
    m_documents->setTabBarAutoHide(true);
    setCentralWidget(m_documents);

    createUI();
    addTab();

    connect(qApp, &QApplication::focusChanged, [=](QWidget *old, QWidget *now) {
        Q_UNUSED(old)
        if (now == m_documents->currentWidget())
            checkTimestamp();
    });

    setCurrentFile("");
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
    doc->setMarginsFont(config.font);
    doc->setMarginWidth(0, "14444");
    doc->setMarginWidth(1, 16);
    
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
    doc->setTabDrawMode(QsciScintilla::TabStrikeOut);

    if (QsciCommand *cmd = doc->standardCommands()->boundTo(Qt::CTRL + Qt::Key_D))
        cmd->setKey(0); // nobody uses that and I want it for the comment toggle

    connect(doc, SIGNAL(copyAvailable(bool)), SIGNAL(copyAvailable(bool)));
    connect(doc, &QsciScintilla::modificationChanged, [=](){
        m_documents->tabBar()->setTabTextColor(m_documents->indexOf(doc),
                                               doc->isModified() ? config.color.error : QColor());
        if (doc == m_documents->currentWidget())
            setWindowModified(doc->isModified());
    });

    setSyntax(Syntax::None, doc);
    doc->setFocus();

    m_tabMenu->setVisible(m_documents->count() > 6); // soon gonna be 8+
    return m_documents->addTab(doc, "");
}

bool Sqriptor::closeTab(int idx)
{
    if (idx < 0)
        idx = m_documents->currentIndex();
    if (maybeSave(idx)) {
        if (m_documents->count() > 1) {
            delete m_documents->widget(idx);
            m_tabMenu->setVisible(m_documents->count() > 7); // still 8+
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
    if (!config.changed)
        return;
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
    settings.setValue("background", config.color.bg.name(QColor::HexRgb));
    settings.setValue("foreground", config.color.fg.name(QColor::HexRgb));
    settings.setValue("string", config.color.string.name(QColor::HexRgb));
    settings.setValue("char", config.color.character.name(QColor::HexRgb));
    settings.setValue("comment", config.color.comment.name(QColor::HexRgb));
    settings.setValue("preproc", config.color.preproc.name(QColor::HexRgb));
    settings.setValue("number", config.color.number.name(QColor::HexRgb));
    settings.setValue("operator", config.color.operateur.name(QColor::HexRgb));
    settings.setValue("keyword", config.color.keyword.name(QColor::HexRgb));
    settings.setValue("error", config.color.error.name(QColor::HexRgb));
    settings.endGroup();
}

bool Sqriptor::maybeSave(int idx)
{
    if (textEdit(idx)->isModified()) {
        int ret = QMessageBox::warning(this, tr("Sqriptor"),
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
    QString fileName = textEdit(idx)->property("sqriptor_filename").toString();
    if (!fileName.isEmpty() && !QFileInfo::exists(fileName)) {
        int ret = QMessageBox::warning(this, tr("Sqriptor"),
                     tr("The documents file no longer exists!\n"
                        "Do you want to save it?"),
                     QMessageBox::Save | QMessageBox::Default,
                     QMessageBox::Discard,
                     QMessageBox::Cancel | QMessageBox::Escape);
        if (ret == QMessageBox::Save)
            return save();
        else if (ret == QMessageBox::Cancel)
            return false;
    }
    return true;
}

void Sqriptor::updateTimestamp(QObject *o)
{
    o->setProperty("sqriptor_timestamp", QDateTime::currentDateTime());
}

void Sqriptor::checkTimestamp()
{
    QsciScintilla *doc = textEdit();
    QDateTime date = doc->property("sqriptor_timestamp").toDateTime();
    QString fileName = doc->property("sqriptor_filename").toString();
    QFileInfo info(fileName);
    if (info.exists() && info.lastModified() > date) {
        int ret = QMessageBox::warning(this, tr("Sqriptor"),
                     tr("The document file was updated outside Sqriptor.\n"
                        "Do you want to reload it?"),
                     QMessageBox::Yes,
                     QMessageBox::No | QMessageBox::Default);
        if (ret == QMessageBox::Yes)
            loadFile(fileName);
        else
            updateTimestamp(doc); // otherwise we'll be asked forever
    }
}

void Sqriptor::loadFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly)) {
        QMessageBox::warning(this, tr("Sqriptor"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return;
    }

    QsciScintilla *doc = textEdit();
    updateTimestamp(doc);
    QTextStream in(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    doc->setText(in.readAll());

    QString vimHints;
    for (int i = 0; i < doc->lines(); ++i) {
        vimHints = doc->text(i);
        if (vimHints.contains("vim:")) {
            bool isModeline = false; // could be some random line talking about "vim: great editor"
            QStringList vimmodes = vimHints.split(QRegExp("[\\s:]"));
            if ((isModeline = (vimmodes.contains("noexpandtab") || vimmodes.contains("noet"))))
                doc->setIndentationsUseTabs(true);
            else if ((isModeline = (vimmodes.contains("expandtab") || vimmodes.contains("et"))))
                doc->setIndentationsUseTabs(false);
            if (!doc->indentationsUseTabs()) { // if we use tabs, we honor the users width preference
                int tabstop = vimmodes.indexOf(QRegExp("(tabstop|ts)=.*"));
                if (tabstop > -1) {
                    bool ok;
                    tabstop = vimmodes.at(tabstop).section('=',-1).toInt(&ok);
                    if (ok) {
                        isModeline = true;
                        doc->setTabWidth(tabstop);
                    }
                }
                // if (vimmodes.contains("shiftwidth") || vimmodes.contains("sw"))
                // nobody cares about the stupid shiftwidth
            }
            // TODO: apply ft/filetype?
            if (isModeline)
                break;
        }
    }
    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);
}

bool Sqriptor::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly)) {
        QMessageBox::warning(this, tr("Sqriptor"),
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
    updateTimestamp(textEdit());
    return true;
}

void Sqriptor::setCurrentFile(const QString &fileName)
{
    textEdit()->setProperty("sqriptor_filename", fileName);
    textEdit()->setModified(false);
    setWindowModified(false);

    QString shownName;
    if (fileName.isEmpty()) {
        static const char *leet[4] = {"5cr4tchp4d", "$¢r@t¢hp@d", "unt1tl3d", "n3w_f1l3"};
        shownName = leet[rand() % 4];
    }
    else {
        shownName = QFileInfo(fileName).fileName();
    }
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
            QsciScintilla *doc = textEdit();
            if (!(doc->length() || doc->isModified())) {
                doc->setText(QString::fromLocal8Bit(input->readAll()));
                if (doc->property("sqriptor_syntax").toInt() == Syntax::None)
                    setSyntax(Syntax::Auto);
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