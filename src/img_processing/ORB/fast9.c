/**
 * @brief took from opencv and cleared from dependencies
 */

#define TAG "ORB_FAST"

 
#include "img_processing/ORB_defs.h"


static void makeOffsets(
    int pixel[],
    uint16_t row_stride
) {
    pixel[0] = 0 + row_stride * 3;
    pixel[1] = 1 + row_stride * 3;
    pixel[2] = 2 + row_stride * 2;
    pixel[3] = 3 + row_stride * 1;
    pixel[4] = 3 + row_stride * 0;
    pixel[5] = 3 + row_stride * -1;
    pixel[6] = 2 + row_stride * -2;
    pixel[7] = 1 + row_stride * -3;
    pixel[8] = 0 + row_stride * -3;
    pixel[9] = -1 + row_stride * -3;
    pixel[10] = -2 + row_stride * -2;
    pixel[11] = -3 + row_stride * -1;
    pixel[12] = -3 + row_stride * 0;
    pixel[13] = -3 + row_stride * 1;
    pixel[14] = -2 + row_stride * 2;
    pixel[15] = -1 + row_stride * 3;
}


static uint8_t fast9CornerScore(
    const uint8_t *p,
    const int pixel[],
    uint8_t bstart
) {
    uint8_t bmin = bstart;
    uint8_t bmax = 255;
    uint8_t b = (bmax + bmin) / 2;

    // Compute the score using binary search
    for (;;)
    {
        uint8_t cb = *p + b;
        uint8_t c_b = *p - b;

        if (p[pixel[0]] > cb)
            if (p[pixel[1]] > cb)
                if (p[pixel[2]] > cb)
                    if (p[pixel[3]] > cb)
                        if (p[pixel[4]] > cb)
                            if (p[pixel[5]] > cb)
                                if (p[pixel[6]] > cb)
                                    if (p[pixel[7]] > cb)
                                        if (p[pixel[8]] > cb)
                                            goto is_a_corner;
                                        else if (p[pixel[15]] > cb)
                                            goto is_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else if (p[pixel[7]] < c_b)
                                        if (p[pixel[14]] > cb)
                                            if (p[pixel[15]] > cb)
                                                goto is_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else if (p[pixel[14]] < c_b)
                                            if (p[pixel[8]] < c_b)
                                                if (p[pixel[9]] < c_b)
                                                    if (p[pixel[10]] < c_b)
                                                        if (p[pixel[11]] < c_b)
                                                            if (p[pixel[12]] < c_b)
                                                                if (p[pixel[13]] < c_b)
                                                                    if (p[pixel[15]] < c_b)
                                                                        goto is_a_corner;
                                                                    else
                                                                        goto is_not_a_corner;
                                                                else
                                                                    goto is_not_a_corner;
                                                            else
                                                                goto is_not_a_corner;
                                                        else
                                                            goto is_not_a_corner;
                                                    else
                                                        goto is_not_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else if (p[pixel[14]] > cb)
                                        if (p[pixel[15]] > cb)
                                            goto is_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else if (p[pixel[6]] < c_b)
                                    if (p[pixel[15]] > cb)
                                        if (p[pixel[13]] > cb)
                                            if (p[pixel[14]] > cb)
                                                goto is_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else if (p[pixel[13]] < c_b)
                                            if (p[pixel[7]] < c_b)
                                                if (p[pixel[8]] < c_b)
                                                    if (p[pixel[9]] < c_b)
                                                        if (p[pixel[10]] < c_b)
                                                            if (p[pixel[11]] < c_b)
                                                                if (p[pixel[12]] < c_b)
                                                                    if (p[pixel[14]] < c_b)
                                                                        goto is_a_corner;
                                                                    else
                                                                        goto is_not_a_corner;
                                                                else
                                                                    goto is_not_a_corner;
                                                            else
                                                                goto is_not_a_corner;
                                                        else
                                                            goto is_not_a_corner;
                                                    else
                                                        goto is_not_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else if (p[pixel[7]] < c_b)
                                        if (p[pixel[8]] < c_b)
                                            if (p[pixel[9]] < c_b)
                                                if (p[pixel[10]] < c_b)
                                                    if (p[pixel[11]] < c_b)
                                                        if (p[pixel[12]] < c_b)
                                                            if (p[pixel[13]] < c_b)
                                                                if (p[pixel[14]] < c_b)
                                                                    goto is_a_corner;
                                                                else
                                                                    goto is_not_a_corner;
                                                            else
                                                                goto is_not_a_corner;
                                                        else
                                                            goto is_not_a_corner;
                                                    else
                                                        goto is_not_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else if (p[pixel[13]] > cb)
                                    if (p[pixel[14]] > cb)
                                        if (p[pixel[15]] > cb)
                                            goto is_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else if (p[pixel[13]] < c_b)
                                    if (p[pixel[7]] < c_b)
                                        if (p[pixel[8]] < c_b)
                                            if (p[pixel[9]] < c_b)
                                                if (p[pixel[10]] < c_b)
                                                    if (p[pixel[11]] < c_b)
                                                        if (p[pixel[12]] < c_b)
                                                            if (p[pixel[14]] < c_b)
                                                                if (p[pixel[15]] < c_b)
                                                                    goto is_a_corner;
                                                                else
                                                                    goto is_not_a_corner;
                                                            else
                                                                goto is_not_a_corner;
                                                        else
                                                            goto is_not_a_corner;
                                                    else
                                                        goto is_not_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else
                                    goto is_not_a_corner;
                            else if (p[pixel[5]] < c_b)
                                if (p[pixel[14]] > cb)
                                    if (p[pixel[12]] > cb)
                                        if (p[pixel[13]] > cb)
                                            if (p[pixel[15]] > cb)
                                                goto is_a_corner;
                                            else if (p[pixel[6]] > cb)
                                                if (p[pixel[7]] > cb)
                                                    if (p[pixel[8]] > cb)
                                                        if (p[pixel[9]] > cb)
                                                            if (p[pixel[10]] > cb)
                                                                if (p[pixel[11]] > cb)
                                                                    goto is_a_corner;
                                                                else
                                                                    goto is_not_a_corner;
                                                            else
                                                                goto is_not_a_corner;
                                                        else
                                                            goto is_not_a_corner;
                                                    else
                                                        goto is_not_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else if (p[pixel[12]] < c_b)
                                        if (p[pixel[6]] < c_b)
                                            if (p[pixel[7]] < c_b)
                                                if (p[pixel[8]] < c_b)
                                                    if (p[pixel[9]] < c_b)
                                                        if (p[pixel[10]] < c_b)
                                                            if (p[pixel[11]] < c_b)
                                                                if (p[pixel[13]] < c_b)
                                                                    goto is_a_corner;
                                                                else
                                                                    goto is_not_a_corner;
                                                            else
                                                                goto is_not_a_corner;
                                                        else
                                                            goto is_not_a_corner;
                                                    else
                                                        goto is_not_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else if (p[pixel[14]] < c_b)
                                    if (p[pixel[7]] < c_b)
                                        if (p[pixel[8]] < c_b)
                                            if (p[pixel[9]] < c_b)
                                                if (p[pixel[10]] < c_b)
                                                    if (p[pixel[11]] < c_b)
                                                        if (p[pixel[12]] < c_b)
                                                            if (p[pixel[13]] < c_b)
                                                                if (p[pixel[6]] < c_b)
                                                                    goto is_a_corner;
                                                                else if (p[pixel[15]] < c_b)
                                                                    goto is_a_corner;
                                                                else
                                                                    goto is_not_a_corner;
                                                            else
                                                                goto is_not_a_corner;
                                                        else
                                                            goto is_not_a_corner;
                                                    else
                                                        goto is_not_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else if (p[pixel[6]] < c_b)
                                    if (p[pixel[7]] < c_b)
                                        if (p[pixel[8]] < c_b)
                                            if (p[pixel[9]] < c_b)
                                                if (p[pixel[10]] < c_b)
                                                    if (p[pixel[11]] < c_b)
                                                        if (p[pixel[12]] < c_b)
                                                            if (p[pixel[13]] < c_b)
                                                                goto is_a_corner;
                                                            else
                                                                goto is_not_a_corner;
                                                        else
                                                            goto is_not_a_corner;
                                                    else
                                                        goto is_not_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else
                                    goto is_not_a_corner;
                            else if (p[pixel[12]] > cb)
                                if (p[pixel[13]] > cb)
                                    if (p[pixel[14]] > cb)
                                        if (p[pixel[15]] > cb)
                                            goto is_a_corner;
                                        else if (p[pixel[6]] > cb)
                                            if (p[pixel[7]] > cb)
                                                if (p[pixel[8]] > cb)
                                                    if (p[pixel[9]] > cb)
                                                        if (p[pixel[10]] > cb)
                                                            if (p[pixel[11]] > cb)
                                                                goto is_a_corner;
                                                            else
                                                                goto is_not_a_corner;
                                                        else
                                                            goto is_not_a_corner;
                                                    else
                                                        goto is_not_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else
                                    goto is_not_a_corner;
                            else if (p[pixel[12]] < c_b)
                                if (p[pixel[7]] < c_b)
                                    if (p[pixel[8]] < c_b)
                                        if (p[pixel[9]] < c_b)
                                            if (p[pixel[10]] < c_b)
                                                if (p[pixel[11]] < c_b)
                                                    if (p[pixel[13]] < c_b)
                                                        if (p[pixel[14]] < c_b)
                                                            if (p[pixel[6]] < c_b)
                                                                goto is_a_corner;
                                                            else if (p[pixel[15]] < c_b)
                                                                goto is_a_corner;
                                                            else
                                                                goto is_not_a_corner;
                                                        else
                                                            goto is_not_a_corner;
                                                    else
                                                        goto is_not_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else
                                    goto is_not_a_corner;
                            else
                                goto is_not_a_corner;
                        else if (p[pixel[4]] < c_b)
                            if (p[pixel[13]] > cb)
                                if (p[pixel[11]] > cb)
                                    if (p[pixel[12]] > cb)
                                        if (p[pixel[14]] > cb)
                                            if (p[pixel[15]] > cb)
                                                goto is_a_corner;
                                            else if (p[pixel[6]] > cb)
                                                if (p[pixel[7]] > cb)
                                                    if (p[pixel[8]] > cb)
                                                        if (p[pixel[9]] > cb)
                                                            if (p[pixel[10]] > cb)
                                                                goto is_a_corner;
                                                            else
                                                                goto is_not_a_corner;
                                                        else
                                                            goto is_not_a_corner;
                                                    else
                                                        goto is_not_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else if (p[pixel[5]] > cb)
                                            if (p[pixel[6]] > cb)
                                                if (p[pixel[7]] > cb)
                                                    if (p[pixel[8]] > cb)
                                                        if (p[pixel[9]] > cb)
                                                            if (p[pixel[10]] > cb)
                                                                goto is_a_corner;
                                                            else
                                                                goto is_not_a_corner;
                                                        else
                                                            goto is_not_a_corner;
                                                    else
                                                        goto is_not_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else if (p[pixel[11]] < c_b)
                                    if (p[pixel[5]] < c_b)
                                        if (p[pixel[6]] < c_b)
                                            if (p[pixel[7]] < c_b)
                                                if (p[pixel[8]] < c_b)
                                                    if (p[pixel[9]] < c_b)
                                                        if (p[pixel[10]] < c_b)
                                                            if (p[pixel[12]] < c_b)
                                                                goto is_a_corner;
                                                            else
                                                                goto is_not_a_corner;
                                                        else
                                                            goto is_not_a_corner;
                                                    else
                                                        goto is_not_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else
                                    goto is_not_a_corner;
                            else if (p[pixel[13]] < c_b)
                                if (p[pixel[7]] < c_b)
                                    if (p[pixel[8]] < c_b)
                                        if (p[pixel[9]] < c_b)
                                            if (p[pixel[10]] < c_b)
                                                if (p[pixel[11]] < c_b)
                                                    if (p[pixel[12]] < c_b)
                                                        if (p[pixel[6]] < c_b)
                                                            if (p[pixel[5]] < c_b)
                                                                goto is_a_corner;
                                                            else if (p[pixel[14]] < c_b)
                                                                goto is_a_corner;
                                                            else
                                                                goto is_not_a_corner;
                                                        else if (p[pixel[14]] < c_b)
                                                            if (p[pixel[15]] < c_b)
                                                                goto is_a_corner;
                                                            else
                                                                goto is_not_a_corner;
                                                        else
                                                            goto is_not_a_corner;
                                                    else
                                                        goto is_not_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else
                                    goto is_not_a_corner;
                            else if (p[pixel[5]] < c_b)
                                if (p[pixel[6]] < c_b)
                                    if (p[pixel[7]] < c_b)
                                        if (p[pixel[8]] < c_b)
                                            if (p[pixel[9]] < c_b)
                                                if (p[pixel[10]] < c_b)
                                                    if (p[pixel[11]] < c_b)
                                                        if (p[pixel[12]] < c_b)
                                                            goto is_a_corner;
                                                        else
                                                            goto is_not_a_corner;
                                                    else
                                                        goto is_not_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else
                                    goto is_not_a_corner;
                            else
                                goto is_not_a_corner;
                        else if (p[pixel[11]] > cb)
                            if (p[pixel[12]] > cb)
                                if (p[pixel[13]] > cb)
                                    if (p[pixel[14]] > cb)
                                        if (p[pixel[15]] > cb)
                                            goto is_a_corner;
                                        else if (p[pixel[6]] > cb)
                                            if (p[pixel[7]] > cb)
                                                if (p[pixel[8]] > cb)
                                                    if (p[pixel[9]] > cb)
                                                        if (p[pixel[10]] > cb)
                                                            goto is_a_corner;
                                                        else
                                                            goto is_not_a_corner;
                                                    else
                                                        goto is_not_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else if (p[pixel[5]] > cb)
                                        if (p[pixel[6]] > cb)
                                            if (p[pixel[7]] > cb)
                                                if (p[pixel[8]] > cb)
                                                    if (p[pixel[9]] > cb)
                                                        if (p[pixel[10]] > cb)
                                                            goto is_a_corner;
                                                        else
                                                            goto is_not_a_corner;
                                                    else
                                                        goto is_not_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else
                                    goto is_not_a_corner;
                            else
                                goto is_not_a_corner;
                        else if (p[pixel[11]] < c_b)
                            if (p[pixel[7]] < c_b)
                                if (p[pixel[8]] < c_b)
                                    if (p[pixel[9]] < c_b)
                                        if (p[pixel[10]] < c_b)
                                            if (p[pixel[12]] < c_b)
                                                if (p[pixel[13]] < c_b)
                                                    if (p[pixel[6]] < c_b)
                                                        if (p[pixel[5]] < c_b)
                                                            goto is_a_corner;
                                                        else if (p[pixel[14]] < c_b)
                                                            goto is_a_corner;
                                                        else
                                                            goto is_not_a_corner;
                                                    else if (p[pixel[14]] < c_b)
                                                        if (p[pixel[15]] < c_b)
                                                            goto is_a_corner;
                                                        else
                                                            goto is_not_a_corner;
                                                    else
                                                        goto is_not_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else
                                    goto is_not_a_corner;
                            else
                                goto is_not_a_corner;
                        else
                            goto is_not_a_corner;
                    else if (p[pixel[3]] < c_b)
                        if (p[pixel[10]] > cb)
                            if (p[pixel[11]] > cb)
                                if (p[pixel[12]] > cb)
                                    if (p[pixel[13]] > cb)
                                        if (p[pixel[14]] > cb)
                                            if (p[pixel[15]] > cb)
                                                goto is_a_corner;
                                            else if (p[pixel[6]] > cb)
                                                if (p[pixel[7]] > cb)
                                                    if (p[pixel[8]] > cb)
                                                        if (p[pixel[9]] > cb)
                                                            goto is_a_corner;
                                                        else
                                                            goto is_not_a_corner;
                                                    else
                                                        goto is_not_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else if (p[pixel[5]] > cb)
                                            if (p[pixel[6]] > cb)
                                                if (p[pixel[7]] > cb)
                                                    if (p[pixel[8]] > cb)
                                                        if (p[pixel[9]] > cb)
                                                            goto is_a_corner;
                                                        else
                                                            goto is_not_a_corner;
                                                    else
                                                        goto is_not_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else if (p[pixel[4]] > cb)
                                        if (p[pixel[5]] > cb)
                                            if (p[pixel[6]] > cb)
                                                if (p[pixel[7]] > cb)
                                                    if (p[pixel[8]] > cb)
                                                        if (p[pixel[9]] > cb)
                                                            goto is_a_corner;
                                                        else
                                                            goto is_not_a_corner;
                                                    else
                                                        goto is_not_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else
                                    goto is_not_a_corner;
                            else
                                goto is_not_a_corner;
                        else if (p[pixel[10]] < c_b)
                            if (p[pixel[7]] < c_b)
                                if (p[pixel[8]] < c_b)
                                    if (p[pixel[9]] < c_b)
                                        if (p[pixel[11]] < c_b)
                                            if (p[pixel[6]] < c_b)
                                                if (p[pixel[5]] < c_b)
                                                    if (p[pixel[4]] < c_b)
                                                        goto is_a_corner;
                                                    else if (p[pixel[12]] < c_b)
                                                        if (p[pixel[13]] < c_b)
                                                            goto is_a_corner;
                                                        else
                                                            goto is_not_a_corner;
                                                    else
                                                        goto is_not_a_corner;
                                                else if (p[pixel[12]] < c_b)
                                                    if (p[pixel[13]] < c_b)
                                                        if (p[pixel[14]] < c_b)
                                                            goto is_a_corner;
                                                        else
                                                            goto is_not_a_corner;
                                                    else
                                                        goto is_not_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else if (p[pixel[12]] < c_b)
                                                if (p[pixel[13]] < c_b)
                                                    if (p[pixel[14]] < c_b)
                                                        if (p[pixel[15]] < c_b)
                                                            goto is_a_corner;
                                                        else
                                                            goto is_not_a_corner;
                                                    else
                                                        goto is_not_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else
                                    goto is_not_a_corner;
                            else
                                goto is_not_a_corner;
                        else
                            goto is_not_a_corner;
                    else if (p[pixel[10]] > cb)
                        if (p[pixel[11]] > cb)
                            if (p[pixel[12]] > cb)
                                if (p[pixel[13]] > cb)
                                    if (p[pixel[14]] > cb)
                                        if (p[pixel[15]] > cb)
                                            goto is_a_corner;
                                        else if (p[pixel[6]] > cb)
                                            if (p[pixel[7]] > cb)
                                                if (p[pixel[8]] > cb)
                                                    if (p[pixel[9]] > cb)
                                                        goto is_a_corner;
                                                    else
                                                        goto is_not_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else if (p[pixel[5]] > cb)
                                        if (p[pixel[6]] > cb)
                                            if (p[pixel[7]] > cb)
                                                if (p[pixel[8]] > cb)
                                                    if (p[pixel[9]] > cb)
                                                        goto is_a_corner;
                                                    else
                                                        goto is_not_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else if (p[pixel[4]] > cb)
                                    if (p[pixel[5]] > cb)
                                        if (p[pixel[6]] > cb)
                                            if (p[pixel[7]] > cb)
                                                if (p[pixel[8]] > cb)
                                                    if (p[pixel[9]] > cb)
                                                        goto is_a_corner;
                                                    else
                                                        goto is_not_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else
                                    goto is_not_a_corner;
                            else
                                goto is_not_a_corner;
                        else
                            goto is_not_a_corner;
                    else if (p[pixel[10]] < c_b)
                        if (p[pixel[7]] < c_b)
                            if (p[pixel[8]] < c_b)
                                if (p[pixel[9]] < c_b)
                                    if (p[pixel[11]] < c_b)
                                        if (p[pixel[12]] < c_b)
                                            if (p[pixel[6]] < c_b)
                                                if (p[pixel[5]] < c_b)
                                                    if (p[pixel[4]] < c_b)
                                                        goto is_a_corner;
                                                    else if (p[pixel[13]] < c_b)
                                                        goto is_a_corner;
                                                    else
                                                        goto is_not_a_corner;
                                                else if (p[pixel[13]] < c_b)
                                                    if (p[pixel[14]] < c_b)
                                                        goto is_a_corner;
                                                    else
                                                        goto is_not_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else if (p[pixel[13]] < c_b)
                                                if (p[pixel[14]] < c_b)
                                                    if (p[pixel[15]] < c_b)
                                                        goto is_a_corner;
                                                    else
                                                        goto is_not_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else
                                    goto is_not_a_corner;
                            else
                                goto is_not_a_corner;
                        else
                            goto is_not_a_corner;
                    else
                        goto is_not_a_corner;
                else if (p[pixel[2]] < c_b)
                    if (p[pixel[9]] > cb)
                        if (p[pixel[10]] > cb)
                            if (p[pixel[11]] > cb)
                                if (p[pixel[12]] > cb)
                                    if (p[pixel[13]] > cb)
                                        if (p[pixel[14]] > cb)
                                            if (p[pixel[15]] > cb)
                                                goto is_a_corner;
                                            else if (p[pixel[6]] > cb)
                                                if (p[pixel[7]] > cb)
                                                    if (p[pixel[8]] > cb)
                                                        goto is_a_corner;
                                                    else
                                                        goto is_not_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else if (p[pixel[5]] > cb)
                                            if (p[pixel[6]] > cb)
                                                if (p[pixel[7]] > cb)
                                                    if (p[pixel[8]] > cb)
                                                        goto is_a_corner;
                                                    else
                                                        goto is_not_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else if (p[pixel[4]] > cb)
                                        if (p[pixel[5]] > cb)
                                            if (p[pixel[6]] > cb)
                                                if (p[pixel[7]] > cb)
                                                    if (p[pixel[8]] > cb)
                                                        goto is_a_corner;
                                                    else
                                                        goto is_not_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else if (p[pixel[3]] > cb)
                                    if (p[pixel[4]] > cb)
                                        if (p[pixel[5]] > cb)
                                            if (p[pixel[6]] > cb)
                                                if (p[pixel[7]] > cb)
                                                    if (p[pixel[8]] > cb)
                                                        goto is_a_corner;
                                                    else
                                                        goto is_not_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else
                                    goto is_not_a_corner;
                            else
                                goto is_not_a_corner;
                        else
                            goto is_not_a_corner;
                    else if (p[pixel[9]] < c_b)
                        if (p[pixel[7]] < c_b)
                            if (p[pixel[8]] < c_b)
                                if (p[pixel[10]] < c_b)
                                    if (p[pixel[6]] < c_b)
                                        if (p[pixel[5]] < c_b)
                                            if (p[pixel[4]] < c_b)
                                                if (p[pixel[3]] < c_b)
                                                    goto is_a_corner;
                                                else if (p[pixel[11]] < c_b)
                                                    if (p[pixel[12]] < c_b)
                                                        goto is_a_corner;
                                                    else
                                                        goto is_not_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else if (p[pixel[11]] < c_b)
                                                if (p[pixel[12]] < c_b)
                                                    if (p[pixel[13]] < c_b)
                                                        goto is_a_corner;
                                                    else
                                                        goto is_not_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else if (p[pixel[11]] < c_b)
                                            if (p[pixel[12]] < c_b)
                                                if (p[pixel[13]] < c_b)
                                                    if (p[pixel[14]] < c_b)
                                                        goto is_a_corner;
                                                    else
                                                        goto is_not_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else if (p[pixel[11]] < c_b)
                                        if (p[pixel[12]] < c_b)
                                            if (p[pixel[13]] < c_b)
                                                if (p[pixel[14]] < c_b)
                                                    if (p[pixel[15]] < c_b)
                                                        goto is_a_corner;
                                                    else
                                                        goto is_not_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else
                                    goto is_not_a_corner;
                            else
                                goto is_not_a_corner;
                        else
                            goto is_not_a_corner;
                    else
                        goto is_not_a_corner;
                else if (p[pixel[9]] > cb)
                    if (p[pixel[10]] > cb)
                        if (p[pixel[11]] > cb)
                            if (p[pixel[12]] > cb)
                                if (p[pixel[13]] > cb)
                                    if (p[pixel[14]] > cb)
                                        if (p[pixel[15]] > cb)
                                            goto is_a_corner;
                                        else if (p[pixel[6]] > cb)
                                            if (p[pixel[7]] > cb)
                                                if (p[pixel[8]] > cb)
                                                    goto is_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else if (p[pixel[5]] > cb)
                                        if (p[pixel[6]] > cb)
                                            if (p[pixel[7]] > cb)
                                                if (p[pixel[8]] > cb)
                                                    goto is_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else if (p[pixel[4]] > cb)
                                    if (p[pixel[5]] > cb)
                                        if (p[pixel[6]] > cb)
                                            if (p[pixel[7]] > cb)
                                                if (p[pixel[8]] > cb)
                                                    goto is_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else
                                    goto is_not_a_corner;
                            else if (p[pixel[3]] > cb)
                                if (p[pixel[4]] > cb)
                                    if (p[pixel[5]] > cb)
                                        if (p[pixel[6]] > cb)
                                            if (p[pixel[7]] > cb)
                                                if (p[pixel[8]] > cb)
                                                    goto is_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else
                                    goto is_not_a_corner;
                            else
                                goto is_not_a_corner;
                        else
                            goto is_not_a_corner;
                    else
                        goto is_not_a_corner;
                else if (p[pixel[9]] < c_b)
                    if (p[pixel[7]] < c_b)
                        if (p[pixel[8]] < c_b)
                            if (p[pixel[10]] < c_b)
                                if (p[pixel[11]] < c_b)
                                    if (p[pixel[6]] < c_b)
                                        if (p[pixel[5]] < c_b)
                                            if (p[pixel[4]] < c_b)
                                                if (p[pixel[3]] < c_b)
                                                    goto is_a_corner;
                                                else if (p[pixel[12]] < c_b)
                                                    goto is_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else if (p[pixel[12]] < c_b)
                                                if (p[pixel[13]] < c_b)
                                                    goto is_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else if (p[pixel[12]] < c_b)
                                            if (p[pixel[13]] < c_b)
                                                if (p[pixel[14]] < c_b)
                                                    goto is_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else if (p[pixel[12]] < c_b)
                                        if (p[pixel[13]] < c_b)
                                            if (p[pixel[14]] < c_b)
                                                if (p[pixel[15]] < c_b)
                                                    goto is_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else
                                    goto is_not_a_corner;
                            else
                                goto is_not_a_corner;
                        else
                            goto is_not_a_corner;
                    else
                        goto is_not_a_corner;
                else
                    goto is_not_a_corner;
            else if (p[pixel[1]] < c_b)
                if (p[pixel[8]] > cb)
                    if (p[pixel[9]] > cb)
                        if (p[pixel[10]] > cb)
                            if (p[pixel[11]] > cb)
                                if (p[pixel[12]] > cb)
                                    if (p[pixel[13]] > cb)
                                        if (p[pixel[14]] > cb)
                                            if (p[pixel[15]] > cb)
                                                goto is_a_corner;
                                            else if (p[pixel[6]] > cb)
                                                if (p[pixel[7]] > cb)
                                                    goto is_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else if (p[pixel[5]] > cb)
                                            if (p[pixel[6]] > cb)
                                                if (p[pixel[7]] > cb)
                                                    goto is_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else if (p[pixel[4]] > cb)
                                        if (p[pixel[5]] > cb)
                                            if (p[pixel[6]] > cb)
                                                if (p[pixel[7]] > cb)
                                                    goto is_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else if (p[pixel[3]] > cb)
                                    if (p[pixel[4]] > cb)
                                        if (p[pixel[5]] > cb)
                                            if (p[pixel[6]] > cb)
                                                if (p[pixel[7]] > cb)
                                                    goto is_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else
                                    goto is_not_a_corner;
                            else if (p[pixel[2]] > cb)
                                if (p[pixel[3]] > cb)
                                    if (p[pixel[4]] > cb)
                                        if (p[pixel[5]] > cb)
                                            if (p[pixel[6]] > cb)
                                                if (p[pixel[7]] > cb)
                                                    goto is_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else
                                    goto is_not_a_corner;
                            else
                                goto is_not_a_corner;
                        else
                            goto is_not_a_corner;
                    else
                        goto is_not_a_corner;
                else if (p[pixel[8]] < c_b)
                    if (p[pixel[7]] < c_b)
                        if (p[pixel[9]] < c_b)
                            if (p[pixel[6]] < c_b)
                                if (p[pixel[5]] < c_b)
                                    if (p[pixel[4]] < c_b)
                                        if (p[pixel[3]] < c_b)
                                            if (p[pixel[2]] < c_b)
                                                goto is_a_corner;
                                            else if (p[pixel[10]] < c_b)
                                                if (p[pixel[11]] < c_b)
                                                    goto is_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else if (p[pixel[10]] < c_b)
                                            if (p[pixel[11]] < c_b)
                                                if (p[pixel[12]] < c_b)
                                                    goto is_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else if (p[pixel[10]] < c_b)
                                        if (p[pixel[11]] < c_b)
                                            if (p[pixel[12]] < c_b)
                                                if (p[pixel[13]] < c_b)
                                                    goto is_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else if (p[pixel[10]] < c_b)
                                    if (p[pixel[11]] < c_b)
                                        if (p[pixel[12]] < c_b)
                                            if (p[pixel[13]] < c_b)
                                                if (p[pixel[14]] < c_b)
                                                    goto is_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else
                                    goto is_not_a_corner;
                            else if (p[pixel[10]] < c_b)
                                if (p[pixel[11]] < c_b)
                                    if (p[pixel[12]] < c_b)
                                        if (p[pixel[13]] < c_b)
                                            if (p[pixel[14]] < c_b)
                                                if (p[pixel[15]] < c_b)
                                                    goto is_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else
                                    goto is_not_a_corner;
                            else
                                goto is_not_a_corner;
                        else
                            goto is_not_a_corner;
                    else
                        goto is_not_a_corner;
                else
                    goto is_not_a_corner;
            else if (p[pixel[8]] > cb)
                if (p[pixel[9]] > cb)
                    if (p[pixel[10]] > cb)
                        if (p[pixel[11]] > cb)
                            if (p[pixel[12]] > cb)
                                if (p[pixel[13]] > cb)
                                    if (p[pixel[14]] > cb)
                                        if (p[pixel[15]] > cb)
                                            goto is_a_corner;
                                        else if (p[pixel[6]] > cb)
                                            if (p[pixel[7]] > cb)
                                                goto is_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else if (p[pixel[5]] > cb)
                                        if (p[pixel[6]] > cb)
                                            if (p[pixel[7]] > cb)
                                                goto is_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else if (p[pixel[4]] > cb)
                                    if (p[pixel[5]] > cb)
                                        if (p[pixel[6]] > cb)
                                            if (p[pixel[7]] > cb)
                                                goto is_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else
                                    goto is_not_a_corner;
                            else if (p[pixel[3]] > cb)
                                if (p[pixel[4]] > cb)
                                    if (p[pixel[5]] > cb)
                                        if (p[pixel[6]] > cb)
                                            if (p[pixel[7]] > cb)
                                                goto is_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else
                                    goto is_not_a_corner;
                            else
                                goto is_not_a_corner;
                        else if (p[pixel[2]] > cb)
                            if (p[pixel[3]] > cb)
                                if (p[pixel[4]] > cb)
                                    if (p[pixel[5]] > cb)
                                        if (p[pixel[6]] > cb)
                                            if (p[pixel[7]] > cb)
                                                goto is_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else
                                    goto is_not_a_corner;
                            else
                                goto is_not_a_corner;
                        else
                            goto is_not_a_corner;
                    else
                        goto is_not_a_corner;
                else
                    goto is_not_a_corner;
            else if (p[pixel[8]] < c_b)
                if (p[pixel[7]] < c_b)
                    if (p[pixel[9]] < c_b)
                        if (p[pixel[10]] < c_b)
                            if (p[pixel[6]] < c_b)
                                if (p[pixel[5]] < c_b)
                                    if (p[pixel[4]] < c_b)
                                        if (p[pixel[3]] < c_b)
                                            if (p[pixel[2]] < c_b)
                                                goto is_a_corner;
                                            else if (p[pixel[11]] < c_b)
                                                goto is_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else if (p[pixel[11]] < c_b)
                                            if (p[pixel[12]] < c_b)
                                                goto is_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else if (p[pixel[11]] < c_b)
                                        if (p[pixel[12]] < c_b)
                                            if (p[pixel[13]] < c_b)
                                                goto is_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else if (p[pixel[11]] < c_b)
                                    if (p[pixel[12]] < c_b)
                                        if (p[pixel[13]] < c_b)
                                            if (p[pixel[14]] < c_b)
                                                goto is_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else
                                    goto is_not_a_corner;
                            else if (p[pixel[11]] < c_b)
                                if (p[pixel[12]] < c_b)
                                    if (p[pixel[13]] < c_b)
                                        if (p[pixel[14]] < c_b)
                                            if (p[pixel[15]] < c_b)
                                                goto is_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else
                                    goto is_not_a_corner;
                            else
                                goto is_not_a_corner;
                        else
                            goto is_not_a_corner;
                    else
                        goto is_not_a_corner;
                else
                    goto is_not_a_corner;
            else
                goto is_not_a_corner;
        else if (p[pixel[0]] < c_b)
            if (p[pixel[1]] > cb)
                if (p[pixel[8]] > cb)
                    if (p[pixel[7]] > cb)
                        if (p[pixel[9]] > cb)
                            if (p[pixel[6]] > cb)
                                if (p[pixel[5]] > cb)
                                    if (p[pixel[4]] > cb)
                                        if (p[pixel[3]] > cb)
                                            if (p[pixel[2]] > cb)
                                                goto is_a_corner;
                                            else if (p[pixel[10]] > cb)
                                                if (p[pixel[11]] > cb)
                                                    goto is_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else if (p[pixel[10]] > cb)
                                            if (p[pixel[11]] > cb)
                                                if (p[pixel[12]] > cb)
                                                    goto is_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else if (p[pixel[10]] > cb)
                                        if (p[pixel[11]] > cb)
                                            if (p[pixel[12]] > cb)
                                                if (p[pixel[13]] > cb)
                                                    goto is_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else if (p[pixel[10]] > cb)
                                    if (p[pixel[11]] > cb)
                                        if (p[pixel[12]] > cb)
                                            if (p[pixel[13]] > cb)
                                                if (p[pixel[14]] > cb)
                                                    goto is_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else
                                    goto is_not_a_corner;
                            else if (p[pixel[10]] > cb)
                                if (p[pixel[11]] > cb)
                                    if (p[pixel[12]] > cb)
                                        if (p[pixel[13]] > cb)
                                            if (p[pixel[14]] > cb)
                                                if (p[pixel[15]] > cb)
                                                    goto is_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else
                                    goto is_not_a_corner;
                            else
                                goto is_not_a_corner;
                        else
                            goto is_not_a_corner;
                    else
                        goto is_not_a_corner;
                else if (p[pixel[8]] < c_b)
                    if (p[pixel[9]] < c_b)
                        if (p[pixel[10]] < c_b)
                            if (p[pixel[11]] < c_b)
                                if (p[pixel[12]] < c_b)
                                    if (p[pixel[13]] < c_b)
                                        if (p[pixel[14]] < c_b)
                                            if (p[pixel[15]] < c_b)
                                                goto is_a_corner;
                                            else if (p[pixel[6]] < c_b)
                                                if (p[pixel[7]] < c_b)
                                                    goto is_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else if (p[pixel[5]] < c_b)
                                            if (p[pixel[6]] < c_b)
                                                if (p[pixel[7]] < c_b)
                                                    goto is_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else if (p[pixel[4]] < c_b)
                                        if (p[pixel[5]] < c_b)
                                            if (p[pixel[6]] < c_b)
                                                if (p[pixel[7]] < c_b)
                                                    goto is_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else if (p[pixel[3]] < c_b)
                                    if (p[pixel[4]] < c_b)
                                        if (p[pixel[5]] < c_b)
                                            if (p[pixel[6]] < c_b)
                                                if (p[pixel[7]] < c_b)
                                                    goto is_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else
                                    goto is_not_a_corner;
                            else if (p[pixel[2]] < c_b)
                                if (p[pixel[3]] < c_b)
                                    if (p[pixel[4]] < c_b)
                                        if (p[pixel[5]] < c_b)
                                            if (p[pixel[6]] < c_b)
                                                if (p[pixel[7]] < c_b)
                                                    goto is_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else
                                    goto is_not_a_corner;
                            else
                                goto is_not_a_corner;
                        else
                            goto is_not_a_corner;
                    else
                        goto is_not_a_corner;
                else
                    goto is_not_a_corner;
            else if (p[pixel[1]] < c_b)
                if (p[pixel[2]] > cb)
                    if (p[pixel[9]] > cb)
                        if (p[pixel[7]] > cb)
                            if (p[pixel[8]] > cb)
                                if (p[pixel[10]] > cb)
                                    if (p[pixel[6]] > cb)
                                        if (p[pixel[5]] > cb)
                                            if (p[pixel[4]] > cb)
                                                if (p[pixel[3]] > cb)
                                                    goto is_a_corner;
                                                else if (p[pixel[11]] > cb)
                                                    if (p[pixel[12]] > cb)
                                                        goto is_a_corner;
                                                    else
                                                        goto is_not_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else if (p[pixel[11]] > cb)
                                                if (p[pixel[12]] > cb)
                                                    if (p[pixel[13]] > cb)
                                                        goto is_a_corner;
                                                    else
                                                        goto is_not_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else if (p[pixel[11]] > cb)
                                            if (p[pixel[12]] > cb)
                                                if (p[pixel[13]] > cb)
                                                    if (p[pixel[14]] > cb)
                                                        goto is_a_corner;
                                                    else
                                                        goto is_not_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else if (p[pixel[11]] > cb)
                                        if (p[pixel[12]] > cb)
                                            if (p[pixel[13]] > cb)
                                                if (p[pixel[14]] > cb)
                                                    if (p[pixel[15]] > cb)
                                                        goto is_a_corner;
                                                    else
                                                        goto is_not_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else
                                    goto is_not_a_corner;
                            else
                                goto is_not_a_corner;
                        else
                            goto is_not_a_corner;
                    else if (p[pixel[9]] < c_b)
                        if (p[pixel[10]] < c_b)
                            if (p[pixel[11]] < c_b)
                                if (p[pixel[12]] < c_b)
                                    if (p[pixel[13]] < c_b)
                                        if (p[pixel[14]] < c_b)
                                            if (p[pixel[15]] < c_b)
                                                goto is_a_corner;
                                            else if (p[pixel[6]] < c_b)
                                                if (p[pixel[7]] < c_b)
                                                    if (p[pixel[8]] < c_b)
                                                        goto is_a_corner;
                                                    else
                                                        goto is_not_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else if (p[pixel[5]] < c_b)
                                            if (p[pixel[6]] < c_b)
                                                if (p[pixel[7]] < c_b)
                                                    if (p[pixel[8]] < c_b)
                                                        goto is_a_corner;
                                                    else
                                                        goto is_not_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else if (p[pixel[4]] < c_b)
                                        if (p[pixel[5]] < c_b)
                                            if (p[pixel[6]] < c_b)
                                                if (p[pixel[7]] < c_b)
                                                    if (p[pixel[8]] < c_b)
                                                        goto is_a_corner;
                                                    else
                                                        goto is_not_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else if (p[pixel[3]] < c_b)
                                    if (p[pixel[4]] < c_b)
                                        if (p[pixel[5]] < c_b)
                                            if (p[pixel[6]] < c_b)
                                                if (p[pixel[7]] < c_b)
                                                    if (p[pixel[8]] < c_b)
                                                        goto is_a_corner;
                                                    else
                                                        goto is_not_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else
                                    goto is_not_a_corner;
                            else
                                goto is_not_a_corner;
                        else
                            goto is_not_a_corner;
                    else
                        goto is_not_a_corner;
                else if (p[pixel[2]] < c_b)
                    if (p[pixel[3]] > cb)
                        if (p[pixel[10]] > cb)
                            if (p[pixel[7]] > cb)
                                if (p[pixel[8]] > cb)
                                    if (p[pixel[9]] > cb)
                                        if (p[pixel[11]] > cb)
                                            if (p[pixel[6]] > cb)
                                                if (p[pixel[5]] > cb)
                                                    if (p[pixel[4]] > cb)
                                                        goto is_a_corner;
                                                    else if (p[pixel[12]] > cb)
                                                        if (p[pixel[13]] > cb)
                                                            goto is_a_corner;
                                                        else
                                                            goto is_not_a_corner;
                                                    else
                                                        goto is_not_a_corner;
                                                else if (p[pixel[12]] > cb)
                                                    if (p[pixel[13]] > cb)
                                                        if (p[pixel[14]] > cb)
                                                            goto is_a_corner;
                                                        else
                                                            goto is_not_a_corner;
                                                    else
                                                        goto is_not_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else if (p[pixel[12]] > cb)
                                                if (p[pixel[13]] > cb)
                                                    if (p[pixel[14]] > cb)
                                                        if (p[pixel[15]] > cb)
                                                            goto is_a_corner;
                                                        else
                                                            goto is_not_a_corner;
                                                    else
                                                        goto is_not_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else
                                    goto is_not_a_corner;
                            else
                                goto is_not_a_corner;
                        else if (p[pixel[10]] < c_b)
                            if (p[pixel[11]] < c_b)
                                if (p[pixel[12]] < c_b)
                                    if (p[pixel[13]] < c_b)
                                        if (p[pixel[14]] < c_b)
                                            if (p[pixel[15]] < c_b)
                                                goto is_a_corner;
                                            else if (p[pixel[6]] < c_b)
                                                if (p[pixel[7]] < c_b)
                                                    if (p[pixel[8]] < c_b)
                                                        if (p[pixel[9]] < c_b)
                                                            goto is_a_corner;
                                                        else
                                                            goto is_not_a_corner;
                                                    else
                                                        goto is_not_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else if (p[pixel[5]] < c_b)
                                            if (p[pixel[6]] < c_b)
                                                if (p[pixel[7]] < c_b)
                                                    if (p[pixel[8]] < c_b)
                                                        if (p[pixel[9]] < c_b)
                                                            goto is_a_corner;
                                                        else
                                                            goto is_not_a_corner;
                                                    else
                                                        goto is_not_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else if (p[pixel[4]] < c_b)
                                        if (p[pixel[5]] < c_b)
                                            if (p[pixel[6]] < c_b)
                                                if (p[pixel[7]] < c_b)
                                                    if (p[pixel[8]] < c_b)
                                                        if (p[pixel[9]] < c_b)
                                                            goto is_a_corner;
                                                        else
                                                            goto is_not_a_corner;
                                                    else
                                                        goto is_not_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else
                                    goto is_not_a_corner;
                            else
                                goto is_not_a_corner;
                        else
                            goto is_not_a_corner;
                    else if (p[pixel[3]] < c_b)
                        if (p[pixel[4]] > cb)
                            if (p[pixel[13]] > cb)
                                if (p[pixel[7]] > cb)
                                    if (p[pixel[8]] > cb)
                                        if (p[pixel[9]] > cb)
                                            if (p[pixel[10]] > cb)
                                                if (p[pixel[11]] > cb)
                                                    if (p[pixel[12]] > cb)
                                                        if (p[pixel[6]] > cb)
                                                            if (p[pixel[5]] > cb)
                                                                goto is_a_corner;
                                                            else if (p[pixel[14]] > cb)
                                                                goto is_a_corner;
                                                            else
                                                                goto is_not_a_corner;
                                                        else if (p[pixel[14]] > cb)
                                                            if (p[pixel[15]] > cb)
                                                                goto is_a_corner;
                                                            else
                                                                goto is_not_a_corner;
                                                        else
                                                            goto is_not_a_corner;
                                                    else
                                                        goto is_not_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else
                                    goto is_not_a_corner;
                            else if (p[pixel[13]] < c_b)
                                if (p[pixel[11]] > cb)
                                    if (p[pixel[5]] > cb)
                                        if (p[pixel[6]] > cb)
                                            if (p[pixel[7]] > cb)
                                                if (p[pixel[8]] > cb)
                                                    if (p[pixel[9]] > cb)
                                                        if (p[pixel[10]] > cb)
                                                            if (p[pixel[12]] > cb)
                                                                goto is_a_corner;
                                                            else
                                                                goto is_not_a_corner;
                                                        else
                                                            goto is_not_a_corner;
                                                    else
                                                        goto is_not_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else if (p[pixel[11]] < c_b)
                                    if (p[pixel[12]] < c_b)
                                        if (p[pixel[14]] < c_b)
                                            if (p[pixel[15]] < c_b)
                                                goto is_a_corner;
                                            else if (p[pixel[6]] < c_b)
                                                if (p[pixel[7]] < c_b)
                                                    if (p[pixel[8]] < c_b)
                                                        if (p[pixel[9]] < c_b)
                                                            if (p[pixel[10]] < c_b)
                                                                goto is_a_corner;
                                                            else
                                                                goto is_not_a_corner;
                                                        else
                                                            goto is_not_a_corner;
                                                    else
                                                        goto is_not_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else if (p[pixel[5]] < c_b)
                                            if (p[pixel[6]] < c_b)
                                                if (p[pixel[7]] < c_b)
                                                    if (p[pixel[8]] < c_b)
                                                        if (p[pixel[9]] < c_b)
                                                            if (p[pixel[10]] < c_b)
                                                                goto is_a_corner;
                                                            else
                                                                goto is_not_a_corner;
                                                        else
                                                            goto is_not_a_corner;
                                                    else
                                                        goto is_not_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else
                                    goto is_not_a_corner;
                            else if (p[pixel[5]] > cb)
                                if (p[pixel[6]] > cb)
                                    if (p[pixel[7]] > cb)
                                        if (p[pixel[8]] > cb)
                                            if (p[pixel[9]] > cb)
                                                if (p[pixel[10]] > cb)
                                                    if (p[pixel[11]] > cb)
                                                        if (p[pixel[12]] > cb)
                                                            goto is_a_corner;
                                                        else
                                                            goto is_not_a_corner;
                                                    else
                                                        goto is_not_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else
                                    goto is_not_a_corner;
                            else
                                goto is_not_a_corner;
                        else if (p[pixel[4]] < c_b)
                            if (p[pixel[5]] > cb)
                                if (p[pixel[14]] > cb)
                                    if (p[pixel[7]] > cb)
                                        if (p[pixel[8]] > cb)
                                            if (p[pixel[9]] > cb)
                                                if (p[pixel[10]] > cb)
                                                    if (p[pixel[11]] > cb)
                                                        if (p[pixel[12]] > cb)
                                                            if (p[pixel[13]] > cb)
                                                                if (p[pixel[6]] > cb)
                                                                    goto is_a_corner;
                                                                else if (p[pixel[15]] > cb)
                                                                    goto is_a_corner;
                                                                else
                                                                    goto is_not_a_corner;
                                                            else
                                                                goto is_not_a_corner;
                                                        else
                                                            goto is_not_a_corner;
                                                    else
                                                        goto is_not_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else if (p[pixel[14]] < c_b)
                                    if (p[pixel[12]] > cb)
                                        if (p[pixel[6]] > cb)
                                            if (p[pixel[7]] > cb)
                                                if (p[pixel[8]] > cb)
                                                    if (p[pixel[9]] > cb)
                                                        if (p[pixel[10]] > cb)
                                                            if (p[pixel[11]] > cb)
                                                                if (p[pixel[13]] > cb)
                                                                    goto is_a_corner;
                                                                else
                                                                    goto is_not_a_corner;
                                                            else
                                                                goto is_not_a_corner;
                                                        else
                                                            goto is_not_a_corner;
                                                    else
                                                        goto is_not_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else if (p[pixel[12]] < c_b)
                                        if (p[pixel[13]] < c_b)
                                            if (p[pixel[15]] < c_b)
                                                goto is_a_corner;
                                            else if (p[pixel[6]] < c_b)
                                                if (p[pixel[7]] < c_b)
                                                    if (p[pixel[8]] < c_b)
                                                        if (p[pixel[9]] < c_b)
                                                            if (p[pixel[10]] < c_b)
                                                                if (p[pixel[11]] < c_b)
                                                                    goto is_a_corner;
                                                                else
                                                                    goto is_not_a_corner;
                                                            else
                                                                goto is_not_a_corner;
                                                        else
                                                            goto is_not_a_corner;
                                                    else
                                                        goto is_not_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else if (p[pixel[6]] > cb)
                                    if (p[pixel[7]] > cb)
                                        if (p[pixel[8]] > cb)
                                            if (p[pixel[9]] > cb)
                                                if (p[pixel[10]] > cb)
                                                    if (p[pixel[11]] > cb)
                                                        if (p[pixel[12]] > cb)
                                                            if (p[pixel[13]] > cb)
                                                                goto is_a_corner;
                                                            else
                                                                goto is_not_a_corner;
                                                        else
                                                            goto is_not_a_corner;
                                                    else
                                                        goto is_not_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else
                                    goto is_not_a_corner;
                            else if (p[pixel[5]] < c_b)
                                if (p[pixel[6]] > cb)
                                    if (p[pixel[15]] < c_b)
                                        if (p[pixel[13]] > cb)
                                            if (p[pixel[7]] > cb)
                                                if (p[pixel[8]] > cb)
                                                    if (p[pixel[9]] > cb)
                                                        if (p[pixel[10]] > cb)
                                                            if (p[pixel[11]] > cb)
                                                                if (p[pixel[12]] > cb)
                                                                    if (p[pixel[14]] > cb)
                                                                        goto is_a_corner;
                                                                    else
                                                                        goto is_not_a_corner;
                                                                else
                                                                    goto is_not_a_corner;
                                                            else
                                                                goto is_not_a_corner;
                                                        else
                                                            goto is_not_a_corner;
                                                    else
                                                        goto is_not_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else if (p[pixel[13]] < c_b)
                                            if (p[pixel[14]] < c_b)
                                                goto is_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else if (p[pixel[7]] > cb)
                                        if (p[pixel[8]] > cb)
                                            if (p[pixel[9]] > cb)
                                                if (p[pixel[10]] > cb)
                                                    if (p[pixel[11]] > cb)
                                                        if (p[pixel[12]] > cb)
                                                            if (p[pixel[13]] > cb)
                                                                if (p[pixel[14]] > cb)
                                                                    goto is_a_corner;
                                                                else
                                                                    goto is_not_a_corner;
                                                            else
                                                                goto is_not_a_corner;
                                                        else
                                                            goto is_not_a_corner;
                                                    else
                                                        goto is_not_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else if (p[pixel[6]] < c_b)
                                    if (p[pixel[7]] > cb)
                                        if (p[pixel[14]] > cb)
                                            if (p[pixel[8]] > cb)
                                                if (p[pixel[9]] > cb)
                                                    if (p[pixel[10]] > cb)
                                                        if (p[pixel[11]] > cb)
                                                            if (p[pixel[12]] > cb)
                                                                if (p[pixel[13]] > cb)
                                                                    if (p[pixel[15]] > cb)
                                                                        goto is_a_corner;
                                                                    else
                                                                        goto is_not_a_corner;
                                                                else
                                                                    goto is_not_a_corner;
                                                            else
                                                                goto is_not_a_corner;
                                                        else
                                                            goto is_not_a_corner;
                                                    else
                                                        goto is_not_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else if (p[pixel[14]] < c_b)
                                            if (p[pixel[15]] < c_b)
                                                goto is_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else if (p[pixel[7]] < c_b)
                                        if (p[pixel[8]] < c_b)
                                            goto is_a_corner;
                                        else if (p[pixel[15]] < c_b)
                                            goto is_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else if (p[pixel[14]] < c_b)
                                        if (p[pixel[15]] < c_b)
                                            goto is_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else if (p[pixel[13]] > cb)
                                    if (p[pixel[7]] > cb)
                                        if (p[pixel[8]] > cb)
                                            if (p[pixel[9]] > cb)
                                                if (p[pixel[10]] > cb)
                                                    if (p[pixel[11]] > cb)
                                                        if (p[pixel[12]] > cb)
                                                            if (p[pixel[14]] > cb)
                                                                if (p[pixel[15]] > cb)
                                                                    goto is_a_corner;
                                                                else
                                                                    goto is_not_a_corner;
                                                            else
                                                                goto is_not_a_corner;
                                                        else
                                                            goto is_not_a_corner;
                                                    else
                                                        goto is_not_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else if (p[pixel[13]] < c_b)
                                    if (p[pixel[14]] < c_b)
                                        if (p[pixel[15]] < c_b)
                                            goto is_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else
                                    goto is_not_a_corner;
                            else if (p[pixel[12]] > cb)
                                if (p[pixel[7]] > cb)
                                    if (p[pixel[8]] > cb)
                                        if (p[pixel[9]] > cb)
                                            if (p[pixel[10]] > cb)
                                                if (p[pixel[11]] > cb)
                                                    if (p[pixel[13]] > cb)
                                                        if (p[pixel[14]] > cb)
                                                            if (p[pixel[6]] > cb)
                                                                goto is_a_corner;
                                                            else if (p[pixel[15]] > cb)
                                                                goto is_a_corner;
                                                            else
                                                                goto is_not_a_corner;
                                                        else
                                                            goto is_not_a_corner;
                                                    else
                                                        goto is_not_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else
                                    goto is_not_a_corner;
                            else if (p[pixel[12]] < c_b)
                                if (p[pixel[13]] < c_b)
                                    if (p[pixel[14]] < c_b)
                                        if (p[pixel[15]] < c_b)
                                            goto is_a_corner;
                                        else if (p[pixel[6]] < c_b)
                                            if (p[pixel[7]] < c_b)
                                                if (p[pixel[8]] < c_b)
                                                    if (p[pixel[9]] < c_b)
                                                        if (p[pixel[10]] < c_b)
                                                            if (p[pixel[11]] < c_b)
                                                                goto is_a_corner;
                                                            else
                                                                goto is_not_a_corner;
                                                        else
                                                            goto is_not_a_corner;
                                                    else
                                                        goto is_not_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else
                                    goto is_not_a_corner;
                            else
                                goto is_not_a_corner;
                        else if (p[pixel[11]] > cb)
                            if (p[pixel[7]] > cb)
                                if (p[pixel[8]] > cb)
                                    if (p[pixel[9]] > cb)
                                        if (p[pixel[10]] > cb)
                                            if (p[pixel[12]] > cb)
                                                if (p[pixel[13]] > cb)
                                                    if (p[pixel[6]] > cb)
                                                        if (p[pixel[5]] > cb)
                                                            goto is_a_corner;
                                                        else if (p[pixel[14]] > cb)
                                                            goto is_a_corner;
                                                        else
                                                            goto is_not_a_corner;
                                                    else if (p[pixel[14]] > cb)
                                                        if (p[pixel[15]] > cb)
                                                            goto is_a_corner;
                                                        else
                                                            goto is_not_a_corner;
                                                    else
                                                        goto is_not_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else
                                    goto is_not_a_corner;
                            else
                                goto is_not_a_corner;
                        else if (p[pixel[11]] < c_b)
                            if (p[pixel[12]] < c_b)
                                if (p[pixel[13]] < c_b)
                                    if (p[pixel[14]] < c_b)
                                        if (p[pixel[15]] < c_b)
                                            goto is_a_corner;
                                        else if (p[pixel[6]] < c_b)
                                            if (p[pixel[7]] < c_b)
                                                if (p[pixel[8]] < c_b)
                                                    if (p[pixel[9]] < c_b)
                                                        if (p[pixel[10]] < c_b)
                                                            goto is_a_corner;
                                                        else
                                                            goto is_not_a_corner;
                                                    else
                                                        goto is_not_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else if (p[pixel[5]] < c_b)
                                        if (p[pixel[6]] < c_b)
                                            if (p[pixel[7]] < c_b)
                                                if (p[pixel[8]] < c_b)
                                                    if (p[pixel[9]] < c_b)
                                                        if (p[pixel[10]] < c_b)
                                                            goto is_a_corner;
                                                        else
                                                            goto is_not_a_corner;
                                                    else
                                                        goto is_not_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else
                                    goto is_not_a_corner;
                            else
                                goto is_not_a_corner;
                        else
                            goto is_not_a_corner;
                    else if (p[pixel[10]] > cb)
                        if (p[pixel[7]] > cb)
                            if (p[pixel[8]] > cb)
                                if (p[pixel[9]] > cb)
                                    if (p[pixel[11]] > cb)
                                        if (p[pixel[12]] > cb)
                                            if (p[pixel[6]] > cb)
                                                if (p[pixel[5]] > cb)
                                                    if (p[pixel[4]] > cb)
                                                        goto is_a_corner;
                                                    else if (p[pixel[13]] > cb)
                                                        goto is_a_corner;
                                                    else
                                                        goto is_not_a_corner;
                                                else if (p[pixel[13]] > cb)
                                                    if (p[pixel[14]] > cb)
                                                        goto is_a_corner;
                                                    else
                                                        goto is_not_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else if (p[pixel[13]] > cb)
                                                if (p[pixel[14]] > cb)
                                                    if (p[pixel[15]] > cb)
                                                        goto is_a_corner;
                                                    else
                                                        goto is_not_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else
                                    goto is_not_a_corner;
                            else
                                goto is_not_a_corner;
                        else
                            goto is_not_a_corner;
                    else if (p[pixel[10]] < c_b)
                        if (p[pixel[11]] < c_b)
                            if (p[pixel[12]] < c_b)
                                if (p[pixel[13]] < c_b)
                                    if (p[pixel[14]] < c_b)
                                        if (p[pixel[15]] < c_b)
                                            goto is_a_corner;
                                        else if (p[pixel[6]] < c_b)
                                            if (p[pixel[7]] < c_b)
                                                if (p[pixel[8]] < c_b)
                                                    if (p[pixel[9]] < c_b)
                                                        goto is_a_corner;
                                                    else
                                                        goto is_not_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else if (p[pixel[5]] < c_b)
                                        if (p[pixel[6]] < c_b)
                                            if (p[pixel[7]] < c_b)
                                                if (p[pixel[8]] < c_b)
                                                    if (p[pixel[9]] < c_b)
                                                        goto is_a_corner;
                                                    else
                                                        goto is_not_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else if (p[pixel[4]] < c_b)
                                    if (p[pixel[5]] < c_b)
                                        if (p[pixel[6]] < c_b)
                                            if (p[pixel[7]] < c_b)
                                                if (p[pixel[8]] < c_b)
                                                    if (p[pixel[9]] < c_b)
                                                        goto is_a_corner;
                                                    else
                                                        goto is_not_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else
                                    goto is_not_a_corner;
                            else
                                goto is_not_a_corner;
                        else
                            goto is_not_a_corner;
                    else
                        goto is_not_a_corner;
                else if (p[pixel[9]] > cb)
                    if (p[pixel[7]] > cb)
                        if (p[pixel[8]] > cb)
                            if (p[pixel[10]] > cb)
                                if (p[pixel[11]] > cb)
                                    if (p[pixel[6]] > cb)
                                        if (p[pixel[5]] > cb)
                                            if (p[pixel[4]] > cb)
                                                if (p[pixel[3]] > cb)
                                                    goto is_a_corner;
                                                else if (p[pixel[12]] > cb)
                                                    goto is_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else if (p[pixel[12]] > cb)
                                                if (p[pixel[13]] > cb)
                                                    goto is_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else if (p[pixel[12]] > cb)
                                            if (p[pixel[13]] > cb)
                                                if (p[pixel[14]] > cb)
                                                    goto is_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else if (p[pixel[12]] > cb)
                                        if (p[pixel[13]] > cb)
                                            if (p[pixel[14]] > cb)
                                                if (p[pixel[15]] > cb)
                                                    goto is_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else
                                    goto is_not_a_corner;
                            else
                                goto is_not_a_corner;
                        else
                            goto is_not_a_corner;
                    else
                        goto is_not_a_corner;
                else if (p[pixel[9]] < c_b)
                    if (p[pixel[10]] < c_b)
                        if (p[pixel[11]] < c_b)
                            if (p[pixel[12]] < c_b)
                                if (p[pixel[13]] < c_b)
                                    if (p[pixel[14]] < c_b)
                                        if (p[pixel[15]] < c_b)
                                            goto is_a_corner;
                                        else if (p[pixel[6]] < c_b)
                                            if (p[pixel[7]] < c_b)
                                                if (p[pixel[8]] < c_b)
                                                    goto is_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else if (p[pixel[5]] < c_b)
                                        if (p[pixel[6]] < c_b)
                                            if (p[pixel[7]] < c_b)
                                                if (p[pixel[8]] < c_b)
                                                    goto is_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else if (p[pixel[4]] < c_b)
                                    if (p[pixel[5]] < c_b)
                                        if (p[pixel[6]] < c_b)
                                            if (p[pixel[7]] < c_b)
                                                if (p[pixel[8]] < c_b)
                                                    goto is_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else
                                    goto is_not_a_corner;
                            else if (p[pixel[3]] < c_b)
                                if (p[pixel[4]] < c_b)
                                    if (p[pixel[5]] < c_b)
                                        if (p[pixel[6]] < c_b)
                                            if (p[pixel[7]] < c_b)
                                                if (p[pixel[8]] < c_b)
                                                    goto is_a_corner;
                                                else
                                                    goto is_not_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else
                                    goto is_not_a_corner;
                            else
                                goto is_not_a_corner;
                        else
                            goto is_not_a_corner;
                    else
                        goto is_not_a_corner;
                else
                    goto is_not_a_corner;
            else if (p[pixel[8]] > cb)
                if (p[pixel[7]] > cb)
                    if (p[pixel[9]] > cb)
                        if (p[pixel[10]] > cb)
                            if (p[pixel[6]] > cb)
                                if (p[pixel[5]] > cb)
                                    if (p[pixel[4]] > cb)
                                        if (p[pixel[3]] > cb)
                                            if (p[pixel[2]] > cb)
                                                goto is_a_corner;
                                            else if (p[pixel[11]] > cb)
                                                goto is_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else if (p[pixel[11]] > cb)
                                            if (p[pixel[12]] > cb)
                                                goto is_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else if (p[pixel[11]] > cb)
                                        if (p[pixel[12]] > cb)
                                            if (p[pixel[13]] > cb)
                                                goto is_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else if (p[pixel[11]] > cb)
                                    if (p[pixel[12]] > cb)
                                        if (p[pixel[13]] > cb)
                                            if (p[pixel[14]] > cb)
                                                goto is_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else
                                    goto is_not_a_corner;
                            else if (p[pixel[11]] > cb)
                                if (p[pixel[12]] > cb)
                                    if (p[pixel[13]] > cb)
                                        if (p[pixel[14]] > cb)
                                            if (p[pixel[15]] > cb)
                                                goto is_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else
                                    goto is_not_a_corner;
                            else
                                goto is_not_a_corner;
                        else
                            goto is_not_a_corner;
                    else
                        goto is_not_a_corner;
                else
                    goto is_not_a_corner;
            else if (p[pixel[8]] < c_b)
                if (p[pixel[9]] < c_b)
                    if (p[pixel[10]] < c_b)
                        if (p[pixel[11]] < c_b)
                            if (p[pixel[12]] < c_b)
                                if (p[pixel[13]] < c_b)
                                    if (p[pixel[14]] < c_b)
                                        if (p[pixel[15]] < c_b)
                                            goto is_a_corner;
                                        else if (p[pixel[6]] < c_b)
                                            if (p[pixel[7]] < c_b)
                                                goto is_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else if (p[pixel[5]] < c_b)
                                        if (p[pixel[6]] < c_b)
                                            if (p[pixel[7]] < c_b)
                                                goto is_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else if (p[pixel[4]] < c_b)
                                    if (p[pixel[5]] < c_b)
                                        if (p[pixel[6]] < c_b)
                                            if (p[pixel[7]] < c_b)
                                                goto is_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else
                                    goto is_not_a_corner;
                            else if (p[pixel[3]] < c_b)
                                if (p[pixel[4]] < c_b)
                                    if (p[pixel[5]] < c_b)
                                        if (p[pixel[6]] < c_b)
                                            if (p[pixel[7]] < c_b)
                                                goto is_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else
                                    goto is_not_a_corner;
                            else
                                goto is_not_a_corner;
                        else if (p[pixel[2]] < c_b)
                            if (p[pixel[3]] < c_b)
                                if (p[pixel[4]] < c_b)
                                    if (p[pixel[5]] < c_b)
                                        if (p[pixel[6]] < c_b)
                                            if (p[pixel[7]] < c_b)
                                                goto is_a_corner;
                                            else
                                                goto is_not_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else
                                    goto is_not_a_corner;
                            else
                                goto is_not_a_corner;
                        else
                            goto is_not_a_corner;
                    else
                        goto is_not_a_corner;
                else
                    goto is_not_a_corner;
            else
                goto is_not_a_corner;
        else if (p[pixel[7]] > cb)
            if (p[pixel[8]] > cb)
                if (p[pixel[9]] > cb)
                    if (p[pixel[6]] > cb)
                        if (p[pixel[5]] > cb)
                            if (p[pixel[4]] > cb)
                                if (p[pixel[3]] > cb)
                                    if (p[pixel[2]] > cb)
                                        if (p[pixel[1]] > cb)
                                            goto is_a_corner;
                                        else if (p[pixel[10]] > cb)
                                            goto is_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else if (p[pixel[10]] > cb)
                                        if (p[pixel[11]] > cb)
                                            goto is_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else if (p[pixel[10]] > cb)
                                    if (p[pixel[11]] > cb)
                                        if (p[pixel[12]] > cb)
                                            goto is_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else
                                    goto is_not_a_corner;
                            else if (p[pixel[10]] > cb)
                                if (p[pixel[11]] > cb)
                                    if (p[pixel[12]] > cb)
                                        if (p[pixel[13]] > cb)
                                            goto is_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else
                                    goto is_not_a_corner;
                            else
                                goto is_not_a_corner;
                        else if (p[pixel[10]] > cb)
                            if (p[pixel[11]] > cb)
                                if (p[pixel[12]] > cb)
                                    if (p[pixel[13]] > cb)
                                        if (p[pixel[14]] > cb)
                                            goto is_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else
                                    goto is_not_a_corner;
                            else
                                goto is_not_a_corner;
                        else
                            goto is_not_a_corner;
                    else if (p[pixel[10]] > cb)
                        if (p[pixel[11]] > cb)
                            if (p[pixel[12]] > cb)
                                if (p[pixel[13]] > cb)
                                    if (p[pixel[14]] > cb)
                                        if (p[pixel[15]] > cb)
                                            goto is_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else
                                    goto is_not_a_corner;
                            else
                                goto is_not_a_corner;
                        else
                            goto is_not_a_corner;
                    else
                        goto is_not_a_corner;
                else
                    goto is_not_a_corner;
            else
                goto is_not_a_corner;
        else if (p[pixel[7]] < c_b)
            if (p[pixel[8]] < c_b)
                if (p[pixel[9]] < c_b)
                    if (p[pixel[6]] < c_b)
                        if (p[pixel[5]] < c_b)
                            if (p[pixel[4]] < c_b)
                                if (p[pixel[3]] < c_b)
                                    if (p[pixel[2]] < c_b)
                                        if (p[pixel[1]] < c_b)
                                            goto is_a_corner;
                                        else if (p[pixel[10]] < c_b)
                                            goto is_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else if (p[pixel[10]] < c_b)
                                        if (p[pixel[11]] < c_b)
                                            goto is_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else if (p[pixel[10]] < c_b)
                                    if (p[pixel[11]] < c_b)
                                        if (p[pixel[12]] < c_b)
                                            goto is_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else
                                    goto is_not_a_corner;
                            else if (p[pixel[10]] < c_b)
                                if (p[pixel[11]] < c_b)
                                    if (p[pixel[12]] < c_b)
                                        if (p[pixel[13]] < c_b)
                                            goto is_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else
                                    goto is_not_a_corner;
                            else
                                goto is_not_a_corner;
                        else if (p[pixel[10]] < c_b)
                            if (p[pixel[11]] < c_b)
                                if (p[pixel[12]] < c_b)
                                    if (p[pixel[13]] < c_b)
                                        if (p[pixel[14]] < c_b)
                                            goto is_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else
                                    goto is_not_a_corner;
                            else
                                goto is_not_a_corner;
                        else
                            goto is_not_a_corner;
                    else if (p[pixel[10]] < c_b)
                        if (p[pixel[11]] < c_b)
                            if (p[pixel[12]] < c_b)
                                if (p[pixel[13]] < c_b)
                                    if (p[pixel[14]] < c_b)
                                        if (p[pixel[15]] < c_b)
                                            goto is_a_corner;
                                        else
                                            goto is_not_a_corner;
                                    else
                                        goto is_not_a_corner;
                                else
                                    goto is_not_a_corner;
                            else
                                goto is_not_a_corner;
                        else
                            goto is_not_a_corner;
                    else
                        goto is_not_a_corner;
                else
                    goto is_not_a_corner;
            else
                goto is_not_a_corner;
        else
            goto is_not_a_corner;

    is_a_corner:
        bmin = b;
        goto end_if;

    is_not_a_corner:
        bmax = b;
        goto end_if;

    end_if:
        if (bmin == bmax - 1 || bmin == bmax)
            return bmin;
        b = (bmin + bmax) / 2;
    }
}


static void fast9ComputeScores(
    const camera_fb_t *fb1, // gray single channel
    const vector_t *c,
    vector_t *scores,
    uint8_t bstart
) {
    uint16_t w = fb1->width;
    int pixel[16];
    makeOffsets(pixel, w);
    uint16_t i, num_corners = c->size;

    if (scores->capacity < num_corners)
        vector_reserve(scores, num_corners);

    for (i = 0; i < num_corners; ++i)
    {
        pixel_coord_t *p = vector_get(c, i);
        uint8_t score = fast9CornerScore(fb1->buf + w * p->y + p->x, pixel, bstart);
        vector_set(scores, i, &score);
        vector_push_back(scores, &score);
    }
    // vector_print(scores);
}


static void fast9Detect(
    const camera_fb_t *fb1, // gray single channel
    vector_t *c,
    int b
) {
    int w = fb1->width, h = fb1->height;
    uint8_t margin = 4;
    int pixel[16];
    makeOffsets(pixel, w);
    vector_reserve(c, 512);

    for (uint16_t y = margin; y < h - margin; ++y)
        for (uint16_t x = margin; x < w - margin; ++x)
        {
            const uint8_t *p = fb1->buf + y * w + x;

            int cb = *p + b;
            int c_b = *p - b;
            if (p[pixel[0]] > cb)
                if (p[pixel[1]] > cb)
                    if (p[pixel[2]] > cb)
                        if (p[pixel[3]] > cb)
                            if (p[pixel[4]] > cb)
                                if (p[pixel[5]] > cb)
                                    if (p[pixel[6]] > cb)
                                        if (p[pixel[7]] > cb)
                                            if (p[pixel[8]] > cb)
                                            {
                                            }
                                            else if (p[pixel[15]] > cb)
                                            {
                                            }
                                            else
                                                continue;
                                        else if (p[pixel[7]] < c_b)
                                            if (p[pixel[14]] > cb)
                                                if (p[pixel[15]] > cb)
                                                {
                                                }
                                                else
                                                    continue;
                                            else if (p[pixel[14]] < c_b)
                                                if (p[pixel[8]] < c_b)
                                                    if (p[pixel[9]] < c_b)
                                                        if (p[pixel[10]] < c_b)
                                                            if (p[pixel[11]] < c_b)
                                                                if (p[pixel[12]] < c_b)
                                                                    if (p[pixel[13]] < c_b)
                                                                        if (p[pixel[15]] < c_b)
                                                                        {
                                                                        }
                                                                        else
                                                                            continue;
                                                                    else
                                                                        continue;
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else if (p[pixel[14]] > cb)
                                            if (p[pixel[15]] > cb)
                                            {
                                            }
                                            else
                                                continue;
                                        else
                                            continue;
                                    else if (p[pixel[6]] < c_b)
                                        if (p[pixel[15]] > cb)
                                            if (p[pixel[13]] > cb)
                                                if (p[pixel[14]] > cb)
                                                {
                                                }
                                                else
                                                    continue;
                                            else if (p[pixel[13]] < c_b)
                                                if (p[pixel[7]] < c_b)
                                                    if (p[pixel[8]] < c_b)
                                                        if (p[pixel[9]] < c_b)
                                                            if (p[pixel[10]] < c_b)
                                                                if (p[pixel[11]] < c_b)
                                                                    if (p[pixel[12]] < c_b)
                                                                        if (p[pixel[14]] < c_b)
                                                                        {
                                                                        }
                                                                        else
                                                                            continue;
                                                                    else
                                                                        continue;
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else if (p[pixel[7]] < c_b)
                                            if (p[pixel[8]] < c_b)
                                                if (p[pixel[9]] < c_b)
                                                    if (p[pixel[10]] < c_b)
                                                        if (p[pixel[11]] < c_b)
                                                            if (p[pixel[12]] < c_b)
                                                                if (p[pixel[13]] < c_b)
                                                                    if (p[pixel[14]] < c_b)
                                                                    {
                                                                    }
                                                                    else
                                                                        continue;
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else if (p[pixel[13]] > cb)
                                        if (p[pixel[14]] > cb)
                                            if (p[pixel[15]] > cb)
                                            {
                                            }
                                            else
                                                continue;
                                        else
                                            continue;
                                    else if (p[pixel[13]] < c_b)
                                        if (p[pixel[7]] < c_b)
                                            if (p[pixel[8]] < c_b)
                                                if (p[pixel[9]] < c_b)
                                                    if (p[pixel[10]] < c_b)
                                                        if (p[pixel[11]] < c_b)
                                                            if (p[pixel[12]] < c_b)
                                                                if (p[pixel[14]] < c_b)
                                                                    if (p[pixel[15]] < c_b)
                                                                    {
                                                                    }
                                                                    else
                                                                        continue;
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else if (p[pixel[5]] < c_b)
                                    if (p[pixel[14]] > cb)
                                        if (p[pixel[12]] > cb)
                                            if (p[pixel[13]] > cb)
                                                if (p[pixel[15]] > cb)
                                                {
                                                }
                                                else if (p[pixel[6]] > cb)
                                                    if (p[pixel[7]] > cb)
                                                        if (p[pixel[8]] > cb)
                                                            if (p[pixel[9]] > cb)
                                                                if (p[pixel[10]] > cb)
                                                                    if (p[pixel[11]] > cb)
                                                                    {
                                                                    }
                                                                    else
                                                                        continue;
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else if (p[pixel[12]] < c_b)
                                            if (p[pixel[6]] < c_b)
                                                if (p[pixel[7]] < c_b)
                                                    if (p[pixel[8]] < c_b)
                                                        if (p[pixel[9]] < c_b)
                                                            if (p[pixel[10]] < c_b)
                                                                if (p[pixel[11]] < c_b)
                                                                    if (p[pixel[13]] < c_b)
                                                                    {
                                                                    }
                                                                    else
                                                                        continue;
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else if (p[pixel[14]] < c_b)
                                        if (p[pixel[7]] < c_b)
                                            if (p[pixel[8]] < c_b)
                                                if (p[pixel[9]] < c_b)
                                                    if (p[pixel[10]] < c_b)
                                                        if (p[pixel[11]] < c_b)
                                                            if (p[pixel[12]] < c_b)
                                                                if (p[pixel[13]] < c_b)
                                                                    if (p[pixel[6]] < c_b)
                                                                    {
                                                                    }
                                                                    else if (p[pixel[15]] < c_b)
                                                                    {
                                                                    }
                                                                    else
                                                                        continue;
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else if (p[pixel[6]] < c_b)
                                        if (p[pixel[7]] < c_b)
                                            if (p[pixel[8]] < c_b)
                                                if (p[pixel[9]] < c_b)
                                                    if (p[pixel[10]] < c_b)
                                                        if (p[pixel[11]] < c_b)
                                                            if (p[pixel[12]] < c_b)
                                                                if (p[pixel[13]] < c_b)
                                                                {
                                                                }
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else if (p[pixel[12]] > cb)
                                    if (p[pixel[13]] > cb)
                                        if (p[pixel[14]] > cb)
                                            if (p[pixel[15]] > cb)
                                            {
                                            }
                                            else if (p[pixel[6]] > cb)
                                                if (p[pixel[7]] > cb)
                                                    if (p[pixel[8]] > cb)
                                                        if (p[pixel[9]] > cb)
                                                            if (p[pixel[10]] > cb)
                                                                if (p[pixel[11]] > cb)
                                                                {
                                                                }
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else if (p[pixel[12]] < c_b)
                                    if (p[pixel[7]] < c_b)
                                        if (p[pixel[8]] < c_b)
                                            if (p[pixel[9]] < c_b)
                                                if (p[pixel[10]] < c_b)
                                                    if (p[pixel[11]] < c_b)
                                                        if (p[pixel[13]] < c_b)
                                                            if (p[pixel[14]] < c_b)
                                                                if (p[pixel[6]] < c_b)
                                                                {
                                                                }
                                                                else if (p[pixel[15]] < c_b)
                                                                {
                                                                }
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else
                                    continue;
                            else if (p[pixel[4]] < c_b)
                                if (p[pixel[13]] > cb)
                                    if (p[pixel[11]] > cb)
                                        if (p[pixel[12]] > cb)
                                            if (p[pixel[14]] > cb)
                                                if (p[pixel[15]] > cb)
                                                {
                                                }
                                                else if (p[pixel[6]] > cb)
                                                    if (p[pixel[7]] > cb)
                                                        if (p[pixel[8]] > cb)
                                                            if (p[pixel[9]] > cb)
                                                                if (p[pixel[10]] > cb)
                                                                {
                                                                }
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else if (p[pixel[5]] > cb)
                                                if (p[pixel[6]] > cb)
                                                    if (p[pixel[7]] > cb)
                                                        if (p[pixel[8]] > cb)
                                                            if (p[pixel[9]] > cb)
                                                                if (p[pixel[10]] > cb)
                                                                {
                                                                }
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else if (p[pixel[11]] < c_b)
                                        if (p[pixel[5]] < c_b)
                                            if (p[pixel[6]] < c_b)
                                                if (p[pixel[7]] < c_b)
                                                    if (p[pixel[8]] < c_b)
                                                        if (p[pixel[9]] < c_b)
                                                            if (p[pixel[10]] < c_b)
                                                                if (p[pixel[12]] < c_b)
                                                                {
                                                                }
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else if (p[pixel[13]] < c_b)
                                    if (p[pixel[7]] < c_b)
                                        if (p[pixel[8]] < c_b)
                                            if (p[pixel[9]] < c_b)
                                                if (p[pixel[10]] < c_b)
                                                    if (p[pixel[11]] < c_b)
                                                        if (p[pixel[12]] < c_b)
                                                            if (p[pixel[6]] < c_b)
                                                                if (p[pixel[5]] < c_b)
                                                                {
                                                                }
                                                                else if (p[pixel[14]] < c_b)
                                                                {
                                                                }
                                                                else
                                                                    continue;
                                                            else if (p[pixel[14]] < c_b)
                                                                if (p[pixel[15]] < c_b)
                                                                {
                                                                }
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else if (p[pixel[5]] < c_b)
                                    if (p[pixel[6]] < c_b)
                                        if (p[pixel[7]] < c_b)
                                            if (p[pixel[8]] < c_b)
                                                if (p[pixel[9]] < c_b)
                                                    if (p[pixel[10]] < c_b)
                                                        if (p[pixel[11]] < c_b)
                                                            if (p[pixel[12]] < c_b)
                                                            {
                                                            }
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else
                                    continue;
                            else if (p[pixel[11]] > cb)
                                if (p[pixel[12]] > cb)
                                    if (p[pixel[13]] > cb)
                                        if (p[pixel[14]] > cb)
                                            if (p[pixel[15]] > cb)
                                            {
                                            }
                                            else if (p[pixel[6]] > cb)
                                                if (p[pixel[7]] > cb)
                                                    if (p[pixel[8]] > cb)
                                                        if (p[pixel[9]] > cb)
                                                            if (p[pixel[10]] > cb)
                                                            {
                                                            }
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else if (p[pixel[5]] > cb)
                                            if (p[pixel[6]] > cb)
                                                if (p[pixel[7]] > cb)
                                                    if (p[pixel[8]] > cb)
                                                        if (p[pixel[9]] > cb)
                                                            if (p[pixel[10]] > cb)
                                                            {
                                                            }
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else
                                    continue;
                            else if (p[pixel[11]] < c_b)
                                if (p[pixel[7]] < c_b)
                                    if (p[pixel[8]] < c_b)
                                        if (p[pixel[9]] < c_b)
                                            if (p[pixel[10]] < c_b)
                                                if (p[pixel[12]] < c_b)
                                                    if (p[pixel[13]] < c_b)
                                                        if (p[pixel[6]] < c_b)
                                                            if (p[pixel[5]] < c_b)
                                                            {
                                                            }
                                                            else if (p[pixel[14]] < c_b)
                                                            {
                                                            }
                                                            else
                                                                continue;
                                                        else if (p[pixel[14]] < c_b)
                                                            if (p[pixel[15]] < c_b)
                                                            {
                                                            }
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else
                                    continue;
                            else
                                continue;
                        else if (p[pixel[3]] < c_b)
                            if (p[pixel[10]] > cb)
                                if (p[pixel[11]] > cb)
                                    if (p[pixel[12]] > cb)
                                        if (p[pixel[13]] > cb)
                                            if (p[pixel[14]] > cb)
                                                if (p[pixel[15]] > cb)
                                                {
                                                }
                                                else if (p[pixel[6]] > cb)
                                                    if (p[pixel[7]] > cb)
                                                        if (p[pixel[8]] > cb)
                                                            if (p[pixel[9]] > cb)
                                                            {
                                                            }
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else if (p[pixel[5]] > cb)
                                                if (p[pixel[6]] > cb)
                                                    if (p[pixel[7]] > cb)
                                                        if (p[pixel[8]] > cb)
                                                            if (p[pixel[9]] > cb)
                                                            {
                                                            }
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else if (p[pixel[4]] > cb)
                                            if (p[pixel[5]] > cb)
                                                if (p[pixel[6]] > cb)
                                                    if (p[pixel[7]] > cb)
                                                        if (p[pixel[8]] > cb)
                                                            if (p[pixel[9]] > cb)
                                                            {
                                                            }
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else
                                    continue;
                            else if (p[pixel[10]] < c_b)
                                if (p[pixel[7]] < c_b)
                                    if (p[pixel[8]] < c_b)
                                        if (p[pixel[9]] < c_b)
                                            if (p[pixel[11]] < c_b)
                                                if (p[pixel[6]] < c_b)
                                                    if (p[pixel[5]] < c_b)
                                                        if (p[pixel[4]] < c_b)
                                                        {
                                                        }
                                                        else if (p[pixel[12]] < c_b)
                                                            if (p[pixel[13]] < c_b)
                                                            {
                                                            }
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else if (p[pixel[12]] < c_b)
                                                        if (p[pixel[13]] < c_b)
                                                            if (p[pixel[14]] < c_b)
                                                            {
                                                            }
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else if (p[pixel[12]] < c_b)
                                                    if (p[pixel[13]] < c_b)
                                                        if (p[pixel[14]] < c_b)
                                                            if (p[pixel[15]] < c_b)
                                                            {
                                                            }
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else
                                    continue;
                            else
                                continue;
                        else if (p[pixel[10]] > cb)
                            if (p[pixel[11]] > cb)
                                if (p[pixel[12]] > cb)
                                    if (p[pixel[13]] > cb)
                                        if (p[pixel[14]] > cb)
                                            if (p[pixel[15]] > cb)
                                            {
                                            }
                                            else if (p[pixel[6]] > cb)
                                                if (p[pixel[7]] > cb)
                                                    if (p[pixel[8]] > cb)
                                                        if (p[pixel[9]] > cb)
                                                        {
                                                        }
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else if (p[pixel[5]] > cb)
                                            if (p[pixel[6]] > cb)
                                                if (p[pixel[7]] > cb)
                                                    if (p[pixel[8]] > cb)
                                                        if (p[pixel[9]] > cb)
                                                        {
                                                        }
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else if (p[pixel[4]] > cb)
                                        if (p[pixel[5]] > cb)
                                            if (p[pixel[6]] > cb)
                                                if (p[pixel[7]] > cb)
                                                    if (p[pixel[8]] > cb)
                                                        if (p[pixel[9]] > cb)
                                                        {
                                                        }
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else
                                    continue;
                            else
                                continue;
                        else if (p[pixel[10]] < c_b)
                            if (p[pixel[7]] < c_b)
                                if (p[pixel[8]] < c_b)
                                    if (p[pixel[9]] < c_b)
                                        if (p[pixel[11]] < c_b)
                                            if (p[pixel[12]] < c_b)
                                                if (p[pixel[6]] < c_b)
                                                    if (p[pixel[5]] < c_b)
                                                        if (p[pixel[4]] < c_b)
                                                        {
                                                        }
                                                        else if (p[pixel[13]] < c_b)
                                                        {
                                                        }
                                                        else
                                                            continue;
                                                    else if (p[pixel[13]] < c_b)
                                                        if (p[pixel[14]] < c_b)
                                                        {
                                                        }
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else if (p[pixel[13]] < c_b)
                                                    if (p[pixel[14]] < c_b)
                                                        if (p[pixel[15]] < c_b)
                                                        {
                                                        }
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else
                                    continue;
                            else
                                continue;
                        else
                            continue;
                    else if (p[pixel[2]] < c_b)
                        if (p[pixel[9]] > cb)
                            if (p[pixel[10]] > cb)
                                if (p[pixel[11]] > cb)
                                    if (p[pixel[12]] > cb)
                                        if (p[pixel[13]] > cb)
                                            if (p[pixel[14]] > cb)
                                                if (p[pixel[15]] > cb)
                                                {
                                                }
                                                else if (p[pixel[6]] > cb)
                                                    if (p[pixel[7]] > cb)
                                                        if (p[pixel[8]] > cb)
                                                        {
                                                        }
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else if (p[pixel[5]] > cb)
                                                if (p[pixel[6]] > cb)
                                                    if (p[pixel[7]] > cb)
                                                        if (p[pixel[8]] > cb)
                                                        {
                                                        }
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else if (p[pixel[4]] > cb)
                                            if (p[pixel[5]] > cb)
                                                if (p[pixel[6]] > cb)
                                                    if (p[pixel[7]] > cb)
                                                        if (p[pixel[8]] > cb)
                                                        {
                                                        }
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else if (p[pixel[3]] > cb)
                                        if (p[pixel[4]] > cb)
                                            if (p[pixel[5]] > cb)
                                                if (p[pixel[6]] > cb)
                                                    if (p[pixel[7]] > cb)
                                                        if (p[pixel[8]] > cb)
                                                        {
                                                        }
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else
                                    continue;
                            else
                                continue;
                        else if (p[pixel[9]] < c_b)
                            if (p[pixel[7]] < c_b)
                                if (p[pixel[8]] < c_b)
                                    if (p[pixel[10]] < c_b)
                                        if (p[pixel[6]] < c_b)
                                            if (p[pixel[5]] < c_b)
                                                if (p[pixel[4]] < c_b)
                                                    if (p[pixel[3]] < c_b)
                                                    {
                                                    }
                                                    else if (p[pixel[11]] < c_b)
                                                        if (p[pixel[12]] < c_b)
                                                        {
                                                        }
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else if (p[pixel[11]] < c_b)
                                                    if (p[pixel[12]] < c_b)
                                                        if (p[pixel[13]] < c_b)
                                                        {
                                                        }
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else if (p[pixel[11]] < c_b)
                                                if (p[pixel[12]] < c_b)
                                                    if (p[pixel[13]] < c_b)
                                                        if (p[pixel[14]] < c_b)
                                                        {
                                                        }
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else if (p[pixel[11]] < c_b)
                                            if (p[pixel[12]] < c_b)
                                                if (p[pixel[13]] < c_b)
                                                    if (p[pixel[14]] < c_b)
                                                        if (p[pixel[15]] < c_b)
                                                        {
                                                        }
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else
                                    continue;
                            else
                                continue;
                        else
                            continue;
                    else if (p[pixel[9]] > cb)
                        if (p[pixel[10]] > cb)
                            if (p[pixel[11]] > cb)
                                if (p[pixel[12]] > cb)
                                    if (p[pixel[13]] > cb)
                                        if (p[pixel[14]] > cb)
                                            if (p[pixel[15]] > cb)
                                            {
                                            }
                                            else if (p[pixel[6]] > cb)
                                                if (p[pixel[7]] > cb)
                                                    if (p[pixel[8]] > cb)
                                                    {
                                                    }
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else if (p[pixel[5]] > cb)
                                            if (p[pixel[6]] > cb)
                                                if (p[pixel[7]] > cb)
                                                    if (p[pixel[8]] > cb)
                                                    {
                                                    }
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else if (p[pixel[4]] > cb)
                                        if (p[pixel[5]] > cb)
                                            if (p[pixel[6]] > cb)
                                                if (p[pixel[7]] > cb)
                                                    if (p[pixel[8]] > cb)
                                                    {
                                                    }
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else if (p[pixel[3]] > cb)
                                    if (p[pixel[4]] > cb)
                                        if (p[pixel[5]] > cb)
                                            if (p[pixel[6]] > cb)
                                                if (p[pixel[7]] > cb)
                                                    if (p[pixel[8]] > cb)
                                                    {
                                                    }
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else
                                    continue;
                            else
                                continue;
                        else
                            continue;
                    else if (p[pixel[9]] < c_b)
                        if (p[pixel[7]] < c_b)
                            if (p[pixel[8]] < c_b)
                                if (p[pixel[10]] < c_b)
                                    if (p[pixel[11]] < c_b)
                                        if (p[pixel[6]] < c_b)
                                            if (p[pixel[5]] < c_b)
                                                if (p[pixel[4]] < c_b)
                                                    if (p[pixel[3]] < c_b)
                                                    {
                                                    }
                                                    else if (p[pixel[12]] < c_b)
                                                    {
                                                    }
                                                    else
                                                        continue;
                                                else if (p[pixel[12]] < c_b)
                                                    if (p[pixel[13]] < c_b)
                                                    {
                                                    }
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else if (p[pixel[12]] < c_b)
                                                if (p[pixel[13]] < c_b)
                                                    if (p[pixel[14]] < c_b)
                                                    {
                                                    }
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else if (p[pixel[12]] < c_b)
                                            if (p[pixel[13]] < c_b)
                                                if (p[pixel[14]] < c_b)
                                                    if (p[pixel[15]] < c_b)
                                                    {
                                                    }
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else
                                    continue;
                            else
                                continue;
                        else
                            continue;
                    else
                        continue;
                else if (p[pixel[1]] < c_b)
                    if (p[pixel[8]] > cb)
                        if (p[pixel[9]] > cb)
                            if (p[pixel[10]] > cb)
                                if (p[pixel[11]] > cb)
                                    if (p[pixel[12]] > cb)
                                        if (p[pixel[13]] > cb)
                                            if (p[pixel[14]] > cb)
                                                if (p[pixel[15]] > cb)
                                                {
                                                }
                                                else if (p[pixel[6]] > cb)
                                                    if (p[pixel[7]] > cb)
                                                    {
                                                    }
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else if (p[pixel[5]] > cb)
                                                if (p[pixel[6]] > cb)
                                                    if (p[pixel[7]] > cb)
                                                    {
                                                    }
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else if (p[pixel[4]] > cb)
                                            if (p[pixel[5]] > cb)
                                                if (p[pixel[6]] > cb)
                                                    if (p[pixel[7]] > cb)
                                                    {
                                                    }
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else if (p[pixel[3]] > cb)
                                        if (p[pixel[4]] > cb)
                                            if (p[pixel[5]] > cb)
                                                if (p[pixel[6]] > cb)
                                                    if (p[pixel[7]] > cb)
                                                    {
                                                    }
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else if (p[pixel[2]] > cb)
                                    if (p[pixel[3]] > cb)
                                        if (p[pixel[4]] > cb)
                                            if (p[pixel[5]] > cb)
                                                if (p[pixel[6]] > cb)
                                                    if (p[pixel[7]] > cb)
                                                    {
                                                    }
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else
                                    continue;
                            else
                                continue;
                        else
                            continue;
                    else if (p[pixel[8]] < c_b)
                        if (p[pixel[7]] < c_b)
                            if (p[pixel[9]] < c_b)
                                if (p[pixel[6]] < c_b)
                                    if (p[pixel[5]] < c_b)
                                        if (p[pixel[4]] < c_b)
                                            if (p[pixel[3]] < c_b)
                                                if (p[pixel[2]] < c_b)
                                                {
                                                }
                                                else if (p[pixel[10]] < c_b)
                                                    if (p[pixel[11]] < c_b)
                                                    {
                                                    }
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else if (p[pixel[10]] < c_b)
                                                if (p[pixel[11]] < c_b)
                                                    if (p[pixel[12]] < c_b)
                                                    {
                                                    }
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else if (p[pixel[10]] < c_b)
                                            if (p[pixel[11]] < c_b)
                                                if (p[pixel[12]] < c_b)
                                                    if (p[pixel[13]] < c_b)
                                                    {
                                                    }
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else if (p[pixel[10]] < c_b)
                                        if (p[pixel[11]] < c_b)
                                            if (p[pixel[12]] < c_b)
                                                if (p[pixel[13]] < c_b)
                                                    if (p[pixel[14]] < c_b)
                                                    {
                                                    }
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else if (p[pixel[10]] < c_b)
                                    if (p[pixel[11]] < c_b)
                                        if (p[pixel[12]] < c_b)
                                            if (p[pixel[13]] < c_b)
                                                if (p[pixel[14]] < c_b)
                                                    if (p[pixel[15]] < c_b)
                                                    {
                                                    }
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else
                                    continue;
                            else
                                continue;
                        else
                            continue;
                    else
                        continue;
                else if (p[pixel[8]] > cb)
                    if (p[pixel[9]] > cb)
                        if (p[pixel[10]] > cb)
                            if (p[pixel[11]] > cb)
                                if (p[pixel[12]] > cb)
                                    if (p[pixel[13]] > cb)
                                        if (p[pixel[14]] > cb)
                                            if (p[pixel[15]] > cb)
                                            {
                                            }
                                            else if (p[pixel[6]] > cb)
                                                if (p[pixel[7]] > cb)
                                                {
                                                }
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else if (p[pixel[5]] > cb)
                                            if (p[pixel[6]] > cb)
                                                if (p[pixel[7]] > cb)
                                                {
                                                }
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else if (p[pixel[4]] > cb)
                                        if (p[pixel[5]] > cb)
                                            if (p[pixel[6]] > cb)
                                                if (p[pixel[7]] > cb)
                                                {
                                                }
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else if (p[pixel[3]] > cb)
                                    if (p[pixel[4]] > cb)
                                        if (p[pixel[5]] > cb)
                                            if (p[pixel[6]] > cb)
                                                if (p[pixel[7]] > cb)
                                                {
                                                }
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else
                                    continue;
                            else if (p[pixel[2]] > cb)
                                if (p[pixel[3]] > cb)
                                    if (p[pixel[4]] > cb)
                                        if (p[pixel[5]] > cb)
                                            if (p[pixel[6]] > cb)
                                                if (p[pixel[7]] > cb)
                                                {
                                                }
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else
                                    continue;
                            else
                                continue;
                        else
                            continue;
                    else
                        continue;
                else if (p[pixel[8]] < c_b)
                    if (p[pixel[7]] < c_b)
                        if (p[pixel[9]] < c_b)
                            if (p[pixel[10]] < c_b)
                                if (p[pixel[6]] < c_b)
                                    if (p[pixel[5]] < c_b)
                                        if (p[pixel[4]] < c_b)
                                            if (p[pixel[3]] < c_b)
                                                if (p[pixel[2]] < c_b)
                                                {
                                                }
                                                else if (p[pixel[11]] < c_b)
                                                {
                                                }
                                                else
                                                    continue;
                                            else if (p[pixel[11]] < c_b)
                                                if (p[pixel[12]] < c_b)
                                                {
                                                }
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else if (p[pixel[11]] < c_b)
                                            if (p[pixel[12]] < c_b)
                                                if (p[pixel[13]] < c_b)
                                                {
                                                }
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else if (p[pixel[11]] < c_b)
                                        if (p[pixel[12]] < c_b)
                                            if (p[pixel[13]] < c_b)
                                                if (p[pixel[14]] < c_b)
                                                {
                                                }
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else if (p[pixel[11]] < c_b)
                                    if (p[pixel[12]] < c_b)
                                        if (p[pixel[13]] < c_b)
                                            if (p[pixel[14]] < c_b)
                                                if (p[pixel[15]] < c_b)
                                                {
                                                }
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else
                                    continue;
                            else
                                continue;
                        else
                            continue;
                    else
                        continue;
                else
                    continue;
            else if (p[pixel[0]] < c_b)
                if (p[pixel[1]] > cb)
                    if (p[pixel[8]] > cb)
                        if (p[pixel[7]] > cb)
                            if (p[pixel[9]] > cb)
                                if (p[pixel[6]] > cb)
                                    if (p[pixel[5]] > cb)
                                        if (p[pixel[4]] > cb)
                                            if (p[pixel[3]] > cb)
                                                if (p[pixel[2]] > cb)
                                                {
                                                }
                                                else if (p[pixel[10]] > cb)
                                                    if (p[pixel[11]] > cb)
                                                    {
                                                    }
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else if (p[pixel[10]] > cb)
                                                if (p[pixel[11]] > cb)
                                                    if (p[pixel[12]] > cb)
                                                    {
                                                    }
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else if (p[pixel[10]] > cb)
                                            if (p[pixel[11]] > cb)
                                                if (p[pixel[12]] > cb)
                                                    if (p[pixel[13]] > cb)
                                                    {
                                                    }
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else if (p[pixel[10]] > cb)
                                        if (p[pixel[11]] > cb)
                                            if (p[pixel[12]] > cb)
                                                if (p[pixel[13]] > cb)
                                                    if (p[pixel[14]] > cb)
                                                    {
                                                    }
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else if (p[pixel[10]] > cb)
                                    if (p[pixel[11]] > cb)
                                        if (p[pixel[12]] > cb)
                                            if (p[pixel[13]] > cb)
                                                if (p[pixel[14]] > cb)
                                                    if (p[pixel[15]] > cb)
                                                    {
                                                    }
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else
                                    continue;
                            else
                                continue;
                        else
                            continue;
                    else if (p[pixel[8]] < c_b)
                        if (p[pixel[9]] < c_b)
                            if (p[pixel[10]] < c_b)
                                if (p[pixel[11]] < c_b)
                                    if (p[pixel[12]] < c_b)
                                        if (p[pixel[13]] < c_b)
                                            if (p[pixel[14]] < c_b)
                                                if (p[pixel[15]] < c_b)
                                                {
                                                }
                                                else if (p[pixel[6]] < c_b)
                                                    if (p[pixel[7]] < c_b)
                                                    {
                                                    }
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else if (p[pixel[5]] < c_b)
                                                if (p[pixel[6]] < c_b)
                                                    if (p[pixel[7]] < c_b)
                                                    {
                                                    }
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else if (p[pixel[4]] < c_b)
                                            if (p[pixel[5]] < c_b)
                                                if (p[pixel[6]] < c_b)
                                                    if (p[pixel[7]] < c_b)
                                                    {
                                                    }
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else if (p[pixel[3]] < c_b)
                                        if (p[pixel[4]] < c_b)
                                            if (p[pixel[5]] < c_b)
                                                if (p[pixel[6]] < c_b)
                                                    if (p[pixel[7]] < c_b)
                                                    {
                                                    }
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else if (p[pixel[2]] < c_b)
                                    if (p[pixel[3]] < c_b)
                                        if (p[pixel[4]] < c_b)
                                            if (p[pixel[5]] < c_b)
                                                if (p[pixel[6]] < c_b)
                                                    if (p[pixel[7]] < c_b)
                                                    {
                                                    }
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else
                                    continue;
                            else
                                continue;
                        else
                            continue;
                    else
                        continue;
                else if (p[pixel[1]] < c_b)
                    if (p[pixel[2]] > cb)
                        if (p[pixel[9]] > cb)
                            if (p[pixel[7]] > cb)
                                if (p[pixel[8]] > cb)
                                    if (p[pixel[10]] > cb)
                                        if (p[pixel[6]] > cb)
                                            if (p[pixel[5]] > cb)
                                                if (p[pixel[4]] > cb)
                                                    if (p[pixel[3]] > cb)
                                                    {
                                                    }
                                                    else if (p[pixel[11]] > cb)
                                                        if (p[pixel[12]] > cb)
                                                        {
                                                        }
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else if (p[pixel[11]] > cb)
                                                    if (p[pixel[12]] > cb)
                                                        if (p[pixel[13]] > cb)
                                                        {
                                                        }
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else if (p[pixel[11]] > cb)
                                                if (p[pixel[12]] > cb)
                                                    if (p[pixel[13]] > cb)
                                                        if (p[pixel[14]] > cb)
                                                        {
                                                        }
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else if (p[pixel[11]] > cb)
                                            if (p[pixel[12]] > cb)
                                                if (p[pixel[13]] > cb)
                                                    if (p[pixel[14]] > cb)
                                                        if (p[pixel[15]] > cb)
                                                        {
                                                        }
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else
                                    continue;
                            else
                                continue;
                        else if (p[pixel[9]] < c_b)
                            if (p[pixel[10]] < c_b)
                                if (p[pixel[11]] < c_b)
                                    if (p[pixel[12]] < c_b)
                                        if (p[pixel[13]] < c_b)
                                            if (p[pixel[14]] < c_b)
                                                if (p[pixel[15]] < c_b)
                                                {
                                                }
                                                else if (p[pixel[6]] < c_b)
                                                    if (p[pixel[7]] < c_b)
                                                        if (p[pixel[8]] < c_b)
                                                        {
                                                        }
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else if (p[pixel[5]] < c_b)
                                                if (p[pixel[6]] < c_b)
                                                    if (p[pixel[7]] < c_b)
                                                        if (p[pixel[8]] < c_b)
                                                        {
                                                        }
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else if (p[pixel[4]] < c_b)
                                            if (p[pixel[5]] < c_b)
                                                if (p[pixel[6]] < c_b)
                                                    if (p[pixel[7]] < c_b)
                                                        if (p[pixel[8]] < c_b)
                                                        {
                                                        }
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else if (p[pixel[3]] < c_b)
                                        if (p[pixel[4]] < c_b)
                                            if (p[pixel[5]] < c_b)
                                                if (p[pixel[6]] < c_b)
                                                    if (p[pixel[7]] < c_b)
                                                        if (p[pixel[8]] < c_b)
                                                        {
                                                        }
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else
                                    continue;
                            else
                                continue;
                        else
                            continue;
                    else if (p[pixel[2]] < c_b)
                        if (p[pixel[3]] > cb)
                            if (p[pixel[10]] > cb)
                                if (p[pixel[7]] > cb)
                                    if (p[pixel[8]] > cb)
                                        if (p[pixel[9]] > cb)
                                            if (p[pixel[11]] > cb)
                                                if (p[pixel[6]] > cb)
                                                    if (p[pixel[5]] > cb)
                                                        if (p[pixel[4]] > cb)
                                                        {
                                                        }
                                                        else if (p[pixel[12]] > cb)
                                                            if (p[pixel[13]] > cb)
                                                            {
                                                            }
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else if (p[pixel[12]] > cb)
                                                        if (p[pixel[13]] > cb)
                                                            if (p[pixel[14]] > cb)
                                                            {
                                                            }
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else if (p[pixel[12]] > cb)
                                                    if (p[pixel[13]] > cb)
                                                        if (p[pixel[14]] > cb)
                                                            if (p[pixel[15]] > cb)
                                                            {
                                                            }
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else
                                    continue;
                            else if (p[pixel[10]] < c_b)
                                if (p[pixel[11]] < c_b)
                                    if (p[pixel[12]] < c_b)
                                        if (p[pixel[13]] < c_b)
                                            if (p[pixel[14]] < c_b)
                                                if (p[pixel[15]] < c_b)
                                                {
                                                }
                                                else if (p[pixel[6]] < c_b)
                                                    if (p[pixel[7]] < c_b)
                                                        if (p[pixel[8]] < c_b)
                                                            if (p[pixel[9]] < c_b)
                                                            {
                                                            }
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else if (p[pixel[5]] < c_b)
                                                if (p[pixel[6]] < c_b)
                                                    if (p[pixel[7]] < c_b)
                                                        if (p[pixel[8]] < c_b)
                                                            if (p[pixel[9]] < c_b)
                                                            {
                                                            }
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else if (p[pixel[4]] < c_b)
                                            if (p[pixel[5]] < c_b)
                                                if (p[pixel[6]] < c_b)
                                                    if (p[pixel[7]] < c_b)
                                                        if (p[pixel[8]] < c_b)
                                                            if (p[pixel[9]] < c_b)
                                                            {
                                                            }
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else
                                    continue;
                            else
                                continue;
                        else if (p[pixel[3]] < c_b)
                            if (p[pixel[4]] > cb)
                                if (p[pixel[13]] > cb)
                                    if (p[pixel[7]] > cb)
                                        if (p[pixel[8]] > cb)
                                            if (p[pixel[9]] > cb)
                                                if (p[pixel[10]] > cb)
                                                    if (p[pixel[11]] > cb)
                                                        if (p[pixel[12]] > cb)
                                                            if (p[pixel[6]] > cb)
                                                                if (p[pixel[5]] > cb)
                                                                {
                                                                }
                                                                else if (p[pixel[14]] > cb)
                                                                {
                                                                }
                                                                else
                                                                    continue;
                                                            else if (p[pixel[14]] > cb)
                                                                if (p[pixel[15]] > cb)
                                                                {
                                                                }
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else if (p[pixel[13]] < c_b)
                                    if (p[pixel[11]] > cb)
                                        if (p[pixel[5]] > cb)
                                            if (p[pixel[6]] > cb)
                                                if (p[pixel[7]] > cb)
                                                    if (p[pixel[8]] > cb)
                                                        if (p[pixel[9]] > cb)
                                                            if (p[pixel[10]] > cb)
                                                                if (p[pixel[12]] > cb)
                                                                {
                                                                }
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else if (p[pixel[11]] < c_b)
                                        if (p[pixel[12]] < c_b)
                                            if (p[pixel[14]] < c_b)
                                                if (p[pixel[15]] < c_b)
                                                {
                                                }
                                                else if (p[pixel[6]] < c_b)
                                                    if (p[pixel[7]] < c_b)
                                                        if (p[pixel[8]] < c_b)
                                                            if (p[pixel[9]] < c_b)
                                                                if (p[pixel[10]] < c_b)
                                                                {
                                                                }
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else if (p[pixel[5]] < c_b)
                                                if (p[pixel[6]] < c_b)
                                                    if (p[pixel[7]] < c_b)
                                                        if (p[pixel[8]] < c_b)
                                                            if (p[pixel[9]] < c_b)
                                                                if (p[pixel[10]] < c_b)
                                                                {
                                                                }
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else if (p[pixel[5]] > cb)
                                    if (p[pixel[6]] > cb)
                                        if (p[pixel[7]] > cb)
                                            if (p[pixel[8]] > cb)
                                                if (p[pixel[9]] > cb)
                                                    if (p[pixel[10]] > cb)
                                                        if (p[pixel[11]] > cb)
                                                            if (p[pixel[12]] > cb)
                                                            {
                                                            }
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else
                                    continue;
                            else if (p[pixel[4]] < c_b)
                                if (p[pixel[5]] > cb)
                                    if (p[pixel[14]] > cb)
                                        if (p[pixel[7]] > cb)
                                            if (p[pixel[8]] > cb)
                                                if (p[pixel[9]] > cb)
                                                    if (p[pixel[10]] > cb)
                                                        if (p[pixel[11]] > cb)
                                                            if (p[pixel[12]] > cb)
                                                                if (p[pixel[13]] > cb)
                                                                    if (p[pixel[6]] > cb)
                                                                    {
                                                                    }
                                                                    else if (p[pixel[15]] > cb)
                                                                    {
                                                                    }
                                                                    else
                                                                        continue;
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else if (p[pixel[14]] < c_b)
                                        if (p[pixel[12]] > cb)
                                            if (p[pixel[6]] > cb)
                                                if (p[pixel[7]] > cb)
                                                    if (p[pixel[8]] > cb)
                                                        if (p[pixel[9]] > cb)
                                                            if (p[pixel[10]] > cb)
                                                                if (p[pixel[11]] > cb)
                                                                    if (p[pixel[13]] > cb)
                                                                    {
                                                                    }
                                                                    else
                                                                        continue;
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else if (p[pixel[12]] < c_b)
                                            if (p[pixel[13]] < c_b)
                                                if (p[pixel[15]] < c_b)
                                                {
                                                }
                                                else if (p[pixel[6]] < c_b)
                                                    if (p[pixel[7]] < c_b)
                                                        if (p[pixel[8]] < c_b)
                                                            if (p[pixel[9]] < c_b)
                                                                if (p[pixel[10]] < c_b)
                                                                    if (p[pixel[11]] < c_b)
                                                                    {
                                                                    }
                                                                    else
                                                                        continue;
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else if (p[pixel[6]] > cb)
                                        if (p[pixel[7]] > cb)
                                            if (p[pixel[8]] > cb)
                                                if (p[pixel[9]] > cb)
                                                    if (p[pixel[10]] > cb)
                                                        if (p[pixel[11]] > cb)
                                                            if (p[pixel[12]] > cb)
                                                                if (p[pixel[13]] > cb)
                                                                {
                                                                }
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else if (p[pixel[5]] < c_b)
                                    if (p[pixel[6]] > cb)
                                        if (p[pixel[15]] < c_b)
                                            if (p[pixel[13]] > cb)
                                                if (p[pixel[7]] > cb)
                                                    if (p[pixel[8]] > cb)
                                                        if (p[pixel[9]] > cb)
                                                            if (p[pixel[10]] > cb)
                                                                if (p[pixel[11]] > cb)
                                                                    if (p[pixel[12]] > cb)
                                                                        if (p[pixel[14]] > cb)
                                                                        {
                                                                        }
                                                                        else
                                                                            continue;
                                                                    else
                                                                        continue;
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else if (p[pixel[13]] < c_b)
                                                if (p[pixel[14]] < c_b)
                                                {
                                                }
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else if (p[pixel[7]] > cb)
                                            if (p[pixel[8]] > cb)
                                                if (p[pixel[9]] > cb)
                                                    if (p[pixel[10]] > cb)
                                                        if (p[pixel[11]] > cb)
                                                            if (p[pixel[12]] > cb)
                                                                if (p[pixel[13]] > cb)
                                                                    if (p[pixel[14]] > cb)
                                                                    {
                                                                    }
                                                                    else
                                                                        continue;
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else if (p[pixel[6]] < c_b)
                                        if (p[pixel[7]] > cb)
                                            if (p[pixel[14]] > cb)
                                                if (p[pixel[8]] > cb)
                                                    if (p[pixel[9]] > cb)
                                                        if (p[pixel[10]] > cb)
                                                            if (p[pixel[11]] > cb)
                                                                if (p[pixel[12]] > cb)
                                                                    if (p[pixel[13]] > cb)
                                                                        if (p[pixel[15]] > cb)
                                                                        {
                                                                        }
                                                                        else
                                                                            continue;
                                                                    else
                                                                        continue;
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else if (p[pixel[14]] < c_b)
                                                if (p[pixel[15]] < c_b)
                                                {
                                                }
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else if (p[pixel[7]] < c_b)
                                            if (p[pixel[8]] < c_b)
                                            {
                                            }
                                            else if (p[pixel[15]] < c_b)
                                            {
                                            }
                                            else
                                                continue;
                                        else if (p[pixel[14]] < c_b)
                                            if (p[pixel[15]] < c_b)
                                            {
                                            }
                                            else
                                                continue;
                                        else
                                            continue;
                                    else if (p[pixel[13]] > cb)
                                        if (p[pixel[7]] > cb)
                                            if (p[pixel[8]] > cb)
                                                if (p[pixel[9]] > cb)
                                                    if (p[pixel[10]] > cb)
                                                        if (p[pixel[11]] > cb)
                                                            if (p[pixel[12]] > cb)
                                                                if (p[pixel[14]] > cb)
                                                                    if (p[pixel[15]] > cb)
                                                                    {
                                                                    }
                                                                    else
                                                                        continue;
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else if (p[pixel[13]] < c_b)
                                        if (p[pixel[14]] < c_b)
                                            if (p[pixel[15]] < c_b)
                                            {
                                            }
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else if (p[pixel[12]] > cb)
                                    if (p[pixel[7]] > cb)
                                        if (p[pixel[8]] > cb)
                                            if (p[pixel[9]] > cb)
                                                if (p[pixel[10]] > cb)
                                                    if (p[pixel[11]] > cb)
                                                        if (p[pixel[13]] > cb)
                                                            if (p[pixel[14]] > cb)
                                                                if (p[pixel[6]] > cb)
                                                                {
                                                                }
                                                                else if (p[pixel[15]] > cb)
                                                                {
                                                                }
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else if (p[pixel[12]] < c_b)
                                    if (p[pixel[13]] < c_b)
                                        if (p[pixel[14]] < c_b)
                                            if (p[pixel[15]] < c_b)
                                            {
                                            }
                                            else if (p[pixel[6]] < c_b)
                                                if (p[pixel[7]] < c_b)
                                                    if (p[pixel[8]] < c_b)
                                                        if (p[pixel[9]] < c_b)
                                                            if (p[pixel[10]] < c_b)
                                                                if (p[pixel[11]] < c_b)
                                                                {
                                                                }
                                                                else
                                                                    continue;
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else
                                    continue;
                            else if (p[pixel[11]] > cb)
                                if (p[pixel[7]] > cb)
                                    if (p[pixel[8]] > cb)
                                        if (p[pixel[9]] > cb)
                                            if (p[pixel[10]] > cb)
                                                if (p[pixel[12]] > cb)
                                                    if (p[pixel[13]] > cb)
                                                        if (p[pixel[6]] > cb)
                                                            if (p[pixel[5]] > cb)
                                                            {
                                                            }
                                                            else if (p[pixel[14]] > cb)
                                                            {
                                                            }
                                                            else
                                                                continue;
                                                        else if (p[pixel[14]] > cb)
                                                            if (p[pixel[15]] > cb)
                                                            {
                                                            }
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else
                                    continue;
                            else if (p[pixel[11]] < c_b)
                                if (p[pixel[12]] < c_b)
                                    if (p[pixel[13]] < c_b)
                                        if (p[pixel[14]] < c_b)
                                            if (p[pixel[15]] < c_b)
                                            {
                                            }
                                            else if (p[pixel[6]] < c_b)
                                                if (p[pixel[7]] < c_b)
                                                    if (p[pixel[8]] < c_b)
                                                        if (p[pixel[9]] < c_b)
                                                            if (p[pixel[10]] < c_b)
                                                            {
                                                            }
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else if (p[pixel[5]] < c_b)
                                            if (p[pixel[6]] < c_b)
                                                if (p[pixel[7]] < c_b)
                                                    if (p[pixel[8]] < c_b)
                                                        if (p[pixel[9]] < c_b)
                                                            if (p[pixel[10]] < c_b)
                                                            {
                                                            }
                                                            else
                                                                continue;
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else
                                    continue;
                            else
                                continue;
                        else if (p[pixel[10]] > cb)
                            if (p[pixel[7]] > cb)
                                if (p[pixel[8]] > cb)
                                    if (p[pixel[9]] > cb)
                                        if (p[pixel[11]] > cb)
                                            if (p[pixel[12]] > cb)
                                                if (p[pixel[6]] > cb)
                                                    if (p[pixel[5]] > cb)
                                                        if (p[pixel[4]] > cb)
                                                        {
                                                        }
                                                        else if (p[pixel[13]] > cb)
                                                        {
                                                        }
                                                        else
                                                            continue;
                                                    else if (p[pixel[13]] > cb)
                                                        if (p[pixel[14]] > cb)
                                                        {
                                                        }
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else if (p[pixel[13]] > cb)
                                                    if (p[pixel[14]] > cb)
                                                        if (p[pixel[15]] > cb)
                                                        {
                                                        }
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else
                                    continue;
                            else
                                continue;
                        else if (p[pixel[10]] < c_b)
                            if (p[pixel[11]] < c_b)
                                if (p[pixel[12]] < c_b)
                                    if (p[pixel[13]] < c_b)
                                        if (p[pixel[14]] < c_b)
                                            if (p[pixel[15]] < c_b)
                                            {
                                            }
                                            else if (p[pixel[6]] < c_b)
                                                if (p[pixel[7]] < c_b)
                                                    if (p[pixel[8]] < c_b)
                                                        if (p[pixel[9]] < c_b)
                                                        {
                                                        }
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else if (p[pixel[5]] < c_b)
                                            if (p[pixel[6]] < c_b)
                                                if (p[pixel[7]] < c_b)
                                                    if (p[pixel[8]] < c_b)
                                                        if (p[pixel[9]] < c_b)
                                                        {
                                                        }
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else if (p[pixel[4]] < c_b)
                                        if (p[pixel[5]] < c_b)
                                            if (p[pixel[6]] < c_b)
                                                if (p[pixel[7]] < c_b)
                                                    if (p[pixel[8]] < c_b)
                                                        if (p[pixel[9]] < c_b)
                                                        {
                                                        }
                                                        else
                                                            continue;
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else
                                    continue;
                            else
                                continue;
                        else
                            continue;
                    else if (p[pixel[9]] > cb)
                        if (p[pixel[7]] > cb)
                            if (p[pixel[8]] > cb)
                                if (p[pixel[10]] > cb)
                                    if (p[pixel[11]] > cb)
                                        if (p[pixel[6]] > cb)
                                            if (p[pixel[5]] > cb)
                                                if (p[pixel[4]] > cb)
                                                    if (p[pixel[3]] > cb)
                                                    {
                                                    }
                                                    else if (p[pixel[12]] > cb)
                                                    {
                                                    }
                                                    else
                                                        continue;
                                                else if (p[pixel[12]] > cb)
                                                    if (p[pixel[13]] > cb)
                                                    {
                                                    }
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else if (p[pixel[12]] > cb)
                                                if (p[pixel[13]] > cb)
                                                    if (p[pixel[14]] > cb)
                                                    {
                                                    }
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else if (p[pixel[12]] > cb)
                                            if (p[pixel[13]] > cb)
                                                if (p[pixel[14]] > cb)
                                                    if (p[pixel[15]] > cb)
                                                    {
                                                    }
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else
                                    continue;
                            else
                                continue;
                        else
                            continue;
                    else if (p[pixel[9]] < c_b)
                        if (p[pixel[10]] < c_b)
                            if (p[pixel[11]] < c_b)
                                if (p[pixel[12]] < c_b)
                                    if (p[pixel[13]] < c_b)
                                        if (p[pixel[14]] < c_b)
                                            if (p[pixel[15]] < c_b)
                                            {
                                            }
                                            else if (p[pixel[6]] < c_b)
                                                if (p[pixel[7]] < c_b)
                                                    if (p[pixel[8]] < c_b)
                                                    {
                                                    }
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else if (p[pixel[5]] < c_b)
                                            if (p[pixel[6]] < c_b)
                                                if (p[pixel[7]] < c_b)
                                                    if (p[pixel[8]] < c_b)
                                                    {
                                                    }
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else if (p[pixel[4]] < c_b)
                                        if (p[pixel[5]] < c_b)
                                            if (p[pixel[6]] < c_b)
                                                if (p[pixel[7]] < c_b)
                                                    if (p[pixel[8]] < c_b)
                                                    {
                                                    }
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else if (p[pixel[3]] < c_b)
                                    if (p[pixel[4]] < c_b)
                                        if (p[pixel[5]] < c_b)
                                            if (p[pixel[6]] < c_b)
                                                if (p[pixel[7]] < c_b)
                                                    if (p[pixel[8]] < c_b)
                                                    {
                                                    }
                                                    else
                                                        continue;
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else
                                    continue;
                            else
                                continue;
                        else
                            continue;
                    else
                        continue;
                else if (p[pixel[8]] > cb)
                    if (p[pixel[7]] > cb)
                        if (p[pixel[9]] > cb)
                            if (p[pixel[10]] > cb)
                                if (p[pixel[6]] > cb)
                                    if (p[pixel[5]] > cb)
                                        if (p[pixel[4]] > cb)
                                            if (p[pixel[3]] > cb)
                                                if (p[pixel[2]] > cb)
                                                {
                                                }
                                                else if (p[pixel[11]] > cb)
                                                {
                                                }
                                                else
                                                    continue;
                                            else if (p[pixel[11]] > cb)
                                                if (p[pixel[12]] > cb)
                                                {
                                                }
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else if (p[pixel[11]] > cb)
                                            if (p[pixel[12]] > cb)
                                                if (p[pixel[13]] > cb)
                                                {
                                                }
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else if (p[pixel[11]] > cb)
                                        if (p[pixel[12]] > cb)
                                            if (p[pixel[13]] > cb)
                                                if (p[pixel[14]] > cb)
                                                {
                                                }
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else if (p[pixel[11]] > cb)
                                    if (p[pixel[12]] > cb)
                                        if (p[pixel[13]] > cb)
                                            if (p[pixel[14]] > cb)
                                                if (p[pixel[15]] > cb)
                                                {
                                                }
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else
                                    continue;
                            else
                                continue;
                        else
                            continue;
                    else
                        continue;
                else if (p[pixel[8]] < c_b)
                    if (p[pixel[9]] < c_b)
                        if (p[pixel[10]] < c_b)
                            if (p[pixel[11]] < c_b)
                                if (p[pixel[12]] < c_b)
                                    if (p[pixel[13]] < c_b)
                                        if (p[pixel[14]] < c_b)
                                            if (p[pixel[15]] < c_b)
                                            {
                                            }
                                            else if (p[pixel[6]] < c_b)
                                                if (p[pixel[7]] < c_b)
                                                {
                                                }
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else if (p[pixel[5]] < c_b)
                                            if (p[pixel[6]] < c_b)
                                                if (p[pixel[7]] < c_b)
                                                {
                                                }
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else if (p[pixel[4]] < c_b)
                                        if (p[pixel[5]] < c_b)
                                            if (p[pixel[6]] < c_b)
                                                if (p[pixel[7]] < c_b)
                                                {
                                                }
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else if (p[pixel[3]] < c_b)
                                    if (p[pixel[4]] < c_b)
                                        if (p[pixel[5]] < c_b)
                                            if (p[pixel[6]] < c_b)
                                                if (p[pixel[7]] < c_b)
                                                {
                                                }
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else
                                    continue;
                            else if (p[pixel[2]] < c_b)
                                if (p[pixel[3]] < c_b)
                                    if (p[pixel[4]] < c_b)
                                        if (p[pixel[5]] < c_b)
                                            if (p[pixel[6]] < c_b)
                                                if (p[pixel[7]] < c_b)
                                                {
                                                }
                                                else
                                                    continue;
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else
                                    continue;
                            else
                                continue;
                        else
                            continue;
                    else
                        continue;
                else
                    continue;
            else if (p[pixel[7]] > cb)
                if (p[pixel[8]] > cb)
                    if (p[pixel[9]] > cb)
                        if (p[pixel[6]] > cb)
                            if (p[pixel[5]] > cb)
                                if (p[pixel[4]] > cb)
                                    if (p[pixel[3]] > cb)
                                        if (p[pixel[2]] > cb)
                                            if (p[pixel[1]] > cb)
                                            {
                                            }
                                            else if (p[pixel[10]] > cb)
                                            {
                                            }
                                            else
                                                continue;
                                        else if (p[pixel[10]] > cb)
                                            if (p[pixel[11]] > cb)
                                            {
                                            }
                                            else
                                                continue;
                                        else
                                            continue;
                                    else if (p[pixel[10]] > cb)
                                        if (p[pixel[11]] > cb)
                                            if (p[pixel[12]] > cb)
                                            {
                                            }
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else if (p[pixel[10]] > cb)
                                    if (p[pixel[11]] > cb)
                                        if (p[pixel[12]] > cb)
                                            if (p[pixel[13]] > cb)
                                            {
                                            }
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else
                                    continue;
                            else if (p[pixel[10]] > cb)
                                if (p[pixel[11]] > cb)
                                    if (p[pixel[12]] > cb)
                                        if (p[pixel[13]] > cb)
                                            if (p[pixel[14]] > cb)
                                            {
                                            }
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else
                                    continue;
                            else
                                continue;
                        else if (p[pixel[10]] > cb)
                            if (p[pixel[11]] > cb)
                                if (p[pixel[12]] > cb)
                                    if (p[pixel[13]] > cb)
                                        if (p[pixel[14]] > cb)
                                            if (p[pixel[15]] > cb)
                                            {
                                            }
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else
                                    continue;
                            else
                                continue;
                        else
                            continue;
                    else
                        continue;
                else
                    continue;
            else if (p[pixel[7]] < c_b)
                if (p[pixel[8]] < c_b)
                    if (p[pixel[9]] < c_b)
                        if (p[pixel[6]] < c_b)
                            if (p[pixel[5]] < c_b)
                                if (p[pixel[4]] < c_b)
                                    if (p[pixel[3]] < c_b)
                                        if (p[pixel[2]] < c_b)
                                            if (p[pixel[1]] < c_b)
                                            {
                                            }
                                            else if (p[pixel[10]] < c_b)
                                            {
                                            }
                                            else
                                                continue;
                                        else if (p[pixel[10]] < c_b)
                                            if (p[pixel[11]] < c_b)
                                            {
                                            }
                                            else
                                                continue;
                                        else
                                            continue;
                                    else if (p[pixel[10]] < c_b)
                                        if (p[pixel[11]] < c_b)
                                            if (p[pixel[12]] < c_b)
                                            {
                                            }
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else if (p[pixel[10]] < c_b)
                                    if (p[pixel[11]] < c_b)
                                        if (p[pixel[12]] < c_b)
                                            if (p[pixel[13]] < c_b)
                                            {
                                            }
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else
                                    continue;
                            else if (p[pixel[10]] < c_b)
                                if (p[pixel[11]] < c_b)
                                    if (p[pixel[12]] < c_b)
                                        if (p[pixel[13]] < c_b)
                                            if (p[pixel[14]] < c_b)
                                            {
                                            }
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else
                                    continue;
                            else
                                continue;
                        else if (p[pixel[10]] < c_b)
                            if (p[pixel[11]] < c_b)
                                if (p[pixel[12]] < c_b)
                                    if (p[pixel[13]] < c_b)
                                        if (p[pixel[14]] < c_b)
                                            if (p[pixel[15]] < c_b)
                                            {
                                            }
                                            else
                                                continue;
                                        else
                                            continue;
                                    else
                                        continue;
                                else
                                    continue;
                            else
                                continue;
                        else
                            continue;
                    else
                        continue;
                else
                    continue;
            else
                continue;

            pixel_coord_t to_push = {x,y};
            vector_push_back(c, &to_push);
        }
}


#define COMPARE(X, Y) ((X) >= (Y))

static void fastNonmaxSuppression(
    vector_t *c,
    vector_t *scores,
    vector_t *res
) {
    uint16_t i, j, num_corners = c->size;

    // Point above points (roughly) to the pixel above the one of interest, if there is a feature there.
    uint16_t point_above = 0;
    uint16_t point_below = 0;

    vector_clear(res);
    if (num_corners < 1)
        return;

    // Find where each row begins
    //  (the corners are output in raster scan order). A beginning of -1 signifies
    //  that there are no corners on that row.
    int16_t last_row = ((pixel_coord_t *)vector_get(c, num_corners-1))->y; // c_std[num_corners - 1];
    vector_t *row_start = vector_create(sizeof(int));
    vector_reserve(row_start, last_row+1);

    for (i = 0; i < last_row + 1; ++i) {
        int _val = -1;
        vector_set(row_start, i, &_val);
    }

    int16_t prev_row = -1;  // 65536; // max uint16_t
    for (i = 0; i < num_corners; ++i)
        if ( ((pixel_coord_t *)vector_get(c, i))->y != prev_row ) {
            // row_start[c_std[i].y] = i;   // for example from std::vector
            vector_set(row_start, ((pixel_coord_t *)vector_get(c, i))->y, &i);
            prev_row = *(uint16_t *)vector_get(c, i);      // = c_std[i].y;
        }

    vector_reserve(res, num_corners);
    
    for (i = 0; i < num_corners; ++i)
    {
        uint8_t score = *(uint8_t *)vector_get(scores, i);
        uint16_t xx = ((pixel_coord_t *)vector_get(c, i))->x;
        uint16_t yy = ((pixel_coord_t *)vector_get(c, i))->y;

        // // Check left
        if (
            i > 0   &&
            ((pixel_coord_t *)vector_get(c, i-1))->x == xx-1   &&
            ((pixel_coord_t *)vector_get(c, i-1))->y == yy     &&
            COMPARE( *(uint8_t *)vector_get(scores, i-1), score )
        )
            continue;

        // Check right
        if (
            i < num_corners-1   &&
            ((pixel_coord_t *)vector_get(c, i+1))->x == xx+1     &&
            ((pixel_coord_t *)vector_get(c, i+1))->y == yy         &&
            COMPARE( *(uint8_t *)vector_get(scores, i+1), score )
        )
            continue;

        uint8_t suppressed = 0;
        // Check above (if there is a valid row above)
        if (yy != 0 && *(int *)vector_get(row_start, yy-1) != -1 )
        {
            // Make sure that current point_above is one row above.
            if (((pixel_coord_t *)vector_get(c, point_above))->y < yy-1)
                point_above = *(int *)vector_get(row_start, yy-1);

            // Make point_above point to the first of the pixels above the current point, if it exists.
            for (;
                ((pixel_coord_t *)vector_get(c, point_above))->y < yy   &&
                ((pixel_coord_t *)vector_get(c, point_above))->x < xx;
                ++ point_above
            )
                ;

            for (
                j = point_above;
                ((pixel_coord_t *)vector_get(c, point_above))->y < yy      &&
                ((pixel_coord_t *)vector_get(c, point_above))->x <= xx+1;
                ++j
            ) {
                int x = ((pixel_coord_t *)vector_get(c, point_above))->x;
                if (
                    (x == xx - 1 || x == xx || x == xx + 1)     &&
                    COMPARE(*(uint8_t *)vector_get(scores, j), score)
                ) {
                    suppressed = 1;
                    break;
                }
            }
            if (suppressed)
                continue;
        }

        if (
            yy != last_row  &&
            *(int *)vector_get(row_start, yy-1) != -1 &&
            point_below < num_corners
        ) {
            if (((pixel_coord_t *)vector_get(c, point_below))->y < yy+1)
                point_below = *(int *)vector_get(row_start, yy+1);

            // Make point below point to one of the pixels belowthe current point, if it exists.
            for (;
                point_below < num_corners   &&
                ((pixel_coord_t *)vector_get(c, point_below))->y == yy+1   &&
                ((pixel_coord_t *)vector_get(c, point_below))->x < xx-1;
                ++ point_below
            )
                ;

            for (
                j = point_below;
                j < num_corners &&
                ((pixel_coord_t *)vector_get(c, point_below))->y == yy+1   &&
                ((pixel_coord_t *)vector_get(c, point_below))->x <= xx+1;
                ++j
            ) {
                int x = ((pixel_coord_t *)vector_get(c, point_below))->x;
                if (
                    (x == xx - 1 || x == xx || x == xx + 1)     &&
                    COMPARE(*(uint8_t *)vector_get(scores, j), score)
                ) {
                    suppressed = 1;
                    break;
                }
            }
            if (suppressed)
                continue;
        }

        vector_push_back(res, vector_get(c, i));
    }
}


esp_err_t fast9(
    const camera_fb_t *fb1, // gray single channel
    vector_t *keypoints,
    uint8_t threshold
) {
    vector_t *ct = vector_create(sizeof(pixel_coord_t));
    fast9Detect(fb1, ct, threshold);
    
    vector_t *scores = vector_create(sizeof(uint8_t));
    fast9ComputeScores(fb1, ct, scores, threshold);

    vector_clear(keypoints);
    fastNonmaxSuppression(ct, scores, keypoints);

    vector_destroy(ct);
    vector_destroy(scores);

    return ESP_OK;
}