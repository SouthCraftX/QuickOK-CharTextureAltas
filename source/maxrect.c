#include "maxrect.h"
#include <math.h>
#include <limits.h>

#define STRATEGY_COUNT 4

qo_bool_t
expand_free_rects(
    _Maxrect *  maxrect 
) {
    qo_uint32_t new_count = maxrect->max_free_rect_count * 2;
    _FreeRect * new_free_rects = realloc_free_rects(maxrect, new_count);
    if (new_free_rects) 
    {
        maxrect->free_rects = new_free_rects;
        maxrect->max_free_rect_count = new_count;
        return qo_true;
    }
    return qo_false;
}

void
incremental_merge_free_rects(
    _Maxrect *  maxrect ,
    qo_int32_t  new_rect_index
) {
    if (new_rect_index < 0 || new_rect_index >= maxrect->free_rect_count)
        return;

    _FreeRect * new_rect = &maxrect->free_rects[new_rect_index];
    qo_bool_t   merged;

    do 
    {
        merged = qo_false;

        for (qo_int32_t i = 0; i < maxrect->free_rect_count; i++)
        {
            if (i == new_rect_index)
                continue;

            _FreeRect * existing_rect = &maxrect->free_rects[i];

            // Check horizontal merge
            if (new_rect->y == existing_rect->y && new_rect->h == existing_rect->h) 
            {

                if (new_rect->x + new_rect->w == existing_rect->x) 
                {
                    // New rectangle is to the left
                    new_rect->w += existing_rect->w;
                    maxrect->free_rects[i] = maxrect->free_rects[maxrect->free_rect_count - 1];
                    maxrect->free_rect_count--;
                    merged = qo_true;
                    break;
                }
                else if (existing_rect->x + existing_rect->w == new_rect->x) 
                {
                    // New rectangle is to the right
                    new_rect->x = existing_rect->x;
                    new_rect->w += existing_rect->w;
                    // Remove merged rectangle
                    maxrect->free_rects[i] = maxrect->free_rects[maxrect->free_rect_count - 1];
                    maxrect->free_rect_count--;
                    merged = qo_true;
                    break;
                }
            }

            // Check vertical merge
            if (new_rect->x == existing_rect->x && new_rect->w == existing_rect->w)
            {
                if (new_rect->y + new_rect->h == existing_rect->y) 
                {
                    // New rectangle is below
                    new_rect->h += existing_rect->h;
                    maxrect->free_rects[i] = maxrect->free_rects[maxrect->free_rect_count - 1];
                    maxrect->free_rect_count--;
                    merged = qo_true;
                    break;
                }
                else if (existing_rect->y + existing_rect->h == new_rect->y)
                {
                    // New rectangle is above
                    new_rect->y = existing_rect->y;
                    new_rect->h += existing_rect->h;
                    // Remove merged rectangle
                    maxrect->free_rects[i] = maxrect->free_rects[maxrect->free_rect_count - 1];
                    maxrect->free_rect_count--;
                    merged = qo_true;
                    break;
                }
            }
        }

    }
    while (merged); // Try to merge until no more merges are possible
}

void
full_merge_free_rects(
    _Maxrect *  maxrect
) {
    if (maxrect->free_rect_count <= 1)
        return;

    qo_bool_t * deleted = mi_mallocn_tp(qo_bool_t, maxrect->free_rect_count);
    if (!deleted)
        return;

    // Check fully contained
    for (qo_int32_t i = 0; i < maxrect->free_rect_count; i++)
    {
        if (deleted[i])
            continue;

        for (qo_int32_t j = i + 1; j < maxrect->free_rect_count; j++)
        {
            if (deleted[j])
                continue;

            _FreeRect * a = &maxrect->free_rects[i];
            _FreeRect * b = &maxrect->free_rects[j];

            // Mergeable? （Fully contained）
            if (a->x <= b->x && a->y <= b->y && a->x + a->w >= b->x + b->w && a->y + a->h >= b->y + b->h)
            {
                // b is fullly contained in a
                deleted[j] = qo_true;
            }
            else if (b->x <= a->x && b->y <= a->y && b->x + b->w >= a->x + a->w && b->y + b->h >= a->y + a->h)
            {
                // a is fully contained in b
                deleted[i] = qo_true;
                break;
            }
        }
    }

    // Try to merge adjacent rectangles
    for (qo_int32_t i = 0; i < maxrect->free_rect_count; i++)
    {
        if (deleted[i])
            continue;

        for (qo_int32_t j = i + 1; j < maxrect->free_rect_count; j++)
        {
            if (deleted[j])
                continue;

            _FreeRect * a = &maxrect->free_rects[i];
            _FreeRect * b = &maxrect->free_rects[j];

            //  horizontally mergeable?
            if (a->y == b->y && a->h == b->h)
            {
                if (a->x + a->w == b->x)
                {
                    // a is to the left of b
                    a->w += b->w;
                    deleted[j] = qo_true;
                    j = 1; // Restart check because more rectangles may be mergeable
                }
                else if (b->x + b->w == a->x)
                {
                // b is to the left of a
                    b->w += a->w;
                    deleted[i] = qo_true;
                    j = 1; // Restart check because more rectangles may be mergeable
                }
            }
            // vertically mergeable?
            if (a->x == b->x && a->w == b->w)
            {
                if (a->y + a->h == b->y)
                {
                    // a is above b
                    a->h += b->h;
                    deleted[j] = qo_true;
                    j = 1; // Restart check because more rectangles may be mergeable
                }
                else if (b->y + b->h == a->y)
                {
                    // b is above a
                    a->y = b->y;
                    a->h += b->h;
                    deleted[i] = qo_true;
                    j = 1; // Restart check because more rectangles may be mergeable
                }
            }

            // L-shape mergeable?
            if (!deleted[j] && (
                (a->x + a->w == b->x && a->y <= b->y &&
                 a->y + a->h >= b->y) ||
                (b->x + b->w == a->x && b->y <= a->y &&
                 b->y + b->h >= a->y) ||
                (a->y + a->h == b->y && a->x <= b->x &&
                 a->x + a->w >= b->x) ||
                (b->y + b->h == a->y && b->x <= a->x &&
                 b->x + b->w >= a->x)))
            {
                qo_int32_t new_x = QO_MIN(a->x, b->x);
                qo_int32_t new_y = QO_MIN(a->y, b->y);
                qo_int32_t new_right = QO_MAX(a->x + a->w, b->x + b->w);
                qo_int32_t new_bottom = QO_MAX(a->y + a->h, b->y + b->h);

                qo_int32_t combined_area = (new_right - new_x) * (new_bottom - new_y);
                qo_int32_t original_area = a->w * a->h + b->w * b->h;

                if (combined_area <= original_area * 1.2)
                {
                    a->x = new_x;
                    a->y = new_y;
                    a->w = new_right - new_x;
                    a->h = new_bottom - new_y;
                    deleted[j] = qo_true;
                    j = 1; // Restart check because more rectangles may be mergeable
                }
            }
        }
    }

    qo_int32_t new_free_rect_count = 0;
    for (qo_int32_t i = 0; i < maxrect->free_rect_count; i++)
    {
        if (!deleted[i])
        {
            if (i != new_free_rect_count)
                maxrect->free_rects[new_free_rect_count] = maxrect->free_rects[i];
            new_free_rect_count++;
        }
    }

    maxrect->free_rect_count = new_free_rect_count;
    mi_free(deleted);
}

void
split_free_rect(
    _Maxrect *  maxrect ,
    qo_int32_t  index ,
    qo_uint32_t  x ,
    qo_uint32_t  y ,
    qo_uint32_t  w ,
    qo_uint32_t  h
) {
    _FreeRect original = maxrect->free_rects[index];

    maxrect->free_rects[index] = maxrect->free_rects[maxrect->free_rect_count - 1];
    maxrect->free_rect_count--;

    qo_int32_t  new_rect_indices[4] = { -1, -1, -1, -1 },
                new_rect_count = 0;

    // Split right
    if (x + w < original.x + original.w) {
        if (maxrect->free_rect_count >= maxrect->max_free_rect_count) {
            if (!expand_free_rects(maxrect))
                return;
        }

        _FreeRect new_rect = {
            .x = x + w,
            .y = original.y,
            .w = original.x + original.w - (x + w),
            .h = original.h
        };

        maxrect->free_rects[maxrect->free_rect_count] = new_rect;
        new_rect_indices[new_rect_count++] = maxrect->free_rect_count;
        maxrect->free_rect_count++;
    }

    // Split up
    if (y + h < original.y + original.h) {
        if (maxrect->free_rect_count >= maxrect->max_free_rect_count) {
            if (!expand_free_rects(maxrect))
                return;
        }

        _FreeRect new_rect = {
            .x = original.x,
            .y = y + h,
            .w = original.w,
            .h = original.y + original.h - (y + h)
        };

        maxrect->free_rects[maxrect->free_rect_count] = new_rect;
        new_rect_indices[new_rect_count++] = maxrect->free_rect_count;
        maxrect->free_rect_count++;
    }

    // Split left
    if (x > original.x) {
        if (maxrect->free_rect_count >= maxrect->max_free_rect_count) {
            if (!expand_free_rects(maxrect))
                return;
        }

        _FreeRect new_rect = {
            .x = original.x,
            .y = original.y,
            .w = x - original.x,
            .h = original.h
        };

        maxrect->free_rects[maxrect->free_rect_count] = new_rect;
        new_rect_indices[new_rect_count++] = maxrect->free_rect_count;
        maxrect->free_rect_count++;
    }

    // Split down
    if (y > original.y) {
        if (maxrect->free_rect_count >= maxrect->max_free_rect_count) {
            if (!expand_free_rects(maxrect))
                return;
        }

        _FreeRect new_rect = {
            .x = original.x,
            .y = original.y,
            .w = original.w,
            .h = y - original.y
        };

        maxrect->free_rects[maxrect->free_rect_count] = new_rect;
        new_rect_indices[new_rect_count++] = maxrect->free_rect_count;
        maxrect->free_rect_count++;
    }

    // Try to merge new-created rectangles
    for (qo_int32_t i = 0; i < new_rect_count; i++) 
    {
        incremental_merge_free_rects(maxrect, new_rect_indices[i]);
    }
}
PLACE_RECT_FN_DECL(SBA)
{
    qo_uint32_t best_y = INT_MAX;
    qo_uint32_t best_x = INT_MAX;
    qo_uint32_t best_index = -1;

    for (qo_int32_t i = 0; i < maxrect->free_rect_count; i++) 
    {
        _FreeRect *  free_rect = &maxrect->free_rects[i];

        // Check if the rectangle can fit
        if (free_rect->w >= rect->w && free_rect->h >= rect->h)
        {
            // Prioritize smallest y coordinate then smallest x coordinate
            if (free_rect->y < best_y || (free_rect->y == best_y && free_rect->x < best_x))
            {
                best_y = free_rect->y;
                best_x = free_rect->x;
                best_index = i;
            }
        }
    }

    if (best_index != -1)
    {
        rect->x = best_x;
        rect->y = best_y;
        split_free_rect(maxrect, best_index, best_x, best_y, rect->w, rect->h);
        full_merge_free_rects(_Maxrect *maxrect);
        return qo_true;
    }
    return qo_false;
}

PLACE_RECT_FN_DECL(BLS)
{
    qo_int32_t best_y = INT_MAX;
    qo_int32_t best_x = INT_MAX;
    qo_int32_t best_index = -1;

    for (qo_int32_t i = 0; i < maxrect->free_rect_count; i++)
    {
        _FreeRect *  free_rect = &maxrect->free_rects[i];

        if (free_rect->w >= rect->w && free_rect->h >= rect->h)
        {
            if (free_rect->y < best_y || (free_rect->y == best_y && free_rect->x < best_x))
            {
                best_y = free_rect->y;
                best_x = free_rect->x;
                best_index = i;
            }
        }
    }

    if (best_index == -1)
    {
        return qo_false;
    }

    // Try to find a better fit (left and top)
    for (qo_int32_t i = 0; i < maxrect->free_rect_count; i++)
    {
        _FreeRect *  free_rect = &maxrect->free_rects[i];

        if (free_rect->w >= rect->w && free_rect->h >= rect->h)
        {
            if (free_rect->y == best_y && free_rect->x < best_x)
            {
                best_x = free_rect->x;
                best_index = i;
            }
        }
    }

    rect->x = best_x;
    rect->y = best_y;
    split_free_rect(maxrect, best_index, best_x, best_y, rect->w, rect->h);
    full_merge_free_rects(maxrect);
    return qo_true;
}

PLACE_RECT_FN_DECL(BAF)
{
    qo_int32_t best_y = 0;
    qo_int32_t best_x = 0;
    qo_int32_t best_index = -1;
    qo_int32_t best_area = INT_MAX;

    for (qo_int32_t i = 0; i < maxrect->free_rect_count; i++)
    {
        _FreeRect *  free_rect = &maxrect->free_rects[i];

        if (free_rect->w >= rect->w && free_rect->h >= rect->h)
        {
            qo_int32_t area_fit = free_rect->w * free_rect->h - rect->w * rect->h;
            if (area_fit < best_area)
            {
                best_area = area_fit;
                best_y = free_rect->y;
                best_x = free_rect->x;
                best_index = i;
            }
        }
    }

    if (best_index == -1)
    {
        return qo_false;
    }

    rect->x = best_x;
    rect->y = best_y;
    split_free_rect(maxrect, best_index, best_x, best_y, rect->w, rect->h);
    full_merge_free_rects(maxrect);
    return qo_true;
}

qo_int32_t
calculate_future_potential(
    _Maxrect *      maxrect ,
    _FreeRect *     free_rect,
    qo_int32_t      rect_x ,
    qo_int32_t      rect_y ,
    qo_int32_t      rect_w ,
    qo_int32_t      rect_h
) {
    qo_int32_t remaining_top = free_rect->h - rect_h;
    qo_int32_t remaining_right = free_rect->w - rect_w;

    qo_int32_t score = 0;

    if (remaining_right > 0)
    {
        qo_fp32_t aspect_ratio = (qo_fp32_t)remaining_right / rect_h;
        if (aspect_ratio > .9 && aspect_ratio < 1.1)
        {
            score += 50;
        }
    }

    if (remaining_top > 0)
    {
        qo_fp32_t aspect_ratio = (qo_fp32_t)rect_w / remaining_top;
        if (aspect_ratio > .9 && aspect_ratio < 1.1)
        {
            score += 50;
        }
    }

    qo_int32_t right_area = remaining_right * rect_h;
    qo_int32_t top_area = rect_w * remaining_top;

    score += (right_area + top_area) / 100;

    if (remaining_right < 10 || remaining_top < 10)
    {
        score -= 30;
    }

    if (rect_x + rect_w == maxrect->width ||
        rect_y + rect_h == maxrect->height)
    {
        score += 40; 
    }

    qo_int32_t continuity_score = 0;

    for (qo_int32_t i = 0; i < maxrect->free_rect_count; i++)
    {
        if (&maxrect->free_rects[i] == free_rect)
        {
            continue;
        }

        if (abs(free_rect->x - maxrect->free_rects[i].x) < 5 ||
            abs(free_rect->y - maxrect->free_rects[i].y) < 5)
        {
            continuity_score += 20;
        }
    }

    return score + continuity_score;
}

PLACE_RECT_FN_DECL(HEURISTIC)
{
    qo_int32_t best_score = INT_MIN;
    qo_int32_t best_index = 0;
    qo_int32_t best_x = 0;
    qo_int32_t best_y = 0;
    qo_int32_t min_required = rect->w * rect->h;

    for (qo_int32_t i = 0; i < maxrect->free_rect_count; i++)
    {
        _FreeRect *  free_rect = &maxrect->free_rects[i];

        if (free_rect->w * free_rect->h < min_required)
        {
            continue;
        }

        if (free_rect->w >= rect->w && free_rect->h >= rect->h)
        {
            qo_int32_t score = 0;
            qo_int32_t contact_score = 0;

            if (free_rect->x == 0)
            {
                contact_score += rect->h;
            }
            if (free_rect->y == 0)
            {
                contact_score += rect->w;
            }

            qo_int32_t area_fit = free_rect->w * free_rect->h - rect->w * rect->h;
            qo_int32_t area_score = 100000 / (area_fit + 1);
            qo_int32_t position_score = 100 * (maxrect->height - free_rect->y) + (maxrect->width - free_rect->x);
            qo_int32_t waste_score = 0;
            if (free_rect->w == rect->w)
            {
                waste_score += 50;
            }
            if (free_rect->h == rect->h)
            {
                waste_score += 50;
            }

            qo_int32_t future_potential_score = calculate_future_potential(maxrect, free_rect, free_rect->x, free_rect->y, rect->w, rect->h);
            score = contact_score * 10 + area_score + position_score / 10 + waste_score + future_potential_score;
            if (score > best_score)
            {
                best_score = score;
                best_index = i;
                best_x = free_rect->x;
                best_y = free_rect->y;
            }
        }
    }

    if (best_index != -1)
    {
        rect->x = best_x;
        rect->y = best_y;
        split_free_rect(maxrect, best_index, rect->x, rect->_y, rect->w, rect->h);
        full_merge_free_rects(maxrect);
        return qo_true;
    }
    return qo_false;
}

void
normalize_strategy_weights(
    _PlacementHistory * history
) {
    qo_fp32_t total_weight = 0;
    for (qo_int32_t i = 0; i < STRATEGY_COUNT; i++)
    {
        total_weight += history->strategy_weights[i];
    }
    for (qo_int32_t i = 0; i < STRATEGY_COUNT; i++)
    {
        history->strategy_weights[i] /= total_weight;
    }
}

void
update_placement_history(
    _Maxrect * maxrect ,
    QORP_Rect * rect ,
    qo_bool_t placed ,
    QORP_RectPackStragy used_strategy
) {
    _PlacementHistory * history = maxrect->history;
    qo_int32_t container_area = maxrect->width * maxrect->height;
    qo_int32_t rect_area = rect->w * rect->h;
    qo_fp32_t  aspect_ratio = (qo_fp32_t)rect->w / rect->h;
    
    if (placed)
    {
        history->total_placed_rects++;
        history->consecutive_failures = 0;
        history->last_success_strategy = used_strategy;

        if (rect_area < container_area * .05)
        {
            history->small_rect_count++;
        }
        else if (rect_area > container_area * .2)
        {
            history->large_rect_count++;
        }

        history->avg_aspect_ratio = history->avg_aspect_ratio * .8 + aspect_ratio * .2;
        history->strategy_weights[used_strategy] *= 1.2;
    }
    else 
    {
        history->consecutive_failures++;
        history->strategy_weights[used_strategy] *= .8;
    }
    normalize_strategy_weights(history);

    qo_int32_t placed_area = 0;
    for (qo_int32_t i = 0; i < history->total_placed_rects; i++)
    {
        if (rect[i].placed)
        {
            placed_area += rect[i].w * rect[i].h;
        }
    }
}

place_rect_f
historically_select_placer(
    _Maxrect * maxrect ,
    QORP_Rect * rect
) {
    qo_int32_t rect_area = rect->w * rect->h;
    qo_fp32_t  rect_aspect = (qo_fp32_t)rect->w / rect->h;
    qo_int32_t container_area = maxrect->width * maxrect->height;

    _PlacementHistory * history = maxrect->history;

    if (history->fragment_count > 5)
        return PLACE_RECT_FN_NAME(BAF);

    if (history->consecutive_failures >= 3)
        return PLACE_RECT_FN_NAME(HEURISTIC);

    // Small rectangle
    if (rect_area < container_area * .05)
        return (history->small_rect_count > history->large_rect_count) ?
               PLACE_RECT_FN_NAME(BLS) : PLACE_RECT_FN_NAME(HEURISTIC);

    // Large rectangle
    if (rect_area > container_area * .2)
        return (history->fill_rate < .5) ? PLACE_RECT_FN_NAME(SBA) : PLACE_RECT_FN_NAME(BAF);

    // Height/width ratio differs significantly from average
    if (fabsf(rect_aspect - history->avg_aspect_ratio) > 1.)
        return PLACE_RECT_FN_NAME(HEURISTIC);

    // According to historical success rate
    // TODO:
}

PLACE_RECT_FN_DECL(HISTORICAL_HEURISTIC)
{

}

// 定义一个函数，用于在历史启发式算法中放置矩形
// PLACE_RECT_FN_DECL(HISTORICAL_HEURISTIC)
// {
//     // 初始化最佳位置的y坐标
//     qo_int32_t best_y = 0;
//     qo_int32_t best_x = 0;
//     qo_int32_t best_index = -1;
//     qo_int32_t best_score = INT_MIN;
//     qo_int32_t min_required = rect->w * rect->h;

//     for (qo_int32_t i = 0; i < maxrect->free_rect_count; i++)
//     {
//         _FreeRect *  free_rect = &maxrect->free_rects[i];

//         if (free_rect->w * free_rect->h < min_required)
//         {
//             continue;
//         }

//         if (free_rect->w >= rect->w && free_rect->h >= rect->h)
//         {
//             qo_int32_t score = 0;
//             qo_int32_t contact_score = 0;

//             if (free_rect->x == 0)
//             {
//                 contact_score +=rect->h;
//             }
//             else 
//             {
//                 for (qo_int32_t j = 0; j <rect_co)
//             }
//         }
//     }
// }

