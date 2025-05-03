#include "qozero.h"
#include "maxrect.h"

#include <math.h>
#include <stdlib.h>


// 插入排序 - 按面积
void
insertion_sort_by_area(
    QORP_Rect * rects ,
    qo_int32_t    left ,
    qo_int32_t    right
) {
    for (int i = left + 1; i <= right; i++)
    {
        QORP_Rect temp = rects[i];
        int         area = temp.width * temp.height;
        int         j = i - 1;

        while (j >= left && (rects[j].width * rects[j].height) < area)
        {
            rects[j + 1] = rects[j];
            j--;
        }
        rects[j + 1] = temp;
    }
}

// 堆化 - 按面积
void
heapify_by_area(
    QORP_Rect * rects ,
    qo_int32_t    n ,
    qo_int32_t    i
) {
    int largest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    if (left < n && (rects[left].width * rects[left].height) > (rects[largest].width * rects[largest].height))
    {
        largest = left;
    }

    if (right < n && (rects[right].width * rects[right].height) > (rects[largest].width * rects[largest].height))
    {
        largest = right;
    }

    if (largest != i)
    {
        QORP_Rect temp = rects[i];
        rects[i] = rects[largest];
        rects[largest] = temp;

        heapify_by_area(rects , n , largest); // Updated call
    }
}

// 堆排序 - 按面积
void
heap_sort_by_area(
    QORP_Rect * rects ,
    qo_int32_t    n
) {
    // 构建堆
    for (int i = n / 2 - 1; i >= 0; i--)
    {
        heapify_by_area(rects , n , i); // Updated call

    }
    // 逐个从堆中提取元素
    for (int i = n - 1; i > 0; i--)
    {
        QORP_Rect temp = rects[0];
        rects[0] = rects[i];
        rects[i] = temp;

        heapify_by_area(rects , i , 0); // Updated call
    }
}

// 分区函数 - 按面积
qo_int32_t
partition_by_area(
    QORP_Rect * rects ,
    qo_int32_t  low ,
    qo_int32_t  high
) {
    // 选择最右边的元素作为基准
    int pivot_area = rects[high].width * rects[high].height;
    int i = low - 1;

    for (int j = low; j <= high - 1; j++)
    {
        if ((rects[j].width * rects[j].height) >= pivot_area)
        {
            i++;
            QORP_Rect temp = rects[i];
            rects[i] = rects[j];
            rects[j] = temp;
        }
    }

    QORP_Rect temp = rects[i + 1];
    rects[i + 1] = rects[high];
    rects[high] = temp;

    return i + 1;
}

// Introsort实现 - 按面积
void
intro_sort_util_by_area(
    QORP_Rect * rects ,
    qo_int32_t  low ,
    qo_int32_t  high ,
    qo_int32_t  depth_limit
) {
    // 如果数组大小小于等于16，使用插入排序
    if (high - low <= 16)
    {
        insertion_sort_by_area(rects , low , high); // Updated call
        return;
    }

    // 如果递归深度超过限制，使用堆排序
    if (depth_limit == 0)
    {
        heap_sort_by_area(rects + low , high - low + 1); // Updated call
        return;
    }

    // 否则使用快速排序
    int pivot = partition_by_area(rects , low , high); // Updated call

    // 递归排序左右两部分
    if (pivot - 1 > low)
    {
        intro_sort_util_by_area(rects , low , pivot - 1 , depth_limit - 1); // Updated call
    }
    if (pivot + 1 < high) // Original logic used < high, keep it
    {
        intro_sort_util_by_area(rects , pivot + 1 , high , depth_limit - 1); // Updated call
    }
}

// 主Introsort函数 - 按面积
void
sort_rects_by_area(
    QORP_Rect * rects ,
    qo_int32_t   rect_count
) {
    if (rect_count <= 1)
    {
        return;
    }

    // 计算递归深度限制为2*log(n)
    int depth_limit = 2 * (int) log2(rect_count);

    // 调用Introsort工具函数
    intro_sort_util_by_area(rects , 0 , rect_count - 1 , depth_limit); // Updated call
}

// --- Height Sort Functions ---

// 插入排序 - 按高度
void
insertion_sort_by_height(
    QORP_Rect * rects ,
    qo_int32_t    left ,
    qo_int32_t    right
) {
    for (int i = left + 1; i <= right; i++)
    {
        QORP_Rect temp = rects[i];
        int         height = temp.height;
        int         j = i - 1;

        while (j >= left && rects[j].height < height)
        {
            rects[j + 1] = rects[j];
            j--;
        }
        rects[j + 1] = temp;
    }
}

// 堆化 - 按高度
void
heapify_by_height(
    QORP_Rect * rects ,
    qo_int32_t    n ,
    qo_int32_t    i
) {
    int largest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    if (left < n && rects[left].height > rects[largest].height)
    {
        largest = left;
    }

    if (right < n && rects[right].height > rects[largest].height)
    {
        largest = right;
    }

    if (largest != i)
    {
        QORP_Rect temp = rects[i];
        rects[i] = rects[largest];
        rects[largest] = temp;

        heapify_by_height(rects , n , largest); // Updated call
    }
}

// 堆排序 - 按高度
void
heap_sort_by_height(
    QORP_Rect * rects ,
    qo_int32_t    n
) {
    // 构建堆
    for (int i = n / 2 - 1; i >= 0; i--)
    {
        heapify_by_height(rects , n , i); // Updated call

    }
    // 逐个从堆中提取元素
    for (int i = n - 1; i > 0; i--)
    {
        QORP_Rect temp = rects[0];
        rects[0] = rects[i];
        rects[i] = temp;

        heapify_by_height(rects , i , 0); // Updated call
    }
}

// 分区函数 - 按高度
int
partition_by_height(
    QORP_Rect * rects ,
    int           low ,
    int           high
) {
    int pivot_height = rects[high].height;
    int i = low - 1;

    for (int j = low; j <= high - 1; j++)
    {
        if (rects[j].height >= pivot_height)
        {
            i++;
            QORP_Rect temp = rects[i];
            rects[i] = rects[j];
            rects[j] = temp;
        }
    }

    QORP_Rect temp = rects[i + 1];
    rects[i + 1] = rects[high];
    rects[high] = temp;

    return i + 1;
}

// Introsort实现 - 按高度
void
intro_sort_util_by_height(
    QORP_Rect * rects ,
    int           low ,
    int           high ,
    int           depth_limit
) {
    if (high - low <= 16)
    {
        insertion_sort_by_height(rects , low , high); // Updated call
        return;
    }

    if (depth_limit == 0)
    {
        heap_sort_by_height(rects + low , high - low + 1); // Updated call
        return;
    }

    int pivot = partition_by_height(rects , low , high); // Updated call

    if (pivot - 1 > low)
    {
        intro_sort_util_by_height(rects , low , pivot - 1 , depth_limit - 1); // Updated call
    }
    if (pivot + 1 < high) // Original logic used < high, keep it
    {
        intro_sort_util_by_height(rects , pivot + 1 , high , depth_limit - 1); // Updated call
    }
}

// 主Introsort函数 - 按高度
void
sort_rects_by_height(
    QORP_Rect * rects ,
    qo_int32_t   rect_count
) {
    if (rect_count <= 1)
    {
        return;
    }

    int depth_limit = 2 * (int) log2(rect_count);
    intro_sort_util_by_height(rects , 0 , rect_count - 1 , depth_limit); // Updated call
}

// --- Width Sort Functions ---

// 插入排序 - 按宽度
void
insertion_sort_by_width(
    QORP_Rect * rects ,
    qo_int32_t    left ,
    qo_int32_t    right
) {
    for (int i = left + 1; i <= right; i++)
    {
        QORP_Rect temp = rects[i];
        int         width = temp.width;
        int         j = i - 1;

        while (j >= left && rects[j].width < width)
        {
            rects[j + 1] = rects[j];
            j--;
        }
        rects[j + 1] = temp;
    }
}

// 堆化 - 按宽度
void
heapify_by_width(
    QORP_Rect * rects ,
    qo_int32_t    n ,
    qo_int32_t    i
) {
    int largest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    if (left < n && rects[left].width > rects[largest].width)
    {
        largest = left;
    }

    if (right < n && rects[right].width > rects[largest].width)
    {
        largest = right;
    }

    if (largest != i)
    {
        QORP_Rect temp = rects[i];
        rects[i] = rects[largest];
        rects[largest] = temp;

        heapify_by_width(rects , n , largest); // Updated call
    }
}

// 堆排序 - 按宽度
void
heap_sort_by_width(
    QORP_Rect * rects ,
    qo_int32_t    n
) {
    for (int i = n / 2 - 1; i >= 0; i--)
    {
        heapify_by_width(rects , n , i); // Updated call

    }
    for (int i = n - 1; i > 0; i--)
    {
        QORP_Rect temp = rects[0];
        rects[0] = rects[i];
        rects[i] = temp;

        heapify_by_width(rects , i , 0); // Updated call
    }
}

// 分区函数 - 按宽度
int
partition_by_width(
    QORP_Rect * rects ,
    int           low ,
    int           high
) {
    int pivotWidth = rects[high].width;
    int i = low - 1;

    for (int j = low; j <= high - 1; j++)
    {
        if (rects[j].width >= pivotWidth)
        {
            i++;
            QORP_Rect temp = rects[i];
            rects[i] = rects[j];
            rects[j] = temp;
        }
    }

    QORP_Rect temp = rects[i + 1];
    rects[i + 1] = rects[high];
    rects[high] = temp;

    return i + 1;
}

// Introsort实现 - 按宽度
void
intro_sort_util_by_width(
    QORP_Rect * rects ,
    int           low ,
    int           high ,
    int           depth_limit
) {
    if (high - low <= 16)
    {
        insertion_sort_by_width(rects , low , high); // Updated call
        return;
    }

    if (depth_limit == 0)
    {
        heap_sort_by_width(rects + low , high - low + 1); // Updated call
        return;
    }

    int pivot = partition_by_width(rects , low , high); // Updated call

    if (pivot - 1 > low)
    {
        intro_sort_util_by_width(rects , low , pivot - 1 , depth_limit - 1); // Updated call
    }
    if (pivot + 1 < high) // Original logic used < high, keep it
    {
        intro_sort_util_by_width(rects , pivot + 1 , high , depth_limit - 1); // Updated call
    }
}

// 主Introsort函数 - 按宽度
void
sort_rects_by_width(
    QORP_Rect * rects ,
    qo_int32_t   rect_count
) {
    if (rect_count <= 1)
    {
        return;
    }

    int depth_limit = 2 * (int) log2(rect_count);
    intro_sort_util_by_width(rects , 0 , rect_count - 1 , depth_limit); // Updated call
}

// --- Perimeter Sort Functions ---

// 插入排序 - 按周长
void
insertion_sort_by_perimeter(
    QORP_Rect * rects ,
    qo_int32_t    left ,
    qo_int32_t    right
) {
    for (int i = left + 1; i <= right; i++)
    {
        QORP_Rect temp = rects[i];
        int         perimeter = 2 * (temp.width + temp.height);
        int         j = i - 1;

        while (j >= left && 2 * (rects[j].width + rects[j].height) < perimeter)
        {
            rects[j + 1] = rects[j];
            j--;
        }
        rects[j + 1] = temp;
    }
}

// 堆化 - 按周长
void
heapify_by_perimeter(
    QORP_Rect * rects ,
    qo_int32_t    n ,
    qo_int32_t    i
) {
    int largest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    if (left < n && 2 * (rects[left].width + rects[left].height) > 2 * (rects[largest].width + rects[largest].height))
    {
        largest = left;
    }

    if (right < n && 2 * (rects[right].width + rects[right].height) > 2 * (rects[largest].width + rects[largest].height))
    {
        largest = right;
    }

    if (largest != i)
    {
        QORP_Rect temp = rects[i];
        rects[i] = rects[largest];
        rects[largest] = temp;

        heapify_by_perimeter(rects , n , largest); // Updated call
    }
}

// 堆排序 - 按周长
void
heap_sort_by_perimeter(
    QORP_Rect * rects ,
    qo_int32_t    n
) {
    for (int i = n / 2 - 1; i >= 0; i--)
    {
        heapify_by_perimeter(rects , n , i); // Updated call

    }
    for (int i = n - 1; i > 0; i--)
    {
        QORP_Rect temp = rects[0];
        rects[0] = rects[i];
        rects[i] = temp;

        heapify_by_perimeter(rects , i , 0); // Updated call
    }
}

// 分区函数 - 按周长
int
partition_by_perimeter(
    QORP_Rect * rects ,
    int           low ,
    int           high
) {
    int pivot_perimeter = 2 * (rects[high].width + rects[high].height);
    int i = low - 1;

    for (int j = low; j <= high - 1; j++)
    {
        if (2 * (rects[j].width + rects[j].height) >= pivot_perimeter)
        {
            i++;
            QORP_Rect temp = rects[i];
            rects[i] = rects[j];
            rects[j] = temp;
        }
    }

    QORP_Rect temp = rects[i + 1];
    rects[i + 1] = rects[high];
    rects[high] = temp;

    return i + 1;
}

// Introsort实现 - 按周长
void
intro_sort_util_by_perimeter(
    QORP_Rect * rects ,
    int           low ,
    int           high ,
    int           depth_limit
) {
    if (high - low <= 16)
    {
        insertion_sort_by_perimeter(rects , low , high); // Updated call
        return;
    }

    if (depth_limit == 0)
    {
        heap_sort_by_perimeter(rects + low , high - low + 1); // Updated call
        return;
    }

    int pivot = partition_by_perimeter(rects , low , high); // Updated call

    if (pivot - 1 > low)
    {
        intro_sort_util_by_perimeter(rects , low , pivot - 1 , depth_limit - 1); // Updated call
    }
    if (pivot + 1 < high) // Original logic used < high, keep it
    {
        intro_sort_util_by_perimeter(rects , pivot + 1 , high , depth_limit - 1); // Updated call
    }
}

// 主Introsort函数 - 按周长
void
sort_rects_by_perimeter(
    QORP_Rect * rects ,
    qo_int32_t   rect_count
) {
    if (rect_count <= 1)
    {
        return;
    }

    int depth_limit = 2 * (int) log2(rect_count);
    intro_sort_util_by_perimeter(rects , 0 , rect_count - 1 , depth_limit); // Updated call
}

// --- Hybrid Sort Functions ---

// 计算混合评分
int
calculate_hybrid_score(
    QORP_Rect * rect
) {
    int longSide = (rect->width > rect->height) ? rect->width : rect->height;
    int shortSide = (rect->width < rect->height) ? rect->width : rect->height;
    return (rect->width * rect->height) * 10 + longSide * 5 + shortSide;
}

// 插入排序 - 按混合评分
void
insertion_sort_by_hybrid(
    QORP_Rect * rects ,
    qo_int32_t    left ,
    qo_int32_t    right
) {
    for (int i = left + 1; i <= right; i++)
    {
        QORP_Rect temp = rects[i];
        int         score = calculate_hybrid_score(&temp); // Updated call
        int         j = i - 1;

        while (j >= left && calculate_hybrid_score(&rects[j]) < score)   // Updated call
        {
            rects[j + 1] = rects[j];
            j--;
        }
        rects[j + 1] = temp;
    }
}

// 堆化 - 按混合评分
void
heapify_by_hybrid(
    QORP_Rect * rects ,
    qo_int32_t    n ,
    qo_int32_t    i
) {
    int largest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    if (left < n && calculate_hybrid_score(&rects[left]) > calculate_hybrid_score(&rects[largest])) // Updated call
    {
        largest = left;
    }

    if (right < n && calculate_hybrid_score(&rects[right]) > calculate_hybrid_score(&rects[largest])) // Updated call
    {
        largest = right;
    }

    if (largest != i)
    {
        QORP_Rect temp = rects[i];
        rects[i] = rects[largest];
        rects[largest] = temp;

        heapify_by_hybrid(rects , n , largest); // Updated call
    }
}

// 堆排序 - 按混合评分
void
heap_sort_by_hybrid(
    QORP_Rect * rects ,
    qo_int32_t    n
) {
    for (int i = n / 2 - 1; i >= 0; i--)
    {
        heapify_by_hybrid(rects , n , i); // Updated call

    }
    for (int i = n - 1; i > 0; i--)
    {
        QORP_Rect temp = rects[0];
        rects[0] = rects[i];
        rects[i] = temp;

        heapify_by_hybrid(rects , i , 0); // Updated call
    }
}

// 分区函数 - 按混合评分
int
partition_by_hybrid(
    QORP_Rect * rects ,
    int           low ,
    int           high
) {
    int pivotScore = calculate_hybrid_score(&rects[high]); // Updated call
    int i = low - 1;

    for (int j = low; j <= high - 1; j++)
    {
        if (calculate_hybrid_score(&rects[j]) >= pivotScore)   // Updated call
        {
            i++;
            QORP_Rect temp = rects[i];
            rects[i] = rects[j];
            rects[j] = temp;
        }
    }

    QORP_Rect temp = rects[i + 1];
    rects[i + 1] = rects[high];
    rects[high] = temp;

    return i + 1;
}

// Introsort实现 - 按混合评分
void
intro_sort_util_by_hybrid(
    QORP_Rect * rects ,
    int           low ,
    int           high ,
    int           depth_limit
) {
    if (high - low <= 16)
    {
        insertion_sort_by_hybrid(rects , low , high); // Updated call
        return;
    }

    if (depth_limit == 0)
    {
        heap_sort_by_hybrid(rects + low , high - low + 1); // Updated call
        return;
    }

    int pivot = partition_by_hybrid(rects , low , high); // Updated call

    if (pivot - 1 > low)
    {
        intro_sort_util_by_hybrid(rects , low , pivot - 1 , depth_limit - 1); // Updated call
    }
    if (pivot + 1 < high) // Original logic used < high, keep it
    {
        intro_sort_util_by_hybrid(rects , pivot + 1 , high , depth_limit - 1); // Updated call
    }
}

// 主Introsort函数 - 按混合评分
void
sort_rects_by_hybrid(
    QORP_Rect * rects ,
    qo_int32_t   rect_count
) {
    if (rect_count <= 1)
    {
        return;
    }

    int depth_limit = 2 * (int) log2(rect_count);
    intro_sort_util_by_hybrid(rects , 0 , rect_count - 1 , depth_limit); // Updated call
}


// --- Dynamic Sort Function ---

// 添加基于历史数据的动态排序
void
dynamic_sort_with_history(
    QORP_Rect *       rects ,
    int                 start_index ,
    int                 rect_count ,
    _Maxrect *         container ,
    _PlacementHistory * history
) {

    if (start_index >= rect_count - 1) // Original comparison
    {
        return;
    }

    // 预先计算剩余空间特征 - 只计算一次
    int   total_width = 0;
    int   total_height = 0;
    int   max_width = 0;
    int   max_height = 0;
    float avg_aspect_ratio = 0;

    for (int i = 0; i < container->free_rect_count; i++)
    {
        _qo_free_rect * free_rect = &container->free_rects[i];
        total_width += free_rect->width;
        total_height += free_rect->height;

        if (free_rect->width > max_width)
        {
            max_width = free_rect->width;
        }
        if (free_rect->height > max_height)
        {
            max_height = free_rect->height;
        }

        // avoid division by zero if height is 0
        float aspect_ratio = (free_rect->height != 0) ? (float) free_rect->width / free_rect->height : 0;
        avg_aspect_ratio += aspect_ratio;
    }

    // 预先计算平均宽高和长宽比 - 避免在循环中重复计算
    float avg_width = (container->free_rect_count > 0) ? (float) total_width / container->free_rect_count : 0;
    float avg_height = (container->free_rect_count > 0) ? (float) total_height / container->free_rect_count : 0;
    avg_aspect_ratio = (container->free_rect_count > 0) ? avg_aspect_ratio / container->free_rect_count : 1.0f;

    // 根据剩余空间特征为每个未放置的矩形计算适应度分数
    // original logic: size based on (rect_count - start_index)
    int * scores = (int *) malloc((rect_count - start_index) * sizeof(int));
    if (!scores)
    {
        return; // original had no error handling, keeping it simple

    }
    for (int i = start_index; i < rect_count; i++)
    {
        qo_char_rect * rect = &rects[i];
        if (rect->placed) // original check
        {
            continue; // original had no score assignment here, just continue

        }
        int   score = 0;
        // avoid division by zero if height is 0
        float rect_aspect = (rect->height != 0) ? (float) rect->width / rect->height : 0;

        // 1. 尺寸匹配度 - 矩形尺寸与平均空闲空间尺寸的匹配程度
        // avoid division by zero if avg_width/avg_height is 0
        float width_fit = (avg_width != 0) ? 1.0f - fabsf(rect->width - avg_width) / avg_width : 0;
        float height_fit = (avg_height != 0) ? 1.0f - fabsf(rect->height - avg_height) / avg_height : 0;
        score += (int) ((width_fit + height_fit) * 50);

        // 2. 长宽比匹配度 - 矩形长宽比与平均空闲空间长宽比的匹配程度
        // original had + 0.1f to avoid division by zero
        float aspect_fit = 1.0f - fabsf(rect_aspect - avg_aspect_ratio) / (avg_aspect_ratio + 0.1f);
        score += (int) (aspect_fit * 30);

        // 3. 最大空间利用度 - 矩形是否能充分利用最大空闲空间
        // avoid division by zero if max_width or max_height is 0
        if (rect->width <= max_width && rect->height <= max_height && max_width > 0 && max_height > 0)
        {
            float utilization_rate = (float) (rect->width * rect->height) / (max_width * max_height);
            score += (int) (utilization_rate * 100);
        }
        // original logic didn't have an else condition here

        // 4. 面积因素 - 更大的矩形优先
        score += rect->width * rect->height / 100;

        // 5. 历史数据调整 - 根据历史放置情况调整分数
        // check if history pointer is valid before dereferencing
        if (history != null)
        {
            if (history->small_rect_count > history->large_rect_count * 2)
            {
                // 如果小矩形过多，优先放置大矩形
                score += rect->width * rect->height / 50;
            }

            if (history->fragment_count > 5)
            {
                // 如果碎片过多，优先放置能减少碎片的矩形
                if (rect_aspect > 0.9f && rect_aspect < 1.1f)
                {
                    // 接近正方形的矩形更容易减少碎片
                    score += 100;
                }
            }

            if (history->fill_rate > 0.7f)
            {
                // 如果填充率已经很高，优先放置小矩形填补空隙
                score -= rect->width * rect->height / 200;
            }
        }

        // original logic for index calculation
        scores[i - start_index] = score;
    }

    // 根据分数对未放置的矩形进行排序 (original used bubble sort logic)
    // the range is from start_index to rect_count - 1
    for (int i = start_index; i < rect_count - 1; i++)
    {
        // the inner loop upper bound decreases with i
        // original logic: j < rect_count - 1 - (i - start_index)
        for (int j = start_index; j < rect_count - 1 - (i - start_index); j++)
        {
            // original indices for comparison and swap
            if (scores[j - start_index] < scores[j + 1 - start_index])
            {
                // 交换矩形 (using original indices)
                qo_char_rect temp_rect = rects[j];
                rects[j] = rects[j + 1];
                rects[j + 1] = temp_rect;

                // 交换分数 (using original indices)
                int temp_score = scores[j - start_index];
                scores[j - start_index] = scores[j + 1 - start_index];
                scores[j + 1 - start_index] = temp_score;
            }
        }
    }

    free(scores); // original cleanup
}