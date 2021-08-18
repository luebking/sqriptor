static void setColorsPascal(QsciLexerPascal *lexer)
{
    lexer->setColor(COLOR_FOREGROUND, QsciLexerPascal::Default);
    QColor c = COLOR_COMMENT;
    lexer->setColor(c, QsciLexerPascal::Comment);
    lexer->setColor(c, QsciLexerPascal::CommentLine);
    lexer->setColor(c, QsciLexerPascal::CommentParenthesis);
    c = COLOR_PREPROC;
    lexer->setColor(c, QsciLexerPascal::PreProcessor);
    lexer->setColor(c, QsciLexerPascal::PreProcessorParenthesis);
    lexer->setColor(c, QsciLexerPascal::Asm);
    c = COLOR_NUMBER;
    lexer->setColor(c, QsciLexerPascal::Number);
    lexer->setColor(c, QsciLexerPascal::HexNumber);
    lexer->setColor(COLOR_STRING, QsciLexerPascal::SingleQuotedString);
    lexer->setColor(COLOR_CHAR, QsciLexerPascal::Character);
    c = COLOR_OPERATOR;
    lexer->setColor(c, QsciLexerPascal::Operator);
    lexer->setColor(c, QsciLexerPascal::UnclosedString);
    lexer->setColor(COLOR_KEYWORD, QsciLexerPascal::Keyword);
    // Identifier
}
