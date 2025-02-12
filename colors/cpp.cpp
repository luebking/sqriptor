static void setColorsCPP(QsciLexerCPP *lexer)
{
    lexer->setColor(COLOR_FOREGROUND, QsciLexerCPP::Default);
    QColor c = COLOR_COMMENT;
    lexer->setColor(c, QsciLexerCPP::Comment);
    lexer->setColor(c, QsciLexerCPP::CommentLine);
    lexer->setColor(c, QsciLexerCPP::CommentDoc);
    lexer->setColor(c, QsciLexerCPP::CommentLineDoc);
    lexer->setColor(c, QsciLexerCPP::PreProcessorComment);
    lexer->setColor(c, QsciLexerCPP::PreProcessorCommentLineDoc);
    c = COLOR_STRING;
    lexer->setColor(c, QsciLexerCPP::DoubleQuotedString);
    lexer->setColor(c, QsciLexerCPP::VerbatimString);
    lexer->setColor(c, QsciLexerCPP::RawString);
    lexer->setColor(c, QsciLexerCPP::TripleQuotedVerbatimString);
    lexer->setColor(c, QsciLexerCPP::HashQuotedString);
    c = COLOR_CHAR;
    lexer->setColor(c, QsciLexerCPP::SingleQuotedString);
    lexer->setColor(c, QsciLexerCPP::EscapeSequence);
    c = COLOR_PREPROC;
    lexer->setColor(c, QsciLexerCPP::PreProcessor);
    lexer->setColor(c, QsciLexerCPP::Regex);
    lexer->setColor(c, QsciLexerCPP::TaskMarker);
    lexer->setColor(c, QsciLexerCPP::UserLiteral);
    lexer->setColor(c, QsciLexerCPP::KeywordSet2); // we use that for Qt macros
    //lexer->setColor(c, QsciLexerCPP::Identifier); // everything is an identifier…
    c = COLOR_NUMBER;
    lexer->setColor(c, QsciLexerCPP::Number);
    lexer->setColor(c, QsciLexerCPP::UUID);
    c = COLOR_OPERATOR;
    lexer->setColor(c, QsciLexerCPP::Operator);
    lexer->setColor(c, QsciLexerCPP::CommentDocKeyword);
    c = COLOR_KEYWORD;
    lexer->setColor(c, QsciLexerCPP::Keyword);
    lexer->setColor(c, QsciLexerCPP::GlobalClass);
    c = COLOR_ERROR;
    lexer->setColor(c, QsciLexerCPP::CommentDocKeywordError);
    lexer->setColor(c, QsciLexerCPP::UnclosedString);
}
