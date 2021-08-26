#ifndef QSCILEXERXORG_H
#define QSCILEXERXORG_H

#include <Qsci/qscilexercustom.h>

class QsciLexerXorg : public QsciLexerCustom {
    Q_OBJECT
public:
    QsciLexerXorg(QObject *parent);
//    const char *keywords(int set) const;
    void updateColors();
    virtual void styleText (int start, int end);
    virtual const char *language () const { return "Xorg"; }
    virtual QString description(int style) const {Q_UNUSED(style); return "Xorg"; }
/*    const char* blockEnd(int *style = 0) const;
    const char* blockStart(int *style = 0) const;
    const char* blockStartKeyword(int *style = 0) const;*/

};

#endif // QSCILEXERXORG_H