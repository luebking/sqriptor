static void setColorsBatch(QsciLexerBatch *lexer)
{
    lexer->setColor(COLOR_FOREGROUND, QsciLexerBatch::Default);
    lexer->setColor(COLOR_COMMENT, QsciLexerBatch::Comment);
    QColor c = COLOR_KEYWORD;
    lexer->setColor(COLOR_KEYWORD, QsciLexerBatch::Keyword);
    lexer->setColor(COLOR_KEYWORD, QsciLexerBatch::ExternalCommand);
    lexer->setColor(COLOR_OPERATOR, QsciLexerBatch::Operator);
    c = COLOR_PREPROC;
    lexer->setColor(c, QsciLexerBatch::Label);
    lexer->setColor(c, QsciLexerBatch::HideCommandChar);
    lexer->setColor(COLOR_STRING, QsciLexerBatch::Variable);
}
