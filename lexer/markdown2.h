#ifndef QSCILEXERTEST_H
#define QSCILEXERTEST_H

#include <Qsci/qscilexercustom.h>

class QsciLexerMarkdown2 : public QsciLexerCustom {
public:
    QsciLexerMarkdown2(QObject *parent);
    void updateColors();
    virtual void styleText (int start, int end);
    virtual const char *language () const { return "Markdown2"; }
    virtual QString description(int style) const {Q_UNUSED(style); return "Markdown2"; }
};

static void setColorsMarkdown2(QsciLexerMarkdown2 *lexer)
{
    lexer->updateColors();
}

#endif // QSCILEXERTEST_H