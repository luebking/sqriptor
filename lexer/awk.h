#ifndef QSCILEXERAWK_H
#define QSCILEXERAWK_H

#include <Qsci/qscilexerbash.h>

class QsciLexerAWK : public QsciLexerBash {
    Q_OBJECT
public:
    QsciLexerAWK(QObject *parent);
    const char *keywords(int set) const;
//    void updateColors();
//    virtual void styleText (int start, int end);
    virtual const char *language () const { return "AWK"; }
    virtual QString description(int style) const {Q_UNUSED(style); return "AWK"; }
};

#endif // QSCILEXERAWK_H