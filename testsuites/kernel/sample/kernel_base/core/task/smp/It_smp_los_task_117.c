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

#include "It_los_task.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

static void HwiF01(void)
{
    UINT32 ret;

    TEST_HwiClear(HWI_NUM_TEST);

    ICUNIT_ASSERT_EQUAL_VOID(g_testCount, 1, g_testCount);

    LOS_AtomicInc(&g_testCount);

    ret = OS_TCB_FROM_TID(g_testTaskID01)->taskStatus;
    ICUNIT_ASSERT_EQUAL_VOID(ret, OS_TASK_STATUS_RUNNING, ret);

    ret = LOS_TaskDelete(g_testTaskID01);
    ICUNIT_ASSERT_EQUAL_VOID(ret, LOS_OK, ret);

    LOS_AtomicInc(&g_testCount);
}

static void TaskF01(void)
{
    LOS_AtomicInc(&g_testCount);

    while (1) {
    }
}

static UINT32 Testcase(void)
{
    UINT32 ret;
    TSK_INIT_PARAM_S task1 = { 0 };
    g_testCount = 0;

    ret = LOS_HwiCreate(HWI_NUM_TEST, 1, 0, HwiF01, 0);
    ICUNIT_ASSERT_EQUAL(ret, LOS_OK, ret);
    HalIrqSetAffinity(HWI_NUM_TEST, CPUID_TO_AFFI_MASK(ArchCurrCpuid())); // current cpu

    TEST_TASK_PARAM_INIT(task1, "it_smp_task_117", (TSK_ENTRY_FUNC)TaskF01, TASK_PRIO_TEST_TASK - 1);

    task1.usCpuAffiMask = CPUID_TO_AFFI_MASK((ArchCurrCpuid() + 1) % (LOSCFG_KERNEL_CORE_NUM));
    ret = LOS_TaskCreate(&g_testTaskID01, &task1);
    ICUNIT_ASSERT_EQUAL(ret, LOS_OK, ret);

    while (g_testCount == 0) {
    } // wait task01 to start

    TestBusyTaskDelay(2); // 2, set delay time

    ICUNIT_ASSERT_EQUAL(g_testCount, 1, g_testCount);

    ret = OS_TCB_FROM_TID(g_testTaskID01)->taskStatus;
    ICUNIT_ASSERT_NOT_EQUAL((ret & OS_TASK_STATUS_RUNNING), 0, ret);

    TestHwiTrigger(HWI_NUM_TEST);

    TEST_HwiDelete(HWI_NUM_TEST);

    TestBusyTaskDelay(10); // 10, set delay time

    ret = OS_TCB_FROM_TID(g_testTaskID01)->taskStatus;
    ICUNIT_ASSERT_NOT_EQUAL((ret & OS_TASK_STATUS_UNUSED), 0, ret);

    ICUNIT_ASSERT_EQUAL(g_testCount, 3, g_testCount); // 3, assert that g_testCount is equal to this.

    ret = LOS_TaskDelete(g_testTaskID01);
    ICUNIT_ASSERT_EQUAL(ret, LOS_ERRNO_TSK_NOT_CREATED, ret);

    return LOS_OK;

EXIT:
    ret = LOS_TaskDelete(g_testTaskID01);
    ICUNIT_ASSERT_EQUAL(ret, LOS_OK, ret);
    return LOS_OK;
}

void ItSmpLosTask117(void)
{
    TEST_ADD_CASE("ItSmpLosTask117", Testcase, TEST_LOS, TEST_TASK, TEST_LEVEL1, TEST_FUNCTION);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */
