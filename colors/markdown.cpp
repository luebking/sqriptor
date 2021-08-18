/*
markdown is special and different from other code highlight.
1. we'll default to sans-serif
2. we operate on font size and weight and slant and whatnot
*/


static void setColorsMarkdown(QsciLexerMarkdown *lexer)
{
    QFont dafont("sans-serif", 9);
    lexer->setFont(dafont, QsciLexerMarkdown::Default);
    QFont font = dafont;
    font.setBold(true);
    lexer->setFont(font, QsciLexerMarkdown::EmphasisAsterisks);
    lexer->setFont(font, QsciLexerMarkdown::Header6);
    font.setStretch(110);
    lexer->setFont(font, QsciLexerMarkdown::Header5);
    font.setStretch(120);
    lexer->setFont(font, QsciLexerMarkdown::Header4);
    font.setStretch(130);
    lexer->setFont(font, QsciLexerMarkdown::Header3);
    font.setStretch(140);
    lexer->setFont(font, QsciLexerMarkdown::Header2);
    font.setStretch(150);
    lexer->setFont(font, QsciLexerMarkdown::Header1);
    
    QColor c = COLOR_NUMBER;
    font = dafont; font.setItalic(true);
    lexer->setFont(font, QsciLexerMarkdown::UnorderedListItem);
    lexer->setFont(font, QsciLexerMarkdown::OrderedListItem);
    lexer->setColor(c, QsciLexerMarkdown::UnorderedListItem);
    lexer->setColor(c, QsciLexerMarkdown::OrderedListItem);

    font.setBold(true);
    lexer->setFont(font, QsciLexerMarkdown::StrongEmphasisAsterisks);
    lexer->setFont(font, QsciLexerMarkdown::StrongEmphasisUnderscores);
    
    font = dafont; font.setItalic(true);
    lexer->setFont(font, QsciLexerMarkdown::EmphasisUnderscores);
    
    font = dafont; font.setFamily("monospace");
    lexer->setFont(font, QsciLexerMarkdown::Prechar);
    lexer->setFont(font, QsciLexerMarkdown::CodeBlock);
    lexer->setFont(font, QsciLexerMarkdown::CodeBackticks);
    lexer->setFont(font, QsciLexerMarkdown::CodeDoubleBackticks);
    
    font = dafont; font.setStrikeOut(true);
    lexer->setFont(font, QsciLexerMarkdown::StrikeOut);
    
    lexer->setColor(qApp->palette().color(QPalette::Link), QsciLexerMarkdown::Link);
    lexer->setColor(COLOR_STRING, QsciLexerMarkdown::BlockQuote);
    lexer->setColor(COLOR_PREPROC, QsciLexerMarkdown::Special);
    lexer->setColor(COLOR_COMMENT, QsciLexerMarkdown::HorizontalRule);
}