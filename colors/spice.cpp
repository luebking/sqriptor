static void setColorsSpice(QsciLexerSpice *lexer)
{
    lexer->setColor(COLOR_FOREGROUND, QsciLexerSpice::Default);
    lexer->setColor(COLOR_COMMENT, QsciLexerSpice::Comment);
    lexer->setColor(COLOR_NUMBER, QsciLexerSpice::Number);
    lexer->setColor(COLOR_OPERATOR, QsciLexerSpice::Command);
    lexer->setColor(COLOR_PREPROC, QsciLexerSpice::Delimiter);
    lexer->setColor(COLOR_KEYWORD, QsciLexerSpice::Function);
    lexer->setColor(COLOR_CHAR, QsciLexerSpice::Parameter);
    lexer->setColor(COLOR_STRING, QsciLexerSpice::Value);
    // Identifier
}
