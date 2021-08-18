static void setColorsDiff(QsciLexerDiff *lexer)
{
    lexer->setColor(COLOR_FOREGROUND, QsciLexerDiff::Default);
    
    QColor c = COLOR_COMMENT;
    lexer->setColor(c, QsciLexerDiff::Comment);
    lexer->setColor(c, QsciLexerDiff::Header);
    
    lexer->setColor(COLOR_NUMBER, QsciLexerDiff::Position);
    
    lexer->setColor(COLOR_KEYWORD, QsciLexerDiff::Command);

    lexer->setColor(COLOR_OPERATOR, QsciLexerDiff::LineChanged);
    
    c = COLOR_STRING;
    lexer->setColor(c, QsciLexerDiff::LineAdded);
    lexer->setColor(c, QsciLexerDiff::AddingPatchAdded);
    lexer->setColor(c, QsciLexerDiff::RemovingPatchAdded);
    
    c = COLOR_ERROR;
    lexer->setColor(c, QsciLexerDiff::LineRemoved);
    lexer->setColor(c, QsciLexerDiff::AddingPatchRemoved);
    lexer->setColor(c, QsciLexerDiff::RemovingPatchRemoved);
}
