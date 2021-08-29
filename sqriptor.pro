CONFIG      += qscintilla2

HEADERS     = sqriptor.h lexer/awk.h lexer/journal.h lexer/markdown2.h \
              lexer/xorg.h lexer/pkgbuild.h
SOURCES     = sqriptor.cpp createui.cpp syntax.cpp filetype.cpp settings.cpp \
              lexer/awk.cpp lexer/journal.cpp lexer/markdown2.cpp lexer/xorg.cpp \
              lexer/pkgbuild.cpp
FORMS       = settings.ui