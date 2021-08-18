#include <QColorDialog>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QMimeData>

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
                // todo: update syntax lexers w/ new palette
            }
        });
    }
    config_bak = config;
    updateTiles();
    gs_dlg->show();
}

