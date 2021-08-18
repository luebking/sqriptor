static void setColorsMatlab(QsciLexerMatlab *lexer)
{
    lexer->setColor(COLOR_FOREGROUND, QsciLexerMatlab::Default);
    lexer->setColor(COLOR_COMMENT, QsciLexerMatlab::Comment);
    lexer->setColor(COLOR_STRING, QsciLexerMatlab::DoubleQuotedString);
    lexer->setColor(COLOR_CHAR, QsciLexerMatlab::SingleQuotedString);
    lexer->setColor(COLOR_NUMBER, QsciLexerMatlab::Number);
    lexer->setColor(COLOR_OPERATOR, QsciLexerMatlab::Operator);
    lexer->setColor(COLOR_KEYWORD, QsciLexerMatlab::Keyword);
    lexer->setColor(COLOR_PREPROC, QsciLexerMatlab::Command);
    // Identifier
}
