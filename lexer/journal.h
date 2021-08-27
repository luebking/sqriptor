#ifndef QSCILEXERJOURNAL_H
#define QSCILEXERJOURNAL_H

#include <Qsci/qscilexercustom.h>

class QsciLexerJournal : public QsciLexerCustom {
    Q_OBJECT
public:
    QsciLexerJournal(QObject *parent);
    void updateColors();
    virtual void styleText (int start, int end);
    virtual const char *language () const { return "Journal"; }
    virtual QString description(int style) const {Q_UNUSED(style); return "Journal"; }
};

#endif // QSCILEXERJOURNAL_H