/*
 * Copyright (c) 2013-2019 Huawei Technologies Co., Ltd. All rights reserved.
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "It_los_queue.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */


static VOID TaskF01(VOID)
{
    UINT32 ret;
    CHAR buff1[QUEUE_SHORT_BUFFER_LENTH] = "UniDSP";

    LOS_AtomicInc(&g_testCount);

    ret = LOS_QueueCreate("Q1", 3, &g_testQueueID01, 0, sizeof(UINTPTR)); // 3, Set the queue length.
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);

    ret = LOS_QueueWrite(g_testQueueID01, &buff1, 8, 0); // 8, Write the setting size of queue buffer.
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);

    LOS_AtomicInc(&g_testCount);

EXIT:
    LOS_TaskDelete(g_testTaskID01);
}

static VOID HwiF01(VOID)
{
    UINT32 ret;
    CHAR buff2[QUEUE_SHORT_BUFFER_LENTH] = "";

    TEST_HwiClear(HWI_NUM_TEST);

    LOS_AtomicInc(&g_testCount);

    ret = LOS_QueueRead(g_testQueueID01, &buff2, 8, 0); // 8, Read the setting size of queue buffer.
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);

    LOS_AtomicInc(&g_testCount);

EXIT:
    LOS_QueueDelete(g_testQueueID01);
}

static UINT32 Testcase(VOID)
{
    UINT32 ret;

    TSK_INIT_PARAM_S task1 = { 0 };
    task1.pfnTaskEntry = (TSK_ENTRY_FUNC)TaskF01;
    task1.pcName = "TskName";
    task1.uwStackSize = TASK_STACK_SIZE_TEST;
    task1.usTaskPrio = 23; // 23, Set the priority according to the task purpose,a smaller number means a higher priority.

    g_testCount = 0;

    ret = LOS_TaskCreate(&g_testTaskID01, &task1);
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);

    TestExtraTaskDelay(TEST_TASKDELAY_2TICK);
    ICUNIT_GOTO_EQUAL(g_testCount, 2, g_testCount, EXIT); // 2, Here, assert that g_testCount is equal to 2.
    LOS_AtomicInc(&g_testCount);

    ret = TEST_HwiCreate(HWI_NUM_TEST, 1, 0, HwiF01, 0);
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);
#if (LOSCFG_KERNEL_SMP == YES)
    HalIrqSetAffinity(HWI_NUM_TEST, CPUID_TO_AFFI_MASK(ArchCurrCpuid()));
#endif

    TestHwiTrigger(HWI_NUM_TEST);
    TestExtraTaskDelay(TEST_TASKDELAY_2TICK);
    ICUNIT_GOTO_EQUAL(g_testCount, 5, g_testCount, EXIT); // 5, Here, assert that g_testCount is equal to 5.

    TEST_HwiDelete(HWI_NUM_TEST);

    return LOS_OK;

EXIT:
    LOS_TaskDelete(g_testTaskID01);
    TEST_HwiDelete(HWI_NUM_TEST);
    return LOS_OK;
}

VOID ItLosQueue047(VOID)
{
    TEST_ADD_CASE("ItLosQueue047", Testcase, TEST_LOS, TEST_QUE, TEST_LEVEL2, TEST_FUNCTION);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */
