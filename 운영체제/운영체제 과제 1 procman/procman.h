#include "stdio.h"
#include "string.h"

//액션에 해당하는 값들을 미리 const로 초기화합니다. 이들을 입력된 문자열과 비교합니다.
const char ACTION_ONCE[10] = "once";
const char ACTION_WAIT[10] = "wait";
const char ACTION_RESPAWN[10] = "respawn" 