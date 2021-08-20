static void setColorsCoffeeScript(QsciLexerCoffeeScript *lexer)
{
    lexer->setColor(COLOR_FOREGROUND, QsciLexerCoffeeScript::Default);
    QColor c = COLOR_COMMENT;
    lexer->setColor(c, QsciLexerCoffeeScript::Comment);
    lexer->setColor(c, QsciLexerCoffeeScript::CommentLine);
    lexer->setColor(c, QsciLexerCoffeeScript::CommentDoc);
    lexer->setColor(c, QsciLexerCoffeeScript::CommentBlock);
    lexer->setColor(c, QsciLexerCoffeeScript::BlockRegexComment);
    c = COLOR_STRING;
    lexer->setColor(c, QsciLexerCoffeeScript::DoubleQuotedString);
    lexer->setColor(c, QsciLexerCoffeeScript::VerbatimString);
    lexer->setColor(COLOR_CHAR, QsciLexerCoffeeScript::SingleQuotedString);
    c = COLOR_PREPROC;
    lexer->setColor(c, QsciLexerCoffeeScript::PreProcessor);
    lexer->setColor(c, QsciLexerCoffeeScript::Regex);
    lexer->setColor(c, QsciLexerCoffeeScript::BlockRegex);
    lexer->setColor(c, QsciLexerCoffeeScript::InstanceProperty);
    c = COLOR_NUMBER;
    lexer->setColor(c, QsciLexerCoffeeScript::Number);
    lexer->setColor(c, QsciLexerCoffeeScript::UUID);
    c = COLOR_OPERATOR;
    lexer->setColor(c, QsciLexerCoffeeScript::Operator);
    c = COLOR_KEYWORD;
    lexer->setColor(c, QsciLexerCoffeeScript::Keyword);
    lexer->setColor(c, QsciLexerCoffeeScript::KeywordSet2);
    lexer->setColor(c, QsciLexerCoffeeScript::GlobalClass);
    lexer->setColor(c, QsciLexerCoffeeScript::CommentDocKeyword);
    c = COLOR_ERROR;
    lexer->setColor(c, QsciLexerCoffeeScript::CommentDocKeywordError);
    lexer->setColor(c, QsciLexerCoffeeScript::UnclosedString);
    // Identifier
}
