#pragma once
#include "qozero.h"
#include "types.h"

struct _QORP_Rect
{
    qo_uint32_t  x;
    qo_uint32_t  y;
    qo_uint32_t  w;
    qo_uint32_t  h;
    qo_bool_t    placed;
};
typedef struct _QORP_Rect QORP_Rect;

struct _QORP_RectPackerView
{
    qo_uint32_t  width;
    qo_uint32_t  height;
    qo_uint32_t  placed_rects;
};
typedef struct _QORP_RectPackerView QORP_RectPackerView;  

typedef enum
{
    QORP_SORT_BY_HEIGHT ,
    QORP_SORT_BY_AREA ,
    QORP_SORT_BY_WIDTH ,
    QORP_SORT_BY_PERIMETER ,
    QORP_SORT_BY_MAX_DIMENSION ,
    QORP_SORT_BY_NONE
} QORP_RectSortMethod;

typedef enum
{
    QO_CHAR_ALTAS_SKYLINE ,
    QO_CHAR_ALTAS_MAX_RECT ,
    QO_CHAR_ALTAS_HYBRID_SHELF
} QORP_RectPackAlgorithm;

typedef enum 
{
    QORP_SBA ,
    QORP_BLS ,
    QORP_BAF ,
    QORP_HERUISTIC ,
    QORP_HISTRICAL_HEURISTIC
} QORP_RectPackStragy;

typedef enum
{
    QORP_PLACE_FAIL_THEN_STOP ,
    QORP_PLACE_FAIL_THEN_SKIP
} QORP_PlaceFailurePolicy;

struct _QORP_RectPacker;
typedef struct _QORP_RectPacker QORP_RectPacker;

QORP_RectPacker *
qorp_packer_new(
    qo_uint32_t width ,
    qo_uint32_t height ,
    QORP_RectPackAlgorithm algorithm ,
    qo_bool_t use_private_heap
);

QORP_RectPacker *
qorp_packer_probing_new(
    qo_uint32_t maximum_width   ,
    qo_uint32_t maximum_heigh    ,
    QORP_RectPackAlgorithm algorithm ,
    QORP_RectSortMethod sort_method ,
    QORP_Rect * rects ,
    qo_uint32_t count ,
    qo_bool_t   use_private_heap
) QO_NONNULL(5);

void
qorp_packer_ref(
    QORP_RectPacker *   packer
) QO_NONNULL(1);

void
qorp_packer_unref(
    QORP_RectPacker *   packer
);

void
qorp_sort_rects(
    QORP_Rect *         rects ,
    qo_uint32_t         count ,
    QORP_RectSortMethod method
) QO_NONNULL(1);

qo_uint32_t //< Number of placed rects
qorp_pack_rects(
    QORP_RectPacker *   packer ,
    QORP_Rect *         rects ,
    qo_uint32_t         count ,
    QORP_PlaceFailurePolicy failure_policy
) QO_NONNULL(1 , 2);

qo_bool_t
qorp_pack_rect(
    QORP_RectPacker *   packer ,
    QORP_Rect *         rect
) QO_NONNULL(1 , 2);

qo_fp32_t
qorp_calculate_utilization(
    QORP_RectPacker * packer
) QO_NONNULL(1);