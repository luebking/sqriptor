static void setColorsD(QsciLexerD *lexer)
{
    lexer->setColor(COLOR_FOREGROUND, QsciLexerD::Default);
    QColor c = COLOR_COMMENT;
    lexer->setColor(c, QsciLexerD::Comment);
    lexer->setColor(c, QsciLexerD::CommentLine);
    lexer->setColor(c, QsciLexerD::CommentDoc);
    lexer->setColor(c, QsciLexerD::CommentLineDoc);
    lexer->setColor(c, QsciLexerD::CommentNested);
    c = COLOR_STRING;
    lexer->setColor(c, QsciLexerD::String);
    lexer->setColor(c, QsciLexerD::RawString);
    lexer->setColor(c, QsciLexerD::BackquoteString);
    lexer->setColor(COLOR_CHAR, QsciLexerD::Character);
    lexer->setColor(COLOR_PREPROC, QsciLexerD::Typedefs);
    lexer->setColor(COLOR_NUMBER, QsciLexerD::Number);
    c = COLOR_OPERATOR;
    lexer->setColor(c, QsciLexerD::Operator);
    lexer->setColor(c, QsciLexerD::CommentDocKeyword);
    c = COLOR_ERROR;
    lexer->setColor(c, QsciLexerD::UnclosedString);
    lexer->setColor(c, QsciLexerD::CommentDocKeywordError);
    c = COLOR_KEYWORD;
    lexer->setColor(c, QsciLexerD::Keyword);
    lexer->setColor(c, QsciLexerD::KeywordSecondary);
    lexer->setColor(c, QsciLexerD::KeywordDoc);
    lexer->setColor(c, QsciLexerD::KeywordSet5);
    lexer->setColor(c, QsciLexerD::KeywordSet6);
    lexer->setColor(c, QsciLexerD::KeywordSet7);
    // Identifier
}
