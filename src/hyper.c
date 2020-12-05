#include "hyper.h"

HYPERSTATUS
HyperMemAlloc(
    void                **src,
    size_t              size
)
{
    if (src == NULL)
        return HYPER_BAD_PARAMETER;

    void *data = NULL;

#ifdef _WIN32
    data = HeapAlloc(GetProcessHeap(), 0, size);
#else
    data = malloc(size);
#endif

    if (data == NULL)
        return HYPER_FAILED;

    *src = data;

    return HYPER_SUCCESS;
}

HYPERSTATUS
HyperMemFree(
    void                *src
)
{
    HYPERSTATUS iResult = HYPER_SUCCESS;

#ifdef _WIN32
    iResult = HeapFree(GetProcessHeap(), 0, src);
#else
    free(src);
#endif

    return iResult;
}

HYPERSTATUS
HyperMemRealloc(
    void                **src,
    size_t              size
)
{
    if (src == NULL)
        return HYPER_BAD_PARAMETER;

#ifdef _WIN32
    *src = HeapReAlloc(GetProcessHeap(), 0, *src, size);
#else
    *src = realloc(*src, size);
#endif

    if (*src == NULL)
        return HYPER_FAILED;

    return HYPER_SUCCESS;
}
