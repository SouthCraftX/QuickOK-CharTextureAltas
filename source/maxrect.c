#include "maxrect.h"
#include "internal/gnuc_spec/types.h"
#include "types.h"
#include <limits.h>

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

    qo_bool_t * deleted = alloc_deleted_flags(maxrect, maxrect->free_rect_count);
    if (!deleted)
        return;

    for (qo_int32_t i = 0; i < maxrect->free_rect_count; i++)
    {
        if (deleted[i])
            continue;

        // Check fully contained
        for (qo_int32_t j = i + 1; j < maxrect->free_rect_count; j++)
        {
            if (deleted[j])
                continue;

            _FreeRect * a = &maxrect->free_rects[i];
            _FreeRect * b = &maxrect->free_rects[j];

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

            // Check horizontal merge
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
            // todo
        }
    }
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
PLACE_RECT_FN(SBA)
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
        
    }
}