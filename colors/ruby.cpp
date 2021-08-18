static void setColorsRuby(QsciLexerRuby *lexer)
{
    lexer->setColor(COLOR_FOREGROUND, QsciLexerRuby::Default);
    QColor c = COLOR_COMMENT;
    lexer->setColor(c, QsciLexerRuby::Comment);
    lexer->setColor(c, QsciLexerRuby::ModuleName);
    c = COLOR_STRING;
    lexer->setColor(c, QsciLexerRuby::DoubleQuotedString);
    lexer->setColor(c, QsciLexerRuby::PercentStringQ);
    lexer->setColor(c, QsciLexerRuby::Stdin);
    lexer->setColor(c, QsciLexerRuby::HereDocument);
    c = COLOR_CHAR;
    lexer->setColor(c, QsciLexerRuby::SingleQuotedString);
    lexer->setColor(c, QsciLexerRuby::PercentStringq);
    c = COLOR_PREPROC;
    lexer->setColor(c, QsciLexerRuby::DataSection);
    lexer->setColor(c, QsciLexerRuby::Keyword); // yeah, it's odd
    lexer->setColor(c, QsciLexerRuby::Regex);
    lexer->setColor(c, QsciLexerRuby::PercentStringr);
    lexer->setColor(c, QsciLexerRuby::Symbol);
    c = COLOR_NUMBER;
    lexer->setColor(c, QsciLexerRuby::Number);
    lexer->setColor(c, QsciLexerRuby::PercentStringw);
    lexer->setColor(c, QsciLexerRuby::POD);
    c = COLOR_OPERATOR;
    lexer->setColor(c, QsciLexerRuby::Operator);
    lexer->setColor(c, QsciLexerRuby::HereDocumentDelimiter);
    lexer->setColor(c, QsciLexerRuby::Backticks);
    lexer->setColor(c, QsciLexerRuby::PercentStringx);
    c = COLOR_KEYWORD;
    lexer->setColor(c, QsciLexerRuby::ClassName);
    lexer->setColor(c, QsciLexerRuby::FunctionMethodName);
    lexer->setColor(c, QsciLexerRuby::DemotedKeyword);
    lexer->setColor(c, QsciLexerRuby::Global);
    c = COLOR_ERROR;
    lexer->setColor(c, QsciLexerRuby::Error);
    lexer->setColor(c, QsciLexerRuby::Stderr);
    lexer->setColor(c, QsciLexerRuby::ClassVariable);
    // Identifier
    // Stdout
    // InstanceVariable
}
