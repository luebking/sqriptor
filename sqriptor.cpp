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
#include <QDateTime>
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPoint>
#include <QProcess>
#include <QRegularExpression>
#include <QScreen>
#include <QSettings>
#include <QSize>
#include <QSocketNotifier>
#include <QTabBar>
#include <QTextStream>

#include <Qsci/qsciscintilla.h>
#include <Qsci/qscicommandset.h>
#include <Qsci/qscicommand.h>

#include <unistd.h>

#include "scrollbar.h"
#include "sqriptor.h"

#include <QtDebug>

Config Sqriptor::config;


Sqriptor::Sqriptor()
{
    setStatusBar(nullptr); // useless waste of screen estate
    readSettings();
    config.changed = false;

    m_documents = new QTabWidget;
    m_documents->setDocumentMode(true);
    m_documents->setTabsClosable(true);
    m_documents->setContentsMargins(0, 0, 0, 0);
    connect(m_documents, SIGNAL(tabCloseRequested(int)), SLOT(closeTab(int)));
    connect(m_documents, &QTabWidget::currentChanged, [=](int idx) {
        QsciScintilla *doc = textEdit();
        setWindowModified(doc->isModified());
        setWindowTitle(tr("%1[*]").arg(m_documents->tabText(idx)));
        menuBar()->update();
        indicateCurrentSyntax();
        indicateCurrentEOL();
        m_EolVis->setChecked(doc->eolVisibility());
        m_wrapped->setChecked(doc->wrapMode() != QsciScintilla::WrapNone);
        m_folds->setChecked(doc->folding() != QsciScintilla::NoFoldStyle);
        checkTimestamp();
    });
    m_documents->setTabPosition(QTabWidget::West);
    m_documents->setTabBarAutoHide(true);
    setCentralWidget(m_documents);
    
    updatePalette();
    
    createUI();
    connect(qApp, &QApplication::focusChanged, [=](QWidget *old, QWidget *now) {
        Q_UNUSED(old)
        if (now == m_documents->currentWidget())
            checkTimestamp();
    });
    newFile("", true);
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

void Sqriptor::analyzeSyntaxOnce()
{
    QsciScintilla *doc = qobject_cast<QsciScintilla*>(sender());
    if (!doc)
        return;
    if (doc->property("sqriptor_syntax").toInt() == Syntax::None) {
        const QString fn = doc->property("sqriptor_filename").toString();
        doc->setProperty("sqriptor_filename", "");
        setSyntax(Syntax::Auto, doc);
        doc->setProperty("sqriptor_filename", fn);
    }
    disconnect (textEdit(), SIGNAL(linesChanged()), this, SLOT(analyzeSyntaxOnce()));
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
    connect (textEdit(), SIGNAL(linesChanged()), SLOT(analyzeSyntaxOnce()));
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

class Embracer : public QObject {
    public:
        Embracer(Sqriptor *parent) : QObject(parent) {}
protected:
    bool eventFilter(QObject *obj, QEvent *ev) {
        if (ev->type() == QEvent::KeyPress) {
            QKeyEvent *kev = static_cast<QKeyEvent*>(ev);
            if (kev->modifiers() & Qt::ControlModifier)
                return false;

            Qt::Key other = Qt::Key_unknown;
            switch (kev->key()) {
                case Qt::Key_BracketLeft:   other = Qt::Key_BracketRight; break;
                case Qt::Key_BraceLeft:     other = Qt::Key_BraceRight; break;
                case Qt::Key_Less:          other = Qt::Key_Greater; break;
                case Qt::Key_ParenLeft:     other = Qt::Key_ParenRight; break;
                case Qt::Key_QuoteDbl:      other = Qt::Key_QuoteDbl; break;
                case Qt::Key_Apostrophe:    other = Qt::Key_Apostrophe; break;
            }
            if (other == Qt::Key_unknown)
                return false;

            QsciScintilla *doc = qobject_cast<QsciScintilla*>(obj);
            QString text = doc->selectedText();
            if (text.isEmpty())
                return false;

            doc->replaceSelectedText(QLatin1Char(kev->key()) + text + QLatin1Char(other));
            return true;
        }
        return false;
    }
};

int Sqriptor::addTab()
{
    QsciScintilla *doc = new QsciScintilla;
    doc->replaceVerticalScrollBar(new ScrollBar);
    doc->setFrameShape(QFrame::NoFrame);
    
    doc->setBraceMatching(QsciScintilla::StrictBraceMatch);
    doc->setWhitespaceVisibility(QsciScintilla::WsVisibleOnlyInIndent);
    doc->setEdgeMode(config.wrap.indicator ? QsciScintilla::EdgeLine : QsciScintilla::EdgeNone);
    doc->setEdgeColumn(config.wrap.indicatorPos);
    
    doc->markerDefine(QsciScintilla::Bookmark, 1);
    doc->setMarkerForegroundColor(config.color.bg, 1);
    doc->setMarkerBackgroundColor(palette().color(QPalette::Link), 1);
    doc->markerDefine(QsciScintilla::LeftRectangle, 2);
//    doc->setMarkerForegroundColor(config.color.error, 2);
    doc->setMarkerBackgroundColor(config.color.error, 2);
    doc->setWrapVisualFlags(QsciScintilla::WrapFlagNone, QsciScintilla::WrapFlagInMargin, 0);
    doc->setWrapMode(config.wrap.words ? QsciScintilla::WrapWhitespace : QsciScintilla::WrapNone);
    m_wrapped->setChecked(config.wrap.words);
    doc->setMarginsFont(Sqriptor::config.font);
    doc->setMarginWidth(0, "14444");
    doc->setMarginWidth(1, 16);
    
    doc->setAutoCompletionSource(QsciScintilla::AcsAll);
    doc->setAutoCompletionThreshold(3);
//    doc->setAutoCompletionUseSingle(AcusNever);
//    doc->setAutoCompletionReplaceWord(true);
    
    doc->setIndentationGuides(true);
    doc->setAutoIndent(true);
    doc->setTabIndents(true);
    doc->setBackspaceUnindents(true);
    doc->setIndentationsUseTabs(config.tab.isTab);
    doc->setTabWidth(config.tab.width);
    doc->setTabDrawMode(QsciScintilla::TabStrikeOut);
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
    #define COMBO(_CMB_) QKeyCombination(_CMB_).toCombined()
#else
    #define COMBO(_CMB_) int(_CMB_)
#endif
    if (QsciCommand *cmd = doc->standardCommands()->boundTo(COMBO(Qt::CTRL|Qt::Key_D)))
        cmd->setKey(0); // double line, nobody uses that and I want it for the comment toggle
    if (QsciCommand *cmd = doc->standardCommands()->boundTo(COMBO(Qt::CTRL|Qt::Key_L)))
        cmd->setKey(0); // delete line, nobody uses that and I want it for the filter
    if (QsciCommand *cmd = doc->standardCommands()->boundTo(COMBO(Qt::CTRL|Qt::Key_C)))
        cmd->setKey(0); // we replace this with our filter-aware copy function

    connect(doc, SIGNAL(copyAvailable(bool)), SIGNAL(copyAvailable(bool)));
    connect(doc, &QsciScintilla::modificationChanged, [=](){
        m_documents->tabBar()->setTabTextColor(m_documents->indexOf(doc),
                                               doc->isModified() ? config.color.error : QColor());
        if (doc == m_documents->currentWidget()) {
            setWindowModified(doc->isModified());
            menuBar()->update();
        }
    });
    connect(doc, &QsciScintilla::textChanged, [=]() {
        int line, index;
        doc->getCursorPosition(&line, &index);
        if (doc->text(line).endsWith(" \n"))
            textEdit()->markerAdd(line, 2);
        else
            textEdit()->markerDelete(line, 2);
    });

    setSyntax(Syntax::None, doc);
    doc->setFocus();

    static Embracer *embracer = new Embracer(this);
    doc->installEventFilter(embracer);

    m_tabMenu->setVisible(m_documents->count() > 6); // soon gonna be 8+
    return m_documents->addTab(doc, "");
}

bool Sqriptor::closeTab(int idx)
{
    if (idx < 0)
        idx = m_documents->currentIndex();
    if (maybeSave(idx)) {
        QString fileName = m_documents->widget(idx)->property("sqriptor_filename").toString();
        fileName = QFileInfo(fileName).canonicalFilePath();
        if (!fileName.isEmpty()) {
            config.recentFiles.removeAll(fileName);
            config.recentFiles.prepend(fileName);
            while (config.recentFiles.count() > 16)
                config.recentFiles.removeLast();
        }
        if (m_documents->count() > 1) {
            delete m_documents->widget(idx);
            m_tabMenu->setVisible(m_documents->count() > 7); // still 8+
            textEdit()->setFocus();
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
    if (textEdit()->markersAtLine(line) & 2)
        textEdit()->markerDelete(line, 1);
    else
        textEdit()->markerAdd(line, 1);
}

void Sqriptor::nextBookmark()
{
    int line, index;
    textEdit()->getCursorPosition(&line, &index);
    if (textEdit()->markersAtLine(line) & 2)
        ++line;
    textEdit()->setCursorPosition(textEdit()->markerFindNext(line, 2), 0);
}

void Sqriptor::prevBookmark()
{
    int line, index;
    textEdit()->getCursorPosition(&line, &index);
    if (textEdit()->markersAtLine(line) & 2)
        --line;
    textEdit()->setCursorPosition(textEdit()->markerFindPrevious(line, 2), 0);
}

void Sqriptor::readSettings()
{
    QSettings settings("sqriptor");

    config.sizeMode = Size::Mode(settings.value("sizeMode", Size::Relative).toInt());
    QSize size(settings.value("size", QSize(62,62)).toSize());
    config.size = size;
    if (config.sizeMode == Size::Relative && size.width() < 101 && size.height() < 101) {
        QRect geo = availableGeometry();
        size.setWidth(qRound(geo.width()*size.width()/100.0f));
        size.setHeight(qRound(geo.height()*size.height()/100.0f));
    }
    resize(size);
    settings.beginGroup("wrap");
    config.wrap.words = settings.value("words", false).toBool();
    config.wrap.indicator = settings.value("indicator", true).toBool();
    config.wrap.indicatorPos = settings.value("indicatorPos", 80).toInt();
    settings.endGroup();
    config.recentFiles = settings.value("recent").toStringList();
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
    settings.setValue("recent", config.recentFiles);
    QSize size(width(), height());
    if (config.sizeMode == Size::Remember)
        settings.setValue("size", size);
    if (!config.changed)
        return;
    settings.setValue("sizeMode", config.sizeMode);
    settings.setValue("size", config.size);
    settings.beginGroup("wrap");
    settings.setValue("words", config.wrap.words);
    settings.setValue("indicator", config.wrap.indicator);
    settings.setValue("indicatorPos", config.wrap.indicatorPos);
    settings.endGroup();
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
    QString fileName = textEdit(idx)->property("sqriptor_filename").toString();
    if (textEdit(idx)->isModified()) {
        if (!textEdit(idx)->length() && fileName.isEmpty())
            return true; // don't ask me to save an empty new file
        int ret = QMessageBox::warning(this, tr("Sqriptor"),
                     tr("<html><h2>Save me?</h2>%1 has been modified.<br>"
                        "Do you want to save your changes?</html>").arg(fileName),
                     QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel, QMessageBox::Yes);
        if (ret == QMessageBox::Yes)
            return save();
        else if (ret & (QMessageBox::Cancel|QMessageBox::Escape))
            return false;
    }
    if (!fileName.isEmpty() && !QFileInfo::exists(fileName)) {
        int ret = QMessageBox::warning(this, tr("Sqriptor"),
                     tr("<html><h2>Rescue me?</h2>%1 no longer exists!<br>"
                        "Do you want to save it?</html>").arg(fileName),
                     QMessageBox::Save|QMessageBox::Discard|QMessageBox::Cancel, QMessageBox::Save);
        if (ret == QMessageBox::Save)
            return save();
        else if (ret & (QMessageBox::Cancel|QMessageBox::Escape))
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
                     tr("<html><h2>Update me?</h2>%1 was updated outside Sqriptor.<br>"
                        "Do you want to reload it?</html>").arg(fileName),
                     QMessageBox::Yes|QMessageBox::No, QMessageBox::No);
        if (ret == QMessageBox::Yes)
            loadFile(fileName);
        else
            updateTimestamp(doc); // otherwise we'll be asked forever
    }
}

void Sqriptor::loadFile(const QString &fileName)
{
    QFile file(fileName);
    if (file.size() > 2*1024*1024) {
        int ret = QMessageBox::warning(this, tr("How much do you RAM, Bro?"),
            tr("<b>The file </b>%1<b><br> is really huge!</b> (%2 MB)<br><br>"
            "Sure you want to open that?!").arg(fileName.section('/',-1,-1)).arg(file.size()/(1024*1024)),
                     QMessageBox::Yes|QMessageBox::No, QMessageBox::No);
        if (ret == QMessageBox::No)
            return;
    }
    if (!file.open(QFile::ReadOnly)) {
        QMessageBox::warning(this, tr("Sqriptor"),
                             tr("<html><h2>%2</h2>Cannot read file<br><b>%1</b></html>")
                             .arg(fileName)
                             .arg(file.errorString()));
        return;
    }

    QsciScintilla *doc = textEdit();
    updateTimestamp(doc);
    QTextStream in(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);

    QProcess usr_bin_file;
    usr_bin_file.start("/usr/bin/file", QStringList() << "-b" << "--mime-encoding" << fileName);
    usr_bin_file.waitForFinished(2000);
    QString codec = usr_bin_file.readAllStandardOutput();
    if (!(codec.isEmpty() || codec.contains("utf-8", Qt::CaseInsensitive) ||
        codec.contains("ascii", Qt::CaseInsensitive))) {
        QApplication::restoreOverrideCursor();
        codec = codec.section('\n', 0, 0);
        if (codec.contains("utf-16", Qt::CaseInsensitive))
            QMessageBox::warning(this, tr("Sqriptor"),
                    tr( "<html><h2>Encoding: ill-conceived</h2>"
                        "<i>%1</i> seems to be encoded in<h3 align=\"center\">%2</h3>"
                        "<b>Notice!</b> that it will be <b>saved as UTF-8!</b><br>"
                        "(Feel free to google why UTF-16 is useless)</html>").arg(fileName).arg(codec));
        else if (codec.contains("binary", Qt::CaseInsensitive))
            QMessageBox::warning(this, tr("Sqriptor"),
                    tr( "<html><h2>Binary Blob?</h2>"
                        "<i>%1</i> seems to be<h3 align=\"center\">binary data</h3>"
                        "<b>Do not save the file!</b><br>It would be <b>converted to UTF-8!</b>"
                        "</html>").arg(fileName));
        else
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
            in.setEncoding(*QStringConverter::encodingForName(ask4Codec(codec, fileName).toLatin1()));
#else
            in.setCodec(ask4Codec(codec, fileName).toLatin1());
#endif
        QApplication::setOverrideCursor(Qt::WaitCursor);
    }
    doc->setText(in.readAll());

    if (doc->lines() < 6666) { // this isn't code and even if, it's insane and you don't deserve this scan.
        QString vimHints = doc->text(0);
        if (vimHints.endsWith("\r\n")) // abomination
            doc->setEolMode(QsciScintilla::EolWindows);
        else if (vimHints.endsWith("\r")) // "retarded"
            doc->setEolMode(QsciScintilla::EolMac);
        else
            doc->setEolMode(QsciScintilla::EolUnix);
        static const QRegularExpression blank_colon("[\\s:]");
        static const QRegularExpression tabstop_ts("(tabstop|ts)=.*");
        static const QRegularExpression filetype_ft("(filetype|ft)=.*");
        for (int i = 0; i < doc->lines(); ++i) {
            vimHints = doc->text(i);
            if (vimHints.contains("vim:")) {
                bool isModeline = false; // could be some random line talking about "vim: great editor"
                QStringList vimmodes = vimHints.split(blank_colon);
                if ((isModeline = (vimmodes.contains("noexpandtab") || vimmodes.contains("noet"))))
                    doc->setIndentationsUseTabs(true);
                else if ((isModeline = (vimmodes.contains("expandtab") || vimmodes.contains("et"))))
                    doc->setIndentationsUseTabs(false);
                if (!doc->indentationsUseTabs()) { // if we use tabs, we honor the users width preference
                    int tabstop = vimmodes.indexOf(tabstop_ts);
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
                int filetype = vimmodes.indexOf(filetype_ft);
                if (filetype > -1) {
                    setSyntax(Sqriptor::syntax(vimmodes.at(filetype).section('=',-1)), doc);
                    isModeline = true;
                }
                // TODO: apply ft/filetype?
                if (isModeline)
                    break;
            }
        }
    }
    indicateCurrentEOL();
    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);
}

bool Sqriptor::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly)) {
        QMessageBox::warning(this, tr("Sqriptor"),
                             tr("<html><h2>%2</h2>Cannot write file<br><b>%1</b></html>")
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
    textEdit()->setProperty("sqriptor_filename", fileName.startsWith("scratch:") ? "" : fileName);
    textEdit()->setModified(false);
    setWindowModified(false);

    QString shownName, path;
    if (fileName.isEmpty()) {
        static const char *leet[4] = {"5cr4tchp4d", "$¢r@t¢hp@d", "unt1tl3d", "n3w_f1l3"};
        path = shownName = leet[rand() % 4];
    }
    else {
        QFileInfo info(fileName);
        shownName = info.fileName();
        path = info.absoluteFilePath();
    }
    textEdit()->setProperty("sqriptor_filepath", path);
    menuBar()->update();
    m_documents->setTabText(m_documents->currentIndex(), shownName);
    setWindowTitle(tr("%1[*]").arg(shownName));
    setSyntax(Syntax::Auto);
}

void Sqriptor::readStdin()
{
#ifdef Q_OS_WIN
    if (_isatty(_fileno(stdin)))
        return;
#else
    if (isatty(fileno(stdin)))
        return;
#endif
    QFile *input = new QFile;
    if (input->open(stdin, QFile::ReadOnly)) {
        QSocketNotifier *snr = new QSocketNotifier(input->handle(), QSocketNotifier::Read, input);
        connect (snr, &QSocketNotifier::activated, [=](){
            QsciScintilla *doc = textEdit();
            if (!(doc->length() || doc->isModified()))
                doc->setText(QString::fromLocal8Bit(input->readAll()));
            input->close();
            delete input;
        });
    } else {
        delete input;
        input = nullptr;
    }
}

QRect Sqriptor::availableGeometry() const
{
    QScreen *s = QGuiApplication::screenAt(pos());
    if (!s)
        s = QGuiApplication::primaryScreen();
    if (!s)
        return QRect(0,0,1024,768); // XGA *shrug*
    return s->availableGeometry();
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