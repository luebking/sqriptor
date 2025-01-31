/*
 *   Sqriptor text editor for Qt5
 *   Copyright 2021 by Thomas Lübking <thomas.luebking@gmail.com>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License version 2
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/


#include <QDebug>
#include <QRegularExpression>

#include <Qsci/qsciscintilla.h>

#include "../sqriptor.h"
#include "lexer/xorg.h"


namespace Style {
    enum Type {
        Default = 0, Keyword, Option, Number, Quote, Comment
    };
};

QsciLexerXorg::QsciLexerXorg(QObject *parent) : QsciLexerCustom(parent)
{
}

void QsciLexerXorg::updateColors()
{
    QColor bg = Sqriptor::config.color.bg;
    QColor fg = Sqriptor::config.color.fg;
    setPaper(bg);
    setColor(fg);
    setColor(fg, Style::Default);
    setColor(Sqriptor::config.color.keyword, Style::Keyword);
    setColor(Sqriptor::config.color.operateur, Style::Option);
    setColor(Sqriptor::config.color.number, Style::Number);
    setColor(Sqriptor::config.color.comment, Style::Comment);
    setColor(Sqriptor::config.color.string, Style::Quote);
}

//static bool operator==(const QStringView&view, const char* string) {
//    return !view.compare(QLatin1String(string));
//}

void QsciLexerXorg::styleText (int start, int end)
{
    static const QRegularExpression tokenizer("\"|#|\\s+|\\w+|\\W+");
    static const QRegularExpression linebreak("\\s*(\\n|\\r)+\\s*");
    static const QStringList keywords =
        QString("Section SubSection Mode EndSection EndSubSection EndMode "
                "Files ServerFlags Module Extensions InputDevice InputClass "
                "OutputClass Device VideoAdaptor Monitor Modes Screen "
                "ServerLayout DRI Vendor " // Sections
                "FontPath ModulePath XkbDir " // ...
                "Option Load Disable Identifier Driver " // ---
                "MatchProduct MatchVendor MatchDevicePath MatchOS MatchPnPID "
                "MatchUSBID MatchDriver MatchTag MatchLayout MatchIsKeyboard "
                "MatchIsPointer MatchIsJoystick MatchIsTablet MatchIsTabletPad "
                "MatchIsTouchpad MatchIsTouchscreen " // matches...
                "BusID Screen Chipset Ramdac DacSpeed Clocks ClockChip "
                "VideoRam MemBase IOBase ChipID ChipRev MatchSeat " // Device
                "VendorName ModelName HorizSync VertRefresh DisplaySize "
                "Gamma UseModes ModeLine " // Monitor
                "GPUDevice DefaultDepth DefaultFbBpp " // Screen
                "Depth FbBpp Weight Virtual ViewPort Modes Visual Black White " // Display
                "Absolute RightOf LeftOf Above Below Relative").split(" "); //Screen
    static const QStringList options =
        QString("Files ServerFlags Module Extensions InputDevice InputClass "
                "OutputClass Device VideoAdaptor Monitor Modes Screen "
                "Debug DefaultServerLayout NoTrapSignals DontVTSwitch DontZap "
                "DontZoom DisableVidModeExtension AllowNonLocalXvidtune "
                "AllowMouseOpenFail BlankTime StandbyTime SuspendTime "
                "OffTime MaxClients NoPM Xinerama IndirectGLX DRI2 "
                "GlxVisuals UseDefaultFontPath IgnoreABI AutoAddDevices "
                "AutoEnableDevices AutoAddGPU AutoBindGPU Log AutoServerLayout "
                "Floating TransformationMatrix AccelerationProfile "
                "ConstantDeceleration AdaptiveDeceleration "
                "AccelerationScheme AccelerationNumerator "
                "AccelerationDenominator AccelerationThreshold "
                "Ignore PrimaryGPU ModeDebug PreferCloneMode "
                "DPMS SyncOnGreen Primary PreferredMode ZoomModes "
                "Position LeftOf RightOf Above Below Enable DefaultModes "
                "MinClock MaxClock Ignore Rotate Accel GlxVendorLibrary "
                "InitPrimary NoInt10 IsolateDevice SingleCard "
                "XkbLayout XkbVariant XkbModel XkbOptions XkbRules "
                "XkbKeycodes AutoRepeat").split(" ");
                
    static const QStringList symbols =
        QString("TRUE on true yes FALSE off false no " // yeah, guess what
                "StaticGray GrayScale StaticColor PseudoColor TrueColor DirectColor").split(" ");
                
    /* QStringList unitstuff = "Hz k kHz M MHz" */
                
    int length = 0;
    bool haveKey = false, quote = false, comment = false, isInt;

    auto finishStyle = [&]() {
        if (quote)
            setStyling(length, Style::Quote);
        else
            setStyling(length, Style::Default);
        length = 0;
    };
                
    QString text = editor()->text(start, end);
    QRegularExpressionMatchIterator i = tokenizer.globalMatch(text/*, 0, QRegularExpression::PartialPreferFirstMatch*/);
    startStyling(start);
    QRegularExpressionMatch match;
    while (i.hasNext()) {
        match = i.next();
        QStringView token = match.capturedView(0);
        if (linebreak.match(token).hasMatch()) {
            haveKey = false;
            if (comment) {
                setStyling(length, Style::Comment);
                comment = false;
                length = 0;
            }
            length += match.capturedLength(0);
            continue;
        }
        if (comment) {
            length += match.capturedLength(0);
            continue;
        }
        if (!haveKey && keywords.contains(token)) {
            haveKey = true;
            finishStyle();
            setStyling(match.capturedLength(0), Style::Keyword);
            continue;
        }
        if (options.contains(token)) {
            finishStyle();
            setStyling(match.capturedLength(0), Style::Option);
            continue;
        }
        if (symbols.contains(token)) {
            finishStyle();
            setStyling(match.capturedLength(0), Style::Number);
            continue;
        }
        if (token == "#") {
            finishStyle();
            comment = true;
            length += 1;
            continue;
        }
        if (token == "\"") {
            if (quote)
                length += 1;
            finishStyle();
            quote = !quote;
            if (quote)
                length += 1;
            continue;
        }
        Q_UNUSED(token.toInt(&isInt));
        if (isInt) {
            finishStyle();
            setStyling(match.capturedLength(0), Style::Number);
            continue;
        }
        length += match.capturedLength(0);
    }
    finishStyle();
}

/*

"CorePointer"
              Deprecated, see Floating
"CoreKeyboard"
              Deprecated, see Floating
"AlwaysCore"  "boolean"
              Deprecated, see Floating
"SendCoreEvents"  "boolean"
              Deprecated, see Floating

Mode  "name"
              DotClock  clock
              HTimings  hdisp hsyncstart hsyncend htotal
              VTimings  vdisp vsyncstart vsyncend vtotal
              Flags  "flag" ...
              HSkew  hskew
              VScan  vscan
        EndMode

*/