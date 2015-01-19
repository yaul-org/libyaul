/*
 * Copyright (c) 2012-2014 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#define ASSERT                  1
#define FIXMATH_NO_OVERFLOW     1

#include <yaul.h>
#include <fixmath.h>
#include <tga.h>

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/queue.h>

#include "test.h"
#include "common.h"

#define FIX16_VERTEX4(x, y, z, w)                                              \
{                                                                              \
            {                                                                  \
                    F16((x)), F16((y)), F16((z)), F16((w))                     \
            }                                                                  \
}

#define FIX16_MATRIX4_INIT(row0, row1, row2, row3)                             \
.arr = {                                                                       \
        row0,                                                                  \
        row1,                                                                  \
        row2,                                                                  \
        row3                                                                   \
}

#define FIX16_MATRIX4_ROW(elem0, elem1, elem2, elem3)                          \
        F16(elem0),                                                            \
        F16(elem1),                                                            \
        F16(elem2),                                                            \
        F16(elem3)

#define RATIO   ((float)SCREEN_WIDTH / (float)SCREEN_HEIGHT)

#define L       -RATIO
#define R        RATIO
#define T        1.0f
#define B       -1.0f
#define N        1.0f
#define F       10.0f

#define CUBE_POLYGON_CNT 6

static fix16_vector4_t __unused cube_vertices[] = {
        FIX16_VERTEX4(-0.25f,  0.25f, -0.25f, 1.0f),
        FIX16_VERTEX4( 0.25f,  0.25f, -0.25f, 1.0f),
        FIX16_VERTEX4( 0.25f, -0.25f, -0.25f, 1.0f),
        FIX16_VERTEX4(-0.25f, -0.25f, -0.25f, 1.0f),

        FIX16_VERTEX4(-0.25f,  0.25f, -0.75f, 1.0f),
        FIX16_VERTEX4( 0.25f,  0.25f, -0.75f, 1.0f),
        FIX16_VERTEX4( 0.25f, -0.25f, -0.75f, 1.0f),
        FIX16_VERTEX4(-0.25f, -0.25f, -0.75f, 1.0f)
};

static uint32_t __unused cube_indices[CUBE_POLYGON_CNT * 4] = {
        /* Front */
        0, 1, 2, 3,
        /* Back */
        4, 5, 6, 7,
        /* Left */
        0, 4, 7, 3,
        /* Right */
        1, 5, 6, 2,
        /* Top */
        0, 4, 5, 1,
        /* Bottom */
        3, 7, 6, 2
};

#define TEAPOT_POLYGON_CNT 209

static fix16_vector4_t __unused teapot_vertices[] = {
        FIX16_VERTEX4(0.378435f, 0.313658f, 0.147146f, 1.0f),
        FIX16_VERTEX4(0.375425f, 0.283457f, 0.145117f, 1.0f),
        FIX16_VERTEX4(0.406266f, 0.283457f, -0.011080f, 1.0f),
        FIX16_VERTEX4(0.410014f, 0.313446f, -0.011080f, 1.0f),
        FIX16_VERTEX4(0.293234f, 0.313870f, 0.273844f, 1.0f),
        FIX16_VERTEX4(0.290859f, 0.283457f, 0.271468f, 1.0f),
        FIX16_VERTEX4(0.166537f, 0.313658f, 0.359044f, 1.0f),
        FIX16_VERTEX4(0.164508f, 0.283457f, 0.356034f, 1.0f),
        FIX16_VERTEX4(0.008387f, 0.313511f, 0.390559f, 1.0f),
        FIX16_VERTEX4(0.008310f, 0.283457f, 0.386876f, 1.0f),
        FIX16_VERTEX4(-0.150807f, 0.313438f, 0.359118f, 1.0f),
        FIX16_VERTEX4(-0.158680f, 0.283457f, 0.356034f, 1.0f),
        FIX16_VERTEX4(-0.280341f, 0.313141f, 0.272300f, 1.0f),
        FIX16_VERTEX4(-0.283832f, 0.283457f, 0.271468f, 1.0f),
        FIX16_VERTEX4(-0.364798f, 0.313202f, 0.140974f, 1.0f),
        FIX16_VERTEX4(-0.362402f, 0.283457f, 0.145117f, 1.0f),
        FIX16_VERTEX4(-0.392581f, 0.313419f, -0.014565f, 1.0f),
        FIX16_VERTEX4(-0.389646f, 0.283457f, -0.011080f, 1.0f),
        FIX16_VERTEX4(-0.361816f, 0.313658f, -0.169306f, 1.0f),
        FIX16_VERTEX4(-0.358804f, 0.283457f, -0.167278f, 1.0f),
        FIX16_VERTEX4(-0.276614f, 0.313870f, -0.296004f, 1.0f),
        FIX16_VERTEX4(-0.274238f, 0.283457f, -0.293629f, 1.0f),
        FIX16_VERTEX4(-0.149916f, 0.313658f, -0.381206f, 1.0f),
        FIX16_VERTEX4(-0.147887f, 0.283457f, -0.378195f, 1.0f),
        FIX16_VERTEX4(0.008310f, 0.313446f, -0.412784f, 1.0f),
        FIX16_VERTEX4(0.008310f, 0.283457f, -0.409036f, 1.0f),
        FIX16_VERTEX4(0.166537f, 0.313658f, -0.381206f, 1.0f),
        FIX16_VERTEX4(0.164508f, 0.283457f, -0.378195f, 1.0f),
        FIX16_VERTEX4(0.293234f, 0.313870f, -0.296004f, 1.0f),
        FIX16_VERTEX4(0.290859f, 0.283457f, -0.293629f, 1.0f),
        FIX16_VERTEX4(0.378435f, 0.313658f, -0.169307f, 1.0f),
        FIX16_VERTEX4(0.375425f, 0.283457f, -0.167278f, 1.0f),
        FIX16_VERTEX4(0.533488f, -0.068506f, 0.219948f, 1.0f),
        FIX16_VERTEX4(0.548636f, 0.026288f, -0.011080f, 1.0f),
        FIX16_VERTEX4(0.570993f, -0.214083f, -0.011080f, 1.0f),
        FIX16_VERTEX4(0.412828f, -0.064140f, 0.393439f, 1.0f),
        FIX16_VERTEX4(0.239338f, -0.068509f, 0.514098f, 1.0f),
        FIX16_VERTEX4(0.008310f, 0.026283f, 0.529246f, 1.0f),
        FIX16_VERTEX4(0.008310f, -0.214084f, 0.551602f, 1.0f),
        FIX16_VERTEX4(-0.222718f, -0.068508f, 0.514097f, 1.0f),
        FIX16_VERTEX4(-0.396208f, -0.064144f, 0.393438f, 1.0f),
        FIX16_VERTEX4(-0.516868f, -0.068510f, 0.219946f, 1.0f),
        FIX16_VERTEX4(-0.532015f, 0.026285f, -0.011080f, 1.0f),
        FIX16_VERTEX4(-0.516868f, -0.068508f, -0.242107f, 1.0f),
        FIX16_VERTEX4(-0.396208f, -0.064139f, -0.415599f, 1.0f),
        FIX16_VERTEX4(-0.222717f, -0.068504f, -0.536257f, 1.0f),
        FIX16_VERTEX4(0.008310f, 0.026287f, -0.551405f, 1.0f),
        FIX16_VERTEX4(0.008310f, -0.214084f, -0.573763f, 1.0f),
        FIX16_VERTEX4(0.239338f, -0.068507f, -0.536257f, 1.0f),
        FIX16_VERTEX4(0.412827f, -0.064139f, -0.415599f, 1.0f),
        FIX16_VERTEX4(0.533488f, -0.068506f, -0.242107f, 1.0f),
        FIX16_VERTEX4(0.504460f, -0.245313f, 0.200961f, 1.0f),
        FIX16_VERTEX4(0.385110f, -0.375621f, 0.152493f, 1.0f),
        FIX16_VERTEX4(0.423204f, -0.370261f, -0.011081f, 1.0f),
        FIX16_VERTEX4(0.392078f, -0.241076f, 0.372689f, 1.0f),
        FIX16_VERTEX4(0.304355f, -0.366609f, 0.284966f, 1.0f),
        FIX16_VERTEX4(0.220350f, -0.245314f, 0.485068f, 1.0f),
        FIX16_VERTEX4(0.003111f, -0.379177f, 0.395934f, 1.0f),
        FIX16_VERTEX4(-0.203729f, -0.245314f, 0.485068f, 1.0f),
        FIX16_VERTEX4(-0.157351f, -0.368902f, 0.371191f, 1.0f),
        FIX16_VERTEX4(-0.375460f, -0.241074f, 0.372687f, 1.0f),
        FIX16_VERTEX4(-0.283027f, -0.374334f, 0.280257f, 1.0f),
        FIX16_VERTEX4(-0.487839f, -0.245315f, 0.200959f, 1.0f),
        FIX16_VERTEX4(-0.357916f, -0.386606f, 0.150061f, 1.0f),
        FIX16_VERTEX4(-0.526562f, -0.250033f, -0.011080f, 1.0f),
        FIX16_VERTEX4(-0.406584f, -0.370261f, -0.011080f, 1.0f),
        FIX16_VERTEX4(-0.487840f, -0.245314f, -0.223121f, 1.0f),
        FIX16_VERTEX4(-0.363319f, -0.378583f, -0.180657f, 1.0f),
        FIX16_VERTEX4(-0.375457f, -0.241072f, -0.394849f, 1.0f),
        FIX16_VERTEX4(-0.287730f, -0.366610f, -0.307120f, 1.0f),
        FIX16_VERTEX4(-0.203730f, -0.245313f, -0.507230f, 1.0f),
        FIX16_VERTEX4(-0.152831f, -0.386606f, -0.377308f, 1.0f),
        FIX16_VERTEX4(0.008310f, -0.370261f, -0.425975f, 1.0f),
        FIX16_VERTEX4(0.220350f, -0.245314f, -0.507230f, 1.0f),
        FIX16_VERTEX4(0.177886f, -0.378583f, -0.382711f, 1.0f),
        FIX16_VERTEX4(0.392083f, -0.241074f, -0.394850f, 1.0f),
        FIX16_VERTEX4(0.304354f, -0.366609f, -0.307120f, 1.0f),
        FIX16_VERTEX4(0.504461f, -0.245313f, -0.223121f, 1.0f),
        FIX16_VERTEX4(0.374537f, -0.386606f, -0.172222f, 1.0f),
        FIX16_VERTEX4(0.142697f, -0.397273f, 0.164640f, 1.0f),
        FIX16_VERTEX4(0.177886f, -0.378582f, 0.360549f, 1.0f),
        FIX16_VERTEX4(-0.670545f, 0.173256f, 0.024375f, 1.0f),
        FIX16_VERTEX4(-0.446233f, 0.183289f, 0.013728f, 1.0f),
        FIX16_VERTEX4(-0.756159f, 0.136092f, 0.022104f, 1.0f),
        FIX16_VERTEX4(-0.703112f, 0.208315f, 0.052183f, 1.0f),
        FIX16_VERTEX4(-0.425166f, 0.231526f, 0.051356f, 1.0f),
        FIX16_VERTEX4(-0.794556f, 0.170494f, 0.051120f, 1.0f),
        FIX16_VERTEX4(-0.833623f, 0.085872f, 0.038402f, 1.0f),
        FIX16_VERTEX4(-0.718815f, 0.231859f, -0.011080f, 1.0f),
        FIX16_VERTEX4(-0.418071f, 0.240819f, -0.011080f, 1.0f),
        FIX16_VERTEX4(-0.818194f, 0.184003f, -0.044666f, 1.0f),
        FIX16_VERTEX4(-0.845811f, 0.068939f, -0.011081f, 1.0f),
        FIX16_VERTEX4(-0.703112f, 0.208315f, -0.074344f, 1.0f),
        FIX16_VERTEX4(-0.425166f, 0.231525f, -0.073516f, 1.0f),
        FIX16_VERTEX4(-0.833623f, 0.085871f, -0.060563f, 1.0f),
        FIX16_VERTEX4(-0.780726f, 0.147961f, -0.072827f, 1.0f),
        FIX16_VERTEX4(-0.671744f, 0.181827f, -0.059714f, 1.0f),
        FIX16_VERTEX4(-0.442055f, 0.186855f, -0.059048f, 1.0f),
        FIX16_VERTEX4(-0.761075f, 0.007837f, 0.051196f, 1.0f),
        FIX16_VERTEX4(-0.722623f, -0.014246f, -0.011080f, 1.0f),
        FIX16_VERTEX4(-0.665364f, -0.102797f, 0.051031f, 1.0f),
        FIX16_VERTEX4(-0.555757f, -0.156249f, 0.036888f, 1.0f),
        FIX16_VERTEX4(-0.560198f, -0.142925f, -0.011080f, 1.0f),
        FIX16_VERTEX4(-0.542501f, -0.218211f, 0.052621f, 1.0f),
        FIX16_VERTEX4(-0.731244f, -0.094963f, 0.037638f, 1.0f),
        FIX16_VERTEX4(-0.728943f, -0.110913f, -0.045695f, 1.0f),
        FIX16_VERTEX4(-0.531773f, -0.228201f, -0.011080f, 1.0f),
        FIX16_VERTEX4(-0.542500f, -0.218211f, -0.074781f, 1.0f),
        FIX16_VERTEX4(-0.761075f, 0.007838f, -0.073357f, 1.0f),
        FIX16_VERTEX4(-0.665366f, -0.102799f, -0.073192f, 1.0f),
        FIX16_VERTEX4(-0.555757f, -0.156249f, -0.059048f, 1.0f),
        FIX16_VERTEX4(0.685757f, 0.004879f, 0.104091f, 1.0f),
        FIX16_VERTEX4(0.491543f, -0.030333f, 0.094449f, 1.0f),
        FIX16_VERTEX4(0.491543f, 0.006309f, -0.011080f, 1.0f),
        FIX16_VERTEX4(0.652915f, 0.054065f, -0.011080f, 1.0f),
        FIX16_VERTEX4(0.758457f, 0.267851f, 0.014139f, 1.0f),
        FIX16_VERTEX4(0.491543f, -0.110946f, 0.129626f, 1.0f),
        FIX16_VERTEX4(0.732197f, 0.138950f, 0.061026f, 1.0f),
        FIX16_VERTEX4(0.938374f, 0.304402f, 0.031654f, 1.0f),
        FIX16_VERTEX4(0.701085f, -0.094942f, 0.084217f, 1.0f),
        FIX16_VERTEX4(0.491543f, -0.191559f, 0.094449f, 1.0f),
        FIX16_VERTEX4(0.826599f, 0.173597f, 0.016558f, 1.0f),
        FIX16_VERTEX4(0.708154f, -0.127530f, -0.011080f, 1.0f),
        FIX16_VERTEX4(0.491543f, -0.228201f, -0.011080f, 1.0f),
        FIX16_VERTEX4(0.980557f, 0.304374f, -0.010729f, 1.0f),
        FIX16_VERTEX4(0.701085f, -0.094943f, -0.106379f, 1.0f),
        FIX16_VERTEX4(0.491543f, -0.191559f, -0.116610f, 1.0f),
        FIX16_VERTEX4(0.807081f, 0.173260f, -0.061297f, 1.0f),
        FIX16_VERTEX4(0.937195f, 0.304097f, -0.053474f, 1.0f),
        FIX16_VERTEX4(0.685756f, 0.004879f, -0.126252f, 1.0f),
        FIX16_VERTEX4(0.491543f, -0.110946f, -0.151786f, 1.0f),
        FIX16_VERTEX4(0.725179f, 0.143554f, -0.081507f, 1.0f),
        FIX16_VERTEX4(0.491543f, -0.030333f, -0.116610f, 1.0f),
        FIX16_VERTEX4(0.834166f, 0.301921f, -0.045377f, 1.0f),
        FIX16_VERTEX4(0.814089f, 0.299936f, 0.002324f, 1.0f),
        FIX16_VERTEX4(0.830329f, 0.284203f, 0.025138f, 1.0f),
        FIX16_VERTEX4(0.808356f, 0.280915f, -0.012330f, 1.0f),
        FIX16_VERTEX4(0.911160f, 0.294125f, -0.009868f, 1.0f),
        FIX16_VERTEX4(0.898618f, 0.281632f, -0.038529f, 1.0f),
        FIX16_VERTEX4(0.092749f, 0.475992f, 0.046429f, 1.0f),
        FIX16_VERTEX4(-0.088126f, 0.491093f, -0.031460f, 1.0f),
        FIX16_VERTEX4(0.103708f, 0.482805f, -0.045848f, 1.0f),
        FIX16_VERTEX4(0.062624f, 0.406075f, 0.010815f, 1.0f),
        FIX16_VERTEX4(0.036668f, 0.369496f, 0.046623f, 1.0f),
        FIX16_VERTEX4(0.028692f, 0.474043f, 0.089587f, 1.0f),
        FIX16_VERTEX4(-0.014547f, 0.403543f, 0.047123f, 1.0f),
        FIX16_VERTEX4(-0.056021f, 0.476855f, 0.069467f, 1.0f),
        FIX16_VERTEX4(-0.045952f, 0.363597f, 0.017357f, 1.0f),
        FIX16_VERTEX4(-0.082728f, 0.446477f, 0.007791f, 1.0f),
        FIX16_VERTEX4(-0.042515f, 0.405233f, -0.043955f, 1.0f),
        FIX16_VERTEX4(-0.032175f, 0.363108f, -0.060751f, 1.0f),
        FIX16_VERTEX4(-0.073075f, 0.476242f, -0.075714f, 1.0f),
        FIX16_VERTEX4(0.008310f, 0.443985f, -0.105536f, 1.0f),
        FIX16_VERTEX4(0.028240f, 0.482604f, -0.108331f, 1.0f),
        FIX16_VERTEX4(0.041185f, 0.405233f, -0.061906f, 1.0f),
        FIX16_VERTEX4(0.059437f, 0.362858f, -0.049345f, 1.0f),
        FIX16_VERTEX4(0.096349f, 0.446765f, -0.049953f, 1.0f),
        FIX16_VERTEX4(0.071988f, 0.347700f, 0.096411f, 1.0f),
        FIX16_VERTEX4(0.356492f, 0.295257f, 0.135633f, 1.0f),
        FIX16_VERTEX4(0.377841f, 0.283457f, -0.011080f, 1.0f),
        FIX16_VERTEX4(0.274597f, 0.292339f, 0.255206f, 1.0f),
        FIX16_VERTEX4(0.155024f, 0.295257f, 0.337103f, 1.0f),
        FIX16_VERTEX4(0.008310f, 0.283457f, 0.358450f, 1.0f),
        FIX16_VERTEX4(-0.179347f, 0.309355f, 0.267840f, 1.0f),
        FIX16_VERTEX4(-0.136730f, 0.283457f, 0.329812f, 1.0f),
        FIX16_VERTEX4(-0.254056f, 0.283457f, 0.251286f, 1.0f),
        FIX16_VERTEX4(-0.339872f, 0.295257f, 0.135634f, 1.0f),
        FIX16_VERTEX4(-0.361220f, 0.283457f, -0.011080f, 1.0f),
        FIX16_VERTEX4(-0.270609f, 0.309355f, -0.198738f, 1.0f),
        FIX16_VERTEX4(-0.332582f, 0.283457f, -0.156121f, 1.0f),
        FIX16_VERTEX4(-0.254056f, 0.283457f, -0.273447f, 1.0f),
        FIX16_VERTEX4(-0.138403f, 0.295257f, -0.359263f, 1.0f),
        FIX16_VERTEX4(0.008310f, 0.283457f, -0.380611f, 1.0f),
        FIX16_VERTEX4(0.155616f, 0.292928f, -0.357447f, 1.0f),
        FIX16_VERTEX4(0.287229f, 0.309355f, -0.198737f, 1.0f),
        FIX16_VERTEX4(0.270677f, 0.283457f, -0.273447f, 1.0f),
        FIX16_VERTEX4(0.349202f, 0.283457f, -0.156121f, 1.0f)
};

static uint32_t __unused teapot_indices[TEAPOT_POLYGON_CNT * 4] = {
        122, 123, 120, 119,
        6, 8, 9, 7,
        84, 81, 83, 86,
        18, 19, 17, 16,
        128, 127, 121, 124,
        41, 40, 60, 62,
        2, 31, 30, 3,
        0, 1, 2, 3,
        30, 50, 33, 3,
        156, 139, 139, 142,
        62, 64, 102, 41,
        60, 40, 39, 58,
        133, 128, 128, 131,
        4, 6, 7, 5,
        42, 16, 14, 41,
        67, 69, 69, 71,
        66, 67, 69, 68,
        34, 77, 78, 53,
        150, 166, 166, 147,
        82, 81, 84, 85,
        43, 66, 68, 44,
        143, 147, 147, 157,
        39, 40, 12, 10,
        148, 151, 151, 149,
        155, 150, 150, 154,
        80, 57, 38, 56,
        94, 95, 95, 108,
        119, 120, 116, 111,
        103, 100, 100, 104,
        148, 149, 149, 145,
        155, 154, 154, 142,
        168, 166, 167, 169,
        142, 145, 144, 139,
        67, 63, 63, 65,
        98, 83, 83, 86,
        88, 92, 93, 89,
        174, 176, 159, 158,
        28, 29, 27, 26,
        83, 81, 96, 95,
        43, 44, 20, 18,
        61, 57, 57, 59,
        32, 35, 4, 0,
        0, 3, 33, 32,
        109, 110, 102, 99,
        74, 73, 47, 72,
        133, 115, 115, 134,
        105, 104, 103, 106,
        52, 78, 78, 53,
        90, 86, 87, 91,
        171, 170, 170, 168,
        42, 43, 18, 16,
        54, 35, 32, 51,
        121, 119, 111, 117,
        144, 146, 146, 145,
        36, 37, 37, 38,
        113, 114, 111, 112,
        34, 53, 52, 51,
        34, 33, 33, 32,
        114, 115, 115, 131,
        52, 80, 80, 79,
        68, 70, 45, 44,
        64, 65, 67, 66,
        69, 71, 70, 68,
        37, 8, 6, 36,
        50, 30, 28, 49,
        16, 17, 15, 14,
        20, 21, 19, 18,
        75, 77, 50, 49,
        14, 15, 13, 12,
        125, 129, 130, 126,
        163, 161, 162, 164,
        87, 104, 104, 98,
        88, 84, 86, 90,
        22, 45, 46, 24,
        12, 13, 11, 10,
        22, 24, 25, 23,
        20, 22, 23, 21,
        79, 80, 80, 57,
        171, 168, 168, 150,
        92, 96, 97, 93,
        52, 55, 55, 80,
        62, 60, 61, 63,
        28, 30, 31, 29,
        100, 99, 102, 101,
        176, 175, 175, 174,
        38, 57, 59, 58,
        152, 154, 154, 149,
        28, 26, 48, 49,
        161, 157, 157, 163,
        157, 158, 158, 155,
        135, 136, 136, 134,
        171, 150, 150, 173,
        10, 8, 37, 39,
        63, 79, 79, 57,
        172, 171, 171, 173,
        155, 174, 174, 173,
        76, 78, 77, 75,
        1, 0, 4, 5,
        166, 163, 163, 147,
        118, 117, 117, 121,
        45, 22, 20, 44,
        110, 107, 107, 109,
        128, 127, 127, 131,
        156, 141, 153, 152,
        36, 35, 54, 56,
        115, 118, 118, 134,
        143, 157, 157, 155,
        135, 134, 134, 118,
        118, 137, 137, 135,
        56, 54, 55, 80,
        96, 81, 82, 97,
        127, 131, 129, 125,
        136, 128, 128, 133,
        128, 136, 136, 138,
        73, 74, 76, 75,
        41, 14, 12, 40,
        47, 70, 71, 72,
        107, 106, 106, 105,
        38, 37, 37, 39,
        61, 60, 58, 59,
        99, 83, 95, 108,
        64, 62, 63, 65,
        48, 73, 75, 49,
        46, 48, 26, 24,
        91, 87, 104, 105,
        136, 133, 133, 134,
        47, 48, 48, 73,
        121, 127, 125, 122,
        88, 92, 92, 90,
        66, 43, 102, 64,
        74, 71, 71, 72,
        4, 35, 36, 6,
        128, 124, 137, 138,
        166, 150, 150, 168,
        10, 11, 9, 8,
        88, 89, 85, 84,
        26, 27, 25, 24,
        77, 50, 50, 34,
        70, 45, 45, 47,
        55, 54, 51, 52,
        108, 105, 105, 94,
        161, 160, 160, 157,
        166, 165, 165, 163,
        152, 156, 156, 154,
        122, 125, 126, 123,
        129, 114, 113, 132,
        41, 42, 42, 102,
        145, 149, 149, 147,
        90, 92, 92, 95,
        39, 58, 58, 38,
        140, 144, 144, 139,
        155, 142, 142, 143,
        170, 169, 169, 168,
        155, 158, 158, 174,
        96, 95, 95, 92,
        32, 51, 51, 34,
        74, 78, 78, 71,
        147, 149, 149, 150,
        98, 99, 99, 83,
        67, 71, 71, 63,
        109, 107, 107, 105,
        124, 118, 118, 121,
        129, 114, 114, 131,
        42, 43, 43, 102,
        146, 148, 148, 145,
        91, 90, 90, 94,
        76, 78, 78, 74,
        140, 146, 146, 144,
        36, 38, 38, 56,
        142, 154, 154, 156,
        52, 79, 79, 78,
        142, 145, 145, 143,
        46, 47, 47, 45,
        133, 131, 131, 115,
        94, 105, 105, 91,
        175, 173, 173, 174,
        79, 63, 63, 71,
        163, 157, 157, 147,
        151, 153, 153, 152,
        140, 153, 153, 151,
        100, 99, 99, 98,
        165, 164, 164, 163,
        117, 115, 115, 114,
        143, 145, 145, 147,
        103, 101, 101, 100,
        33, 34, 34, 50,
        116, 111, 111, 112,
        160, 158, 158, 157,
        109, 108, 108, 99,
        46, 48, 48, 47,
        94, 90, 90, 95,
        130, 132, 132, 129,
        118, 115, 115, 117,
        79, 71, 71, 78,
        86, 87, 87, 98,
        98, 104, 104, 100,
        122, 121, 121, 119,
        149, 154, 154, 150,
        124, 137, 137, 118,
        111, 117, 117, 114,
        146, 140, 140, 148,
        140, 141, 141, 153,
        173, 150, 150, 155,
        61, 63, 63, 57,
        140, 139, 139, 141,
        156, 141, 141, 139,
        151, 152, 152, 149,
        108, 109, 109, 105,
        148, 140, 140, 151
};

static uint16_t colors[TEAPOT_POLYGON_CNT] __unused;

static fix16_matrix4_t __unused matrix_model = {
        FIX16_MATRIX4_INIT(
                FIX16_MATRIX4_ROW(1.0f, 0.0f, 0.0f, 0.0f),
                FIX16_MATRIX4_ROW(0.0f, 1.0f, 0.0f, 0.0f),
                FIX16_MATRIX4_ROW(0.0f, 0.0f, 1.0f, 0.0f),
                FIX16_MATRIX4_ROW(0.0f, 0.0f, 0.0f, 1.0f))
};

static fix16_matrix4_t __unused matrix_view = {
        FIX16_MATRIX4_INIT(
                FIX16_MATRIX4_ROW(1.0f, 0.0f, 0.0f,   0.0f),
                FIX16_MATRIX4_ROW(0.0f, 1.0f, 0.0f,   0.0f),
                FIX16_MATRIX4_ROW(0.0f, 0.0f, 1.0f,  10.0f),
                FIX16_MATRIX4_ROW(0.0f, 0.0f, 0.0f,   1.0f))
};

static fix16_matrix4_t __unused matrix_projection = {
        FIX16_MATRIX4_INIT(
                FIX16_MATRIX4_ROW(2.0f / (R - L), 0.0f, 0.0f, -(R + L) / (R - L)),
                FIX16_MATRIX4_ROW(0.0f, 2.0f / (T - B), 0.0f, -(T + B) / (T - B)),
                FIX16_MATRIX4_ROW(0.0f, 0.0f, -2.0f / (F - N), -(F + N) / (F - N)),
                FIX16_MATRIX4_ROW(0.0f, 0.0f, 0.0f, 1.0f))
};


#define SORT_PRIMITIVE_POOL_CNT VDP1_CMDT_COUNT_MAX

struct sort_primitive {
        int16_vector2_t sp_coords[4];
        int16_t sp_avg;
        uint32_t sp_idx;
};

static struct sort_primitive __unused sort_primitive_pool[SORT_PRIMITIVE_POOL_CNT];
static struct sort_primitive __unused *sort_primitive_ptr[SORT_PRIMITIVE_POOL_CNT];

static uint32_t sort_primitive_idx;
static uint32_t sort_primitive_cnt;

static void sort(void);
static void sort_init(void);
static void sort_polygon_add(const fix16_vector4_t *, uint32_t);


static inline void model_project(const fix16_vector4_t *, const uint32_t *,
    uint32_t, uint32_t);
static void model_polygon_project(const fix16_vector4_t *, const uint32_t *,
    uint32_t);


#define MATRIX_STACK_MODEL_VIEW_DEPTH   14
#define MATRIX_STACK_PROJECTION_DEPTH   2
#define MATRIX_STACK_DEPTH              (MATRIX_STACK_MODEL_VIEW_DEPTH +       \
    MATRIX_STACK_PROJECTION_DEPTH)

#define MATRIX_STACK_MODE_INVALID       -1
#define MATRIX_STACK_MODE_PROJECTION    0
#define MATRIX_STACK_MODE_MODEL_VIEW    1

#define MATRIX_STACK_MODES              2

struct matrix_stack;

static SLIST_HEAD(matrix_stacks, matrix_stack) matrix_stacks[MATRIX_STACK_MODES];

struct matrix_stack {
        fix16_matrix4_t *ms_matrix;

        SLIST_ENTRY(matrix_stack) ms_entries;
} __aligned(8);

static struct matrix_stack_state {
        bool mss_initialized;
        int32_t mss_mode;
} matrix_stack_state;

MEMB(matrix_stack_pool, struct matrix_stack, MATRIX_STACK_DEPTH,
    sizeof(struct matrix_stack));
MEMB(matrix_stack_matrix_pool, fix16_matrix4_t, MATRIX_STACK_DEPTH,
    sizeof(fix16_matrix4_t));

static void matrix_stack_init(void);
static void matrix_stack_mode(int32_t);
static void matrix_stack_push(void);
static void matrix_stack_pop(void);
static struct matrix_stack *matrix_stack_top(int32_t);
static void matrix_stack_load(fix16_matrix4_t *);

static void matrix_stack_identity_load(void);
static void matrix_stack_translate(fix16_t, fix16_t, fix16_t);
static void matrix_stack_scale(fix16_t, fix16_t, fix16_t);
static void matrix_stack_rotate(fix16_t, int32_t);


#define MODEL_TRANSFORMATIONS   1 /* 0: No transforms   1: Apply transforms */
#define MODEL_PROJECT           1 /* 0: Upload          1: Upload and project */
#define POLYGON_SORT            1 /* 0: No sort         1: Sort */
#define RENDER                  1 /* 0: No render       1: Render */
#define MATRIX_STACK            1 /* 0: No matrix stack 1: Matrix stack */


void
test_07_init(void)
{
        init();

        STATIC_ASSERT(SORT_PRIMITIVE_POOL_CNT > TEAPOT_POLYGON_CNT);

        uint32_t color_idx;
        for (color_idx = 0; color_idx < TEAPOT_POLYGON_CNT; color_idx++) {
                uint32_t idx;
                idx = color_idx;

                uint32_t polygon_idx;
                fix16_vector4_t *polygon[4];
                for (polygon_idx = 0; polygon_idx < 4; polygon_idx++) {
                        uint32_t vertex_idx;
                        vertex_idx = teapot_indices[(4 * idx) + polygon_idx];
                        fix16_vector4_t *vertex;
                        vertex = &teapot_vertices[vertex_idx];

                        polygon[polygon_idx] = vertex;
                }

                fix16_t avg;
                avg = fix16_mul(fix16_add(
                            fix16_add(polygon[0]->z, polygon[1]->z),
                            fix16_add(polygon[2]->z, polygon[3]->z)),
                    F16(1.0 / 4.0f));

                uint16_t color;
                color = fix16_to_int(fix16_add(
                            fix16_mul(fix16_abs(avg), F16(255.0f)),
                            F16(64.0f)));

                colors[color_idx] = RGB888_TO_RGB555(color, color, color);
        }

        sort_init();
        matrix_stack_init();

        matrix_stack_mode(MATRIX_STACK_MODE_PROJECTION);
        matrix_stack_load(&matrix_projection);

        matrix_stack_mode(MATRIX_STACK_MODE_MODEL_VIEW);
        matrix_stack_load(&matrix_view);

        vdp1_cmdt_list_begin(0); {
                vdp1_cmdt_local_coord_set(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
                vdp1_cmdt_end();
        } vdp1_cmdt_list_end(0);
}

void
test_07_update(void)
{
        static fix16_t angle __unused = F16(-1.0f);

        if (digital_pad.connected == 1) {
                if (digital_pad.held.button.start) {
                        return;
                }
        }

        cons_buffer(&cons, "[H[2J");

        uint16_t start_tick;
        start_tick = tick;

        uint16_t start_scanline;
        start_scanline = vdp2_tvmd_vcount_get();

#if MATRIX_STACK == 1
        matrix_stack_mode(MATRIX_STACK_MODE_MODEL_VIEW);
        matrix_stack_push(); {
#endif
                /* C=VM
                 * D=CR=V(MR)
                 * E=DS=V(MRS) */
#if MODEL_TRANSFORMATIONS == 1
                matrix_stack_translate(F16(0.0f), F16(0.0f), F16(-10.0f));
                matrix_stack_rotate(fix16_mul(F16(2.0f), angle), 0);
                matrix_stack_rotate(angle, 1);
                matrix_stack_rotate(angle, 2);
                matrix_stack_translate(F16(0.0f), F16(0.0f), F16(0.0f));
#endif
                angle = fix16_mod(fix16_add(angle, F16(-2.0f)), F16(360.0f));
                angle = fix16_add(angle, F16(-2.0f));

                model_polygon_project(teapot_vertices, teapot_indices,
                    TEAPOT_POLYGON_CNT * 4);
#if MATRIX_STACK == 1
        } matrix_stack_pop();
#endif

        sort();

#if RENDER == 1
        vdp1_cmdt_list_begin(1); {
                uint32_t idx;
                for (idx = 0; idx < sort_primitive_cnt; idx++) {
                        struct sort_primitive *sort_primitive;
                        sort_primitive = sort_primitive_ptr[idx];

                        uint16_t color;
                        color = colors[sort_primitive->sp_idx];

                        struct vdp1_cmdt_polygon polygon;
                        memset(&polygon, 0x00, sizeof(polygon));
                        polygon.cp_color = color;
                        polygon.cp_mode.transparent_pixel = true;
                        polygon.cp_mode.end_code = true;
                        polygon.cp_vertex.a.x = sort_primitive->sp_coords[0].x;
                        polygon.cp_vertex.a.y = sort_primitive->sp_coords[0].y;
                        polygon.cp_vertex.b.x = sort_primitive->sp_coords[1].x;
                        polygon.cp_vertex.b.y = sort_primitive->sp_coords[1].y;
                        polygon.cp_vertex.c.x = sort_primitive->sp_coords[2].x;
                        polygon.cp_vertex.c.y = sort_primitive->sp_coords[2].y;
                        polygon.cp_vertex.d.x = sort_primitive->sp_coords[3].x;
                        polygon.cp_vertex.d.y = sort_primitive->sp_coords[3].y;

                        vdp1_cmdt_polygon_draw(&polygon);
                }
                vdp1_cmdt_end();
        } vdp1_cmdt_list_end(1);
#endif

        uint16_t end_tick;
        end_tick = tick;

        uint16_t end_scanline;
        end_scanline = vdp2_tvmd_vcount_get();

        (void)sprintf(text, "%i-%i=%i, vcnt: %i-%i=%i, tick: %i-%i=%i\n",
            (int)end_tick,
            (int)start_tick,
            (int)(end_tick - start_tick),
            (int)end_scanline,
            (int)start_scanline,
            (((end_tick - start_tick) == 0)
                ? (int)(end_scanline - start_scanline)
                : 0),
            (int)tick,
            (int)tick2,
            (int)(tick - tick2));
        cons_buffer(&cons, text);
}

void
test_07_draw(void)
{
        cons_flush(&cons);

        vdp1_cmdt_list_commit();
}

void
test_07_exit(void)
{
}

static inline void __always_inline __unused
model_project(const fix16_vector4_t *vb __unused, const uint32_t *ib __unused,
    uint32_t ib_cnt, uint32_t vertex_cnt)
{
        /* How many primitives there are in the model */
        uint32_t primitives;
        primitives = ib_cnt / vertex_cnt;

        uint32_t primitive_idx;
        for (primitive_idx = 0; primitive_idx < primitives; primitive_idx++) {
                fix16_vector4_t projected[vertex_cnt];

                uint32_t vertex_idx;
                for (vertex_idx = 0; vertex_idx < vertex_cnt; vertex_idx++) {
#if MODEL_PROJECT
                        fix16_vector4_t vtx4;
                        memcpy(&vtx4, &vb[ib[(4 * primitive_idx) + vertex_idx]],
                            sizeof(fix16_vector4_t));

                        fix16_matrix4_t *matrix_projection __unused;
                        matrix_projection = matrix_stack_top(
                                MATRIX_STACK_MODE_PROJECTION)->ms_matrix;

                        fix16_matrix4_t *matrix_model_view __unused;
                        matrix_model_view = matrix_stack_top(
                                MATRIX_STACK_MODE_MODEL_VIEW)->ms_matrix;

                        fix16_vector4_t *cur_projected;
                        cur_projected = &projected[vertex_idx];

                        /* PVMv = (P(VM))v */
                        fix16_matrix4_t matrix_mvp __unused;
                        fix16_matrix4_multiply(matrix_projection,
                            matrix_model_view, &matrix_mvp);
                        /* Object space */
                        fix16_vector4_matrix4_multiply(&matrix_mvp, &vtx4,
                            cur_projected);
                        /* Clip space */
                        fix16_vector4_scale(
                                fix16_div(F16(1.0f), cur_projected->w),
                                cur_projected);
                        /* NDC */
#else
                        memcpy(&projected[vertex_idx],
                            &vb[ib[(4 * primitive_idx) + vertex_idx]],
                            sizeof(fix16_vector4_t));
#endif
                }

                switch (vertex_cnt) {
                case 4:
                         sort_polygon_add(projected, primitive_idx);
                         break;
                }
        }
}

static void __unused
model_polygon_project(const fix16_vector4_t *vb, const uint32_t *ib,
    uint32_t ib_cnt)
{
        model_project(vb, ib, ib_cnt, 4);
}

static void __unused
sort_init(void)
{
        sort_primitive_idx = 0;
        sort_primitive_cnt = 0;
}

static void __unused
sort_polygon_add(const fix16_vector4_t *polygon __unused, uint32_t idx __unused)
{
#ifdef ASSERT
        assert(sort_primitive_idx < SORT_PRIMITIVE_POOL_CNT);
#endif /* ASSERT */

        struct sort_primitive *sort_primitive;
        sort_primitive = &sort_primitive_pool[sort_primitive_idx];

        uint32_t primitive_idx;
        for (primitive_idx = 0; primitive_idx < 4; primitive_idx++) {
                sort_primitive->sp_coords[primitive_idx].x =
                    fix16_to_int(fix16_mul(polygon[primitive_idx].x,
                            F16((float)SCREEN_WIDTH / 2.0f)));
                sort_primitive->sp_coords[primitive_idx].y =
                    fix16_to_int(fix16_mul(polygon[primitive_idx].y,
                            F16((float)-SCREEN_HEIGHT / 2.0f)));
                /* Screen coordinates */
        }

        fix16_t avg;
        avg = fix16_mul(fix16_add(fix16_add(polygon[0].z, polygon[1].z),
                fix16_add(polygon[2].z, polygon[3].z)), F16(1.0 / 4.0f));

        sort_primitive->sp_avg = avg;
        sort_primitive->sp_idx = idx;

        sort_primitive_ptr[sort_primitive_idx] = sort_primitive;
        sort_primitive_idx++;
}

static void __unused
sort(void)
{
        sort_primitive_cnt = sort_primitive_idx;

#if POLYGON_SORT == 1
        /* Bubble sort! */
        if (sort_primitive_cnt > 1) {
                bool swapped;
                do {
                        swapped = false;

                        uint32_t i;
                        for (i = 0; i < sort_primitive_cnt - 1; i++) {
                                struct sort_primitive *sp_i;
                                sp_i = sort_primitive_ptr[i];

                                struct sort_primitive *sp_j;
                                sp_j = sort_primitive_ptr[i + 1];

                                if (sp_j->sp_avg > sp_i->sp_avg) {
                                        sort_primitive_ptr[i] = sp_j;
                                        sort_primitive_ptr[i + 1] = sp_i;
                                        swapped = true;
                                }
                        }
                } while (swapped);
        }
#endif

        sort_primitive_idx = 0;
}

static void __unused
matrix_stack_init(void)
{
        if (matrix_stack_state.mss_initialized) {
                return;
        }

        SLIST_INIT(&matrix_stacks[MATRIX_STACK_MODE_PROJECTION]);
        SLIST_INIT(&matrix_stacks[MATRIX_STACK_MODE_MODEL_VIEW]);

        memb_init(&matrix_stack_pool);
        memb_init(&matrix_stack_matrix_pool);

        matrix_stack_state.mss_initialized = true;
        matrix_stack_state.mss_mode = MATRIX_STACK_MODE_INVALID;

        matrix_stack_mode(MATRIX_STACK_MODE_PROJECTION);
        matrix_stack_push();
        matrix_stack_identity_load();

        matrix_stack_mode(MATRIX_STACK_MODE_MODEL_VIEW);
        matrix_stack_push();
        matrix_stack_identity_load();
}

static void __unused
matrix_stack_mode(int32_t mode)
{
#ifdef ASSERT
        assert(matrix_stack_state.mss_initialized);
        assert((mode == MATRIX_STACK_MODE_PROJECTION) ||
               (mode == MATRIX_STACK_MODE_MODEL_VIEW));
#endif /* ASSERT */

        matrix_stack_state.mss_mode = mode;
}

static void __unused
matrix_stack_push(void)
{
#ifdef ASSERT
        /* Make sure the correct state is set */
        assert(matrix_stack_state.mss_initialized);
        assert(matrix_stack_state.mss_mode != MATRIX_STACK_MODE_INVALID);
#endif /* ASSERT */

        struct matrix_stack *ms;
        ms = (struct matrix_stack *)memb_alloc(&matrix_stack_pool);

        struct matrix_stack *top_ms;
        top_ms = matrix_stack_top(matrix_stack_state.mss_mode);

        ms->ms_matrix = (fix16_matrix4_t *)memb_alloc(
                &matrix_stack_matrix_pool);
#ifdef ASSERT
        assert(ms->ms_matrix != NULL);
#endif /* ASSERT */

        if (top_ms != NULL) {
                memcpy(ms->ms_matrix, top_ms->ms_matrix,
                    sizeof(fix16_matrix4_t));
        }

        SLIST_INSERT_HEAD(&matrix_stacks[matrix_stack_state.mss_mode], ms,
            ms_entries);
}

static void __unused
matrix_stack_pop(void)
{
#ifdef ASSERT
        /* Make sure the correct state is set */
        assert(matrix_stack_state.mss_initialized);
        assert(matrix_stack_state.mss_mode != MATRIX_STACK_MODE_INVALID);
#endif /* ASSERT */

        struct matrix_stack *top_ms;
        top_ms = matrix_stack_top(matrix_stack_state.mss_mode);
#ifdef ASSERT
        assert(top_ms != NULL);
#endif /* ASSERT */

        int error_code __unused;
        error_code = memb_free(&matrix_stack_matrix_pool, top_ms->ms_matrix);
#ifdef ASSERT
        assert(error_code == 0);
#endif /* ASSERT */
        error_code = memb_free(&matrix_stack_pool, top_ms);
#ifdef ASSERT
        assert(error_code == 0);
#endif
        SLIST_REMOVE_HEAD(&matrix_stacks[matrix_stack_state.mss_mode],
            ms_entries);

        /* Make sure we didn't pop off the last matrix in the stack */
#ifdef ASSERT
        assert(!(SLIST_EMPTY(&matrix_stacks[matrix_stack_state.mss_mode])));
#endif /* ASSERT */
}

static struct matrix_stack * __unused
matrix_stack_top(int32_t mode)
{
        /* Make sure the correct state is set */
#ifdef ASSERT
        assert(matrix_stack_state.mss_initialized);
        assert((mode == MATRIX_STACK_MODE_PROJECTION) ||
               (mode == MATRIX_STACK_MODE_MODEL_VIEW));
#endif /* ASSERT */

        return SLIST_FIRST(&matrix_stacks[mode]);
}

static void __unused
matrix_stack_load(fix16_matrix4_t *matrix)
{
#ifdef ASSERT
        /* Make sure the correct state is set */
        assert(matrix_stack_state.mss_initialized);
        assert(matrix_stack_state.mss_mode != MATRIX_STACK_MODE_INVALID);
#endif /* ASSERT */

        struct matrix_stack *top_ms;
        top_ms = matrix_stack_top(matrix_stack_state.mss_mode);
#ifdef ASSERT
        assert(top_ms != NULL);
#endif /* ASSERT */

        memcpy(top_ms->ms_matrix, matrix, sizeof(fix16_matrix4_t));
}

static void __unused
matrix_stack_identity_load(void)
{
#ifdef ASSERT
        /* Make sure the correct state is set */
        assert(matrix_stack_state.mss_initialized);
        assert(matrix_stack_state.mss_mode != MATRIX_STACK_MODE_INVALID);
#endif /* ASSERT */

        struct matrix_stack *top_ms;
        top_ms = matrix_stack_top(matrix_stack_state.mss_mode);
#ifdef ASSERT
        assert(top_ms != NULL);
#endif /* ASSERT */

        fix16_matrix4_identity(top_ms->ms_matrix);
}

static void __unused
matrix_stack_translate(fix16_t x, fix16_t y, fix16_t z)
{
#ifdef ASSERT
        /* Make sure the correct state is set */
        assert(matrix_stack_state.mss_initialized);
        assert(matrix_stack_state.mss_mode != MATRIX_STACK_MODE_INVALID);
#endif /* ASSERT */

        struct matrix_stack *top_ms;
        top_ms = matrix_stack_top(matrix_stack_state.mss_mode);
#ifdef ASSERT
        assert(top_ms != NULL);
#endif /* ASSERT */

        fix16_matrix4_t transform;
        fix16_matrix4_identity(&transform);

        transform.frow[0][3] = x;
        transform.frow[1][3] = y;
        transform.frow[2][3] = z;

        fix16_matrix4_t matrix;
        fix16_matrix4_multiply(top_ms->ms_matrix, &transform, &matrix);

        matrix_stack_load(&matrix);
}

static void __unused
matrix_stack_scale(fix16_t x, fix16_t y, fix16_t z)
{
#ifdef ASSERT
        /* Make sure the correct state is set */
        assert(matrix_stack_state.mss_initialized);
        assert(matrix_stack_state.mss_mode != MATRIX_STACK_MODE_INVALID);
#endif /* ASSERT */

        struct matrix_stack *top_ms;
        top_ms = matrix_stack_top(matrix_stack_state.mss_mode);

#ifdef ASSERT
        assert(top_ms != NULL);
#endif /* ASSERT */

        fix16_matrix4_t transform;
        fix16_matrix4_identity(&transform);

        transform.frow[0][0] = x;
        transform.frow[1][1] = y;
        transform.frow[2][2] = z;

        fix16_matrix4_t matrix;
        fix16_matrix4_multiply(top_ms->ms_matrix, &transform, &matrix);

        matrix_stack_load(&matrix);
}

static void __unused
matrix_stack_rotate(fix16_t angle, int32_t component)
{
#ifdef ASSERT
        /* Make sure the correct state is set */
        assert(matrix_stack_state.mss_initialized);
        assert(matrix_stack_state.mss_mode != MATRIX_STACK_MODE_INVALID);

        assert((component >= 0) && (component <= 2));
#endif /* ASSERT */

        struct matrix_stack *top_ms;
        top_ms = matrix_stack_top(matrix_stack_state.mss_mode);
#ifdef ASSERT
        assert(top_ms != NULL);
#endif /* ASSERT */

        fix16_matrix4_t transform;
        fix16_matrix4_identity(&transform);

        fix16_t sin;
        sin = fix16_sin(fix16_deg_to_rad(angle));
        fix16_t cos;
        cos = fix16_cos(fix16_deg_to_rad(angle));

        switch (component) {
        case 0: /* X */
                transform.frow[1][1] = cos;
                transform.frow[1][2] = -sin;
                transform.frow[2][1] = sin;
                transform.frow[2][2] = cos;
                break;
        case 1: /* Y */
                transform.frow[0][0] = cos;
                transform.frow[0][2] = sin;
                transform.frow[2][0] = -sin;
                transform.frow[2][2] = cos;
                break;
        case 2: /* Z */
                transform.frow[0][0] = cos;
                transform.frow[0][1] = -sin;
                transform.frow[1][0] = sin;
                transform.frow[1][1] = cos;
                break;
        }

        fix16_matrix4_t matrix;
        fix16_matrix4_multiply(top_ms->ms_matrix, &transform, &matrix);

        matrix_stack_load(&matrix);
}
