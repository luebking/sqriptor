static void setColorsProperties(QsciLexerProperties *lexer)
{
    lexer->setColor(COLOR_FOREGROUND, QsciLexerProperties::Default);
    lexer->setColor(COLOR_COMMENT, QsciLexerProperties::Comment);
    lexer->setColor(COLOR_NUMBER, QsciLexerProperties::Key);
    lexer->setColor(COLOR_STRING, QsciLexerProperties::DefaultValue);
    lexer->setColor(COLOR_OPERATOR, QsciLexerProperties::Assignment);
    lexer->setColor(COLOR_PREPROC, QsciLexerProperties::Section);
}
