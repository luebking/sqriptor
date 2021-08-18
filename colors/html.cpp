/*
    This is sheer insanity.
    There're not enough colors in the fucking rainbow for this stuff.
    *sigh*
*/

static void setColorsHTML(QsciLexerHTML *lexer)
{
    QColor c = COLOR_FOREGROUND;
    lexer->setColor(c, QsciLexerHTML::Default);
    lexer->setColor(c, QsciLexerHTML::SGMLDefault);
    lexer->setColor(c, QsciLexerHTML::SGMLBlockDefault);
    lexer->setColor(c, QsciLexerHTML::JavaScriptDefault);
    lexer->setColor(c, QsciLexerHTML::ASPJavaScriptDefault);
    lexer->setColor(c, QsciLexerHTML::VBScriptDefault);
    lexer->setColor(c, QsciLexerHTML::ASPVBScriptDefault);
    lexer->setColor(c, QsciLexerHTML::PythonDefault);
    lexer->setColor(c, QsciLexerHTML::ASPPythonDefault);
    lexer->setColor(c, QsciLexerHTML::ASPPythonDefault);
    lexer->setColor(c, QsciLexerHTML::PHPDefault);
    lexer->setColor(c, QsciLexerHTML::PHPVariable);
    lexer->setColor(c, QsciLexerHTML::PHPDoubleQuotedVariable);
    lexer->setColor(c, QsciLexerHTML::Script);
    lexer->setColor(c, QsciLexerHTML::JavaScriptWord);
    lexer->setColor(c, QsciLexerHTML::ASPJavaScriptWord);
    lexer->setColor(c, QsciLexerHTML::OtherInTag);
    lexer->setColor(c, QsciLexerHTML::SGMLParameter);
    
    c = COLOR_COMMENT;
    lexer->setColor(c, QsciLexerHTML::HTMLComment);
    lexer->setColor(c, QsciLexerHTML::ASPXCComment);
    lexer->setColor(c, QsciLexerHTML::SGMLComment);
    lexer->setColor(c, QsciLexerHTML::SGMLParameterComment);
    lexer->setColor(c, QsciLexerHTML::JavaScriptComment);
    lexer->setColor(c, QsciLexerHTML::JavaScriptCommentLine);
    lexer->setColor(c, QsciLexerHTML::JavaScriptCommentDoc);
    lexer->setColor(c, QsciLexerHTML::ASPJavaScriptComment);
    lexer->setColor(c, QsciLexerHTML::ASPJavaScriptCommentLine);
    lexer->setColor(c, QsciLexerHTML::ASPJavaScriptCommentDoc);
    lexer->setColor(c, QsciLexerHTML::VBScriptComment);
    lexer->setColor(c, QsciLexerHTML::ASPVBScriptComment);
    lexer->setColor(c, QsciLexerHTML::PythonComment);
    lexer->setColor(c, QsciLexerHTML::ASPPythonComment);
    lexer->setColor(c, QsciLexerHTML::PHPComment);
    lexer->setColor(c, QsciLexerHTML::PHPCommentLine);
    lexer->setColor(c, QsciLexerHTML::CDATA);
    
    c = COLOR_STRING;
    lexer->setColor(c, QsciLexerHTML::HTMLDoubleQuotedString);
    lexer->setColor(c, QsciLexerHTML::SGMLDoubleQuotedString);
    lexer->setColor(c, QsciLexerHTML::JavaScriptDoubleQuotedString);
    lexer->setColor(c, QsciLexerHTML::ASPJavaScriptDoubleQuotedString);
    lexer->setColor(c, QsciLexerHTML::PythonDoubleQuotedString);
    lexer->setColor(c, QsciLexerHTML::PythonTripleDoubleQuotedString);
    lexer->setColor(c, QsciLexerHTML::ASPPythonDoubleQuotedString);
    lexer->setColor(c, QsciLexerHTML::ASPPythonTripleDoubleQuotedString);
    lexer->setColor(c, QsciLexerHTML::PHPDoubleQuotedString);
    lexer->setColor(c, QsciLexerHTML::HTMLValue);
    lexer->setColor(c, QsciLexerHTML::VBScriptString);
    lexer->setColor(c, QsciLexerHTML::ASPVBScriptString);

    c = COLOR_CHAR;
    lexer->setColor(c, QsciLexerHTML::HTMLSingleQuotedString);
    lexer->setColor(c, QsciLexerHTML::SGMLSingleQuotedString);
    lexer->setColor(c, QsciLexerHTML::JavaScriptSingleQuotedString);
    lexer->setColor(c, QsciLexerHTML::ASPJavaScriptSingleQuotedString);
    lexer->setColor(c, QsciLexerHTML::PythonSingleQuotedString);
    lexer->setColor(c, QsciLexerHTML::PythonTripleSingleQuotedString);
    lexer->setColor(c, QsciLexerHTML::ASPPythonSingleQuotedString);
    lexer->setColor(c, QsciLexerHTML::ASPPythonTripleSingleQuotedString);
    lexer->setColor(c, QsciLexerHTML::PHPSingleQuotedString);

    c = COLOR_PREPROC;
    lexer->setColor(c, QsciLexerHTML::XMLStart);
    lexer->setColor(c, QsciLexerHTML::XMLEnd);
    lexer->setColor(c, QsciLexerHTML::ASPAtStart);
    lexer->setColor(c, QsciLexerHTML::ASPStart);
    lexer->setColor(c, QsciLexerHTML::PHPStart);
    lexer->setColor(c, QsciLexerHTML::JavaScriptStart);
    lexer->setColor(c, QsciLexerHTML::ASPJavaScriptStart);
    lexer->setColor(c, QsciLexerHTML::VBScriptStart);
    lexer->setColor(c, QsciLexerHTML::ASPVBScriptStart);
    lexer->setColor(c, QsciLexerHTML::PythonStart);
    lexer->setColor(c, QsciLexerHTML::ASPPythonStart);
    lexer->setColor(c, QsciLexerHTML::JavaScriptRegex);
    lexer->setColor(c, QsciLexerHTML::ASPJavaScriptRegex);
    lexer->setColor(c, QsciLexerHTML::SGMLSpecial);
    
    //lexer->setColor(c, QsciLexerCPP::Identifier); // everything is an identifier…
    c = COLOR_NUMBER;
    lexer->setColor(c, QsciLexerHTML::HTMLNumber);
    lexer->setColor(c, QsciLexerHTML::JavaScriptNumber);
    lexer->setColor(c, QsciLexerHTML::ASPJavaScriptNumber);
    lexer->setColor(c, QsciLexerHTML::VBScriptNumber);
    lexer->setColor(c, QsciLexerHTML::ASPVBScriptNumber);
    lexer->setColor(c, QsciLexerHTML::PythonNumber);
    lexer->setColor(c, QsciLexerHTML::ASPPythonNumber);
    lexer->setColor(c, QsciLexerHTML::PHPNumber);
    lexer->setColor(c, QsciLexerHTML::Entity);
    lexer->setColor(c, QsciLexerHTML::SGMLEntity);
    
    c = COLOR_OPERATOR;
    lexer->setColor(c, QsciLexerHTML::PythonOperator);
    lexer->setColor(c, QsciLexerHTML::ASPPythonOperator);
    lexer->setColor(c, QsciLexerHTML::PHPOperator);
    lexer->setColor(c, QsciLexerHTML::JavaScriptUnclosedString);
    lexer->setColor(c, QsciLexerHTML::ASPJavaScriptUnclosedString);
    lexer->setColor(c, QsciLexerHTML::VBScriptUnclosedString);
    lexer->setColor(c, QsciLexerHTML::ASPVBScriptUnclosedString);
    lexer->setColor(c, QsciLexerHTML::Tag);
    lexer->setColor(c, QsciLexerHTML::XMLTagEnd);
    lexer->setColor(c, QsciLexerHTML::JavaScriptSymbol);
    lexer->setColor(c, QsciLexerHTML::ASPJavaScriptSymbol);
    
    c = COLOR_KEYWORD;
    lexer->setColor(c, QsciLexerHTML::JavaScriptKeyword);
    lexer->setColor(c, QsciLexerHTML::ASPJavaScriptKeyword);
    lexer->setColor(c, QsciLexerHTML::VBScriptKeyword);
    lexer->setColor(c, QsciLexerHTML::ASPVBScriptKeyword);
    lexer->setColor(c, QsciLexerHTML::PythonKeyword);
    lexer->setColor(c, QsciLexerHTML::ASPPythonKeyword);
    lexer->setColor(c, QsciLexerHTML::PHPKeyword);
    lexer->setColor(c, QsciLexerHTML::PHPKeyword);
    lexer->setColor(c, QsciLexerHTML::PythonClassName);
    lexer->setColor(c, QsciLexerHTML::PythonFunctionMethodName);
    lexer->setColor(c, QsciLexerHTML::ASPPythonClassName);
    lexer->setColor(c, QsciLexerHTML::ASPPythonFunctionMethodName);
    lexer->setColor(c, QsciLexerHTML::SGMLCommand);
    lexer->setColor(c, QsciLexerHTML::Attribute);

    c = COLOR_ERROR;
    lexer->setColor(c, QsciLexerHTML::SGMLError);
    lexer->setColor(c, QsciLexerHTML::UnknownTag);
    lexer->setColor(c, QsciLexerHTML::UnknownAttribute);
    
    // VBScriptIdentifier
    // ASPVBScriptIdentifier
    // PythonIdentifier
    // ASPPythonIdentifier
}
