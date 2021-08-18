static void setColorsBash(QsciLexerBash *lexer)
{
    lexer->setColor(COLOR_FOREGROUND, QsciLexerBash::Default);
    lexer->setColor(COLOR_ERROR, QsciLexerBash::Error);
    lexer->setColor(COLOR_COMMENT, QsciLexerBash::Comment);
    lexer->setColor(COLOR_NUMBER, QsciLexerBash::Number);
    lexer->setColor(COLOR_KEYWORD, QsciLexerBash::Keyword);
    QColor c = COLOR_STRING;
    lexer->setColor(c, QsciLexerBash::DoubleQuotedString);
    lexer->setColor(c, QsciLexerBash::SingleQuotedHereDocument);
    c = COLOR_CHAR;
    lexer->setColor(c, QsciLexerBash::SingleQuotedString);
    lexer->setColor(c, QsciLexerBash::HereDocumentDelimiter);
    lexer->setColor(COLOR_OPERATOR, QsciLexerBash::Operator);
    c = COLOR_PREPROC;
    lexer->setColor(c, QsciLexerBash::Scalar);
    lexer->setColor(c, QsciLexerBash::ParameterExpansion);
    lexer->setColor(c, QsciLexerBash::Backticks);
}
