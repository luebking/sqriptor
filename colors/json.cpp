static void setColorsJSON(QsciLexerJSON *lexer)
{
    lexer->setColor(COLOR_FOREGROUND, QsciLexerJSON::Default);
    QColor c = COLOR_COMMENT;
    lexer->setColor(c, QsciLexerJSON::CommentBlock);
    lexer->setColor(c, QsciLexerJSON::CommentLine);
    lexer->setColor(COLOR_STRING, QsciLexerJSON::String);
    c = COLOR_CHAR;
    lexer->setColor(c, QsciLexerJSON::IRI);
    lexer->setColor(c, QsciLexerJSON::IRICompact);
    lexer->setColor(c, QsciLexerJSON::Property);
    lexer->setColor(COLOR_PREPROC, QsciLexerJSON::EscapeSequence);
    lexer->setColor(COLOR_NUMBER, QsciLexerJSON::Number);
    c = COLOR_OPERATOR;
    lexer->setColor(c, QsciLexerJSON::Operator);
    c = COLOR_KEYWORD;
    lexer->setColor(c, QsciLexerJSON::Keyword);
    lexer->setColor(c, QsciLexerJSON::KeywordLD);
    c = COLOR_ERROR;
    lexer->setColor(c, QsciLexerJSON::Error);
    lexer->setColor(c, QsciLexerJSON::UnclosedString);
}
