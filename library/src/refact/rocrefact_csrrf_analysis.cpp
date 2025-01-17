/* **************************************************************************
 * Copyright (C) 2023-2024 Advanced Micro Devices, Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * *************************************************************************/

#include "rocrefact_csrrf_analysis.hpp"

#include "rocblas.hpp"
#include "rocsolver/rocsolver.h"

ROCSOLVER_BEGIN_NAMESPACE

template <typename T, typename U>
rocblas_status rocsolver_csrrf_analysis_impl(rocblas_handle handle,
                                             const rocblas_int n,
                                             const rocblas_int nrhs,
                                             const rocblas_int nnzM,
                                             rocblas_int* ptrM,
                                             rocblas_int* indM,
                                             U valM,
                                             const rocblas_int nnzT,
                                             rocblas_int* ptrT,
                                             rocblas_int* indT,
                                             U valT,
                                             rocblas_int* pivP,
                                             rocblas_int* pivQ,
                                             U B,
                                             const rocblas_int ldb,
                                             rocsolver_rfinfo rfinfo)
{
    ROCSOLVER_ENTER_TOP("csrrf_analysis", "-n", n, "--nnzM", nnzM, "--nnzT", nnzT, "--nrhs", nrhs,
                        "--ldb", ldb);

    if(handle == nullptr)
        return rocblas_status_invalid_handle;

    // argument checking
    rocblas_status st = rocsolver_csrrf_analysis_argCheck(
        handle, n, nrhs, nnzM, ptrM, indM, valM, nnzT, ptrT, indT, valT, pivP, pivQ, B, ldb, rfinfo);
    if(st != rocblas_status_continue)
        return st;

    // TODO: add batched versions
    // working with unshifted arrays
    // normal (non-batched non-strided) execution

    // memory workspace sizes:
    // size for temp buffer in analysis calls
    size_t size_work = 0;

    rocsolver_csrrf_analysis_getMemorySize<T>(n, nrhs, nnzT, ptrT, indT, valT, B, ldb, rfinfo,
                                              &size_work);

    if(rocblas_is_device_memory_size_query(handle))
        return rocblas_set_optimal_device_memory_size(handle, size_work);

    // memory workspace allocation
    void* work = nullptr;
    rocblas_device_malloc mem(handle, size_work);

    if(!mem)
        return rocblas_status_memory_error;

    work = mem[0];

    // execution
    return rocsolver_csrrf_analysis_template<T>(handle, n, nrhs, nnzM, ptrM, indM, valM, nnzT, ptrT,
                                                indT, valT, pivP, pivQ, B, ldb, rfinfo, work);
}

ROCSOLVER_END_NAMESPACE

/*
 * ===========================================================================
 *    C wrapper
 * ===========================================================================
 */

extern "C" {

rocblas_status rocsolver_scsrrf_analysis(rocblas_handle handle,
                                         const rocblas_int n,
                                         const rocblas_int nrhs,
                                         const rocblas_int nnzM,
                                         rocblas_int* ptrM,
                                         rocblas_int* indM,
                                         float* valM,
                                         const rocblas_int nnzT,
                                         rocblas_int* ptrT,
                                         rocblas_int* indT,
                                         float* valT,
                                         rocblas_int* pivP,
                                         rocblas_int* pivQ,
                                         float* B,
                                         const rocblas_int ldb,
                                         rocsolver_rfinfo rfinfo)
{
    return rocsolver::rocsolver_csrrf_analysis_impl<float>(
        handle, n, nrhs, nnzM, ptrM, indM, valM, nnzT, ptrT, indT, valT, pivP, pivQ, B, ldb, rfinfo);
}

rocblas_status rocsolver_dcsrrf_analysis(rocblas_handle handle,
                                         const rocblas_int n,
                                         const rocblas_int nrhs,
                                         const rocblas_int nnzM,
                                         rocblas_int* ptrM,
                                         rocblas_int* indM,
                                         double* valM,
                                         const rocblas_int nnzT,
                                         rocblas_int* ptrT,
                                         rocblas_int* indT,
                                         double* valT,
                                         rocblas_int* pivP,
                                         rocblas_int* pivQ,
                                         double* B,
                                         const rocblas_int ldb,
                                         rocsolver_rfinfo rfinfo)
{
    return rocsolver::rocsolver_csrrf_analysis_impl<double>(
        handle, n, nrhs, nnzM, ptrM, indM, valM, nnzT, ptrT, indT, valT, pivP, pivQ, B, ldb, rfinfo);
}

} // extern C
