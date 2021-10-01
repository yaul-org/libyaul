/*
 * No copyright.
 */

#ifndef _SEGA3D_SGL_H_
#define _SEGA3D_SGL_H_

#include <sys/cdefs.h>

/* XXX: Hack: There is a strange compilation warning with ATTRIBUTE */
#pragma GCC diagnostic ignored "-Wpedantic"

#ifndef M_PI
#define M_PI (3.1415926535897932f)
#endif /* !M_PI */

#define toFIXED(a)                              ((FIXED)(65536.0f * (a)))
#define POStoFIXED(x, y, z)                     { toFIXED(x), toFIXED(y), toFIXED(z) }
#define RADtoANG(d)                             ((ANGLE)((65536.0f * (d)) / (2 * M_PI)))
#define DEGtoANG(d)                             ((ANGLE)((65536.0f * (d)) / 360.0f))
#define RGB(r,g,b)                              (0x8000 | ((b) << 10) | ((g) << 5) | (r))

#define ATTRIBUTE(f, s, t, c, g, a, d, o) {                                    \
        f, /* flag */                                                          \
        (s) | (((d) >> 16) & 0x1C) | (o), /* sort */                           \
        t, /* texno */                                                         \
        (a) | (((d) >> 24) & 0xC0), /* atrb */                                 \
        c, /* colno */                                                         \
        g, /* gstb */                                                          \
        (d) & 0x3F /* dir */                                                   \
}

#define SPR_ATTRIBUTE(t, c, g, a, d) {                                         \
        t,                                                                     \
        (a) | (((d) >> 24) & 0xC0),                                            \
        c,                                                                     \
        g,                                                                     \
        (d) & 0x0F3F                                                           \
}

#define NORMAL(x, y, z)                         { POStoFIXED(x, y, z)
#define VERTICES(v0, v1, v2, v3)                {v0, v1, v2, v3} }
#define C_RGB(r, g, b)                          (((b) & 0x1F) << 10 | ((g) & 0x1F) << 5 | ((r) & 0x1F) | 0x8000)

/// Define this if using TEXDEF
#ifndef cgaddress
#define cgaddress       0x10000
#endif /* !cgaddress */

/// Define this if using TEXDEF
#ifndef pal
#define pal             COL_32K
#endif /* !pal */

#define TEXDEF(h, v, presize) {                                                \
        h,                                                                     \
        v,                                                                     \
        (cgaddress + (((presize) * 4) >> (pal))) / 8,                          \
        TEXHVSIZE(h, v)                                                        \
}
#define PICDEF(texno, cmode, pcsrc)             { (Uint16)(texno), (Uint16)(cmode), (void *)(pcsrc) }

#ifndef CGADDRESS
#define CGADDRESS       0x10000
#endif /* !CGADDRESS */
#define AdjCG(cga, hs, vs, col)                 ((cga) + (((((hs) * (vs) * 4) >> (col)) + 0x1F) & 0x7FFE0))

#define TEXTBL(hs, vs, cga) {                                                  \
        hs,                                                                    \
        vs,                                                                    \
        (cga) >> 3,                                                            \
        TEXHVSIZE(hs, vs)                                                      \
}

#define TEXHVSIZE(hs, vs) ((((hs) & 0x01F8) << 5) | (vs))

#define PICTBL(texno, cmode, pcsrc)             { (Uint16)(texno), (Uint16)(cmode), (void *)(pcsrc) }

#define TRANSLATION(x, y, z)                    { toFIXED(x), toFIXED(y), toFIXED(z) }
#define ROTATION(x, y, z)                       { DEGtoANG(x), DEGtoANG(y), DEGtoANG(z) }
#define SCALING(x, y, z)                        { toFIXED(x), toFIXED(y), toFIXED(z) }

#define FUNC_Sprite             (1)
#define FUNC_Texture            (2)
#define FUNC_Polygon            (4)
#define FUNC_PolyLine           (5)
#define FUNC_Line               (6)
#define FUNC_SystemClip         (9)
#define FUNC_UserClip           (8)
#define FUNC_BasePosition       (10)
#define FUNC_End                (-1)

#define MSBon           (1 << 15)
#define HSSon           (1 << 12)
#define HSSoff          (0 << 12)
#define No_Window       (0 << 9)
#define Window_In       (2 << 9)
#define Window_Out      (3 << 9)
#define MESHoff         (0 << 8)
#define MESHon          (1 << 8)
#define ECdis           (1 << 7)
#define ECenb           (0 << 7)
#define SPdis           (1 << 6)
#define SPenb           (0 << 6)
#define CL16Bnk         (0 << 3)
#define CL16Look        (1 << 3)
#define CL64Bnk         (2 << 3)
#define CL128Bnk        (3 << 3)
#define CL256Bnk        (4 << 3)
#define CL32KRGB        (5 << 3)
#define CL_Replace      0
#define CL_Shadow       1
#define CL_Half         2
#define CL_Trans        3
#define CL_Gouraud      4

#define UseTexture      (1 << 2)
#define UseLight        (1 << 3)
#define UsePalette      (1 << 5)

#define UseNearClip     (1 << 6)
#define UseGouraud      (1 << 7)
#define UseDepth        (1 << 4)

#define UseClip         UseNearClip

#define sprHflip        ((1 << 4) | FUNC_Texture | (UseTexture << 16))
#define sprVflip        ((1 << 5) | FUNC_Texture | (UseTexture << 16))
#define sprHVflip       ((3 << 4) | FUNC_Texture | (UseTexture << 16))
#define sprNoflip       ((0) | FUNC_Texture | (UseTexture << 16))
#define sprPolygon      (FUNC_Polygon | ((ECdis | SPdis) << 24))
#define sprPolyLine     (FUNC_PolyLine | ((ECdis | SPdis) << 24))
#define sprLine         (FUNC_Line | ((ECdis | SPdis) << 24))

#define No_Texture      0
#define No_Option       0
#define No_Gouraud      0
#define No_Palet        0
#define GouraudRAM      (0x00080000 - (32 * 8)) /* 光源の影響用のグーローバッファ */

#define COL_16          (2 + 1)
#define COL_64          (2 + 0)
#define COL_128         (2 + 0)
#define COL_256         (2 + 0)
#define COL_32K         (2 - 1)

typedef unsigned char Uint8;
typedef signed char Sint8;
typedef unsigned short Uint16;
typedef signed short Sint16;
typedef unsigned long Uint32;
typedef signed long Sint32;
typedef int Int;
typedef int Bool;

typedef Uint16 TEXDAT;

enum mtrx {
        M00, M01, M02, M03,
        M10, M11, M12, M13,
        M20, M21, M22, M23,
        MTRX
};

enum ps {
        X,
        Y,
        Z,
        XYZ,
        XYZS,
        XYZSS,
        XY = Z,
        S = XYZ,
        Sh = S,
        Sv = XYZS
};

enum base {
        SORT_BFR,
        SORT_MIN,
        SORT_MAX,
        SORT_CEN
};

enum pln {
        Single_Plane = 0,
        Dual_Plane   = 1
};

typedef Sint16 ANGLE;
typedef Sint32 FIXED;

typedef FIXED MATRIX[4][3];
typedef FIXED VECTOR[XYZ];
typedef FIXED POINT[XYZ];

typedef struct {
        VECTOR norm;
        Uint16 Vertices[4];
} POLYGON;

typedef struct {
        Uint8 flag;
        Uint8 sort;
        Uint16 texno;
        Uint16 atrb;
        Uint16 colno;
        Uint16 gstb;
        Uint16 dir;
} ATTR;

typedef struct {
        POINT *pntbl;
        Uint32 nbPoint;
        POLYGON *pltbl;
        Uint32 nbPolygon;
        ATTR *attbl;
} __packed __aligned(4) PDATA;

typedef struct {
        POINT *pntbl;
        Uint32 nbPoint;
        POLYGON *pltbl;
        Uint32 nbPolygon;
        ATTR *attbl;
        VECTOR *vntbl;
} __packed __aligned(4) XPDATA;

typedef struct OBJECT {
        PDATA *pat;
        FIXED pos[XYZ];
        ANGLE ang[XYZ];
        FIXED scl[XYZ];
        struct OBJECT *child;
        struct OBJECT *sibling;
} OBJECT;

typedef struct {
        Uint16 Hsize;
        Uint16 Vsize;
        Uint16 CGadr;
        Uint16 HVsize;
} TEXTURE;

typedef struct {
        Uint16 Color;
} PALETTE;

typedef struct {
        Uint16 texno;
        Uint16 cmode;
        void *pcsrc;
} PICTURE;

#endif /* !_SEGA3D_SGL_H_ */
