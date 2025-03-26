CONFIG      += qscintilla2
QT          += gui widgets
HEADERS     = sqriptor.h scrollbar.h \
              lexer/awk.h lexer/journal.h lexer/markdown2.h lexer/xorg.h \
              lexer/pkgbuild.h  lexer/xorglog.h lexer/nim.h lexer/lisp.h lexer/fontconfig.h \
              lexer/cppqt.h lexer/qtpro.h lexer/glsl.h
SOURCES     = sqriptor.cpp createui.cpp syntax.cpp filetype.cpp settings.cpp scrollbar.cpp \
              lexer/awk.cpp lexer/journal.cpp lexer/markdown2.cpp lexer/xorg.cpp \
              lexer/pkgbuild.cpp lexer/xorglog.cpp lexer/nim.cpp lexer/lisp.cpp lexer/fontconfig.cpp \
              lexer/cppqt.cpp lexer/qtpro.cpp lexer/glsl.cpp
FORMS       = settings.ui