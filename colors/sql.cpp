static void setColorsSQL(QsciLexerSQL *lexer)
{
    lexer->setColor(COLOR_FOREGROUND, QsciLexerSQL::Default);
    QColor c = COLOR_COMMENT;
    lexer->setColor(c, QsciLexerSQL::Comment);
    lexer->setColor(c, QsciLexerSQL::CommentLine);
    lexer->setColor(c, QsciLexerSQL::CommentDoc);
    lexer->setColor(c, QsciLexerSQL::PlusComment);
    lexer->setColor(c, QsciLexerSQL::CommentLineHash);
    c = COLOR_STRING;
    lexer->setColor(c, QsciLexerSQL::DoubleQuotedString);
    c = COLOR_CHAR;
    lexer->setColor(c, QsciLexerSQL::SingleQuotedString);
    // c = COLOR_PREPROC;
    lexer->setColor(COLOR_NUMBER, QsciLexerSQL::Number);
    c = COLOR_OPERATOR;
    lexer->setColor(c, QsciLexerSQL::Operator);
    lexer->setColor(c, QsciLexerSQL::PlusPrompt);
    lexer->setColor(c, QsciLexerSQL::QuotedOperator);
    c = COLOR_KEYWORD;
    lexer->setColor(c, QsciLexerSQL::CommentDocKeyword);
    lexer->setColor(c, QsciLexerSQL::Keyword);
    lexer->setColor(c, QsciLexerSQL::KeywordSet5);
    lexer->setColor(c, QsciLexerSQL::KeywordSet6);
    lexer->setColor(c, QsciLexerSQL::KeywordSet7);
    lexer->setColor(c, QsciLexerSQL::KeywordSet8);
    lexer->setColor(c, QsciLexerSQL::PlusKeyword);
    c = COLOR_ERROR;
    lexer->setColor(c, QsciLexerSQL::CommentDocKeywordError);
    // Identifier
    // QuotedIdentifier
}
