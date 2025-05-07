#ifndef _HCOMMON
#define _HCOMMON

enum CodeError
{
    OK                  = 0x00,
    INVALID_ARGUMENT    = 0x01,
    INVALID_FORMAT      = 0x02,
    MEM_ALLOC_FAIL      = 0x03,
    FILE_NOT_OPEN       = 0x04,
    INVALID_OPERATION   = 0x05,
    INVALID_NODE_TYPE   = 0x06,
};

#endif //_HCOMMON
