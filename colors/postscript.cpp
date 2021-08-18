static void setColorsPostScript(QsciLexerPostScript *lexer)
{
    QColor c = COLOR_FOREGROUND;
    lexer->setColor(c, QsciLexerPostScript::Default);
    lexer->setColor(c, QsciLexerPostScript::Text);
    c = COLOR_COMMENT;
    lexer->setColor(c, QsciLexerPostScript::Comment);
    lexer->setColor(c, QsciLexerPostScript::DSCComment);
    lexer->setColor(COLOR_CHAR, QsciLexerPostScript::DSCCommentValue);
    c = COLOR_NUMBER;
    lexer->setColor(c, QsciLexerPostScript::Number);
    lexer->setColor(c, QsciLexerPostScript::HexString);
    lexer->setColor(c, QsciLexerPostScript::Base85String);
    lexer->setColor(COLOR_STRING, QsciLexerPostScript::Literal);
    lexer->setColor(COLOR_KEYWORD, QsciLexerPostScript::Keyword);
    c = COLOR_OPERATOR;
    lexer->setColor(c, QsciLexerPostScript::ArrayParenthesis);
    lexer->setColor(c, QsciLexerPostScript::DictionaryParenthesis);
    lexer->setColor(c, QsciLexerPostScript::ProcedureParenthesis);
    c = COLOR_PREPROC;
    lexer->setColor(c, QsciLexerPostScript::Name);
    lexer->setColor(c, QsciLexerPostScript::ImmediateEvalLiteral);
    lexer->setColor(COLOR_ERROR, QsciLexerPostScript::BadStringCharacter);
}
