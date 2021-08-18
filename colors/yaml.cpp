static void setColorsYAML(QsciLexerYAML *lexer)
{
    lexer->setColor(COLOR_FOREGROUND, QsciLexerYAML::Default);
    lexer->setColor(COLOR_COMMENT, QsciLexerYAML::Comment);
    lexer->setColor(COLOR_KEYWORD, QsciLexerYAML::Keyword);
    lexer->setColor(COLOR_PREPROC, QsciLexerYAML::DocumentDelimiter);
    lexer->setColor(COLOR_NUMBER, QsciLexerYAML::Number);
    lexer->setColor(COLOR_OPERATOR, QsciLexerYAML::Operator);
    lexer->setColor(COLOR_STRING, QsciLexerYAML::TextBlockMarker);
    lexer->setColor(COLOR_CHAR, QsciLexerYAML::Reference);
    lexer->setColor(COLOR_ERROR, QsciLexerYAML::SyntaxErrorMarker);
    // Identifier
}
