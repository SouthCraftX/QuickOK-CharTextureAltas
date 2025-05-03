#pragma once
#include "internal/gnuc_spec/types.h"
#define __QO_MAXRECT_PRIV_H__

#include "../include/rectpack.h"
#include <mimalloc.h>

struct __FreeRect
{
    qo_int32_t  x;
    qo_int32_t  y;
    qo_int32_t  w;
    qo_int32_t  h;
};
typedef struct __FreeRect _FreeRect;

struct __PlacementHistory
{
    qo_uint32_t  total_placed_rects;     // 已放置矩形总数
    qo_uint32_t  small_rect_count;       // 小矩形数量 (面积 < 容器面积的5%)
    qo_uint32_t  large_rect_count;       // 大矩形数量 (面积 > 容器面积的20%)
    qo_uint32_t  fragment_count;         // 碎片数量
    qo_uint32_t  consecutive_failures;   // 连续放置失败次数
    qo_uint32_t  last_success_strategy;  // 上次成功使用的策略
    qo_fp32_t    fill_rate;              // 当前填充率
    qo_fp32_t    avg_aspect_ratio;       // 平均长宽比
    qo_fp32_t    strategy_weights[4];    // 各策略权重 [SBA, BLS, BAF, HEURISTIC]
};
typedef struct __PlacementHistory _PlacementHistory;

struct __Maxrect
{
    qo_uint32_t  width;
    qo_uint32_t  height;
    qo_uint32_t  placed_rect_count;

    _FreeRect *  free_rects;
    qo_uint32_t  free_rect_count;
    qo_uint32_t  max_free_rect_count;

    mi_heap_t *  heap;

    _PlacementHistory * history; //< NULL if not used
};
typedef struct __Maxrect _Maxrect;

QORP_Rect *
alloc_rects(
    _Maxrect *  maxrect ,
    qo_uint32_t count
) {
    return mi_heap_mallocn_tp(maxrect->heap , QORP_Rect , count);
}

_FreeRect *
alloc_free_rects(
    _Maxrect *  maxrect ,
    qo_uint32_t count
) {
    return mi_heap_mallocn_tp(maxrect->heap , _FreeRect , count);
}

QORP_Rect *
alloc_deleted_flags(
    _Maxrect *  maxrect ,
    qo_uint32_t count
) {
    return mi_heap_mallocn_tp(maxrect->heap , qo_bool_t , count);
}

_FreeRect *
realloc_free_rects(
    _Maxrect *  maxrect ,
    qo_uint32_t new_count
) {
    return mi_heap_reallocn_tp(maxrect->heap , maxrect->free_rects , _FreeRect , new_count);
}

typedef 
    void 
    (* sort_rects_f)(
        QORP_Rect *  rects ,
        qo_uint32_t  count
    );

typedef 
    qo_bool_t
    (* place_rect_f)(
        _Maxrect *  maxrect ,
        QORP_Rect *  rect
    );

typedef
    qo_uint32_t
    (* place_rects_f)(
        _Maxrect *  maxrect ,
        QORP_Rect *  rects ,
        qo_uint32_t  count
    );

#define PLACE_RECT_FN_NAME(policy) \
    place_rect_##policy

#define PLACE_RECT_FN_DECL(policy) \
    qo_bool_t \
    place_rect_##policy( \
        _Maxrect *  maxrect , \
        QORP_Rect *  rect  \
    )

#define PLACE_RECTS_FN(policy) \
    qo_uint32_t \
    place_rects_##policy( \
        _Maxrect *  maxrect , \
        QORP_Rect *  rects , \
        qo_uint32_t  count \
    )

// Sorting
void
sort_rects_by_area(
    QORP_Rect * rects ,
    qo_int32_t  count
) QO_NONNULL(1);

void
sort_rects_by_height(
    QORP_Rect * rects ,
    qo_int32_t  count
) QO_NONNULL(1);

void
sort_rects_by_width(
    QORP_Rect * rects ,
    qo_int32_t  count   
) QO_NONNULL(1);

void
sort_rects_by_perimeter(
    QORP_Rect * rects ,
    qo_int32_t  count
) QO_NONNULL(1);

void
sort_rects_by_hybrid(
    QORP_Rect * rects ,
    qo_int32_t  count
) QO_NONNULL(1);
