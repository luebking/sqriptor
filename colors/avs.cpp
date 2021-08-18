static void setColorsAVS(QsciLexerAVS *lexer)
{
    lexer->setColor(COLOR_FOREGROUND, QsciLexerAVS::Default);
    QColor c = COLOR_COMMENT;
    lexer->setColor(c, QsciLexerAVS::BlockComment);
    lexer->setColor(c, QsciLexerAVS::NestedBlockComment);
    lexer->setColor(c, QsciLexerAVS::LineComment);
    lexer->setColor(COLOR_NUMBER, QsciLexerAVS::Number);
    lexer->setColor(COLOR_OPERATOR, QsciLexerAVS::Operator);
    // Identifier
    c = COLOR_STRING;
    lexer->setColor(c, QsciLexerAVS::String);
    lexer->setColor(c, QsciLexerAVS::TripleString);
    c = COLOR_KEYWORD;
    lexer->setColor(c, QsciLexerAVS::Keyword);
    lexer->setColor(c, QsciLexerAVS::Function);
    lexer->setColor(c, QsciLexerAVS::KeywordSet6);
    c = COLOR_PREPROC;
    lexer->setColor(c, QsciLexerAVS::Filter);
    lexer->setColor(c, QsciLexerAVS::Plugin);
    lexer->setColor(COLOR_CHAR, QsciLexerAVS::ClipProperty);
}
