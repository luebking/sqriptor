#include <QColorDialog>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QScreen>

#include <Qsci/qsciscintilla.h>

#include <QDebug>

#include "sqriptor.h"
#include "ui_settings.h"

static Ui::Settings *gs_ui = nullptr;
static QDialog *gs_dlg = nullptr;
static Config config_bak;

static void updateTiles()
{
    QPalette pal;
    pal = gs_ui->c_background->palette();
    pal.setColor(gs_ui->c_background->backgroundRole(), config_bak.color.bg);
    pal.setColor(gs_ui->c_background->foregroundRole(), config_bak.color.fg);
    gs_ui->c_background->setPalette(pal);
    gs_ui->c_foreground->setPalette(pal);
#define SETPAL(_TILE_, _KEY_) pal.setColor(gs_ui->_TILE_->foregroundRole(), config_bak.color._KEY_); gs_ui->_TILE_->setPalette(pal)
    SETPAL(c_string, string);
    SETPAL(c_char, character);
    SETPAL(c_comment, comment);
    SETPAL(c_preproc, preproc);
    SETPAL(c_number, number);
    SETPAL(c_operator, operateur);
    SETPAL(c_keyword, keyword);
    SETPAL(c_error, error);
}


class ColorDropper : public QObject {
public:
    ColorDropper(QObject *o) : QObject(o) {}
protected:
    bool eventFilter(QObject *o, QEvent *e) {
        if (e->type() == QEvent::DragEnter) {
            QDropEvent *de = static_cast<QDragMoveEvent*>(e);
            de->mimeData()->hasColor() ? de->accept() : de->ignore();
            return false;
        }
        if (e->type() == QEvent::Drop) {
            QDropEvent *de = static_cast<QDropEvent*>(e);
            if (de->mimeData()->hasColor()) {
                QColor c = qvariant_cast<QColor>(de->mimeData()->colorData());
                if (o == gs_ui->c_background) config_bak.color.bg = c;
                else if (o == gs_ui->c_foreground) config_bak.color.fg = c;
                else if (o == gs_ui->c_string) config_bak.color.string = c;
                else if (o == gs_ui->c_char) config_bak.color.character = c;
                else if (o == gs_ui->c_comment) config_bak.color.comment = c;
                else if (o == gs_ui->c_preproc) config_bak.color.preproc = c;
                else if (o == gs_ui->c_number) config_bak.color.number = c;
                else if (o == gs_ui->c_operator) config_bak.color.operateur = c;
                else if (o == gs_ui->c_keyword) config_bak.color.keyword = c;
                else if (o == gs_ui->c_error) config_bak.color.error = c;

                updateTiles();
            }
            return false;
        }
        return false;
    }
};

void Sqriptor::updatePalette()
{
    // todo: update syntax lexers w/ new palette
    for (int syntax = Syntax::Auto + 1; syntax < Syntax::Count; ++syntax)
        setSyntax(static_cast<Syntax>(syntax), nullptr, true); // updateColorsOnly
    for (int i = 0; i < m_documents->count(); ++i) {
        QsciScintilla *doc = textEdit(i);
        doc->setIndentationsUseTabs(config.tab.isTab);
        doc->setTabWidth(config.tab.width);
        int syntax = doc->property("sqriptor_syntax").toInt();
        setSyntax(static_cast<Syntax>(syntax), doc);
        doc->setMarginsFont(config.font);
        doc->setMarginWidth(0, "14444");
    }
    QPalette pal = palette(); // qApp->palette();
    pal.setColor(QPalette::Window, config.color.bg);
    pal.setColor(QPalette::Base, config.color.bg);
    pal.setColor(QPalette::Button, config.color.bg);
    pal.setColor(QPalette::WindowText, config.color.fg);
    pal.setColor(QPalette::Text, config.color.fg);
    pal.setColor(QPalette::ButtonText, config.color.fg);
    setPalette(pal);
}

void Sqriptor::showSettings()
{
    if (!gs_ui) {
        delete gs_dlg; // safe > sorry
        gs_dlg = new QDialog(this);
        gs_ui = new Ui::Settings;
        gs_ui->setupUi(gs_dlg);
        ColorDropper *dropper = new ColorDropper(gs_dlg);
        gs_ui->c_background->installEventFilter(dropper);
        gs_ui->c_foreground->installEventFilter(dropper);
        gs_ui->c_string->installEventFilter(dropper);
        gs_ui->c_char->installEventFilter(dropper);
        gs_ui->c_comment->installEventFilter(dropper);
        gs_ui->c_preproc->installEventFilter(dropper);
        gs_ui->c_number->installEventFilter(dropper);
        gs_ui->c_operator->installEventFilter(dropper);
        gs_ui->c_keyword->installEventFilter(dropper);
        gs_ui->c_error->installEventFilter(dropper);
        QColorDialog *cd = new QColorDialog(gs_dlg);
        cd->hide();
        connect (gs_ui->colorbutton, SIGNAL(clicked()), cd, SLOT(show()));
        connect (gs_ui->colorbutton, SIGNAL(clicked()), cd, SLOT(raise()));
        connect (gs_dlg, &QDialog::finished, [=](int result) {
            cd->hide();
            if (result == QDialog::Accepted) {
                config = config_bak;
                config.changed = true;
                updatePalette();
                QSize size = config.size;
                if (config.sizeMode == Size::Relative && size.width() < 101 && size.height() < 101) {
                    QRect geo = availableGeometry();
                    size.setWidth(qRound(geo.width()*size.width()/100.0f));
                    size.setHeight(qRound(geo.height()*size.height()/100.0f));
                }
                resize(size);
            }
        });
        connect (gs_ui->windowSizeMode, QOverload<int>::of(&QComboBox::currentIndexChanged),
                                        [=](const int index) {
            config_bak.sizeMode = Size::Mode(index);
            gs_ui->windowSizeX->setRange(0, 8194);
            gs_ui->windowSizeY->setRange(0, 8194);
            if (index == Size::Relative) {
                gs_ui->windowSizeX->setSuffix(" %");
                gs_ui->windowSizeY->setSuffix(" %");
                if (gs_ui->windowSizeX->value() > 100) { // comes from relative
                    QRect geo = availableGeometry();
                    gs_ui->windowSizeX->setValue(qRound(gs_ui->windowSizeX->value()*100.0f/geo.width()));
                    gs_ui->windowSizeY->setValue(qRound(gs_ui->windowSizeY->value()*100.0f/geo.height()));
                }
                gs_ui->windowSizeX->setRange(0, 100);
                gs_ui->windowSizeY->setRange(0, 100);
            } else if (index == Size::Absolute) {
                gs_ui->windowSizeX->setSuffix(" px");
                gs_ui->windowSizeY->setSuffix(" px");
                if (gs_ui->windowSizeX->value() < 101) { // comes from relative
                    QRect geo = availableGeometry();
                    gs_ui->windowSizeX->setValue(qRound(geo.width()*gs_ui->windowSizeX->value()/100.0f));
                    gs_ui->windowSizeY->setValue(qRound(geo.height()*gs_ui->windowSizeY->value()/100.0f));
                }
                gs_ui->windowSizeX->setRange(101, 8194);
                gs_ui->windowSizeY->setRange(101, 8194);
            } else if (index == Size::Remember) {
                gs_ui->windowSizeX->setSuffix(" px");
                gs_ui->windowSizeY->setSuffix(" px");
            }
            gs_ui->windowSizeX->setEnabled(index != Size::Remember);
            gs_ui->windowSizeY->setEnabled(index != Size::Remember);
        });
        gs_ui->windowSizeX->setValue(config.sizeMode == Size::Remember ? width() : config.size.width());
        connect (gs_ui->windowSizeX, QOverload<int>::of(&QSpinBox::valueChanged),
                                        [=](int v) {config_bak.size.setWidth(v);});
        gs_ui->windowSizeY->setValue(config.sizeMode == Size::Remember ? height() : config.size.height());
        connect (gs_ui->windowSizeY, QOverload<int>::of(&QSpinBox::valueChanged),
                                        [=](int v) {config_bak.size.setHeight(v);});
        connect (gs_ui->markdownProcessor, &QLineEdit::textEdited,
                                        [=](const QString &t) { config_bak.markdownProcessor = t; });
        connect (gs_ui->previewCSS, &QPlainTextEdit::textChanged,
                                        [=]() { config_bak.previewCSS = gs_ui->previewCSS->toPlainText(); });
        // apply...
        gs_ui->windowSizeMode->setCurrentIndex(config.sizeMode);

        gs_ui->markdownProcessor->setText(config.markdownProcessor);
        gs_ui->previewCSS->setPlainText(config.previewCSS);

        gs_ui->wrapWords->setChecked(config.wrap.words);
        connect (gs_ui->wrapWords, &QCheckBox::stateChanged,
                                        [=](int s) {config_bak.wrap.words = s;});
        gs_ui->wrapIndicator->setChecked(config.wrap.indicator);
        connect (gs_ui->wrapIndicator, &QCheckBox::stateChanged,
                                        [=](int s) {config_bak.wrap.indicator = s;});
        gs_ui->wrapIndicatorPos->setValue(config.wrap.indicatorPos);
        connect (gs_ui->wrapIndicatorPos, QOverload<int>::of(&QSpinBox::valueChanged),
                                        [=](int v) {config_bak.wrap.indicatorPos = v;});

        gs_ui->tabIsTab->setChecked(config.tab.isTab);
        connect (gs_ui->tabIsTab, &QCheckBox::stateChanged,
                                        [=](int s) {config_bak.tab.isTab = s;});
        gs_ui->tabWidth->setValue(config.tab.width);
        connect (gs_ui->tabWidth, QOverload<int>::of(&QSpinBox::valueChanged),
                                        [=](int v) {config_bak.tab.width = v;});
        gs_ui->font->setCurrentFont(config.font);
        connect (gs_ui->font, &QFontComboBox::currentFontChanged,
                                        [=](const QFont &font) {config_bak.font.setFamily(font.family());});
        gs_ui->fontSize->setValue(config.font.pointSize());
        connect (gs_ui->fontSize, QOverload<int>::of(&QSpinBox::valueChanged),
                                        [=](int v) {config_bak.font.setPointSize(v);});
    }
    config_bak = config;
    updateTiles();
    gs_dlg->show();
}

