static void setColorsCSS(QsciLexerCSS *lexer)
{
    lexer->setColor(COLOR_FOREGROUND, QsciLexerCSS::Default);
    lexer->setColor(COLOR_COMMENT, QsciLexerCSS::Comment);
    
       
    QColor c = COLOR_NUMBER;
    lexer->setColor(c, QsciLexerCSS::CSS1Property);
    lexer->setColor(c, QsciLexerCSS::CSS2Property);
    lexer->setColor(c, QsciLexerCSS::CSS3Property);
    lexer->setColor(c, QsciLexerCSS::ExtendedCSSProperty);
    
    c = COLOR_CHAR;
    lexer->setColor(c, QsciLexerCSS::ClassSelector);
    lexer->setColor(c, QsciLexerCSS::IDSelector);
    
    c = COLOR_OPERATOR;
    lexer->setColor(c, QsciLexerCSS::Operator);
    lexer->setColor(c, QsciLexerCSS::Tag);
    
    c = COLOR_KEYWORD;
    lexer->setColor(c, QsciLexerCSS::PseudoClass);
    lexer->setColor(c, QsciLexerCSS::ExtendedPseudoClass);
    lexer->setColor(c, QsciLexerCSS::PseudoElement);
    lexer->setColor(c, QsciLexerCSS::ExtendedPseudoElement);
    
    c = COLOR_PREPROC;
    lexer->setColor(c, QsciLexerCSS::MediaRule);
    lexer->setColor(c, QsciLexerCSS::AtRule);
    
    
    c = COLOR_STRING;
    lexer->setColor(c, QsciLexerCSS::DoubleQuotedString);
    lexer->setColor(c, QsciLexerCSS::SingleQuotedString);
    lexer->setColor(c, QsciLexerCSS::Attribute); 
    
    c = COLOR_ERROR;
    lexer->setColor(c, QsciLexerCSS::UnknownPseudoClass);
    lexer->setColor(c, QsciLexerCSS::UnknownProperty);
    lexer->setColor(c, QsciLexerCSS::Important);
    
    // Value
    // Variable
}
