/*
 *   Sqriptor text editor for Qt5
 *   Copyright 2025 by Thomas Lübking <thomas.luebking@gmail.com>
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
#include "lexer/glsl.h"


QsciLexerGLSL::QsciLexerGLSL(QObject *parent) : QsciLexerCPP(parent)
{
}

const char *QsciLexerGLSL::keywords(int set) const
{
    if (set == 1) {
        return
            "auto if break int case long char register continue default short do sizeof "
            "double static else struct entry switch extern typedef float union for unsigned "
            "goto while enum void const signed volatile "
            "mat2 mat3 mat4 dmat2 dmat3 dmat4 mat2x2 mat2x3 mat2x4 "
            "dmat2x2 dmat2x3 dmat2x4 mat3x2 mat3x3 mat3x4 dmat3x2 dmat3x3 "
            "dmat3x4 mat4x2 mat4x3 mat4x4 dmat4x2 dmat4x3 dmat4x4 "
            "vec2 vec3 vec4 ivec2 ivec3 ivec4 bvec2 bvec3 bvec4 dvec2 dvec3 dvec4 "
            "uvec2 uvec3 uvec4 lowp mediump highp precision "
            "sampler1D sampler2D sampler3D samplerCube "
            "sampler1DShadow sampler2DShadow samplerCubeShadow "
            "sampler1DArray sampler2DArray sampler1DArrayShadow sampler2DArrayShadow "
            "isampler1D isampler2D isampler3D isamplerCube isampler1DArray isampler2DArray "
            "usampler1D usampler2D usampler3D usamplerCube usampler1DArray "
            "usampler2DArray sampler2DRect sampler2DRectShadow isampler2DRect usampler2DRect "
            "samplerBuffer isamplerBuffer usamplerBuffer sampler2DMS isampler2DMS "
            "usampler2DMS sampler2DMSArray isampler2DMSArray usampler2DMSArray samplerCubeArray "
            "samplerCubeArrayShadow isamplerCubeArray usamplerCubeArray";
    }
    if (set == 2) {
//        qDebug() << QsciLexerCPP::keywords(set);
        return 
            "return attribute uniform varying layout centroid flat smooth "
            "noperspective patch sample subroutine in out inout invariant discard";
    }
/*
    if (set == 3) {
    }
*/

    if (set == 4) { // reserved
        return
            "common partition active asm class union enum typedef template this "
            "packed goto inline noinline volatile public static extern external "
            "interface long short half fixed unsigned superp input output "
            "hvec2 hvec3 hvec4 fvec2 fvec3 fvec4 sampler3DRect filter "
            "image1D image2D image3D imageCube iimage1D iimage2D iimage3D "
            "iimageCube uimage1D uimage2D uimage3D uimageCube image1DArray "
            "image2DArray iimage1DArray iimage2DArray uimage1DArray "
            "uimage2DArray image1DShadow image2DShadow image1DArrayShadow "
            "image2DArrayShadow imageBuffer iimageBuffer uimageBuffer "
            "sizeof cast namespace using row_major";
    }

    return QsciLexerCPP::keywords(set);
}
