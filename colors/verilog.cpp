static void setColorsVerilog(QsciLexerVerilog *lexer)
{
    lexer->setColor(COLOR_FOREGROUND, QsciLexerVerilog::Default);
    
    QColor c = COLOR_COMMENT;
    lexer->setColor(c, QsciLexerVerilog::Comment);
    lexer->setColor(c, QsciLexerVerilog::CommentLine);
    lexer->setColor(c, QsciLexerVerilog::CommentBang);
    lexer->setColor(c, QsciLexerVerilog::CommentKeyword);

    lexer->setColor(COLOR_STRING, QsciLexerVerilog::String);

    lexer->setColor(COLOR_CHAR, QsciLexerVerilog::DeclareInputPort);

    c = COLOR_PREPROC;
    lexer->setColor(c, QsciLexerVerilog::Preprocessor);
    lexer->setColor(c, QsciLexerVerilog::PortConnection);
    
    lexer->setColor(COLOR_NUMBER, QsciLexerVerilog::Number);
    
    c = COLOR_OPERATOR;
    lexer->setColor(c, QsciLexerVerilog::Operator);
    lexer->setColor(c, QsciLexerVerilog::UnclosedString);
    lexer->setColor(c, QsciLexerVerilog::SystemTask);
    
    c = COLOR_KEYWORD;
    lexer->setColor(c, QsciLexerVerilog::Keyword);
    lexer->setColor(c, QsciLexerVerilog::KeywordSet2);
    lexer->setColor(c, QsciLexerVerilog::UserKeywordSet);

    c = COLOR_ERROR;
    lexer->setColor(c, QsciLexerVerilog::DeclareInputOutputPort);
    lexer->setColor(c, QsciLexerVerilog::DeclareOutputPort);
    
    // Identifier
}
