/*
 * Copyright (c) 2008-2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include <ctype.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "vt100.h"

/*-
 * Parameter
 *   1. A string of zero or more decimal characters which represent a
 *      single value. Leading zeroes are ignored. The decimal characters
 *      have a range of 0 (\60) to 9 (\71).
 *   2. The value so represented.
 *
 * Numeric Parameter
 *   1. A parameter that represents a number, designated by Pn.
 *
 * Selective Parameter
 *   1. A parameter that selects a subfunction from a specified list of
 *      subfunctions, designated by Ps. In general, a control sequence
 *      with more than one selective parameter causes the same effect as
 *      several control sequences, each with one selective parameter,
 *      e.g., CSI Psa; Psb; Psc F is identical to CSI Psa F CSI Psb F
 *      CSI Psc F.
 *
 * Parameter String
 *   1. A string of parameters separated by a semicolon (\73).
 *
 * Default
 *   1. A function-dependent value that is assumed when no explicit
 *      value, or a value of 0, is specified.
 */

enum fc_23 {
        IND = 'D',      /* IND -- Index */
        NEL = 'E',      /* NEL -- Next Line */
        HTS = 'H',      /* HTS -- Horizontal Tabulation Set */
        RI = 'M',       /* RI -- Reverse Index */
        RIS = 'c',      /* RIS -- Reset To Initial State */
        DECSC = '7',    /* DECSC -- Save Cursor (DEC Private) */
        DECRC = '8',    /* DECRC -- Restore Cursor (DEC Private) */
};

enum fc_58 {
        CUU = 'A',      /* CUU -- Cursor Up */
        CUD = 'B',      /* CUD -- Cursor Down */
        CUF = 'C',      /* CUF -- Cursor Forward */
        CUB = 'D',      /* CUF -- Cursor Forward */
        CUP = 'H',      /* CUP -- Cursor Position */
        ES = 'J',       /* ES -- Erase Screen */
        EL = 'K',       /* EL -- Erase Line */
        CPR = 'R',      /* CPR -- Cursor Position Report */
        CHA = 'm',      /* CHA -- Character Attributes */
};

enum csi_tags {
        CSI_TAG,
        CSI_TAG_58,
        CSI_TAG_23,
        CSI_ESC_INVALID
};

static void
cha_at_set(uint32_t param, struct cha *cha_opt)
{
        enum cha_attributes {
                CHA_AT_RESET,
                CHA_AT_BRIGHT,
                CHA_AT_DIM,
                CHA_AT_UNDERSCORE,
                CHA_AT_BLINK,
                CHA_AT_REVERSE,
                CHA_AT_HIDDEN
        };

        int color;

        switch (param) {
        case CHA_AT_RESET:
                break;
        case CHA_AT_BRIGHT:
                cha_opt->fg += 8;
                cha_opt->bg += 8;
                break;
        case CHA_AT_DIM:
                if (cha_opt->fg >= 8)
                        cha_opt->fg -= 8;

                if (cha_opt->bg >= 8)
                        cha_opt->bg -= 8;
                break;
        case CHA_AT_UNDERSCORE:
                break;
        case CHA_AT_BLINK:
                break;
        case CHA_AT_REVERSE:
                /* Swap the colors. */
                color = cha_opt->fg;
                cha_opt->fg = cha_opt->bg;
                cha_opt->bg = color;
                break;
        case CHA_AT_HIDDEN:
                cha_opt->fg = 0;
                cha_opt->bg = 0;
                break;
        }
}

static int
is_csi(char **buf)
{
        /* Search for escape and tag character. */
        if (*(*buf) != '')
                return CSI_ESC_INVALID;

        (*buf)++;

        switch (*(*buf)) {
        case '[':
                return CSI_TAG_58;
        case '#':
                return CSI_TAG_23;
        case IND:
        case NEL:
        case HTS:
        case RI:
        case RIS:
        case DECSC:
        case DECRC:
                return CSI_TAG;
        default:
                return CSI_ESC_INVALID;
        }
}

static bool
is_tag_fc(char **buf, int tag)
{
        if (tag != CSI_TAG)
                return false;

        switch (*(*buf)) {
        case IND:
                (*buf)++;
                return true;
        case NEL:
                (*buf)++;
                return true;
        case HTS:
                (*buf)++;
                return true;
        case RI:
                (*buf)++;
                return true;
        case RIS:
                (*buf)++;
                return true;
        case DECSC:
                (*buf)++;
                return true;
        case DECRC:
                (*buf)++;
                return true;
        default:
                return false;
        }
}

static bool
is_tag_23_fc(char **buf, int tag)
{
        if (tag != CSI_TAG_23)
                return false;

        switch (*(*buf)) {
        default:
                return false;
        }
}

static bool
is_tag_58_fc(char **buf, int tag, int *pn, size_t len, struct cha *cha_opt)
{
        enum cha_bg_colors {
                CHA_BG_BLACK = 40,
                CHA_BG_RED,
                CHA_BG_GREEN,
                CHA_BG_YELLOW,
                CHA_BG_BLUE,
                CHA_BG_MAGNETA,
                CHA_BG_CYAN,
                CHA_BG_WHITE
        };

        enum cha_fg_colors {
                CHA_FG_BLACK = 30,
                CHA_FG_RED,
                CHA_FG_GREEN,
                CHA_FG_YELLOW,
                CHA_FG_BLUE,
                CHA_FG_MAGNETA,
                CHA_FG_CYAN,
                CHA_FG_WHITE
        };

        uint32_t i;

        if (tag != CSI_TAG_58)
                return false;

        switch (*(*buf)) {
        case CUU:
                /* ESC [ Pn A */
                (*buf)--;
                return true;
        case CUD:
                (*buf)--;
                /* ESC [ Pn B */
                return true;
        case CUF:
                (*buf)--;
                /* ESC [ Pn C */
                return true;
        case CUB:
                (*buf)--;
                /* ESC [ Pn D */
                return true;
        case CUP:
                (*buf)--;
                /* ESC [ Pn H */
                return true;
        case ES:
                (*buf)--;
                /* ESC [ Ps J */
                return true;
        case EL:
                (*buf)--;
                /* ESC [ Ps K */
                return true;
        case CPR:
                (*buf)--;
                /* ESC [ Pn;Pn R */

                if (len != 2)
                        return false;
                return true;
        case CHA:
                (*buf)++;
                /* ESC [ Ps;Ps;Ps;...;Ps m */
                cha_opt->fg = FOREGROUND;
                cha_opt->bg = BACKGROUND;
                cha_opt->is_trans = true;

                /* Obtain character attributes. */
                for (i = 0; i < len; i++) {
                        if ((pn[i] >= CHA_FG_BLACK) && (pn[i] <= CHA_FG_WHITE))
                                cha_opt->fg = (pn[i] - CHA_FG_BLACK);
                        if ((pn[i] >= CHA_BG_BLACK) && (pn[i] <= CHA_BG_WHITE))
                                cha_opt->bg = (pn[i] - CHA_BG_BLACK);
                }

                for (i = 0; i < len; i++)
                        cha_at_set(pn[i], cha_opt);

                if ((cha_opt->bg & 0x7) != BACKGROUND)
                        cha_opt->is_trans = false;

                return true;
        default:
                return false;
        }
}

static bool
is_delimiter(char **buf, int *c)
{
        char *tmp;
        char num[5];
        uint32_t i;

        for (i = 0, tmp = num; *(*buf) != '\0'; i++) {
                if (!(isdigit(*(*buf))))
                        break;

                /* Stop at the second to last element. */
                if (i == (sizeof(num) - 2)) {
                        /* Truncate all the discarded digits. */
                        while (isdigit(*(*buf)))
                                (*buf)++;
                        break;
                }
                *tmp++ = *(*buf)++;
        }
        *(tmp) = '\0';

        if (*num != '\0') {
                *c = atoi(num);
                return true;
        }

        return false;
}

int
vt100_write(write_hdl write_to, const char *s)
{
        static struct cha cha_opt = {
                FOREGROUND,
                BACKGROUND,
                false
        };

        int c;
        uint32_t i;
        uint32_t j;
        int n;
        int tag;
        int pn[4];
        char *k;
        size_t len;

        k = (char *)s;
        len = strlen(k);

        if (write_to == NULL)
                return 0;

        for (i = 0; (*k != '\0') && (i <= len); i++) {
                if ((tag = is_csi(&k)) != CSI_ESC_INVALID) {
                        for (j = i, n = 0; (*k != '\0') && (j <= len); j++) {
                                if (is_tag_fc(&k, tag))
                                        break;
                                else if (is_tag_58_fc(&k, tag, pn, n, &cha_opt))
                                        break;
                                else if (is_tag_23_fc(&k, tag))
                                        break;
                                else if (is_delimiter(&k, &c))
                                        pn[n++] = c;
                                else
                                        k++;
                        }
                } else
                        write_to(*k++, &cha_opt);
        }

        return i;
}
