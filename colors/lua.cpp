static void setColorsLua(QsciLexerLua *lexer)
{
    lexer->setColor(COLOR_FOREGROUND, QsciLexerLua::Default);
    QColor c = COLOR_COMMENT;
    lexer->setColor(c, QsciLexerLua::Comment);
    lexer->setColor(c, QsciLexerLua::LineComment);
    c = COLOR_STRING;
    lexer->setColor(c, QsciLexerLua::String);
    c = COLOR_CHAR;
    lexer->setColor(c, QsciLexerLua::LiteralString);
    lexer->setColor(c, QsciLexerLua::Character);
    c = COLOR_PREPROC;
    lexer->setColor(c, QsciLexerLua::Preprocessor);
    lexer->setColor(c, QsciLexerLua::Label);
    lexer->setColor(c, QsciLexerLua::BasicFunctions);
    lexer->setColor(c, QsciLexerLua::StringTableMathsFunctions);
    lexer->setColor(c, QsciLexerLua::CoroutinesIOSystemFacilities);
    c = COLOR_NUMBER;
    lexer->setColor(c, QsciLexerLua::Number);
    lexer->setColor(c, QsciLexerCPP::UUID);
    c = COLOR_OPERATOR;
    lexer->setColor(c, QsciLexerLua::Operator);
    lexer->setColor(COLOR_ERROR, QsciLexerLua::UnclosedString);
    c = COLOR_KEYWORD;
    lexer->setColor(c, QsciLexerLua::Keyword);
    lexer->setColor(c, QsciLexerLua::KeywordSet5);
    lexer->setColor(c, QsciLexerLua::KeywordSet6);
    lexer->setColor(c, QsciLexerLua::KeywordSet7);
    lexer->setColor(c, QsciLexerLua::KeywordSet8);
    // Identifier
}