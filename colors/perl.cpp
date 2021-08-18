static void setColorsPerl(QsciLexerPerl *lexer)
{
    lexer->setColor(COLOR_FOREGROUND, QsciLexerPerl::Default);
    QColor c = COLOR_COMMENT;
    lexer->setColor(c, QsciLexerPerl::Comment);
    lexer->setColor(c, QsciLexerPerl::POD);
    lexer->setColor(c, QsciLexerPerl::PODVerbatim);
    lexer->setColor(c, QsciLexerPerl::Translation);
    lexer->setColor(c, QsciLexerPerl::DataSection);
    
    c = COLOR_STRING;
    lexer->setColor(c, QsciLexerPerl::DoubleQuotedString);
    lexer->setColor(c, QsciLexerPerl::QuotedStringQQ);
    lexer->setColor(c, QsciLexerPerl::QuotedStringQQVar);
    lexer->setColor(c, QsciLexerPerl::DoubleQuotedStringVar);
    lexer->setColor(c, QsciLexerPerl::SubstitutionVar);
    lexer->setColor(c, QsciLexerPerl::DoubleQuotedHereDocument);
    lexer->setColor(c, QsciLexerPerl::DoubleQuotedHereDocumentVar);
    
    c = COLOR_CHAR;
    lexer->setColor(c, QsciLexerPerl::SingleQuotedString);
    lexer->setColor(c, QsciLexerPerl::QuotedStringQ);
    lexer->setColor(c, QsciLexerPerl::SingleQuotedHereDocument);
    
    c = COLOR_PREPROC;
    lexer->setColor(c, QsciLexerPerl::Regex);
    lexer->setColor(c, QsciLexerPerl::RegexVar);
    lexer->setColor(c, QsciLexerPerl::QuotedStringQR);
    lexer->setColor(c, QsciLexerPerl::QuotedStringQRVar);
    lexer->setColor(c, QsciLexerPerl::SymbolTable);
    lexer->setColor(c, QsciLexerPerl::FormatIdentifier);
    lexer->setColor(c, QsciLexerPerl::QuotedStringQW);

    c = COLOR_NUMBER;
    lexer->setColor(c, QsciLexerPerl::Number);
    lexer->setColor(c, QsciLexerPerl::Scalar);
    lexer->setColor(c, QsciLexerPerl::Array);
    lexer->setColor(c, QsciLexerPerl::Hash);
    
    c = COLOR_OPERATOR;
    lexer->setColor(c, QsciLexerPerl::Operator);
    lexer->setColor(c, QsciLexerPerl::QuotedStringQXVar);
    lexer->setColor(c, QsciLexerPerl::QuotedStringQX);
    lexer->setColor(c, QsciLexerPerl::Backticks);
    lexer->setColor(c, QsciLexerPerl::BackticksVar);
    lexer->setColor(c, QsciLexerPerl::HereDocumentDelimiter);
    lexer->setColor(c, QsciLexerPerl::BacktickHereDocument);
    lexer->setColor(c, QsciLexerPerl::BacktickHereDocumentVar);
    
    c = COLOR_KEYWORD;
    lexer->setColor(c, QsciLexerPerl::Keyword);
    lexer->setColor(c, QsciLexerPerl::SubroutinePrototype);
    lexer->setColor(c, QsciLexerPerl::Substitution);

    lexer->setColor(COLOR_ERROR, QsciLexerPerl::Error);
    // Identifier
    // FormatBody
}
