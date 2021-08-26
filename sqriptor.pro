CONFIG      += qscintilla2

HEADERS     = sqriptor.h lexer/awk.h lexer/markdown2.h lexer/xorg.h
SOURCES     = sqriptor.cpp createui.cpp syntax.cpp filetype.cpp settings.cpp \
              lexer/awk.cpp lexer/markdown2.cpp lexer/xorg.cpp
FORMS       = settings.ui