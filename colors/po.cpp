static void setColorsPO(QsciLexerPO *lexer)
{
    lexer->setColor(COLOR_FOREGROUND, QsciLexerPO::Default);
    QColor c = COLOR_COMMENT;
    lexer->setColor(c, QsciLexerPO::Comment);
    lexer->setColor(c, QsciLexerPO::ProgrammerComment);
    c = COLOR_NUMBER;
    lexer->setColor(c, QsciLexerPO::MessageId);
    lexer->setColor(c, QsciLexerPO::MessageIdText);
    c = COLOR_STRING;
    lexer->setColor(c, QsciLexerPO::MessageString);
    lexer->setColor(c, QsciLexerPO::MessageStringText);
    c = COLOR_CHAR;
    lexer->setColor(c, QsciLexerPO::MessageContext);
    lexer->setColor(c, QsciLexerPO::MessageContextText);
    lexer->setColor(COLOR_KEYWORD, QsciLexerPO::Reference);
    lexer->setColor(COLOR_OPERATOR, QsciLexerPO::Flags);
    c = COLOR_PREPROC;
    lexer->setColor(c, QsciLexerPO::MessageIdTextEOL);
    lexer->setColor(c, QsciLexerPO::MessageStringTextEOL);
    lexer->setColor(c, QsciLexerPO::MessageContextTextEOL);
    lexer->setColor(COLOR_ERROR, QsciLexerPO::Fuzzy);
}
