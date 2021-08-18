static void setColorsFortran77(QsciLexerFortran77 *lexer)
{
    lexer->setColor(COLOR_FOREGROUND, QsciLexerFortran77::Default);
    lexer->setColor(COLOR_COMMENT, QsciLexerFortran77::Comment);
    lexer->setColor(COLOR_STRING, QsciLexerFortran77::DoubleQuotedString);
    lexer->setColor(COLOR_CHAR, QsciLexerFortran77::SingleQuotedString);
    QColor c = COLOR_PREPROC;
    lexer->setColor(c, QsciLexerFortran77::PreProcessor);
    lexer->setColor(c, QsciLexerFortran77::Label);
    lexer->setColor(COLOR_NUMBER, QsciLexerFortran77::Number);
    c = COLOR_OPERATOR;
    lexer->setColor(c, QsciLexerFortran77::Operator);
    lexer->setColor(c, QsciLexerFortran77::UnclosedString);
    lexer->setColor(c, QsciLexerFortran77::DottedOperator);
    lexer->setColor(c, QsciLexerFortran77::Continuation);
    c = COLOR_KEYWORD;
    lexer->setColor(c, QsciLexerFortran77::Keyword);
    lexer->setColor(c, QsciLexerFortran77::IntrinsicFunction);
    lexer->setColor(c, QsciLexerFortran77::ExtendedFunction);
    // Identifier
}
