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

#include <QHash>
#include <QRegularExpression>
#include <QString>
#include <QDebug>

#include "sqriptor.h"

/*
None, AVS, Bash, Batch, CMake, CoffeeScript, CPP, CSS,
D, Diff, EDIFACT, Fortran77, HTML, JSON, Lua, Makefile,
Markdown2, Matlab, Pascal, Perl, PO, PostScript, POV, Properties,
Python, Ruby, Spice, SQL, TCL, TeX, Verilog, VHDL, YAML, 
CSharp, IDL, Java, JavaScript, Fortran, XML, Octave
*/

Sqriptor::Syntax Sqriptor::syntax(QString suffix) const {
    suffix = suffix.section('/', -1);
    if (suffix == "CMakeLists.txt")
        return Syntax::CMake;
    static QRegularExpression xorg_n_log("^xorg\\.[0-9\\.]*log\(\\.old)*$", QRegularExpression::CaseInsensitiveOption);
    if (xorg_n_log.match(suffix).hasMatch())
        return Syntax::XorgLog;
    suffix = suffix.section('.', -1);
    static QHash<QString, Syntax> suffixes;
    if (!suffixes.isEmpty())
        return suffixes.value(suffix, Syntax::Auto);
    suffixes.reserve(100);
    suffixes["cpp"] = Syntax::CPP;
    suffixes["c++"] = Syntax::CPP;
    suffixes["cxx"] = Syntax::CPP;
    suffixes["cc"] = Syntax::CPP;
    suffixes["c"] = Syntax::CPP;
    suffixes["h"] = Syntax::CPP;
    suffixes["hh"] = Syntax::CPP;
    suffixes["cs"] = Syntax::CSharp;
    suffixes["html"] = Syntax::HTML;
    suffixes["htm"] = Syntax::HTML;
    suffixes["php"] = Syntax::HTML;
    suffixes["php3"] = Syntax::HTML;
    suffixes["php4"] = Syntax::HTML;
    suffixes["php5"] = Syntax::HTML;
    suffixes["phps"] = Syntax::HTML;
    suffixes["phtml"] = Syntax::HTML;
    suffixes["xml"] = Syntax::XML;
    suffixes["sh"] = Syntax::Bash;
    suffixes["bash"] = Syntax::Bash; // shebang
    suffixes["zsh"] = Syntax::Bash; // dto.
    suffixes["fish"] = Syntax::Bash; // you're welcome…
    suffixes["PKGBUILD"] = Syntax::PkgBuild;
    suffixes["md"] = Syntax::Markdown2;
    suffixes["pl"] = Syntax::Perl;
    suffixes["plx"] = Syntax::Perl;
    suffixes["pm"] = Syntax::Perl;
    suffixes["perl"] = Syntax::Perl;
    suffixes["py"] = Syntax::Python;
    suffixes["python"] = Syntax::Python;
    suffixes["python2"] = Syntax::Python; // shebang
    suffixes["python3"] = Syntax::Python; // dto.
    suffixes["ps"] = Syntax::PostScript;
    suffixes["rb"] = Syntax::Ruby;
    suffixes["java"] = Syntax::Java;
    suffixes["js"] = Syntax::JavaScript;
    suffixes["f77"] = Syntax::Fortran77;
    suffixes["f"] = Syntax::Fortran;
    suffixes["F"] = Syntax::Fortran;
    suffixes["f03"] = Syntax::Fortran;
    suffixes["F03"] = Syntax::Fortran;
    suffixes["f08"] = Syntax::Fortran;
    suffixes["F08"] = Syntax::Fortran;
    suffixes["f18"] = Syntax::Fortran;
    suffixes["F18"] = Syntax::Fortran;
    suffixes["f90"] = Syntax::Fortran;
    suffixes["F90"] = Syntax::Fortran;
    suffixes["f95"] = Syntax::Fortran;
    suffixes["F95"] = Syntax::Fortran;
    suffixes["pas"] = Syntax::Pascal;
    suffixes["pp"] = Syntax::Pascal;
    suffixes["json"] = Syntax::JSON;
    suffixes["bat"] = Syntax::Batch;
    suffixes["lua"] = Syntax::Lua;
    suffixes["css"] = Syntax::CSS;
    suffixes["tex"] = Syntax::TeX;
    suffixes["diff"] = Syntax::Diff;
    suffixes["patch"] = Syntax::Diff;
    suffixes["txt"] = Syntax::None;
    suffixes["m"] = Syntax::Matlab;
    suffixes["sql"] = Syntax::SQL;
    suffixes["yaml"] = Syntax::YAML;
    suffixes["yml"] = Syntax::YAML;
    suffixes["vhd"] = Syntax::VHDL;
    suffixes["tcl"] = Syntax::TCL;
    suffixes["tclsh"] = Syntax::TCL; // shebang
    suffixes["coffee"] = Syntax::CoffeeScript;
    suffixes["avs"] = Syntax::AVS;
    suffixes["ini"] = Syntax::Properties;
    suffixes["conf"] = Syntax::Properties;
    suffixes["awk"] = Syntax::AWK;
    suffixes["nim"] = Syntax::NIM;
    suffixes["lsp"] = Syntax::LISP;
    suffixes["lisp"] = Syntax::LISP;
    suffixes["el"] = Syntax::LISP;
    
    suffixes["Makefile"] = Syntax::Makefile;

    return suffixes.value(suffix, Syntax::Auto);
}