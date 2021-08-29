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

#include <QDebug>

#include "lexer/pkgbuild.h"

PkgBuildAPI::PkgBuildAPI(QsciLexer *lexer) : QsciAbstractAPIs(lexer) { }

void PkgBuildAPI::updateAutoCompletionList(const QStringList &context, QStringList &list)
{
    static QStringList api = { "pkgname=()", "pkgver=", "pkgrel=", "epoch=", "pkgdesc=",
        "url=", "license=()", "install=", "changelog=", "source=()", "validpgpkeys=()",
        "noextract=()", "cksums=()", "md5sums=()", "sha1sums=()", "sha224sums=()",
        "sha256sums=()", "sha384sums=()", "sha512sums=()", "b2sums=()", "groups=()",
        "arch=()", "backup=()", "depends=()", "makedepends=()", "checkdepends=()",
        "optdepends=()", "conflicts=()", "provides=()", "replaces=()", "options=()",
        "package(){}", "prepare(){}", "build(){}", "check(){}", "srcdir=", "pkgdir=",
        "pkgbase=" };

    const QString &token = context.last();
    foreach (const QString &str, api) {
        if (str.startsWith(token))
            list << str;
    }
}

QStringList PkgBuildAPI::callTips(const QStringList &context, int commas, QsciScintilla::CallTipsStyle style, QList<int> &shifts)
{
    Q_UNUSED(context); Q_UNUSED(commas); Q_UNUSED(style); Q_UNUSED(shifts);
    static QStringList opts = { "strip", "docs", "libtool", "staticlibs", "emptydirs",
        "zipman", "ccache", "distcc", "buildflags", "makeflags", "debug", "lto" };
//    qDebug() << context << commas << shifts;
    return QStringList();
}


QsciLexerPkgBuild::QsciLexerPkgBuild(QObject *parent) : QsciLexerBash(parent)
{
    setAPIs(new PkgBuildAPI(this));
}
