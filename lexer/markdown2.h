#ifndef QSCILEXERMARKDOWN2_H
#define QSCILEXERMARKDOWN2_H

#include <Qsci/qscilexercustom.h>

class QsciLexerMarkdown2 : public QsciLexerCustom {
    Q_OBJECT
public:
    QsciLexerMarkdown2(QObject *parent);
    void updateColors();
    virtual void styleText (int start, int end);
    virtual const char *language () const { return "Markdown2"; }
    virtual QString description(int style) const {Q_UNUSED(style); return "Markdown2"; }
};

#endif // QSCILEXERMARKDOWN2_H