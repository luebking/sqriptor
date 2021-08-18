static void setColorsPython(QsciLexerPython *lexer)
{
    lexer->setColor(COLOR_FOREGROUND, QsciLexerPython::Default);
    QColor c = COLOR_COMMENT;
    lexer->setColor(c, QsciLexerPython::Comment);
    lexer->setColor(c, QsciLexerPython::CommentBlock);
    c = COLOR_STRING;
    lexer->setColor(c, QsciLexerPython::DoubleQuotedString);
    lexer->setColor(c, QsciLexerPython::TripleDoubleQuotedString);
    c = COLOR_CHAR;
    lexer->setColor(c, QsciLexerPython::SingleQuotedString);
    lexer->setColor(c, QsciLexerPython::TripleSingleQuotedString);
    c = COLOR_PREPROC;
    lexer->setColor(c, QsciLexerPython::HighlightedIdentifier);
    lexer->setColor(c, QsciLexerPython::Decorator);
    lexer->setColor(c, QsciLexerPython::Keyword); // yeah, it's odd
    lexer->setColor(COLOR_NUMBER, QsciLexerPython::Number);
    c = COLOR_OPERATOR;
    lexer->setColor(c, QsciLexerPython::Operator);
    lexer->setColor(c, QsciLexerPython::UnclosedString);
    c = COLOR_KEYWORD;
    lexer->setColor(c, QsciLexerPython::ClassName);
    lexer->setColor(c, QsciLexerPython::FunctionMethodName);
    c = COLOR_ERROR;
    lexer->setColor(c, QsciLexerPython::DoubleQuotedFString);
    lexer->setColor(c, QsciLexerPython::SingleQuotedFString);
    lexer->setColor(c, QsciLexerPython::TripleSingleQuotedFString);
    lexer->setColor(c, QsciLexerPython::TripleDoubleQuotedFString);
    // Identifier
}
