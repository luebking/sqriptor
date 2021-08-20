static void setColorsVHDL(QsciLexerVHDL *lexer)
{
    lexer->setColor(COLOR_FOREGROUND, QsciLexerVHDL::Default);
    QColor c = COLOR_COMMENT;
    lexer->setColor(c, QsciLexerVHDL::Comment);
    lexer->setColor(c, QsciLexerVHDL::CommentLine);
    lexer->setColor(c, QsciLexerVHDL::CommentBlock);
    c = COLOR_KEYWORD;
    lexer->setColor(c, QsciLexerVHDL::Keyword);
    lexer->setColor(c, QsciLexerVHDL::StandardFunction);
    lexer->setColor(c, QsciLexerVHDL::StandardType);
    lexer->setColor(c, QsciLexerVHDL::KeywordSet7);
    lexer->setColor(COLOR_PREPROC, QsciLexerVHDL::StandardPackage);
    lexer->setColor(COLOR_NUMBER, QsciLexerVHDL::Number);
    c = COLOR_OPERATOR;
    lexer->setColor(c, QsciLexerVHDL::Operator);
    lexer->setColor(c, QsciLexerVHDL::StandardOperator);
    lexer->setColor(COLOR_ERROR, QsciLexerVHDL::UnclosedString);
    lexer->setColor(COLOR_STRING, QsciLexerVHDL::String);
    lexer->setColor(COLOR_CHAR, QsciLexerVHDL::Attribute);
    // Identifier
}
