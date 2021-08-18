static void setColorsPOV(QsciLexerPOV *lexer)
{
    lexer->setColor(COLOR_FOREGROUND, QsciLexerPOV::Default);
    QColor c = COLOR_COMMENT;
    lexer->setColor(c, QsciLexerPOV::Comment);
    lexer->setColor(c, QsciLexerPOV::CommentLine);
    
    c = COLOR_KEYWORD;
    lexer->setColor(c, QsciLexerPOV::PredefinedIdentifiers);
    lexer->setColor(c, QsciLexerPOV::PredefinedFunctions);
    lexer->setColor(c, QsciLexerPOV::KeywordSet6);
    lexer->setColor(c, QsciLexerPOV::KeywordSet7);
    lexer->setColor(c, QsciLexerPOV::KeywordSet8);
    
    lexer->setColor(COLOR_PREPROC, QsciLexerPOV::Directive);
    
    lexer->setColor(COLOR_NUMBER, QsciLexerPOV::Number);
    
    c = COLOR_OPERATOR;
    lexer->setColor(c, QsciLexerPOV::Operator);
    lexer->setColor(c, QsciLexerPOV::UnclosedString);
        
    lexer->setColor(COLOR_STRING, QsciLexerPOV::String);
    
    c = COLOR_CHAR;
    lexer->setColor(c, QsciLexerPOV::ObjectsCSGAppearance);
    lexer->setColor(c, QsciLexerPOV::TypesModifiersItems);
    
    lexer->setColor(COLOR_ERROR, QsciLexerPOV::BadDirective);

    // Identifier
}