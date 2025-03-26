/*
 *   Sqriptor text editor for Qt5
 *   Copyright 2025 by Thomas Lübking <thomas.luebking@gmail.com>
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
#include "lexer/rust.h"


QsciLexerRust::QsciLexerRust(QObject *parent) : QsciLexerCPP(parent)
{
}

const char *QsciLexerRust::keywords(int set) const
{
    if (set == 1) {
        return
            "as break const continue crate else enum extern false fn for "
            "if impl in let loop match mod move mut pub ref "
            "self Self static struct super trait true type use where while "
            "async await dyn "
            "bool integer float char str None "
            "String OsStr OsString Path PathBuf CStr CString Vec Option "
            "Result Box  Rc Arc Cell Mutex RwLock Condvar Duration HashMap "
            "BTreeMap";
    }
    if (set == 2) {
        return 
            "return unsafe println! i8 u8 i32 u32 i64 u64 f32 f64 isize usize";
    }
/*
    if (set == 3) {
    }
*/
/*    if (set == 4) { // operator
    }
*/
    return QsciLexerCPP::keywords(set);
}
