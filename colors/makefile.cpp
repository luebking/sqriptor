static void setColorsMakefile(QsciLexerMakefile *lexer)
{
    lexer->setColor(COLOR_FOREGROUND, QsciLexerMakefile::Default);
    lexer->setColor(COLOR_COMMENT, QsciLexerMakefile::Comment);
    lexer->setColor(COLOR_PREPROC, QsciLexerMakefile::Preprocessor);
    lexer->setColor(COLOR_NUMBER, QsciLexerMakefile::Variable);
    lexer->setColor(COLOR_OPERATOR, QsciLexerMakefile::Operator);
    lexer->setColor(COLOR_CHAR, QsciLexerMakefile::Target);
    lexer->setColor(COLOR_ERROR, QsciLexerMakefile::Error);
}