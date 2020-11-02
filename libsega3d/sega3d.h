/*
 * No copyright.
 */

#ifndef SEGA3D_H_
#define SEGA3D_H_

#include <stdint.h>

#include <fix16.h>

#include <vdp1/cmdt.h>

/* XXX: Hack: There is a strange compilation warning with ATTRIBUTE */
#pragma GCC diagnostic ignored "-Wpedantic"

#define M_PI (3.1415926535897932f)

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

#define cgaddress       0x10000
#define pal             COL_32K
#define TEXDEF(h, v, presize)                   { h, v, (cgaddress + (((presize) * 4) >> (pal))) / 8, (((h) & 0x01F8) << 5 | (v)) }
#define PICDEF(texno, cmode, pcsrc)             { (Uint16)(texno), (Uint16)(cmode), (void *)(pcsrc) }

#define CGADDRESS       0x10000
#define AdjCG(cga, hs, vs, col)                 ((cga) + (((((hs) * (vs) * 4) >> (col)) + 0x1F) & 0x7FFE0))
#define TEXTBL(hs, vs, cga)                     { hs, vs, (cga) >> 3, ((hs) & 0x01F8) << 5 | (vs) }
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
        M00, M01, M02,
        M10, M11, M12,
        M20, M21, M22,
        M30, M31, M32,
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
} PDATA;

typedef struct {
        POINT *pntbl;
        Uint32 nbPoint;
        POLYGON *pltbl;
        Uint32 nbPolygon;
        ATTR *attbl;
        VECTOR *vntbl;
} XPDATA;

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
        Uint16 texno;
        Uint16 cmode;
        void *pcsrc;
} PICTURE;

typedef enum {
        MATRIX_TYPE_PUSH     = 0,
        MATRIX_TYPE_MOVE_PTR = 1
} matrix_type_t;

extern void sega3d_tlist_alloc(Uint16 count);
extern void sega3d_tlist_free(void);
extern void sega3d_tlist_set(TEXTURE *textures, Uint16 count);
extern Uint16 sega3d_tlist_count_get(void);
extern Uint16 sega3d_tlist_cursor_get(void);
extern void sega3d_tlist_cursor_reset(void);
extern TEXTURE *sega3d_tlist_tex_append(void);
extern TEXTURE *sega3d_tlist_tex_get(Uint16 cursor);

extern void sega3d_matrix_push(matrix_type_t matrix_type);
extern void sega3d_matrix_pop(void);
extern void sega3d_matrix_load(const MATRIX *matrix);
extern void sega3d_matrix_copy(void);
extern void sega3d_matrix_translate(FIXED tx, FIXED ty, FIXED tz);
extern void sega3d_matrix_scale(FIXED sx, FIXED sy, FIXED sz);

extern Uint16 sega3d_polycount_get(const PDATA *pdata);
extern void sega3d_cmdt_prepare(const PDATA *pdata, vdp1_cmdt_list_t *cmdt_list, Uint16 offset);

#endif /* SEGA3D_H_ */
