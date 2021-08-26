#include "lexer/awk.h"


QsciLexerAWK::QsciLexerAWK(QObject *parent) : QsciLexerBash(parent)
{
}


const char *QsciLexerAWK::keywords(int set) const
{
    if (set == 1)
        return  "break case continue default do else exit function for if in "
                "next return switch while @include delete nextfile print printf "
                "BEGIN BEGINFILE END atan2 cos exp int log rand sin sqrt srand "
                "asort asorti gensub gsub index length match patsplit split "
                "sprintf strtonum sub substr tolower toupper close fflush "
                "system mktime strftime systime and compl lshift rshift xor "
                "isarray bindtextdomain dcgettext dcngettext" //;
//    if (set == 2)
                " ARGC ARGIND ARGV FILENAME FNR FS NF NR OFMT OFS ORS RLENGTH RS"
                "RSTART SUBSEP TEXTDOMAIN BINMODE CONVFMT FIELDWIDTHS FPAT "
                "IGNORECASE LINT TEXTDOMAIN ENVIRON ERRNO PROCINFO RT";
    return nullptr;
}
