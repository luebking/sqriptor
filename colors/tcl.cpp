static void setColorsTCL(QsciLexerTCL *lexer)
{
    lexer->setColor(COLOR_FOREGROUND, QsciLexerTCL::Default);
    QColor c = COLOR_COMMENT;
    lexer->setColor(c, QsciLexerTCL::Comment);
    lexer->setColor(c, QsciLexerTCL::CommentLine);
    lexer->setColor(c, QsciLexerTCL::CommentBox);
    lexer->setColor(c, QsciLexerTCL::CommentBlock);
    
    c = COLOR_KEYWORD;
    lexer->setColor(c, QsciLexerTCL::ExpandKeyword);
    lexer->setColor(c, QsciLexerTCL::TCLKeyword);
    lexer->setColor(c, QsciLexerTCL::TkKeyword);
    lexer->setColor(c, QsciLexerTCL::ITCLKeyword);
    lexer->setColor(c, QsciLexerTCL::KeywordSet6);
    lexer->setColor(c, QsciLexerTCL::KeywordSet7);
    lexer->setColor(c, QsciLexerTCL::KeywordSet8);
    lexer->setColor(c, QsciLexerTCL::KeywordSet9);
    lexer->setColor(c, QsciLexerTCL::QuotedKeyword);
    
    c = COLOR_PREPROC;
    lexer->setColor(c, QsciLexerTCL::Substitution);
    lexer->setColor(c, QsciLexerTCL::SubstitutionBrace);
    lexer->setColor(c, QsciLexerTCL::Modifier);
    
    lexer->setColor(COLOR_NUMBER, QsciLexerTCL::Number);
    
    c= COLOR_OPERATOR;
    lexer->setColor(c, QsciLexerTCL::Operator);
    lexer->setColor(c, QsciLexerTCL::TkCommand);
        
    lexer->setColor(COLOR_STRING, QsciLexerTCL::QuotedString);

    // Identifier
}