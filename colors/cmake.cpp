static void setColorsCMake(QsciLexerCMake *lexer)
{
    lexer->setColor(COLOR_FOREGROUND, QsciLexerCMake::Default);
    lexer->setColor(COLOR_COMMENT, QsciLexerCMake::Comment);
    QColor c = COLOR_KEYWORD;
    lexer->setColor(c, QsciLexerCMake::Function);
    lexer->setColor(c, QsciLexerCMake::KeywordSet3);
    c = COLOR_STRING;
    lexer->setColor(c, QsciLexerCMake::String);
    lexer->setColor(c, QsciLexerCMake::StringVariable);
    // Variable
    c = COLOR_PREPROC;
    lexer->setColor(c, QsciLexerCMake::Label);
    lexer->setColor(c, QsciLexerCMake::BlockWhile);
    lexer->setColor(c, QsciLexerCMake::BlockForeach);
    lexer->setColor(c, QsciLexerCMake::BlockIf);
    lexer->setColor(c, QsciLexerCMake::BlockMacro);
    lexer->setColor(COLOR_NUMBER, QsciLexerCMake::Number);
    lexer->setColor(COLOR_OPERATOR, QsciLexerCMake::StringLeftQuote);
    lexer->setColor(COLOR_CHAR, QsciLexerCMake::StringRightQuote);
}
