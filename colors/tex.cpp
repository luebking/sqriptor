static void setColorsTeX(QsciLexerTeX *lexer)
{
    QColor c = COLOR_FOREGROUND;
    lexer->setColor(c, QsciLexerTeX::Default);
    lexer->setColor(c, QsciLexerTeX::Text);
    lexer->setColor(COLOR_KEYWORD, QsciLexerTeX::Command);
    lexer->setColor(COLOR_STRING, QsciLexerTeX::Group);
    lexer->setColor(COLOR_NUMBER, QsciLexerTeX::Symbol);
    lexer->setColor(COLOR_PREPROC, QsciLexerTeX::Special);
}
