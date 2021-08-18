static void setColorsEDIFACT(QsciLexerEDIFACT *lexer)
{
    lexer->setColor(COLOR_FOREGROUND, QsciLexerEDIFACT::Default);
    lexer->setColor(COLOR_ERROR, QsciLexerEDIFACT::BadSegment);
    lexer->setColor(COLOR_COMMENT, QsciLexerEDIFACT::UNASegmentHeader);
    QColor c = COLOR_STRING;
    lexer->setColor(c, QsciLexerEDIFACT::SegmentStart);
    lexer->setColor(c, QsciLexerEDIFACT::SegmentEnd);
    c = COLOR_OPERATOR;
    lexer->setColor(c, QsciLexerEDIFACT::ElementSeparator);
    lexer->setColor(c, QsciLexerEDIFACT::CompositeSeparator);
    lexer->setColor(c, QsciLexerEDIFACT::ReleaseSeparator);
    lexer->setColor(COLOR_PREPROC, QsciLexerEDIFACT::UNASegmentHeader);
}
