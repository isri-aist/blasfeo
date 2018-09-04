/**************************************************************************************************
*                                                                                                 *
* This file is part of BLASFEO.                                                                   *
*                                                                                                 *
* BLASFEO -- BLAS For Embedded Optimization.                                                      *
* Copyright (C) 2016-2018 by Gianluca Frison.                                                     *
* Developed at IMTEK (University of Freiburg) under the supervision of Moritz Diehl.              *
* All rights reserved.                                                                            *
*                                                                                                 *
* This program is free software: you can redistribute it and/or modify                            *
* it under the terms of the GNU General Public License as published by                            *
* the Free Software Foundation, either version 3 of the License, or                               *
* (at your option) any later version                                                              *.
*                                                                                                 *
* This program is distributed in the hope that it will be useful,                                 *
* but WITHOUT ANY WARRANTY; without even the implied warranty of                                  *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                                   *
* GNU General Public License for more details.                                                    *
*                                                                                                 *
* You should have received a copy of the GNU General Public License                               *
* along with this program.  If not, see <https://www.gnu.org/licenses/>.                          *
*                                                                                                 *
* The authors designate this particular file as subject to the "Classpath" exception              *
* as provided by the authors in the LICENSE file that accompained this code.                      *
*                                                                                                 *
* Author: Gianluca Frison, gianluca.frison (at) imtek.uni-freiburg.de                             *
*                                                                                                 *
**************************************************************************************************/

#include <stdlib.h>
#include <stdio.h>

#include "../include/blasfeo_common.h"
#include "../include/blasfeo_d_kernel.h"
#include "../include/blasfeo_d_aux.h"



/****************************
* old interface
****************************/

void dgemm_nt_lib(int m, int n, int k, double alpha, double *pA, int sda, double *pB, int sdb, double beta, double *pC, int sdc, double *pD, int sdd)
	{

	if(m<=0 || n<=0)
		return;

	const int ps = 4;

	int i, j, l;

	i = 0;

#if defined(TARGET_X64_INTEL_HASWELL)
	for(; i<m-11; i+=12)
		{
		j = 0;
		for(; j<n-3; j+=4)
			{
			kernel_dgemm_nt_12x4_lib4(k, &alpha, &pA[i*sda], sda, &pB[j*sdb], &beta, &pC[j*ps+i*sdc], sdc, &pD[j*ps+i*sdd], sdd);
			}
		if(j<n)
			{
			kernel_dgemm_nt_12x4_vs_lib4(k, &alpha, &pA[i*sda], sda, &pB[j*sdb], &beta, &pC[j*ps+i*sdc], sdc, &pD[j*ps+i*sdd], sdd, m-i, n-j);
			}
		}
	if(m>i)
		{
		if(m-i<=4)
			{
			goto left_4;
			}
		else if(m-i<=8)
			{
			goto left_8;
			}
		else
			{
			goto left_12;
			}
		}
#elif defined(TARGET_X64_INTEL_SANDY_BRIDGE)
	for(; i<m-7; i+=8)
		{
		j = 0;
		for(; j<n-3; j+=4)
			{
			kernel_dgemm_nt_8x4_lib4(k, &alpha, &pA[i*sda], sda, &pB[j*sdb], &beta, &pC[j*ps+i*sdc], sdc, &pD[j*ps+i*sdd], sdd);
			}
		if(j<n)
			{
			kernel_dgemm_nt_8x4_vs_lib4(k, &alpha, &pA[i*sda], sda, &pB[j*sdb], &beta, &pC[j*ps+i*sdc], sdc, &pD[j*ps+i*sdd], sdd, m-i, n-j);
			}
		}
	if(m>i)
		{
		if(m-i<=4)
			{
			goto left_4;
			}
		else
			{
			goto left_8;
			}
		}
#elif defined(TARGET_ARMV8A_ARM_CORTEX_A53)
	for(; i<m-11; i+=12)
		{
		j = 0;
		for(; j<n-3; j+=4)
			{
			kernel_dgemm_nt_12x4_lib4(k, &alpha, &pA[i*sda], sda, &pB[j*sdb], &beta, &pC[j*ps+i*sdc], sdc, &pD[j*ps+i*sdd], sdd);
			}
		if(j<n)
			{
			kernel_dgemm_nt_4x4_vs_lib4(k, &alpha, &pA[i*sda], &pB[j*sdb], &beta, &pC[j*ps+i*sdc], &pD[j*ps+i*sdd], m-i, n-j);
			kernel_dgemm_nt_4x4_vs_lib4(k, &alpha, &pA[(i+4)*sda], &pB[j*sdb], &beta, &pC[j*ps+(i+4)*sdc], &pD[j*ps+(i+4)*sdd], m-(i+4), n-j);
			kernel_dgemm_nt_4x4_vs_lib4(k, &alpha, &pA[(i+8)*sda], &pB[j*sdb], &beta, &pC[j*ps+(i+8)*sdc], &pD[j*ps+(i+8)*sdd], m-(i+8), n-j);
			}
		}
	if(m>i)
		{
		if(m-i<=4)
			{
			goto left_4;
			}
		else if(m-i<=8)
			{
			goto left_8;
			}
		else
			{
			goto left_12;
			}
		}
#elif defined(TARGET_ARMV8A_ARM_CORTEX_A57)
	for(; i<m-7; i+=8)
		{
		j = 0;
		for(; j<n-3; j+=4)
			{
			kernel_dgemm_nt_8x4_lib4(k, &alpha, &pA[i*sda], sda, &pB[j*sdb], &beta, &pC[j*ps+i*sdc], sdc, &pD[j*ps+i*sdd], sdd);
			}
		if(j<n)
			{
//			kernel_dgemm_nt_4x4_vs_lib4(k, &alpha, &pA[i*sda], &pB[j*sdb], &beta, &pC[j*ps+i*sdc], &pD[j*ps+i*sdd], m-i, n-j);
//			kernel_dgemm_nt_4x4_vs_lib4(k, &alpha, &pA[(i+4)*sda], &pB[j*sdb], &beta, &pC[j*ps+(i+4)*sdc], &pD[j*ps+(i+4)*sdd], m-(i+4), n-j);
			kernel_dgemm_nt_8x4_vs_lib4(k, &alpha, &pA[i*sda], sda, &pB[j*sdb], &beta, &pC[j*ps+i*sdc], sdc, &pD[j*ps+i*sdd], sdd, m-i, n-j);
			}
		}
	if(m>i)
		{
		if(m-i<=4)
			{
			goto left_4;
			}
		else
			{
			goto left_8;
			}
		}
#elif defined(TARGET_X86_AMD_BARCELONA)
	for(; i<m-3; i+=4)
		{
		j = 0;
		for(; j<n-3; j+=4)
			{
			kernel_dgemm_nt_4x2_lib4(k, &alpha, &pA[i*sda], &pB[j*sdb+0], &beta, &pC[(j+0)*ps+i*sdc], &pD[(j+0)*ps+i*sdd]);
			kernel_dgemm_nt_4x2_lib4(k, &alpha, &pA[i*sda], &pB[j*sdb+2], &beta, &pC[(j+2)*ps+i*sdc], &pD[(j+2)*ps+i*sdd]);
			}
		if(j<n-2)
			{
			kernel_dgemm_nt_4x2_lib4(k, &alpha, &pA[i*sda], &pB[j*sdb+0], &beta, &pC[(j+0)*ps+i*sdc], &pD[(j+0)*ps+i*sdd]);
			kernel_dgemm_nt_4x2_vs_lib4(k, &alpha, &pA[i*sda], &pB[j*sdb+2], &beta, &pC[(j+2)*ps+i*sdc], &pD[(j+2)*ps+i*sdd], m-i, n-j-2);
			}
		else if(j<n)
			{
			kernel_dgemm_nt_4x2_vs_lib4(k, &alpha, &pA[i*sda], &pB[j*sdb], &beta, &pC[j*ps+i*sdc], &pD[j*ps+i*sdd], m-i, n-j);
			}
		}
	if(m>i)
		{
		goto left_4;
		}
#else
	for(; i<m-3; i+=4)
		{
		j = 0;
		for(; j<n-3; j+=4)
			{
			kernel_dgemm_nt_4x4_lib4(k, &alpha, &pA[i*sda], &pB[j*sdb], &beta, &pC[j*ps+i*sdc], &pD[j*ps+i*sdd]);
			}
		if(j<n)
			{
			kernel_dgemm_nt_4x4_vs_lib4(k, &alpha, &pA[i*sda], &pB[j*sdb], &beta, &pC[j*ps+i*sdc], &pD[j*ps+i*sdd], m-i, n-j);
			}
		}
	if(m>i)
		{
		goto left_4;
		}
#endif

	// common return if i==m
	return;

	// clean up loops definitions

#if defined(TARGET_X64_INTEL_HASWELL)
	left_12:
	j = 0;
	for(; j<n; j+=4)
		{
		kernel_dgemm_nt_12x4_vs_lib4(k, &alpha, &pA[i*sda], sda, &pB[j*sdb], &beta, &pC[j*ps+i*sdc], sdc, &pD[j*ps+i*sdd], sdd, m-i, n-j);
		}
	return;
#endif
#if defined(TARGET_ARMV8A_ARM_CORTEX_A53)
	left_12:
	j = 0;
	for(; j<n; j+=4)
		{
		kernel_dgemm_nt_4x4_vs_lib4(k, &alpha, &pA[i*sda], &pB[j*sdb], &beta, &pC[j*ps+i*sdc], &pD[j*ps+i*sdd], m-i, n-j);
		kernel_dgemm_nt_4x4_vs_lib4(k, &alpha, &pA[(i+4)*sda], &pB[j*sdb], &beta, &pC[j*ps+(i+4)*sdc], &pD[j*ps+(i+4)*sdd], m-(i+4), n-j);
		kernel_dgemm_nt_4x4_vs_lib4(k, &alpha, &pA[(i+8)*sda], &pB[j*sdb], &beta, &pC[j*ps+(i+8)*sdc], &pD[j*ps+(i+8)*sdd], m-(i+8), n-j);
		}
	return;
#endif

#if defined(TARGET_X64_INTEL_HASWELL)
	left_8:
	j = 0;
	for(; j<n-8; j+=12)
		{
		kernel_dgemm_nt_8x8l_vs_lib4(k, &alpha, &pA[i*sda], sda, &pB[j*sdb], sdb, &beta, &pC[j*ps+i*sdc], sdc, &pD[j*ps+i*sdd], sdd, m-i, n-j);
		kernel_dgemm_nt_8x8u_vs_lib4(k, &alpha, &pA[i*sda], sda, &pB[(j+4)*sdb], sdb, &beta, &pC[(j+4)*ps+i*sdc], sdc, &pD[(j+4)*ps+i*sdd], sdd, m-i, n-(j+4));
		}

	if(j<n-4)
		{
		kernel_dgemm_nt_8x8l_vs_lib4(k, &alpha, &pA[i*sda], sda, &pB[j*sdb], sdb, &beta, &pC[j*ps+i*sdc], sdc, &pD[j*ps+i*sdd], sdd, m-i, n-j);
		kernel_dgemm_nt_4x4_vs_lib4(k, &alpha, &pA[i*sda], &pB[(j+4)*sdb], &beta, &pC[(j+4)*ps+i*sdc], &pD[(j+4)*ps+i*sdd], m-i, n-(j+4));
		}
	else if(j<n)
		{
		kernel_dgemm_nt_8x4_vs_lib4(k, &alpha, &pA[i*sda], sda, &pB[j*sdb], &beta, &pC[j*ps+i*sdc], sdc, &pD[j*ps+i*sdd], sdd, m-i, n-j);
		}
	return;
#endif
#if defined(TARGET_X64_INTEL_SANDY_BRIDGE)
	left_8:
	j = 0;
	for(; j<n; j+=4)
		{
		kernel_dgemm_nt_8x4_vs_lib4(k, &alpha, &pA[i*sda], sda, &pB[j*sdb], &beta, &pC[j*ps+i*sdc], sdc, &pD[j*ps+i*sdd], sdd, m-i, n-j);
		}
	return;
#endif
#if defined(TARGET_ARMV8A_ARM_CORTEX_A57) || defined(TARGET_ARMV8A_ARM_CORTEX_A53)
	left_8:
	j = 0;
	for(; j<n; j+=4)
		{
//		kernel_dgemm_nt_4x4_vs_lib4(k, &alpha, &pA[i*sda], &pB[j*sdb], &beta, &pC[j*ps+i*sdc], &pD[j*ps+i*sdd], m-i, n-j);
//		kernel_dgemm_nt_4x4_vs_lib4(k, &alpha, &pA[(i+4)*sda], &pB[j*sdb], &beta, &pC[j*ps+(i+4)*sdc], &pD[j*ps+(i+4)*sdd], m-(i+4), n-j);
		kernel_dgemm_nt_8x4_vs_lib4(k, &alpha, &pA[i*sda], sda, &pB[j*sdb], &beta, &pC[j*ps+i*sdc], sdc, &pD[j*ps+i*sdd], sdd, m-i, n-j);
		}
	return;
#endif

#if defined(TARGET_X64_INTEL_HASWELL)
	left_4:
	j = 0;
	for(; j<n-8; j+=12)
		{
		kernel_dgemm_nt_4x12_vs_lib4(k, &alpha, &pA[i*sda], &pB[j*sdb], sdb, &beta, &pC[j*ps+i*sdc], &pD[j*ps+i*sdd], m-i, n-j);
		}
	if(j<n-4)
		{
		kernel_dgemm_nt_4x8_vs_lib4(k, &alpha, &pA[i*sda], &pB[j*sdb], sdb, &beta, &pC[j*ps+i*sdc], &pD[j*ps+i*sdd], m-i, n-j);
		}
	else if(j<n)
		{
		kernel_dgemm_nt_4x4_vs_lib4(k, &alpha, &pA[i*sda], &pB[j*sdb], &beta, &pC[j*ps+i*sdc], &pD[j*ps+i*sdd], m-i, n-j);
		}
	return;
#elif defined(TARGET_X64_INTEL_SANDY_BRIDGE)
	left_4:
	j = 0;
	for(; j<n-4; j+=8)
		{
		kernel_dgemm_nt_4x8_vs_lib4(k, &alpha, &pA[i*sda], &pB[j*sdb], sdb, &beta, &pC[j*ps+i*sdc], &pD[j*ps+i*sdd], m-i, n-j);
		}
	if(j<n)
		{
		kernel_dgemm_nt_4x4_vs_lib4(k, &alpha, &pA[i*sda], &pB[j*sdb], &beta, &pC[j*ps+i*sdc], &pD[j*ps+i*sdd], m-i, n-j);
		}
	return;
#elif defined(TARGET_X86_AMD_BARCELONA)
	left_4:
	j = 0;
	for(; j<n-2; j+=4)
		{
		kernel_dgemm_nt_4x2_vs_lib4(k, &alpha, &pA[i*sda], &pB[j*sdb+0], &beta, &pC[(j+0)*ps+i*sdc], &pD[(j+0)*ps+i*sdd], m-i, n-j-0);
		kernel_dgemm_nt_4x2_vs_lib4(k, &alpha, &pA[i*sda], &pB[j*sdb+2], &beta, &pC[(j+2)*ps+i*sdc], &pD[(j+2)*ps+i*sdd], m-i, n-j-2);
		}
	if(j<n)
		{
		kernel_dgemm_nt_4x2_vs_lib4(k, &alpha, &pA[i*sda], &pB[j*sdb], &beta, &pC[j*ps+i*sdc], &pD[j*ps+i*sdd], m-i, n-j);
		}
	return;
#else
	left_4:
	j = 0;
	for(; j<n; j+=4)
		{
		kernel_dgemm_nt_4x4_vs_lib4(k, &alpha, &pA[i*sda], &pB[j*sdb], &beta, &pC[j*ps+i*sdc], &pD[j*ps+i*sdd], m-i, n-j);
		}
	return;
#endif

	}



// D <= B * A^{-T} , with A lower triangular with unit diagonal
void dtrsm_nt_rl_one_lib(int m, int n, double *pA, int sda, double *pB, int sdb, double *pD, int sdd)
	{

	if(m<=0 || n<=0)
		return;

	const int ps = 4;

	int i, j;

	i = 0;

#if defined(TARGET_X64_INTEL_HASWELL)
	for(; i<m-11; i+=12)
		{
		j = 0;
		for(; j<n-3; j+=4)
			{
			kernel_dtrsm_nt_rl_one_12x4_lib4(j, &pD[i*sdd], sdd, &pA[j*sda], &pB[j*ps+i*sdb], sdb, &pD[j*ps+i*sdd], sdd, &pA[j*ps+j*sda]);
			}
		if(j<n)
			{
			kernel_dtrsm_nt_rl_one_12x4_vs_lib4(j, &pD[i*sdd], sdd, &pA[j*sda], &pB[j*ps+i*sdb], sdb, &pD[j*ps+i*sdd], sdd, &pA[j*ps+j*sda], m-i, n-j);
			}
		}
	if(m>i)
		{
		if(m-i<=4)
			{
			goto left_4;
			}
		else if(m-i<=8)
			{
			goto left_8;
			}
		else
			{
			goto left_12;
			}
		}
#elif defined(TARGET_X64_INTEL_SANDY_BRIDGE)
	for(; i<m-7; i+=8)
		{
		j = 0;
		for(; j<n-3; j+=4)
			{
			kernel_dtrsm_nt_rl_one_8x4_lib4(j, &pD[i*sdd], sdd, &pA[j*sda], &pB[j*ps+i*sdb], sdb, &pD[j*ps+i*sdd], sdd, &pA[j*ps+j*sda]);
			}
		if(j<n)
			{
			kernel_dtrsm_nt_rl_one_8x4_vs_lib4(j, &pD[i*sdd], sdd, &pA[j*sda], &pB[j*ps+i*sdb], sdb, &pD[j*ps+i*sdd], sdd, &pA[j*ps+j*sda], m-i, n-j);
			}
		}
	if(m>i)
		{
		if(m-i<=4)
			{
			goto left_4;
			}
		else
			{
			goto left_8;
			}
		}
#else
	for(; i<m-3; i+=4)
		{
		j = 0;
		for(; j<n-3; j+=4)
			{
			kernel_dtrsm_nt_rl_one_4x4_lib4(j, &pD[i*sdd], &pA[j*sda], &pB[j*ps+i*sdb], &pD[j*ps+i*sdd], &pA[j*ps+j*sda]);
			}
		if(j<n)
			{
			kernel_dtrsm_nt_rl_one_4x4_vs_lib4(j, &pD[i*sdd], &pA[j*sda], &pB[j*ps+i*sdb], &pD[j*ps+i*sdd], &pA[j*ps+j*sda], m-i, n-j);
			}
		}
	if(m>i)
		{
		goto left_4;
		}
#endif

	// common return if i==m
	return;

#if defined(TARGET_X64_INTEL_HASWELL)
	left_12:
	j = 0;
	for(; j<n; j+=4)
		{
		kernel_dtrsm_nt_rl_one_12x4_vs_lib4(j, &pD[i*sdd], sdd, &pA[j*sda], &pB[j*ps+i*sdb], sdb, &pD[j*ps+i*sdd], sdd, &pA[j*ps+j*sda], m-i, n-j);
		}
	return;
#endif

#if defined(TARGET_X64_INTEL_SANDY_BRIDGE) || defined(TARGET_X64_INTEL_HASWELL)
	left_8:
	j = 0;
	for(; j<n; j+=4)
		{
		kernel_dtrsm_nt_rl_one_8x4_vs_lib4(j, &pD[i*sdd], sdd, &pA[j*sda], &pB[j*ps+i*sdb], sdb, &pD[j*ps+i*sdd], sdd, &pA[j*ps+j*sda], m-i, n-j);
		}
	return;
#endif

	left_4:
	j = 0;
	for(; j<n; j+=4)
		{
		kernel_dtrsm_nt_rl_one_4x4_vs_lib4(j, &pD[i*sdd], &pA[j*sda], &pB[j*ps+i*sdb], &pD[j*ps+i*sdd], &pA[j*ps+j*sda], m-i, n-j);
		}
	return;

	}



// D <= B * A^{-T} , with A upper triangular employing explicit inverse of diagonal
void dtrsm_nt_ru_inv_lib(int m, int n, double *pA, int sda, double *inv_diag_A, double *pB, int sdb, double *pD, int sdd)
	{

	if(m<=0 || n<=0)
		return;

	const int ps = 4;

	int i, j, idx;

	int rn = n%4;

	double *dummy;

	i = 0;

#if defined(TARGET_X64_INTEL_HASWELL)
	for(; i<m-11; i+=12)
		{
		j = 0;
		// clean at the end
		if(rn>0)
			{
			idx = n-rn;
			kernel_dtrsm_nt_ru_inv_12x4_vs_lib4(0, dummy, 0, dummy, &pB[i*sdb+idx*ps], sdb, &pD[i*sdd+idx*ps], sdd, &pA[idx*sda+idx*ps], &inv_diag_A[idx], m-i, rn);
			j += rn;
			}
		for(; j<n; j+=4)
			{
			idx = n-j-4;
			kernel_dtrsm_nt_ru_inv_12x4_lib4(j, &pD[i*sdd+(idx+4)*ps], sdd, &pA[idx*sda+(idx+4)*ps], &pB[i*sdb+idx*ps], sdb, &pD[i*sdd+idx*ps], sdd, &pA[idx*sda+idx*ps], &inv_diag_A[idx]);
			}
		}
	if(m>i)
		{
		if(m-i<=4)
			{
			goto left_4;
			}
		else if(m-i<=8)
			{
			goto left_8;
			}
		else
			{
			goto left_12;
			}
		}
#elif defined(TARGET_X64_INTEL_SANDY_BRIDGE)
	for(; i<m-7; i+=8)
		{
		j = 0;
		// clean at the end
		if(rn>0)
			{
			idx = n-rn;
			kernel_dtrsm_nt_ru_inv_8x4_vs_lib4(0, dummy, 0, dummy, &pB[i*sdb+idx*ps], sdb, &pD[i*sdd+idx*ps], sdd, &pA[idx*sda+idx*ps], &inv_diag_A[idx], m-i, rn);
			j += rn;
			}
		for(; j<n; j+=4)
			{
			idx = n-j-4;
			kernel_dtrsm_nt_ru_inv_8x4_lib4(j, &pD[i*sdd+(idx+4)*ps], sdd, &pA[idx*sda+(idx+4)*ps], &pB[i*sdb+idx*ps], sdb, &pD[i*sdd+idx*ps], sdd, &pA[idx*sda+idx*ps], &inv_diag_A[idx]);
			}
		}
	if(m>i)
		{
		if(m-i<=4)
			{
			goto left_4;
			}
		else
			{
			goto left_8;
			}
		}
#else
	for(; i<m-3; i+=4)
		{
		j = 0;
		// clean at the end
		if(rn>0)
			{
			idx = n-rn;
			kernel_dtrsm_nt_ru_inv_4x4_vs_lib4(0, dummy, dummy, &pB[i*sdb+idx*ps], &pD[i*sdd+idx*ps], &pA[idx*sda+idx*ps], &inv_diag_A[idx], m-i, rn);
			j += rn;
			}
		for(; j<n; j+=4)
			{
			idx = n-j-4;
			kernel_dtrsm_nt_ru_inv_4x4_lib4(j, &pD[i*sdd+(idx+4)*ps], &pA[idx*sda+(idx+4)*ps], &pB[i*sdb+idx*ps], &pD[i*sdd+idx*ps], &pA[idx*sda+idx*ps], &inv_diag_A[idx]);
			}
		}
	if(m>i)
		{
		goto left_4;
		}
#endif

	// common return if i==m
	return;

#if defined(TARGET_X64_INTEL_HASWELL)
	left_12:
	j = 0;
	// TODO
	// clean at the end
	if(rn>0)
		{
		idx = n-rn;
		kernel_dtrsm_nt_ru_inv_12x4_vs_lib4(0, dummy, 0, dummy, &pB[i*sdb+idx*ps], sdb, &pD[i*sdd+idx*ps], sdd, &pA[idx*sda+idx*ps], &inv_diag_A[idx], m-i, rn);
		j += rn;
		}
	for(; j<n; j+=4)
		{
		idx = n-j-4;
		kernel_dtrsm_nt_ru_inv_12x4_vs_lib4(j, &pD[i*sdd+(idx+4)*ps], sdd, &pA[idx*sda+(idx+4)*ps], &pB[i*sdb+idx*ps], sdb, &pD[i*sdd+idx*ps], sdd, &pA[idx*sda+idx*ps], &inv_diag_A[idx], m-i, 4);
		}
	return;

#endif

#if defined(TARGET_X64_INTEL_SANDY_BRIDGE) || defined(TARGET_X64_INTEL_HASWELL)
	left_8:
	j = 0;
	// TODO
	// clean at the end
	if(rn>0)
		{
		idx = n-rn;
		kernel_dtrsm_nt_ru_inv_8x4_vs_lib4(0, dummy, 0, dummy, &pB[i*sdb+idx*ps], sdb, &pD[i*sdd+idx*ps], sdd, &pA[idx*sda+idx*ps], &inv_diag_A[idx], m-i, rn);
		j += rn;
		}
	for(; j<n; j+=4)
		{
		idx = n-j-4;
		kernel_dtrsm_nt_ru_inv_8x4_vs_lib4(j, &pD[i*sdd+(idx+4)*ps], sdd, &pA[idx*sda+(idx+4)*ps], &pB[i*sdb+idx*ps], sdb, &pD[i*sdd+idx*ps], sdd, &pA[idx*sda+idx*ps], &inv_diag_A[idx], m-i, 4);
		}
	return;

#endif

	left_4:
	j = 0;
	// TODO
	// clean at the end
	if(rn>0)
		{
		idx = n-rn;
		kernel_dtrsm_nt_ru_inv_4x4_vs_lib4(0, dummy, dummy, &pB[i*sdb+idx*ps], &pD[i*sdd+idx*ps], &pA[idx*sda+idx*ps], &inv_diag_A[idx], m-i, rn);
		j += rn;
		}
	for(; j<n; j+=4)
		{
		idx = n-j-4;
		kernel_dtrsm_nt_ru_inv_4x4_vs_lib4(j, &pD[i*sdd+(idx+4)*ps], &pA[idx*sda+(idx+4)*ps], &pB[i*sdb+idx*ps], &pD[i*sdd+idx*ps], &pA[idx*sda+idx*ps], &inv_diag_A[idx], m-i, 4);
		}
	return;

	}



// D <= A^{-1} * B , with A lower triangular with unit diagonal
void dtrsm_nn_ll_one_lib(int m, int n, double *pA, int sda, double *pB, int sdb, double *pD, int sdd)
	{

	if(m<=0 || n<=0)
		return;

	const int ps = 4;

	int i, j;

	i = 0;

#if defined(TARGET_X64_INTEL_HASWELL)
	for( ; i<m-11; i+=12)
		{
		j = 0;
		for( ; j<n-3; j+=4)
			{
			kernel_dtrsm_nn_ll_one_12x4_lib4(i, pA+i*sda, sda, pD+j*ps, sdd, pB+i*sdb+j*ps, sdb, pD+i*sdd+j*ps, sdd, pA+i*sda+i*ps, sda);
			}
		if(j<n)
			{
			kernel_dtrsm_nn_ll_one_12x4_vs_lib4(i, pA+i*sda, sda, pD+j*ps, sdd, pB+i*sdb+j*ps, sdb, pD+i*sdd+j*ps, sdd, pA+i*sda+i*ps, sda, m-i, n-j);
			}
		}
	if(i<m)
		{
		if(m-i<=4)
			goto left_4;
		if(m-i<=8)
			goto left_8;
		else
			goto left_12;
		}
#elif defined(TARGET_X64_INTEL_SANDY_BRIDGE)
	for( ; i<m-7; i+=8)
		{
		j = 0;
		for( ; j<n-3; j+=4)
			{
			kernel_dtrsm_nn_ll_one_8x4_lib4(i, pA+i*sda, sda, pD+j*ps, sdd, pB+i*sdb+j*ps, sdb, pD+i*sdd+j*ps, sdd, pA+i*sda+i*ps, sda);
			}
		if(j<n)
			{
			kernel_dtrsm_nn_ll_one_8x4_vs_lib4(i, pA+i*sda, sda, pD+j*ps, sdd, pB+i*sdb+j*ps, sdb, pD+i*sdd+j*ps, sdd, pA+i*sda+i*ps, sda, m-i, n-j);
			}
		}
	if(i<m)
		{
		if(m-i<=4)
			goto left_4;
		else
			goto left_8;
		}
#else
	for( ; i<m-3; i+=4)
		{
		j = 0;
		for( ; j<n-3; j+=4)
			{
			kernel_dtrsm_nn_ll_one_4x4_lib4(i, pA+i*sda, pD+j*ps, sdd, pB+i*sdb+j*ps, pD+i*sdd+j*ps, pA+i*sda+i*ps);
			}
		if(j<n)
			{
			kernel_dtrsm_nn_ll_one_4x4_vs_lib4(i, pA+i*sda, pD+j*ps, sdd, pB+i*sdb+j*ps, pD+i*sdd+j*ps, pA+i*sda+i*ps, m-i, n-j);
			}
		}
	if(i<m)
		{
		goto left_4;
		}
#endif

	// common return
	return;

#if defined(TARGET_X64_INTEL_HASWELL)
	left_12:
	j = 0;
	for( ; j<n; j+=4)
		{
		kernel_dtrsm_nn_ll_one_12x4_vs_lib4(i, pA+i*sda, sda, pD+j*ps, sdd, pB+i*sdb+j*ps, sdb, pD+i*sdd+j*ps, sdd, pA+i*sda+i*ps, sda, m-i, n-j);
		}
	return;
#endif

#if defined(TARGET_X64_INTEL_SANDY_BRIDGE) || defined(TARGET_X64_INTEL_HASWELL)
	left_8:
	j = 0;
	for( ; j<n; j+=4)
		{
		kernel_dtrsm_nn_ll_one_8x4_vs_lib4(i, pA+i*sda, sda, pD+j*ps, sdd, pB+i*sdb+j*ps, sdb, pD+i*sdd+j*ps, sdd, pA+i*sda+i*ps, sda, m-i, n-j);
		}
	return;
#endif

	left_4:
	j = 0;
	for( ; j<n; j+=4)
		{
		kernel_dtrsm_nn_ll_one_4x4_vs_lib4(i, pA+i*sda, pD+j*ps, sdd, pB+i*sdb+j*ps, pD+i*sdd+j*ps, pA+i*sda+i*ps, m-i, n-j);
		}
	return;

	}



// D <= A^{-1} * B , with A upper triangular employing explicit inverse of diagonal
void dtrsm_nn_lu_inv_lib(int m, int n, double *pA, int sda, double *inv_diag_A, double *pB, int sdb, double *pD, int sdd)
	{

	if(m<=0 || n<=0)
		return;

	const int ps = 4;

	int i, j, idx;
	double *dummy;

	i = 0;
	int rm = m%4;
	if(rm>0)
		{
		// TODO code expliticly the final case
		idx = m-rm; // position of the part to do
		j = 0;
		for( ; j<n; j+=4)
			{
			kernel_dtrsm_nn_lu_inv_4x4_vs_lib4(0, dummy, dummy, 0, pB+idx*sdb+j*ps, pD+idx*sdd+j*ps, pA+idx*sda+idx*ps, inv_diag_A+idx, rm, n-j);
			}
		// TODO
		i += rm;
		}
//	int em = m-rm;
#if defined(TARGET_X64_INTEL_HASWELL)
	for( ; i<m-8; i+=12)
		{
		idx = m-i; // position of already done part
		j = 0;
		for( ; j<n-3; j+=4)
			{
			kernel_dtrsm_nn_lu_inv_12x4_lib4(i, pA+(idx-12)*sda+idx*ps, sda, pD+idx*sdd+j*ps, sdd, pB+(idx-12)*sdb+j*ps, sdb, pD+(idx-12)*sdd+j*ps, sdd, pA+(idx-12)*sda+(idx-12)*ps, sda, inv_diag_A+(idx-12));
			}
		if(j<n)
			{
			kernel_dtrsm_nn_lu_inv_12x4_vs_lib4(i, pA+(idx-12)*sda+idx*ps, sda, pD+idx*sdd+j*ps, sdd, pB+(idx-12)*sdb+j*ps, sdb, pD+(idx-12)*sdd+j*ps, sdd, pA+(idx-12)*sda+(idx-12)*ps, sda, inv_diag_A+(idx-12), 12, n-j);
//			kernel_dtrsm_nn_lu_inv_4x4_vs_lib4(i, pA+(idx-4)*sda+idx*ps, pD+idx*sdd+j*ps, sdd, pB+(idx-4)*sdb+j*ps, pD+(idx-4)*sdd+j*ps, pA+(idx-4)*sda+(idx-4)*ps, inv_diag_A+(idx-4), 4, n-j);
//			kernel_dtrsm_nn_lu_inv_4x4_vs_lib4(i+4, pA+(idx-8)*sda+(idx-4)*ps, pD+(idx-4)*sdd+j*ps, sdd, pB+(idx-8)*sdb+j*ps, pD+(idx-8)*sdd+j*ps, pA+(idx-8)*sda+(idx-8)*ps, inv_diag_A+(idx-8), 4, n-j);
//			kernel_dtrsm_nn_lu_inv_4x4_vs_lib4(i+8, pA+(idx-12)*sda+(idx-8)*ps, pD+(idx-8)*sdd+j*ps, sdd, pB+(idx-12)*sdb+j*ps, pD+(idx-12)*sdd+j*ps, pA+(idx-12)*sda+(idx-12)*ps, inv_diag_A+(idx-12), 4, n-j);
			}
		}
#endif
#if defined(TARGET_X64_INTEL_SANDY_BRIDGE) || defined(TARGET_X64_INTEL_HASWELL)
	for( ; i<m-4; i+=8)
		{
		idx = m-i; // position of already done part
		j = 0;
		for( ; j<n-3; j+=4)
			{
			kernel_dtrsm_nn_lu_inv_8x4_lib4(i, pA+(idx-8)*sda+idx*ps, sda, pD+idx*sdd+j*ps, sdd, pB+(idx-8)*sdb+j*ps, sdb, pD+(idx-8)*sdd+j*ps, sdd, pA+(idx-8)*sda+(idx-8)*ps, sda, inv_diag_A+(idx-8));
			}
		if(j<n)
			{
			kernel_dtrsm_nn_lu_inv_8x4_vs_lib4(i, pA+(idx-8)*sda+idx*ps, sda, pD+idx*sdd+j*ps, sdd, pB+(idx-8)*sdb+j*ps, sdb, pD+(idx-8)*sdd+j*ps, sdd, pA+(idx-8)*sda+(idx-8)*ps, sda, inv_diag_A+(idx-8), 8, n-j);
//			kernel_dtrsm_nn_lu_inv_4x4_vs_lib4(i, pA+(idx-4)*sda+idx*ps, pD+idx*sdd+j*ps, sdd, pB+(idx-4)*sdb+j*ps, pD+(idx-4)*sdd+j*ps, pA+(idx-4)*sda+(idx-4)*ps, inv_diag_A+(idx-4), 4, n-j);
//			kernel_dtrsm_nn_lu_inv_4x4_vs_lib4(i+4, pA+(idx-8)*sda+(idx-4)*ps, pD+(idx-4)*sdd+j*ps, sdd, pB+(idx-8)*sdb+j*ps, pD+(idx-8)*sdd+j*ps, pA+(idx-8)*sda+(idx-8)*ps, inv_diag_A+(idx-8), 4, n-j);
			}
		}
#endif
	for( ; i<m; i+=4)
		{
		idx = m-i; // position of already done part
		j = 0;
		for( ; j<n-3; j+=4)
			{
			kernel_dtrsm_nn_lu_inv_4x4_lib4(i, pA+(idx-4)*sda+idx*ps, pD+idx*sdd+j*ps, sdd, pB+(idx-4)*sdb+j*ps, pD+(idx-4)*sdd+j*ps, pA+(idx-4)*sda+(idx-4)*ps, inv_diag_A+(idx-4));
			}
		if(j<n)
			{
			kernel_dtrsm_nn_lu_inv_4x4_vs_lib4(i, pA+(idx-4)*sda+idx*ps, pD+idx*sdd+j*ps, sdd, pB+(idx-4)*sdb+j*ps, pD+(idx-4)*sdd+j*ps, pA+(idx-4)*sda+(idx-4)*ps, inv_diag_A+(idx-4), 4, n-j);
			}
		}

	// common return
	return;

	}



#if 0
void dlauum_blk_nt_l_lib(int m, int n, int nv, int *rv, int *cv, double *pA, int sda, double *pB, int sdb, int alg, double *pC, int sdc, double *pD, int sdd)
	{

	if(m<=0 || n<=0)
		return;

	// TODO remove
	double alpha, beta;
	if(alg==0)
		{
		alpha = 1.0;
		beta = 0.0;
		}
	else if(alg==1)
		{
		alpha = 1.0;
		beta = 1.0;
		}
	else
		{
		alpha = -1.0;
		beta = 1.0;
		}

	// TODO remove
	int k = cv[nv-1];

	const int ps = 4;

	int i, j, l;
	int ii, iii, jj, kii, kiii, kjj, k0, k1;

	i = 0;
	ii = 0;
	iii = 0;

#if defined(TARGET_X64_INTEL_SANDY_BRIDGE) || defined(TARGET_X64_INTEL_HASWELL)
	for(; i<m-7; i+=8)
		{

		while(ii<nv && rv[ii]<i+8)
			ii++;
		if(ii<nv)
			kii = cv[ii];
		else
			kii = cv[ii-1];

		j = 0;
		jj = 0;
		for(; j<i && j<n-3; j+=4)
			{

			while(jj<nv && rv[jj]<j+4)
				jj++;
			if(jj<nv)
				kjj = cv[jj];
			else
				kjj = cv[jj-1];
			k0 = kii<kjj ? kii : kjj;

			kernel_dgemm_nt_8x4_lib4(k0, &alpha, &pA[i*sda], sda, &pB[j*sdb], &beta, &pC[j*ps+i*sdc], sdc, &pD[j*ps+i*sdd], sdd);
			}
		if(j<n)
			{

			while(jj<nv && rv[jj]<j+4)
				jj++;
			if(jj<nv)
				kjj = cv[jj];
			else
				kjj = cv[jj-1];
			k0 = kii<kjj ? kii : kjj;

			if(j<i) // dgemm
				{
				kernel_dgemm_nt_8x4_vs_lib4(k0, &alpha, &pA[i*sda], sda, &pB[j*sdb], &beta, &pC[j*ps+i*sdc], sdc, &pD[j*ps+i*sdd], sdd, 8, n-j);
				}
			else // dsyrk
				{
				kernel_dsyrk_nt_l_8x4_vs_lib4(k0, &alpha, &pA[i*sda], sda, &pB[j*sdb], &beta, &pC[j*ps+i*sdc], sdc, &pD[j*ps+i*sdd], sdd, 8, n-j);
				if(j<n-4)
					{
					kernel_dsyrk_nt_l_4x4_vs_lib4(k, &alpha, &pA[(i+4)*sda], &pB[(j+4)*sdb], &beta, &pC[(j+4)*ps+(i+4)*sdc], &pD[(j+4)*ps+(i+4)*sdd], 4, n-j-4); // TODO
					}
				}
			}
		}
	if(m>i)
		{
		if(m-i<=4)
			{
			goto left_4;
			}
		else
			{
			goto left_8;
			}
		}
#else
	for(; i<m-3; i+=4)
		{

		while(ii<nv && rv[ii]<i+4)
			ii++;
		if(ii<nv)
			kii = cv[ii];
		else
			kii = cv[ii-1];
//		k0 = kii;
//		printf("\nii %d %d %d %d %d\n", i, ii, rv[ii], cv[ii], kii);

		j = 0;
		jj = 0;
		for(; j<i && j<n-3; j+=4)
			{

			while(jj<nv && rv[jj]<j+4)
				jj++;
			if(jj<nv)
				kjj = cv[jj];
			else
				kjj = cv[jj-1];
			k0 = kii<kjj ? kii : kjj;
//			printf("\njj %d %d %d %d %d\n", j, jj, rv[jj], cv[jj], kjj);

			kernel_dgemm_nt_4x4_lib4(k0, &alpha, &pA[i*sda], &pB[j*sdb], &beta, &pC[j*ps+i*sdc], &pD[j*ps+i*sdd]);
			}
		if(j<n)
			{

			while(jj<nv && rv[jj]<j+4)
				jj++;
			if(jj<nv)
				kjj = cv[jj];
			else
				kjj = cv[jj-1];
			k0 = kii<kjj ? kii : kjj;
//			printf("\njj %d %d %d %d %d\n", j, jj, rv[jj], cv[jj], kjj);

			if(j<i) // dgemm
				{
				kernel_dgemm_nt_4x4_vs_lib4(k0, &alpha, &pA[i*sda], &pB[j*sdb], &beta, &pC[j*ps+i*sdc], &pD[j*ps+i*sdd], 4, n-j);
				}
			else // dsyrk
				{
				kernel_dsyrk_nt_l_4x4_vs_lib4(k0, &alpha, &pA[i*sda], &pB[j*sdb], &beta, &pC[j*ps+i*sdc], &pD[j*ps+i*sdd], 4, n-j);
				}
			}
		}
	if(m>i)
		{
		goto left_4;
		}
#endif

	// common return if i==m
	return;

	// clean up loops definitions

#if defined(TARGET_X64_INTEL_SANDY_BRIDGE) || defined(TARGET_X64_INTEL_HASWELL)
	left_8:

	kii = cv[nv-1];

	j = 0;
	jj = 0;
	for(; j<i && j<n-3; j+=4)
		{

		while(jj<nv && rv[jj]<j+4)
			jj++;
		if(jj<nv)
			kjj = cv[jj];
		else
			kjj = cv[jj-1];
		k0 = kii<kjj ? kii : kjj;

		kernel_dgemm_nt_8x4_vs_lib4(k0, &alpha, &pA[i*sda], sda, &pB[j*sdb], &beta, &pC[j*ps+i*sdc], sdc, &pD[j*ps+i*sdd], sdd, m-i, n-j);
		}
	if(j<n)
		{

		while(jj<nv && rv[jj]<j+4)
			jj++;
		if(jj<nv)
			kjj = cv[jj];
		else
			kjj = cv[jj-1];
		k0 = kii<kjj ? kii : kjj;

		if(j<i) // dgemm
			{
			kernel_dgemm_nt_8x4_vs_lib4(k0, &alpha, &pA[i*sda], sda, &pB[j*sdb], &beta, &pC[j*ps+i*sdc], sdc, &pD[j*ps+i*sdd], sdd, m-i, n-j);
			}
		else // dsyrk
			{
			kernel_dsyrk_nt_l_8x4_vs_lib4(k0, &alpha, &pA[i*sda], sda, &pB[j*sdb], &beta, &pC[j*ps+i*sdc], sdc, &pD[j*ps+i*sdd], sdd, m-i, n-j);
			if(j<n-4)
				{
				kernel_dsyrk_nt_l_4x4_vs_lib4(k, &alpha, &pA[(i+4)*sda], &pB[(j+4)*sdb], &beta, &pC[(j+4)*ps+(i+4)*sdc], &pD[(j+4)*ps+(i+4)*sdd], m-i-4, n-j-4); // TODO
				}
			}
		}
	return;
#endif

	left_4:

	kii = cv[nv-1];

	j = 0;
	jj = 0;
	for(; j<i && j<n-3; j+=4)
		{

		while(jj<nv && rv[jj]<j+4)
			jj++;
		if(jj<nv)
			kjj = cv[jj];
		else
			kjj = cv[jj-1];
		k0 = kii<kjj ? kii : kjj;

		kernel_dgemm_nt_4x4_vs_lib4(k0, &alpha, &pA[i*sda], &pB[j*sdb], &beta, &pC[j*ps+i*sdc], &pD[j*ps+i*sdd], m-i, n-j);
		}
	if(j<n)
		{

		while(jj<nv && rv[jj]<j+4)
			jj++;
		if(jj<nv)
			kjj = cv[jj];
		else
			kjj = cv[jj-1];
		k0 = kii<kjj ? kii : kjj;

		if(j<i) // dgemm
			{
			kernel_dgemm_nt_4x4_vs_lib4(k0, &alpha, &pA[i*sda], &pB[j*sdb], &beta, &pC[j*ps+i*sdc], &pD[j*ps+i*sdd], m-i, n-j);
			}
		else // dsyrk
			{
			kernel_dsyrk_nt_l_4x4_vs_lib4(k0, &alpha, &pA[i*sda], &pB[j*sdb], &beta, &pC[j*ps+i*sdc], &pD[j*ps+i*sdd], m-i, n-j);
			}
		}
	return;

	}
#endif



/****************************
* new interface
****************************/



#if defined(LA_HIGH_PERFORMANCE)



// dgemm nn
void blasfeo_dgemm_nn(int m, int n, int k, double alpha, struct blasfeo_dmat *sA, int ai, int aj, struct blasfeo_dmat *sB, int bi, int bj, double beta, struct blasfeo_dmat *sC, int ci, int cj, struct blasfeo_dmat *sD, int di, int dj)
	{
	if(m<=0 || n<=0)
		return;

	// invalidate stored inverse diagonal of result matrix
	sD->use_dA = 0;

	const int ps = 4;

	int sda = sA->cn;
	int sdb = sB->cn;
	int sdc = sC->cn;
	int sdd = sD->cn;

	int air = ai & (ps-1);
	int bir = bi & (ps-1);

	// pA, pB point to panels edges
	double *pA = sA->pA + aj*ps + (ai-air)*sda;
	double *pB = sB->pA + bj*ps + (bi-bir)*sdb;
	double *pC = sC->pA + cj*ps;
	double *pD = sD->pA + dj*ps;

	int offsetB = bir;

	int ci0 = ci-air;
	int di0 = di-air;
	int offsetC;
	int offsetD;
	if(ci0>=0)
		{
		pC += ci0/ps*ps*sdd;
		offsetC = ci0%ps;
		}
	else
		{
		pC += -ps*sdc;
		offsetC = ps+ci0;
		}

	if(di0>=0)
		{
		pD += di0/ps*ps*sdd;
		offsetD = di0%ps;
		}
	else
		{
		pD += -ps*sdd;
		offsetD = ps+di0;
		}

	int i, j, l;

	// clean up at the beginning
	if(air!=0)
		{
#if defined(TARGET_X64_INTEL_SANDY_BRIDGE) || defined(TARGET_X64_INTEL_HASWELL)
		if(m>5)
			{
			j = 0;
			for(; j<n; j+=4)
				{
				kernel_dgemm_nn_8x4_gen_lib4(k, &alpha, &pA[0], sda, offsetB, &pB[j*ps], sdb, &beta, offsetC, &pC[j*ps], sdc, offsetD, &pD[j*ps], sdd, air, air+m, 0, n-j);
				}
			m -= 2*ps-air;
			pA += 2*ps*sda;
			pC += 2*ps*sdc;
			pD += 2*ps*sdd;
			}
		else // m-i<=4
			{
#endif
			j = 0;
			for(; j<n; j+=4)
				{
				kernel_dgemm_nn_4x4_gen_lib4(k, &alpha, &pA[0], offsetB, &pB[j*ps], sdb, &beta, offsetC, &pC[j*ps], sdc, offsetD, &pD[j*ps], sdd, air, air+m, 0, n-j);
				}
			m -= 1*ps-air;
			pA += 1*ps*sda;
			pC += 1*ps*sdc;
			pD += 1*ps*sdd;
#if defined(TARGET_X64_INTEL_SANDY_BRIDGE) || defined(TARGET_X64_INTEL_HASWELL)
			// nothing more to do
			}
#endif
		}
	// main loop
	i = 0;
	if(offsetC==0 & offsetD==0)
		{
#if defined(TARGET_X64_INTEL_HASWELL)
		for(; i<m-11; i+=12)
			{
			j = 0;
			for(; j<n-3; j+=4)
				{
				kernel_dgemm_nn_12x4_lib4(k, &alpha, &pA[i*sda], sda, offsetB, &pB[j*ps], sdb, &beta, &pC[j*ps+i*sdc], sdc, &pD[j*ps+i*sdd], sdd);
				}
			if(j<n)
				{
				kernel_dgemm_nn_12x4_vs_lib4(k, &alpha, &pA[i*sda], sda, offsetB, &pB[j*ps], sdb, &beta, &pC[j*ps+i*sdc], sdc, &pD[j*ps+i*sdd], sdd, m-i, n-j);
				}
			}
		if(m>i)
			{
			if(m-i<=4)
				{
				goto left_4;
				}
			else if(m-i<=8)
				{
				goto left_8;
				}
			else
				{
				goto left_12;
				}
			}
#elif defined(TARGET_X64_INTEL_SANDY_BRIDGE)
		for(; i<m-12 | i==m-8; i+=8)
			{
			j = 0;
			for(; j<n-3; j+=4)
				{
				kernel_dgemm_nn_8x4_lib4(k, &alpha, &pA[i*sda], sda, offsetB, &pB[j*ps], sdb, &beta, &pC[j*ps+i*sdc], sdc, &pD[j*ps+i*sdd], sdd);
				}
			if(j<n)
				{
				kernel_dgemm_nn_8x4_vs_lib4(k, &alpha, &pA[i*sda], sda, offsetB, &pB[j*ps], sdb, &beta, &pC[j*ps+i*sdc], sdc, &pD[j*ps+i*sdd], sdd, m-i, n-j);
				}
			}
		if(m>i)
			{
			if(m-i<=4)
				{
				goto left_4;
				}
			else if(m-i<=8)
				{
				goto left_8;
				}
			else
				{
				goto left_12;
				}
			}
#elif defined(TARGET_X86_AMD_BARCELONA)
		for(; i<m-3; i+=4)
			{
			j = 0;
			for(; j<n-1; j+=2)
				{
				kernel_dgemm_nn_4x2_lib4(k, &alpha, &pA[i*sda], offsetB, &pB[j*ps], sdb, &beta, &pC[j*ps+i*sdc], &pD[j*ps+i*sdd]);
				}
			if(j<n)
				{
				kernel_dgemm_nn_4x2_vs_lib4(k, &alpha, &pA[i*sda], offsetB, &pB[j*ps], sdb, &beta, &pC[j*ps+i*sdc], &pD[j*ps+i*sdd], m-i, n-j);
				}
			}
		if(m>i)
			{
			goto left_4;
			}
#else // all others
		for(; i<m-3; i+=4)
			{
			j = 0;
			for(; j<n-3; j+=4)
				{
				kernel_dgemm_nn_4x4_lib4(k, &alpha, &pA[i*sda], offsetB, &pB[j*ps], sdb, &beta, &pC[j*ps+i*sdc], &pD[j*ps+i*sdd]);
				}
			if(j<n)
				{
				kernel_dgemm_nn_4x4_vs_lib4(k, &alpha, &pA[i*sda], offsetB, &pB[j*ps], sdb, &beta, &pC[j*ps+i*sdc], &pD[j*ps+i*sdd], m-i, n-j);
				}
			}
		if(m>i)
			{
			goto left_4;
			}
#endif
		}
	else // ! (offsetC==0 & offsetD==0)
		{
// TODO 12x4
#if defined(TARGET_X64_INTEL_HASWELL) || defined(TARGET_X64_INTEL_SANDY_BRIDGE)
		for(; i<m-4; i+=8)
			{
			j = 0;
			for(; j<n; j+=4)
				{
				kernel_dgemm_nn_8x4_gen_lib4(k, &alpha, &pA[i*sda], sda, offsetB, &pB[j*ps], sdb, &beta, offsetC, &pC[j*ps+i*sdc], sdc, offsetD, &pD[j*ps+i*sdd], sdd, 0, m-i, 0, n-j);
				}
			}
		if(m>i)
			{
			goto left_4_g;
			}
#else
		for(; i<m; i+=4)
			{
			j = 0;
			for(; j<n; j+=4)
				{
				kernel_dgemm_nn_4x4_gen_lib4(k, &alpha, &pA[i*sda], offsetB, &pB[j*ps], sdb, &beta, offsetC, &pC[j*ps+i*sdc], sdc, offsetD, &pD[j*ps+i*sdd], sdd, 0, m-i, 0, n-j);
				}
			}
#endif
		}

	// common return if i==m
	return;

	// clean up loops definitions

#if defined(TARGET_X64_INTEL_HASWELL)
	left_12_g:
	j = 0;
	for(; j<n; j+=4)
		{
//		kernel_dgemm_nn_12x4_gen_lib4(k, &alpha, &pA[i*sda], sda, offsetB, &pB[j*ps], sdb, &beta, offsetC, &pC[j*ps+i*sdc], sdc, offsetD, &pD[j*ps+i*sdd], sdd, 0, m-i, 0, n-j);
		kernel_dgemm_nn_8x4_gen_lib4(k, &alpha, &pA[i*sda], sda, offsetB, &pB[j*ps], sdb, &beta, offsetC, &pC[j*ps+i*sdc], sdc, offsetD, &pD[j*ps+i*sdd], sdd, 0, m-i, 0, n-j);
		kernel_dgemm_nn_4x4_gen_lib4(k, &alpha, &pA[(i+8)*sda], offsetB, &pB[j*ps], sdb, &beta, offsetC, &pC[j*ps+(i+8)*sdc], sdc, offsetD, &pD[j*ps+(i+8)*sdd], sdd, 0, m-(i+8), 0, n-j);
		}
	return;
#endif

#if defined(TARGET_X64_INTEL_HASWELL) | defined(TARGET_X64_INTEL_SANDY_BRIDGE)
	left_12:
	j = 0;
	for(; j<n; j+=4)
		{
		kernel_dgemm_nn_12x4_vs_lib4(k, &alpha, &pA[i*sda], sda, offsetB, &pB[j*ps], sdb, &beta, &pC[j*ps+i*sdc], sdc, &pD[j*ps+i*sdd], sdd, m-i, n-j);
		}
//	if(j<n)
//		{
//		kernel_dgemm_nn_12x2_vs_lib4(k, &alpha, &pA[i*sda], sda, offsetB, &pB[j*ps], sdb, &beta, &pC[j*ps+i*sdc], sdc, &pD[j*ps+i*sdd], sdd, m-i, n-j);
//		}
	return;
#endif

#if defined(TARGET_X64_INTEL_SANDY_BRIDGE) | defined(TARGET_X64_INTEL_HASWELL)
	left_8_g:
	j = 0;
	for(; j<n; j+=4)
		{
		kernel_dgemm_nn_8x4_gen_lib4(k, &alpha, &pA[i*sda], sda, offsetB, &pB[j*ps], sdb, &beta, offsetC, &pC[j*ps+i*sdc], sdc, offsetD, &pD[j*ps+i*sdd], sdd, 0, m-i, 0, n-j);
		}
	return;
#endif

#if defined(TARGET_X64_INTEL_HASWELL) | defined(TARGET_X64_INTEL_SANDY_BRIDGE)
	left_8:
	j = 0;
	for(; j<n; j+=4)
		{
		kernel_dgemm_nn_8x4_vs_lib4(k, &alpha, &pA[i*sda], sda, offsetB, &pB[j*ps], sdb, &beta, &pC[j*ps+i*sdc], sdc, &pD[j*ps+i*sdd], sdd, m-i, n-j);
		}
	return;
#endif

	left_4_g:
	j = 0;
	for(; j<n; j+=4)
		{
		kernel_dgemm_nn_4x4_gen_lib4(k, &alpha, &pA[i*sda], offsetB, &pB[j*ps], sdb, &beta, offsetC, &pC[j*ps+i*sdc], sdc, offsetD, &pD[j*ps+i*sdd], sdd, 0, m-i, 0, n-j);
		}
	return;

#if defined(TARGET_X64_INTEL_HASWELL) | defined(TARGET_X64_INTEL_SANDY_BRIDGE)
	left_4:
	j = 0;
	for(; j<n-4; j+=8)
		{
		kernel_dgemm_nn_4x8_vs_lib4(k, &alpha, &pA[i*sda], offsetB, &pB[j*ps], sdb, &beta, &pC[j*ps+i*sdc], &pD[j*ps+i*sdd], m-i, n-j);
		}
	if(j<n)
		{
		kernel_dgemm_nn_4x4_vs_lib4(k, &alpha, &pA[i*sda], offsetB, &pB[j*ps], sdb, &beta, &pC[j*ps+i*sdc], &pD[j*ps+i*sdd], m-i, n-j);
		}
	return;
#elif defined(TARGET_X86_AMD_BARCELONA)
	left_4:
	j = 0;
	for(; j<n; j+=2)
		{
		kernel_dgemm_nn_4x2_vs_lib4(k, &alpha, &pA[i*sda], offsetB, &pB[j*ps], sdb, &beta, &pC[j*ps+i*sdc], &pD[j*ps+i*sdd], m-i, n-j);
		}
	return;
#else // all others
	left_4:
	j = 0;
	for(; j<n; j+=4)
		{
		kernel_dgemm_nn_4x4_vs_lib4(k, &alpha, &pA[i*sda], offsetB, &pB[j*ps], sdb, &beta, &pC[j*ps+i*sdc], &pD[j*ps+i*sdd], m-i, n-j);
		}
	return;
#endif

	return;
	}



// dgemm nt
void blasfeo_dgemm_nt(int m, int n, int k, double alpha, struct blasfeo_dmat *sA, int ai, int aj, struct blasfeo_dmat *sB, int bi, int bj, double beta, struct blasfeo_dmat *sC, int ci, int cj, struct blasfeo_dmat *sD, int di, int dj)
	{
	if(m<=0 | n<=0)
		return;

	// invalidate stored inverse diagonal of result matrix
	sD->use_dA = 0;

	const int ps = 4;

	int sda = sA->cn;
	int sdb = sB->cn;
	int sdc = sC->cn;
	int sdd = sD->cn;
	int air = ai & (ps-1);
	int bir = bi & (ps-1);
	double *pA = sA->pA + aj*ps + (ai-air)*sda;
	double *pB = sB->pA + bj*ps + (bi-bir)*sdb;
	double *pC = sC->pA + cj*ps;
	double *pD = sD->pA + dj*ps;

	if(ai==0 & bi==0 & ci==0 & di==0)
		{
		dgemm_nt_lib(m, n, k, alpha, pA, sda, pB, sdb, beta, pC, sdc, pD, sdd);
		return;
		}

	int ci0 = ci-air;
	int di0 = di-air;
	int offsetC;
	int offsetD;
	if(ci0>=0)
		{
		pC += ci0/ps*ps*sdd;
		offsetC = ci0%ps;
		}
	else
		{
		pC += -4*sdc;
		offsetC = ps+ci0;
		}
	if(di0>=0)
		{
		pD += di0/ps*ps*sdd;
		offsetD = di0%ps;
		}
	else
		{
		pD += -4*sdd;
		offsetD = ps+di0;
		}

	int i, j, l;

	int idxB;

	// clean up at the beginning
	if(air!=0)
		{
#if defined(TARGET_X64_INTEL_HASWELL) || defined(TARGET_X64_INTEL_SANDY_BRIDGE)
		if(m>5)
			{
			j = 0;
			idxB = 0;
			// clean up at the beginning
			if(bir!=0)
				{
				kernel_dgemm_nt_8x4_gen_lib4(k, &alpha, &pA[0], sda, &pB[idxB*sdb], &beta, offsetC, &pC[j*ps]-bir*ps, sdc, offsetD, &pD[j*ps]-bir*ps, sdd, air, air+m, bir, bir+n-j);
				j += ps-bir;
				idxB += 4;
				}
			// main loop
			for(; j<n; j+=4)
				{
				kernel_dgemm_nt_8x4_gen_lib4(k, &alpha, &pA[0], sda, &pB[idxB*sdb], &beta, offsetC, &pC[j*ps], sdc, offsetD, &pD[j*ps], sdd, air, air+m, 0, n-j);
				idxB += 4;
				}
			m -= 2*ps-air;
			pA += 2*ps*sda;
			pC += 2*ps*sdc;
			pD += 2*ps*sdd;
			}
		else // m<=4
			{
#endif
			j = 0;
			idxB = 0;
			// clean up at the beginning
			if(bir!=0)
				{
				kernel_dgemm_nt_4x4_gen_lib4(k, &alpha, &pA[0], &pB[idxB*sdb], &beta, offsetC, &pC[j*ps]-bir*ps, sdc, offsetD, &pD[j*ps]-bir*ps, sdd, air, air+m, bir, bir+n-j);
				j += ps-bir;
				idxB += 4;
				}
			// main loop
			for(; j<n; j+=4)
				{
				kernel_dgemm_nt_4x4_gen_lib4(k, &alpha, &pA[0], &pB[idxB*sdb], &beta, offsetC, &pC[j*ps], sdc, offsetD, &pD[j*ps], sdd, air, air+m, 0, n-j);
				idxB += 4;
				}
			m -= ps-air;
			pA += ps*sda;
			pC += ps*sdc;
			pD += ps*sdd;
#if defined(TARGET_X64_INTEL_HASWELL) || defined(TARGET_X64_INTEL_SANDY_BRIDGE)
			// nothing more to do
//			return;
			}
#endif
		}
	i = 0;
	// main loop
#if defined(TARGET_X64_INTEL_HASWELL) || defined(TARGET_X64_INTEL_SANDY_BRIDGE)
	for(; i<m-4; i+=8)
		{
		j = 0;
		idxB = 0;
		// clean up at the beginning
		if(bir!=0)
			{
			kernel_dgemm_nt_8x4_gen_lib4(k, &alpha, &pA[i*sda], sda, &pB[idxB*sdb], &beta, offsetC, &pC[j*ps+i*sdc]-bir*ps, sdc, offsetD, &pD[j*ps+i*sdd]-bir*ps, sdd, 0, m-i, bir, bir+n-j);
			j += ps-bir;
			idxB += 4;
			}
		// main loop
		for(; j<n; j+=4)
			{
			kernel_dgemm_nt_8x4_gen_lib4(k, &alpha, &pA[i*sda], sda, &pB[idxB*sdb], &beta, offsetC, &pC[j*ps+i*sdc], sdc, offsetD, &pD[j*ps+i*sdd], sdd, 0, m-i, 0, n-j);
			idxB += 4;
			}
		}
	if(i<m)
		{
		j = 0;
		idxB = 0;
		// clean up at the beginning
		if(bir!=0)
			{
			kernel_dgemm_nt_4x4_gen_lib4(k, &alpha, &pA[i*sda], &pB[idxB*sdb], &beta, offsetC, &pC[j*ps+i*sdc]-bir*ps, sdc, offsetD, &pD[j*ps+i*sdd]-bir*ps, sdd, 0, m-i, bir, bir+n-j);
			j += ps-bir;
			idxB += 4;
			}
		// main loop
		for(; j<n; j+=4)
			{
			kernel_dgemm_nt_4x4_gen_lib4(k, &alpha, &pA[i*sda], &pB[idxB*sdb], &beta, offsetC, &pC[j*ps+i*sdc], sdc, offsetD, &pD[j*ps+i*sdd], sdd, 0, m-i, 0, n-j);
			idxB += 4;
			}
		}
#else
	for(; i<m; i+=4)
		{
		j = 0;
		idxB = 0;
		// clean up at the beginning
		if(bir!=0)
			{
			kernel_dgemm_nt_4x4_gen_lib4(k, &alpha, &pA[i*sda], &pB[idxB*sdb], &beta, offsetC, &pC[j*ps+i*sdc]-bir*ps, sdc, offsetD, &pD[j*ps+i*sdd]-bir*ps, sdd, 0, m-i, bir, bir+n-j);
			j += ps-bir;
			idxB += 4;
			}
		// main loop
		for(; j<n; j+=4)
			{
			kernel_dgemm_nt_4x4_gen_lib4(k, &alpha, &pA[i*sda], &pB[idxB*sdb], &beta, offsetC, &pC[j*ps+i*sdc], sdc, offsetD, &pD[j*ps+i*sdd], sdd, 0, m-i, 0, n-j);
			idxB += 4;
			}
		}
#endif

	return;
	}



// dgemm_tn
void blasfeo_dgemm_tn(int m, int n, int k, double alpha, struct blasfeo_dmat *sA, int ai, int aj, struct blasfeo_dmat *sB, int bi, int bj, double beta, struct blasfeo_dmat *sC, int ci, int cj, struct blasfeo_dmat *sD, int di, int dj)
	{
	if(m<=0 || n<=0)
		return;

	// invalidate stored inverse diagonal of result matrix
	sD->use_dA = 0;

	const int ps = 4;

	int sda = sA->cn;
	int sdb = sB->cn;
	int sdc = sC->cn;
	int sdd = sD->cn;

	int air = ai & (ps-1);
	int bir = bi & (ps-1);
	int cir = ci & (ps-1);
	int dir = di & (ps-1);

	if(cir!=0 | dir!=0)
		{
		printf("\nblasfeo_dgemm_tn: feature not implemented yet: ci, di not 0\n");
		exit(1);
		}

	// pA, pB point to panels edges
	double *pA = sA->pA + aj*ps + (ai-air)*sda + air;
	double *pB = sB->pA + bj*ps + (bi-bir)*sdb;
	double *pC = sC->pA + cj*ps;
	double *pD = sD->pA + dj*ps;

	int offsetA = air;
	int offsetB = bir;

	// TODO offsetC offsetD

// TODO visual studio alignment
#if defined(TARGET_X64_INTEL_HASWELL)
	double pU[3*4*K_MAX] __attribute__ ((aligned (64)));
#elif defined(TARGET_X64_INTEL_SANDY_BRIDGE)
	double pU[2*4*K_MAX] __attribute__ ((aligned (64)));
#elif defined(TARGET_GENERIC)
	double pU[1*4*K_MAX];
#else
	double pU[1*4*K_MAX] __attribute__ ((aligned (64)));
#endif
	int sdu = (m+3)/4*4;
	sdu = sdu<K_MAX ? sdu : K_MAX;

	int ii, jj;

	if(1)
		{
		goto tn_0;
		}

	return;

tn_0:

	ii = 0;
#if defined(TARGET_X64_INTEL_SANDY_BRIDGE)
	for(; ii<m-7; ii+=8)
		{
		kernel_dpacp_tn_4_lib4(k, offsetA, pA+(ii+0)*ps, sda, pU+0*sdu);
		kernel_dpacp_tn_4_lib4(k, offsetA, pA+(ii+4)*ps, sda, pU+4*sdu);
		for(jj=0; jj<n-3; jj+=4)
			{
			kernel_dgemm_nn_8x4_lib4(k, &alpha, pU, sdu, offsetB, pB+jj*ps, sdb, &beta, pC+ii*sdc+jj*ps, sdc, pD+ii*sdd+jj*ps, sdd);
			}
		if(jj<n)
			{
			kernel_dgemm_nn_8x4_vs_lib4(k, &alpha, pU, sdu, offsetB, pB+jj*ps, sdb, &beta, pC+ii*sdc+jj*ps, sdu, pD+ii*sdd+jj*ps, sdd, m-ii, n-jj);
			}
		}
	if(ii<m)
		{
		if(m-ii<=4)
			{
			goto tn_0_left_4;
			}
		else
			{
			goto tn_0_left_8;
			}
		}
#else
	for(; ii<m-3; ii+=4)
		{
		kernel_dpacp_tn_4_lib4(k, offsetA, pA+ii*ps, sda, pU);
		for(jj=0; jj<n-3; jj+=4)
			{
			kernel_dgemm_nn_4x4_lib4(k, &alpha, pU, offsetB, pB+jj*ps, sdb, &beta, pC+ii*sdc+jj*ps, pD+ii*sdd+jj*ps);
			}
		if(jj<n)
			{
			kernel_dgemm_nn_4x4_vs_lib4(k, &alpha, pU, offsetB, pB+jj*ps, sdb, &beta, pC+ii*sdc+jj*ps, pD+ii*sdd+jj*ps, m-ii, n-jj);
			}
		}
	if(ii<m)
		{
		goto tn_0_left_4;
		}
#endif
	goto tn_0_return;

#if defined(TARGET_X64_INTEL_HASWELL) | defined(TARGET_X64_INTEL_SANDY_BRIDGE)
tn_0_left_8:
		kernel_dpacp_tn_4_lib4(k, offsetA, pA+(ii+0)*ps, sda, pU+0*sdu);
		kernel_dpacp_tn_4_lib4(k, offsetA, pA+(ii+4)*ps, sda, pU+4*sdu);
		for(jj=0; jj<n; jj+=4)
			{
			kernel_dgemm_nn_8x4_vs_lib4(k, &alpha, pU, sdu, offsetB, pB+jj*ps, sdb, &beta, pC+ii*sdc+jj*ps, sdc, pD+ii*sdd+jj*ps, sdd, m-ii, n-jj);
			}
	goto tn_0_return;
#endif

tn_0_left_4:
	kernel_dpacp_tn_4_lib4(k, offsetA, pA+ii*ps, sda, pU);
	for(jj=0; jj<n; jj+=4)
		{
		kernel_dgemm_nn_4x4_vs_lib4(k, &alpha, pU, offsetB, pB+jj*ps, sdb, &beta, pC+ii*sdc+jj*ps, pD+ii*sdd+jj*ps, m-ii, n-jj);
		}
	goto tn_0_return;

tn_0_return:
	return;
	}



// dtrsm_llnu
void blasfeo_dtrsm_llnu(int m, int n, double alpha, struct blasfeo_dmat *sA, int ai, int aj, struct blasfeo_dmat *sB, int bi, int bj, struct blasfeo_dmat *sD, int di, int dj)
	{
	// invalidate stored inverse diagonal of result matrix
	sD->use_dA = 0;

	if(ai!=0 | bi!=0 | di!=0 | alpha!=1.0)
		{
		printf("\nblasfeo_dtrsm_llnu: feature not implemented yet: ai=%d, bi=%d, di=%d, alpha=%f\n", ai, bi, di, alpha);
		exit(1);
		}

	const int ps = 4;
	// TODO alpha
	int sda = sA->cn;
	int sdb = sB->cn;
	int sdd = sD->cn;
	double *pA = sA->pA + aj*ps;
	double *pB = sB->pA + bj*ps;
	double *pD = sD->pA + dj*ps;
	dtrsm_nn_ll_one_lib(m, n, pA, sda, pB, sdb, pD, sdd);
	return;
	}



// dtrsm_nn_lun
void blasfeo_dtrsm_lunn(int m, int n, double alpha, struct blasfeo_dmat *sA, int ai, int aj, struct blasfeo_dmat *sB, int bi, int bj, struct blasfeo_dmat *sD, int di, int dj)
	{
	if(ai!=0 | bi!=0 | di!=0 | alpha!=1.0)
		{
		printf("\nblasfeo_dtrsm_lunn: feature not implemented yet: ai=%d, bi=%d, di=%d, alpha=%f\n", ai, bi, di, alpha);
		exit(1);
		}

	// invalidate stored inverse diagonal of result matrix
	sD->use_dA = 0;

	const int ps = 4;
	// TODO alpha
	int sda = sA->cn;
	int sdb = sB->cn;
	int sdd = sD->cn;
	double *pA = sA->pA + aj*ps;
	double *pB = sB->pA + bj*ps;
	double *pD = sD->pA + dj*ps;
	double *dA = sA->dA;
	int ii;

	if(ai==0 & aj==0)
		{
		// recompute diagonal if size of operation grows
		if(sA->use_dA<m)
			{
			ddiaex_lib(m, 1.0, ai, pA, sda, dA);
			for(ii=0; ii<m; ii++)
				dA[ii] = 1.0 / dA[ii];
			sA->use_dA = m;
			}
		}
	// if submatrix recompute diagonal
	else
		{
		ddiaex_lib(m, 1.0, ai, pA, sda, dA);
		for(ii=0; ii<m; ii++)
			dA[ii] = 1.0 / dA[ii];
		sA->use_dA = 0;
		}
	dtrsm_nn_lu_inv_lib(m, n, pA, sda, dA, pB, sdb, pD, sdd);

	return;
	}



// dtrsm_right_lower_transposed_notunit
void blasfeo_dtrsm_rltn(int m, int n, double alpha, struct blasfeo_dmat *sA, int ai, int aj, struct blasfeo_dmat *sB, int bi, int bj, struct blasfeo_dmat *sD, int di, int dj)
	{

	if(m<=0 || n<=0)
		return;

	const int ps = 4;

	// invalidate stored inverse diagonal of result matrix
	sD->use_dA = 0;

	// TODO alpha !!!!!

	int sda = sA->cn;
	int sdb = sB->cn;
	int sdd = sD->cn;
	int bir = bi & (ps-1);
	int dir = di & (ps-1);
	double *pA = sA->pA + aj*ps;
	double *pB = sB->pA + bj*ps + (bi-bir)*sdb;
	double *pD = sD->pA + dj*ps + (di-dir)*sdd;
	double *dA = sA->dA;

	if(ai!=0 | bir!=0 | dir!=0 | alpha!=1.0)
		{
		printf("\nblasfeo_dtrsm_rltn: feature not implemented yet: ai=%d, bi=%d, di=%d, alpha=%f\n", ai, bi, di, alpha);
		exit(1);
		}

	int i, j;

	if(ai==0 & aj==0)
		{
		if(sA->use_dA<n)
			{
			ddiaex_lib(n, 1.0, ai, pA, sda, dA);
			for(i=0; i<n; i++)
				dA[i] = 1.0 / dA[i];
			sA->use_dA = n;
			}
		}
	else
		{
		ddiaex_lib(n, 1.0, ai, pA, sda, dA);
		for(i=0; i<n; i++)
			dA[i] = 1.0 / dA[i];
		sA->use_dA = 0;
		}

//	dtrsm_nt_rl_inv_lib(m, n, pA, sda, dA, pB, sdb, pD, sdd);
	i = 0;
#if defined(TARGET_X64_INTEL_HASWELL)
	for(; i<m-11; i+=12)
		{
		j = 0;
		for(; j<n-3; j+=4)
			{
			kernel_dtrsm_nt_rl_inv_12x4_lib4(j, &pD[i*sdd], sdd, &pA[j*sda], &pB[j*ps+i*sdb], sdb, &pD[j*ps+i*sdd], sdd, &pA[j*ps+j*sda], &dA[j]);
			}
		if(j<n)
			{
			kernel_dtrsm_nt_rl_inv_12x4_vs_lib4(j, &pD[i*sdd], sdd, &pA[j*sda], &pB[j*ps+i*sdb], sdb, &pD[j*ps+i*sdd], sdd, &pA[j*ps+j*sda], &dA[j], m-i, n-j);
			}
		}
	if(m>i)
		{
		if(m-i<=4)
			{
			goto left_4;
			}
		else if(m-i<=8)
			{
			goto left_8;
			}
		else
			{
			goto left_12;
			}
		}
#elif defined(TARGET_X64_INTEL_SANDY_BRIDGE)
	for(; i<m-7; i+=8)
		{
		j = 0;
		for(; j<n-3; j+=4)
			{
			kernel_dtrsm_nt_rl_inv_8x4_lib4(j, &pD[i*sdd], sdd, &pA[j*sda], &pB[j*ps+i*sdb], sdb, &pD[j*ps+i*sdd], sdd, &pA[j*ps+j*sda], &dA[j]);
			}
		if(j<n)
			{
			kernel_dtrsm_nt_rl_inv_8x4_vs_lib4(j, &pD[i*sdd], sdd, &pA[j*sda], &pB[j*ps+i*sdb], sdb, &pD[j*ps+i*sdd], sdd, &pA[j*ps+j*sda], &dA[j], m-i, n-j);
			}
		}
	if(m>i)
		{
		if(m-i<=4)
			{
			goto left_4;
			}
		else
			{
			goto left_8;
			}
		}
#elif defined(TARGET_X86_AMD_BARCELONA)
	for(; i<m-3; i+=4)
		{
		j = 0;
		for(; j<n-3; j+=4)
			{
			kernel_dtrsm_nt_rl_inv_4x2_lib4(j, &pD[i*sdd], &pA[j*sda], &pB[j*ps+i*sdb], &pD[j*ps+i*sdd], &pA[j*ps+j*sda], &dA[j]);
			kernel_dtrsm_nt_rl_inv_4x2_lib4(j+2, &pD[i*sdd], &pA[j*sda+2], &pB[(j+2)*ps+i*sdb], &pD[(j+2)*ps+i*sdd], &pA[(j+2)*ps+j*sda+2], &dA[j+2]);
			}
		if(j<n)
			{
			kernel_dtrsm_nt_rl_inv_4x2_vs_lib4(j, &pD[i*sdd], &pA[j*sda], &pB[j*ps+i*sdb], &pD[j*ps+i*sdd], &pA[j*ps+j*sda], &dA[j], m-i, n-j);
			if(j<n-2)
				kernel_dtrsm_nt_rl_inv_4x2_vs_lib4(j+2, &pD[i*sdd], &pA[j*sda+2], &pB[(j+2)*ps+i*sdb], &pD[(j+2)*ps+i*sdd], &pA[(j+2)*ps+j*sda+2], &dA[j+2], m-i, n-(j+2));
			}
		}
	if(m>i)
		{
		goto left_4;
		}
#else
	for(; i<m-3; i+=4)
		{
		j = 0;
		for(; j<n-3; j+=4)
			{
			kernel_dtrsm_nt_rl_inv_4x4_lib4(j, &pD[i*sdd], &pA[j*sda], &pB[j*ps+i*sdb], &pD[j*ps+i*sdd], &pA[j*ps+j*sda], &dA[j]);
			}
		if(j<n)
			{
			kernel_dtrsm_nt_rl_inv_4x4_vs_lib4(j, &pD[i*sdd], &pA[j*sda], &pB[j*ps+i*sdb], &pD[j*ps+i*sdd], &pA[j*ps+j*sda], &dA[j], m-i, n-j);
			}
		}
	if(m>i)
		{
		goto left_4;
		}
#endif

	// common return if i==m
	return;

#if defined(TARGET_X64_INTEL_HASWELL)
	left_12:
	j = 0;
	for(; j<n; j+=4)
		{
		kernel_dtrsm_nt_rl_inv_12x4_vs_lib4(j, &pD[i*sdd], sdd, &pA[j*sda], &pB[j*ps+i*sdb], sdb, &pD[j*ps+i*sdd], sdd, &pA[j*ps+j*sda], &dA[j], m-i, n-j);
		}
	return;
#endif

#if defined(TARGET_X64_INTEL_HASWELL)
	left_8:
	j = 0;
	for(; j<n-8; j+=12)
		{
		kernel_dtrsm_nt_rl_inv_8x8l_vs_lib4(j, &pD[i*sdd], sdd, &pA[j*sda], sda, &pB[j*ps+i*sdb], sdb, &pD[j*ps+i*sdd], sdd, &pA[j*ps+j*sda], sda, &dA[j], m-i, n-j);
		kernel_dtrsm_nt_rl_inv_8x8u_vs_lib4((j+4), &pD[i*sdd], sdd, &pA[(j+4)*sda], sda, &pB[(j+4)*ps+i*sdb], sdb, &pD[(j+4)*ps+i*sdd], sdd, &pA[(j+4)*ps+(j+4)*sda], sda, &dA[(j+4)], m-i, n-(j+4));
		}
	if(j<n-4)
		{
		kernel_dtrsm_nt_rl_inv_8x8l_vs_lib4(j, &pD[i*sdd], sdd, &pA[j*sda], sda, &pB[j*ps+i*sdb], sdb, &pD[j*ps+i*sdd], sdd, &pA[j*ps+j*sda], sda, &dA[j], m-i, n-j);
		kernel_dtrsm_nt_rl_inv_4x4_vs_lib4((j+4), &pD[i*sdd], &pA[(j+4)*sda], &pB[(j+4)*ps+i*sdb], &pD[(j+4)*ps+i*sdd], &pA[(j+4)*ps+(j+4)*sda], &dA[(j+4)], m-i, n-(j+4));
		j += 8;
		}
	else if(j<n)
		{
		kernel_dtrsm_nt_rl_inv_8x4_vs_lib4(j, &pD[i*sdd], sdd, &pA[j*sda], &pB[j*ps+i*sdb], sdb, &pD[j*ps+i*sdd], sdd, &pA[j*ps+j*sda], &dA[j], m-i, n-j);
		j += 4;
		}
	return;
#elif defined(TARGET_X64_INTEL_SANDY_BRIDGE)
	left_8:
	j = 0;
	for(; j<n; j+=4)
		{
		kernel_dtrsm_nt_rl_inv_8x4_vs_lib4(j, &pD[i*sdd], sdd, &pA[j*sda], &pB[j*ps+i*sdb], sdb, &pD[j*ps+i*sdd], sdd, &pA[j*ps+j*sda], &dA[j], m-i, n-j);
		}
	return;
#endif

#if defined(TARGET_X64_INTEL_HASWELL)
	left_4:
	j = 0;
	for(; j<n-8; j+=12)
		{
		kernel_dtrsm_nt_rl_inv_4x12_vs_lib4(j, &pD[i*sdd], &pA[j*sda], sda, &pB[j*ps+i*sdb], &pD[j*ps+i*sdd], &pA[j*ps+j*sda], sda, &dA[j], m-i, n-j);
		}
	if(j<n-4)
		{
		kernel_dtrsm_nt_rl_inv_4x8_vs_lib4(j, &pD[i*sdd], &pA[j*sda], sda, &pB[j*ps+i*sdb], &pD[j*ps+i*sdd], &pA[j*ps+j*sda], sda, &dA[j], m-i, n-j);
		j += 8;
		}
	else if(j<n)
		{
		kernel_dtrsm_nt_rl_inv_4x4_vs_lib4(j, &pD[i*sdd], &pA[j*sda], &pB[j*ps+i*sdb], &pD[j*ps+i*sdd], &pA[j*ps+j*sda], &dA[j], m-i, n-j);
		j += 4;
		}
	return;
#elif defined(TARGET_X86_AMD_BARCELONA)
	left_4:
	j = 0;
	for(; j<n; j+=4)
		{
		kernel_dtrsm_nt_rl_inv_4x2_vs_lib4(j, &pD[i*sdd], &pA[j*sda], &pB[j*ps+i*sdb], &pD[j*ps+i*sdd], &pA[j*ps+j*sda], &dA[j], m-i, n-j);
		if(j<n-2)
		{
			kernel_dtrsm_nt_rl_inv_4x2_vs_lib4(j+2, &pD[i*sdd], &pA[j*sda+2], &pB[(j+2)*ps+i*sdb], &pD[(j+2)*ps+i*sdd], &pA[(j+2)*ps+j*sda+2], &dA[j+2], m-i, n-(j+2));
			}
		}
	return;
#else
	left_4:
	j = 0;
	for(; j<n; j+=4)
		{
		kernel_dtrsm_nt_rl_inv_4x4_vs_lib4(j, &pD[i*sdd], &pA[j*sda], &pB[j*ps+i*sdb], &pD[j*ps+i*sdd], &pA[j*ps+j*sda], &dA[j], m-i, n-j);
		}
	return;
#endif
	}



// dtrsm_right_lower_transposed_unit
void blasfeo_dtrsm_rltu(int m, int n, double alpha, struct blasfeo_dmat *sA, int ai, int aj, struct blasfeo_dmat *sB, int bi, int bj, struct blasfeo_dmat *sD, int di, int dj)
	{
	if(ai!=0 | bi!=0 | di!=0 | alpha!=1.0)
		{
		printf("\nblasfeo_dtrsm_rltu: feature not implemented yet: ai=%d, bi=%d, di=%d, alpha=%f\n", ai, bi, di, alpha);
		exit(1);
		}

	// invalidate stored inverse diagonal of result matrix
	sD->use_dA = 0;

	const int ps = 4;
	// TODO alpha
	int sda = sA->cn;
	int sdb = sB->cn;
	int sdd = sD->cn;
	double *pA = sA->pA + aj*ps;
	double *pB = sB->pA + bj*ps;
	double *pD = sD->pA + dj*ps;
	dtrsm_nt_rl_one_lib(m, n, pA, sda, pB, sdb, pD, sdd);
	return;
	}



// dtrsm_right_upper_transposed_notunit
void blasfeo_dtrsm_rutn(int m, int n, double alpha, struct blasfeo_dmat *sA, int ai, int aj, struct blasfeo_dmat *sB, int bi, int bj, struct blasfeo_dmat *sD, int di, int dj)
	{
	if(ai!=0 | bi!=0 | di!=0 | alpha!=1.0)
		{
		printf("\nblasfeo_dtrsm_rutn: feature not implemented yet: ai=%d, bi=%d, di=%d, alpha=%f\n", ai, bi, di, alpha);
		exit(1);
		}

	// invalidate stored inverse diagonal of result matrix
	sD->use_dA = 0;

	const int ps = 4;
	// TODO alpha
	int sda = sA->cn;
	int sdb = sB->cn;
	int sdd = sD->cn;
	double *pA = sA->pA + aj*ps;
	double *pB = sB->pA + bj*ps;
	double *pD = sD->pA + dj*ps;
	double *dA = sA->dA;
	int ii;
	if(ai==0 & aj==0)
		{
		if(sA->use_dA<n)
			{
			ddiaex_lib(n, 1.0, ai, pA, sda, dA);
			for(ii=0; ii<n; ii++)
				dA[ii] = 1.0 / dA[ii];
			sA->use_dA = n;
			}
		}
	else
		{
		ddiaex_lib(n, 1.0, ai, pA, sda, dA);
		for(ii=0; ii<n; ii++)
			dA[ii] = 1.0 / dA[ii];
		sA->use_dA = 0;
		}
	dtrsm_nt_ru_inv_lib(m, n, pA, sda, dA, pB, sdb, pD, sdd);
	return;
	}



// dtrmm_right_upper_transposed_notunit (B, i.e. the first matrix, is triangular !!!)
void blasfeo_dtrmm_rutn(int m, int n, double alpha, struct blasfeo_dmat *sB, int bi, int bj, struct blasfeo_dmat *sA, int ai, int aj, struct blasfeo_dmat *sD, int di, int dj)
	{
	// invalidate stored inverse diagonal of result matrix
	sD->use_dA = 0;

	if(ai!=0 | bi!=0 | di!=0)
		{
		printf("\nblasfeo_dtrmm_rutn: feature not implemented yet: ai=%d, bi=%d, di=%d\n", ai, bi, di);
		exit(1);
		}

	if(m<=0 || n<=0)
		return;

	const int ps = 4;

	int sda = sA->cn;
	int sdb = sB->cn;
	int sdd = sD->cn;
	double *pA = sA->pA + aj*ps;
	double *pB = sB->pA + bj*ps;
	double *pD = sD->pA + dj*ps;

	int i, j;

	i = 0;
#if defined(TARGET_X64_INTEL_HASWELL)
// XXX there is a bug here !!!!!!
	for(; i<m-11; i+=12)
		{
		j = 0;
		for(; j<n-3; j+=4)
			{
			kernel_dtrmm_nt_ru_12x4_lib4(n-j, &alpha, &pA[j*ps+i*sda], sda, &pB[j*ps+j*sdb], &pD[j*ps+i*sdd], sdd);
			}
		if(j<n)
			{
			kernel_dtrmm_nt_ru_12x4_vs_lib4(n-j, &alpha, &pA[j*ps+i*sda], sda, &pB[j*ps+j*sdb], &pD[j*ps+i*sdd], sdd, m-i, n-j);
			}
		}
	if(i<m)
		{
		if(m-i<5)
			{
			goto left_4;
			}
		if(m-i<9)
			{
			goto left_8;
			}
		else
			{
			goto left_12;
			}
		}

#elif defined(TARGET_X64_INTEL_SANDY_BRIDGE) || defined(TARGET_X64_INTEL_HASWELL)
	for(; i<m-7; i+=8)
		{
		j = 0;
		for(; j<n-3; j+=4)
			{
			kernel_dtrmm_nt_ru_8x4_lib4(n-j, &alpha, &pA[j*ps+i*sda], sda, &pB[j*ps+j*sdb], &pD[j*ps+i*sdd], sdd);
			}
		if(j<n)
			{
			kernel_dtrmm_nt_ru_8x4_vs_lib4(n-j, &alpha, &pA[j*ps+i*sda], sda, &pB[j*ps+j*sdb], &pD[j*ps+i*sdd], sdd, m-i, n-j);
			}
		}
	if(i<m)
		{
		if(m-i<5)
			{
			goto left_4;
			}
		else
			{
			goto left_8;
			}
		}

#else
	for(; i<m-3; i+=4)
		{
		j = 0;
		for(; j<n-3; j+=4)
			{
			kernel_dtrmm_nt_ru_4x4_lib4(n-j, &alpha, &pA[j*ps+i*sda], &pB[j*ps+j*sdb], &pD[j*ps+i*sdd]);
			}
		if(j<n)
			{
			kernel_dtrmm_nt_ru_4x4_vs_lib4(n-j, &alpha, &pA[j*ps+i*sda], &pB[j*ps+j*sdb], &pD[j*ps+i*sdd], m-i, n-j);
			}
		}
	if(i<m)
		{
		goto left_4;
		}
#endif

	// common return
	return;

#if defined(TARGET_X64_INTEL_HASWELL)
	// clean up
	left_12:
	j = 0;
	for(; j<n; j+=4)
		{
		kernel_dtrmm_nt_ru_12x4_vs_lib4(n-j, &alpha, &pA[j*ps+i*sda], sda, &pB[j*ps+j*sdb], &pD[j*ps+i*sdd], sdd, m-i, n-j);
		}
	return;
#endif

#if defined(TARGET_X64_INTEL_SANDY_BRIDGE) || defined(TARGET_X64_INTEL_HASWELL)
	// clean up
	left_8:
	j = 0;
	for(; j<n; j+=4)
		{
		kernel_dtrmm_nt_ru_8x4_vs_lib4(n-j, &alpha, &pA[j*ps+i*sda], sda, &pB[j*ps+j*sdb], &pD[j*ps+i*sdd], sdd, m-i, n-j);
		}
	return;
#endif

	left_4:
	j = 0;
	for(; j<n; j+=4)
		{
		kernel_dtrmm_nt_ru_4x4_vs_lib4(n-j, &alpha, &pA[j*ps+i*sda], &pB[j*ps+j*sdb], &pD[j*ps+i*sdd], m-i, n-j);
		}
	return;

	}



// dtrmm_right_lower_nottransposed_notunit (B, i.e. the first matrix, is triangular !!!)
void blasfeo_dtrmm_rlnn(int m, int n, double alpha, struct blasfeo_dmat *sB, int bi, int bj, struct blasfeo_dmat *sA, int ai, int aj, struct blasfeo_dmat *sD, int di, int dj)
	{

	const int ps = 4;

	int sda = sA->cn;
	int sdb = sB->cn;
	int sdd = sD->cn;
	int air = ai & (ps-1);
	int bir = bi & (ps-1);
	double *pA = sA->pA + aj*ps + (ai-air)*sda;
	double *pB = sB->pA + bj*ps + (bi-bir)*sdb;
	double *pD = sD->pA + dj*ps;

	int offsetB = bir;

	int di0 = di-air;
	int offsetD;

	// invalidate stored inverse diagonal of result matrix
	sD->use_dA = 0;

	if(di0>=0)
		{
		pD += di0/ps*ps*sdd;
		offsetD = di0%ps;
		}
	else
		{
		pD += -4*sdd;
		offsetD = ps+di0;
		}

	int ii, jj;

	if(air!=0)
		{
		jj = 0;
		for(; jj<n; jj+=4)
			{
			kernel_dtrmm_nn_rl_4x4_gen_lib4(n-jj, &alpha, &pA[jj*ps], offsetB, &pB[jj*sdb+jj*ps], sdb, offsetD, &pD[jj*ps], sdd, air, air+m, 0, n-jj);
			}
		m -= ps-air;
		pA += ps*sda;
		pD += ps*sdd;
		}
	ii = 0;
	if(offsetD==0)
		{
#if defined(TARGET_X64_INTEL_HASWELL)
		for(; ii<m-11; ii+=12)
			{
			jj = 0;
			for(; jj<n-5; jj+=4)
				{
				kernel_dtrmm_nn_rl_12x4_lib4(n-jj, &alpha, &pA[ii*sda+jj*ps], sda, offsetB, &pB[jj*sdb+jj*ps], sdb, &pD[ii*sdd+jj*ps], sdd); // n-j>=6 !!!!!
				}
			for(; jj<n; jj+=4)
				{
				kernel_dtrmm_nn_rl_12x4_vs_lib4(n-jj, &alpha, &pA[ii*sda+jj*ps], sda, offsetB, &pB[jj*sdb+jj*ps], sdb, &pD[ii*sdd+jj*ps], sdd, 12, n-jj);
				}
			}
		if(ii<m)
			{
			if(ii<m-8)
				goto left_12;
			else if(ii<m-4)
				goto left_8;
			else
				goto left_4;
			}
#elif defined(TARGET_X64_INTEL_SANDY_BRIDGE)
		for(; ii<m-7; ii+=8)
			{
			jj = 0;
			for(; jj<n-5; jj+=4)
				{
				kernel_dtrmm_nn_rl_8x4_lib4(n-jj, &alpha, &pA[ii*sda+jj*ps], sda, offsetB, &pB[jj*sdb+jj*ps], sdb, &pD[ii*sdd+jj*ps], sdd);
				}
			for(; jj<n; jj+=4)
				{
				kernel_dtrmm_nn_rl_8x4_vs_lib4(n-jj, &alpha, &pA[ii*sda+jj*ps], sda, offsetB, &pB[jj*sdb+jj*ps], sdb, &pD[ii*sdd+jj*ps], sdd, 8, n-jj);
				}
			}
		if(ii<m)
			{
			if(ii<m-4)
				goto left_8;
			else
				goto left_4;
			}
#elif defined(TARGET_X86_AMD_BARCELONA)
		for(; ii<m-3; ii+=4)
			{
			jj = 0;
			for(; jj<n-3; jj+=4)
				{
				kernel_dtrmm_nn_rl_4x2_lib4(n-jj, &alpha, &pA[ii*sda+jj*ps], offsetB, &pB[jj*sdb+jj*ps], sdb, &pD[ii*sdd+jj*ps]);
				if(offsetB+2<4)
					kernel_dtrmm_nn_rl_4x2_lib4(n-(jj+2), &alpha, &pA[ii*sda+(jj+2)*ps], offsetB+2, &pB[jj*sdb+(jj+2)*ps], sdb, &pD[ii*sdd+(jj+2)*ps]);
				else
					kernel_dtrmm_nn_rl_4x2_lib4(n-(jj+2), &alpha, &pA[ii*sda+(jj+2)*ps], offsetB+2-ps, &pB[(jj+ps)*sdb+(jj+2)*ps], sdb, &pD[ii*sdd+(jj+2)*ps]);
				}
			for(; jj<n; jj+=4)
				{
				kernel_dtrmm_nn_rl_4x2_vs_lib4(n-jj, &alpha, &pA[ii*sda+jj*ps], offsetB, &pB[jj*sdb+jj*ps], sdb, &pD[ii*sdd+jj*ps], 4, n-jj);
				if(jj<n-2)
					{
					if(offsetB+2<4)
						kernel_dtrmm_nn_rl_4x2_vs_lib4(n-(jj+2), &alpha, &pA[ii*sda+(jj+2)*ps], offsetB+2, &pB[jj*sdb+(jj+2)*ps], sdb, &pD[ii*sdd+(jj+2)*ps], 4, n-(jj+2));
					else
						kernel_dtrmm_nn_rl_4x2_vs_lib4(n-(jj+2), &alpha, &pA[ii*sda+(jj+2)*ps], offsetB+2-ps, &pB[(jj+ps)*sdb+(jj+2)*ps], sdb, &pD[ii*sdd+(jj+2)*ps], 4, n-(jj+2));
					}
				}
			}
		if(ii<m)
			{
			goto left_4;
			}
#else
		for(; ii<m-3; ii+=4)
			{
			jj = 0;
			for(; jj<n-5; jj+=4)
				{
				kernel_dtrmm_nn_rl_4x4_lib4(n-jj, &alpha, &pA[ii*sda+jj*ps], offsetB, &pB[jj*sdb+jj*ps], sdb, &pD[ii*sdd+jj*ps]);
				}
			for(; jj<n; jj+=4)
				{
				kernel_dtrmm_nn_rl_4x4_vs_lib4(n-jj, &alpha, &pA[ii*sda+jj*ps], offsetB, &pB[jj*sdb+jj*ps], sdb, &pD[ii*sdd+jj*ps], 4, n-jj);
				}
			}
		if(ii<m)
			{
			goto left_4;
			}
#endif
		}
	else
		{
#if defined(TARGET_X64_INTEL_HASWELL) || defined(TARGET_X64_INTEL_SANDY_BRIDGE)
		for(; ii<m-4; ii+=8)
			{
			jj = 0;
			for(; jj<n; jj+=4)
				{
				kernel_dtrmm_nn_rl_8x4_gen_lib4(n-jj, &alpha, &pA[ii*sda+jj*ps], sda, offsetB, &pB[jj*sdb+jj*ps], sdb, offsetD, &pD[ii*sdd+jj*ps], sdd, 0, m-ii, 0, n-jj);
				}
			}
		if(ii<m)
			{
			goto left_4_gen;
			}
#else
		for(; ii<m; ii+=4)
			{
			jj = 0;
			for(; jj<n; jj+=4)
				{
				kernel_dtrmm_nn_rl_4x4_gen_lib4(n-jj, &alpha, &pA[ii*sda+jj*ps], offsetB, &pB[jj*sdb+jj*ps], sdb, offsetD, &pD[ii*sdd+jj*ps], sdd, 0, m-ii, 0, n-jj);
				}
			}
#endif
		}

	// common return if i==m
	return;

	// clean up loops definitions

#if defined(TARGET_X64_INTEL_HASWELL)
	left_12:
	jj = 0;
	for(; jj<n; jj+=4)
		{
		kernel_dtrmm_nn_rl_12x4_vs_lib4(n-jj, &alpha, &pA[ii*sda+jj*ps], sda, offsetB, &pB[jj*sdb+jj*ps], sdb, &pD[ii*sdd+jj*ps], sdd, m-ii, n-jj);
		}
	return;
#endif

#if defined(TARGET_X64_INTEL_HASWELL) || defined(TARGET_X64_INTEL_SANDY_BRIDGE)
	left_8:
	jj = 0;
	for(; jj<n; jj+=4)
		{
		kernel_dtrmm_nn_rl_8x4_vs_lib4(n-jj, &alpha, &pA[ii*sda+jj*ps], sda, offsetB, &pB[jj*sdb+jj*ps], sdb, &pD[ii*sdd+jj*ps], sdd, m-ii, n-jj);
		}
	return;
#endif

#if defined(TARGET_X64_INTEL_HASWELL) || defined(TARGET_X64_INTEL_SANDY_BRIDGE)
	left_8_gen:
	jj = 0;
	for(; jj<n; jj+=4)
		{
		kernel_dtrmm_nn_rl_8x4_gen_lib4(n-jj, &alpha, &pA[ii*sda+jj*ps], sda, offsetB, &pB[jj*sdb+jj*ps], sdb, offsetD, &pD[ii*sdd+jj*ps], sdd, 0, m-ii, 0, n-jj);
		}
	return;
#endif

#if defined(TARGET_X86_AMD_BARCELONA)
	left_4:
	jj = 0;
	for(; jj<n; jj+=4)
		{
		kernel_dtrmm_nn_rl_4x2_vs_lib4(n-jj, &alpha, &pA[ii*sda+jj*ps], offsetB, &pB[jj*sdb+jj*ps], sdb, &pD[ii*sdd+jj*ps], m-ii, n-jj);
		if(jj<n-2)
			{
			if(offsetB+2<4)
				kernel_dtrmm_nn_rl_4x2_vs_lib4(n-(jj+2), &alpha, &pA[ii*sda+(jj+2)*ps], offsetB+2, &pB[jj*sdb+(jj+2)*ps], sdb, &pD[ii*sdd+(jj+2)*ps], m-ii, n-(jj+2));
			else
				kernel_dtrmm_nn_rl_4x2_vs_lib4(n-(jj+2), &alpha, &pA[ii*sda+(jj+2)*ps], offsetB+2-ps, &pB[(jj+ps)*sdb+(jj+2)*ps], sdb, &pD[ii*sdd+(jj+2)*ps], m-ii, n-(jj+2));
			}
		}
	return;
#else
	left_4:
	jj = 0;
	for(; jj<n; jj+=4)
		{
		kernel_dtrmm_nn_rl_4x4_vs_lib4(n-jj, &alpha, &pA[ii*sda+jj*ps], offsetB, &pB[jj*sdb+jj*ps], sdb, &pD[ii*sdd+jj*ps], m-ii, n-jj);
		}
	return;
#endif

	left_4_gen:
	jj = 0;
	for(; jj<n; jj+=4)
		{
		kernel_dtrmm_nn_rl_4x4_gen_lib4(n-jj, &alpha, &pA[ii*sda+jj*ps], offsetB, &pB[jj*sdb+jj*ps], sdb, offsetD, &pD[ii*sdd+jj*ps], sdd, 0, m-ii, 0, n-jj);
		}
	return;
	}



void blasfeo_dsyrk_ln(int m, int k, double alpha, struct blasfeo_dmat *sA, int ai, int aj, struct blasfeo_dmat *sB, int bi, int bj, double beta, struct blasfeo_dmat *sC, int ci, int cj, struct blasfeo_dmat *sD, int di, int dj)
	{
	if(m<=0)
		return;

	if(ai!=0 | bi!=0)
		{
		printf("\nblasfeo_dsyrk_ln: feature not implemented yet: ai=%d, bi=%d\n", ai, bi);
		exit(1);
		}

	// invalidate stored inverse diagonal of result matrix
	sD->use_dA = 0;

	const int ps = 4;

	int i, j;

	int sda = sA->cn;
	int sdb = sB->cn;
	int sdc = sC->cn;
	int sdd = sD->cn;
	double *pA = sA->pA + aj*ps;
	double *pB = sB->pA + bj*ps;
	double *pC = sC->pA + cj*ps + (ci-(ci&(ps-1)))*sdc;
	double *pD = sD->pA + dj*ps + (di-(di&(ps-1)))*sdd;

	// TODO ai and bi
	int offsetC;
	int offsetD;
	offsetC = ci&(ps-1);
	offsetD = di&(ps-1);

	// main loop
	i = 0;
	if(offsetC==0 & offsetD==0)
		{
#if defined(TARGET_X64_INTEL_HASWELL)
		for(; i<m-11; i+=12)
			{
			j = 0;
			for(; j<i; j+=4)
				{
				kernel_dgemm_nt_12x4_lib4(k, &alpha, &pA[i*sda], sda, &pB[j*sdb], &beta, &pC[j*ps+i*sdc], sdc, &pD[j*ps+i*sdd], sdd);
				}
			kernel_dsyrk_nt_l_12x4_lib4(k, &alpha, &pA[i*sda], sda, &pB[j*sdb], &beta, &pC[j*ps+i*sdc], sdc, &pD[j*ps+i*sdd], sdd);
			kernel_dsyrk_nt_l_8x8_lib4(k, &alpha, &pA[(i+4)*sda], sda, &pB[(j+4)*sdb], sdb, &beta, &pC[(j+4)*ps+(i+4)*sdc], sdc, &pD[(j+4)*ps+(i+4)*sdd], sdd);
			}
		if(m>i)
			{
			if(m-i<=4)
				{
				goto left_4;
				}
			else if(m-i<=8)
				{
				goto left_8;
				}
			else
				{
				goto left_12;
				}
			}
#elif defined(TARGET_X64_INTEL_SANDY_BRIDGE)
		for(; i<m-7; i+=8)
			{
			j = 0;
			for(; j<i; j+=4)
				{
				kernel_dgemm_nt_8x4_lib4(k, &alpha, &pA[i*sda], sda, &pB[j*sdb], &beta, &pC[j*ps+i*sdc], sdc, &pD[j*ps+i*sdd], sdd);
				}
			kernel_dsyrk_nt_l_8x4_lib4(k, &alpha, &pA[i*sda], sda, &pB[j*sdb], &beta, &pC[j*ps+i*sdc], sdc, &pD[j*ps+i*sdd], sdd);
			kernel_dsyrk_nt_l_4x4_lib4(k, &alpha, &pA[(i+4)*sda], &pB[(j+4)*sdb], &beta, &pC[(j+4)*ps+(i+4)*sdc], &pD[(j+4)*ps+(i+4)*sdd]);
			}
		if(m>i)
			{
			if(m-i<=4)
				{
				goto left_4;
				}
			else
				{
				goto left_8;
				}
			}
#elif defined(TARGET_X86_AMD_BARCELONA)
		for(; i<m-3; i+=4)
			{
			j = 0;
			for(; j<i; j+=4)
				{
				kernel_dgemm_nt_4x2_lib4(k, &alpha, &pA[i*sda], &pB[j*sdb], &beta, &pC[j*ps+i*sdc], &pD[j*ps+i*sdd]);
				kernel_dgemm_nt_4x2_lib4(k, &alpha, &pA[i*sda], &pB[j*sdb+2], &beta, &pC[(j+2)*ps+i*sdc], &pD[(j+2)*ps+i*sdd]);
				}
			kernel_dsyrk_nt_l_4x2_lib4(k, &alpha, &pA[i*sda], &pB[j*sdb], &beta, &pC[j*ps+i*sdc], &pD[j*ps+i*sdd]);
			kernel_dsyrk_nt_l_2x2_lib4(k, &alpha, &pA[i*sda+2], &pB[j*sdb+2], &beta, &pC[(j+2)*ps+i*sdc+2], &pD[(j+2)*ps+i*sdd+2]);
			}
		if(m>i)
			{
			goto left_4;
			}
#else
		for(; i<m-3; i+=4)
			{
			j = 0;
			for(; j<i; j+=4)
				{
				kernel_dgemm_nt_4x4_lib4(k, &alpha, &pA[i*sda], &pB[j*sdb], &beta, &pC[j*ps+i*sdc], &pD[j*ps+i*sdd]);
				}
			kernel_dsyrk_nt_l_4x4_lib4(k, &alpha, &pA[i*sda], &pB[j*sdb], &beta, &pC[j*ps+i*sdc], &pD[j*ps+i*sdd]);
			}
		if(m>i)
			{
			goto left_4;
			}
#endif
		}
	else
		{
#if defined(TARGET_X64_INTEL_HASWELL) | defined(TARGET_X64_INTEL_SANDY_BRIDGE)
		for(; i<m-4; i+=8)
			{
			j = 0;
			for(; j<i; j+=4)
				{
				kernel_dgemm_nt_8x4_gen_lib4(k, &alpha, &pA[i*sda], sda, &pB[j*sdb], &beta, offsetC, &pC[j*ps+i*sdc], sdc, offsetD, &pD[j*ps+i*sdd], sdd, 0, m-i, 0, m-j);
				}
			kernel_dsyrk_nt_l_8x4_gen_lib4(k, &alpha, &pA[i*sda], sda, &pB[j*sdb], &beta, offsetC, &pC[j*ps+i*sdc], sdc, offsetD, &pD[j*ps+i*sdd], sdd, 0, m-i, 0, m-j);
			kernel_dsyrk_nt_l_4x4_gen_lib4(k, &alpha, &pA[(i+4)*sda], &pB[(j+4)*sdb], &beta, offsetC, &pC[(j+4)*ps+(i+4)*sdc], sdc, offsetD, &pD[(j+4)*ps+(i+4)*sdd], sdd, 0, m-i-4, 0, m-j-4);
			}
		if(m>i)
			{
			goto left_4_gen;
			}
#else
		for(; i<m; i+=4)
			{
			j = 0;
			for(; j<i; j+=4)
				{
				kernel_dgemm_nt_4x4_gen_lib4(k, &alpha, &pA[i*sda], &pB[j*sdb], &beta, offsetC, &pC[j*ps+i*sdc], sdc, offsetD, &pD[j*ps+i*sdd], sdd, 0, m-i, 0, m-j);
				}
			kernel_dsyrk_nt_l_4x4_gen_lib4(k, &alpha, &pA[i*sda], &pB[j*sdb], &beta, offsetC, &pC[j*ps+i*sdc], sdc, offsetD, &pD[j*ps+i*sdd], sdd, 0, m-i, 0, m-j);
			}
#endif
		}

	// common return if i==m
	return;

	// clean up loops definitions

#if defined(TARGET_X64_INTEL_HASWELL)
	left_12:
	j = 0;
	for(; j<i; j+=4)
		{
		kernel_dgemm_nt_12x4_vs_lib4(k, &alpha, &pA[i*sda], sda, &pB[j*sdb], &beta, &pC[j*ps+i*sdc], sdc, &pD[j*ps+i*sdd], sdd, m-i, m-j);
		}
	kernel_dsyrk_nt_l_12x4_vs_lib4(k, &alpha, &pA[i*sda], sda, &pB[j*sdb], &beta, &pC[j*ps+i*sdc], sdc, &pD[j*ps+i*sdd], sdd, m-i, m-j);
	kernel_dsyrk_nt_l_8x8_vs_lib4(k, &alpha, &pA[(i+4)*sda], sda, &pB[(j+4)*sdb], sdb, &beta, &pC[(j+4)*ps+(i+4)*sdc], sdc, &pD[(j+4)*ps+(i+4)*sdd], sdd, m-i-4, m-j-4);
//	kernel_dsyrk_nt_l_4x4_vs_lib4(k, &alpha, &pA[(i+8)*sda], &pB[(j+8)*sdb], &beta, &pC[(j+8)*ps+(i+8)*sdc], &pD[(j+8)*ps+(i+8)*sdd], m-i-8, n-j-8);
	return;
#endif

#if defined(TARGET_X64_INTEL_HASWELL)
	left_8:
	j = 0;
	for(; j<i-8; j+=12)
		{
		kernel_dgemm_nt_8x8l_vs_lib4(k, &alpha, &pA[i*sda], sda, &pB[j*sdb], sdb, &beta, &pC[j*ps+i*sdc], sdc, &pD[j*ps+i*sdd], sdd, m-i, m-j);
		kernel_dgemm_nt_8x8u_vs_lib4(k, &alpha, &pA[i*sda], sda, &pB[(j+4)*sdb], sdb, &beta, &pC[(j+4)*ps+i*sdc], sdc, &pD[(j+4)*ps+i*sdd], sdd, m-i, m-(j+4));
		}
	if(j<i-4)
		{
		kernel_dgemm_nt_8x8l_vs_lib4(k, &alpha, &pA[i*sda], sda, &pB[j*sdb], sdb, &beta, &pC[j*ps+i*sdc], sdc, &pD[j*ps+i*sdd], sdd, m-i, m-j);
		kernel_dgemm_nt_4x4_vs_lib4(k, &alpha, &pA[i*sda], &pB[(j+4)*sdb], &beta, &pC[(j+4)*ps+i*sdc], &pD[(j+4)*ps+i*sdd], m-i, m-(j+4));
		j += 8;
		}
	else if(j<i)
		{
		kernel_dgemm_nt_8x4_vs_lib4(k, &alpha, &pA[i*sda], sda, &pB[j*sdb], &beta, &pC[j*ps+i*sdc], sdc, &pD[j*ps+i*sdd], sdd, m-i, m-j);
		j += 4;
		}
	kernel_dsyrk_nt_l_8x8_vs_lib4(k, &alpha, &pA[i*sda], sda, &pB[j*sdb], sdb, &beta, &pC[j*ps+i*sdc], sdc, &pD[j*ps+i*sdd], sdd, m-i, m-j);
//	kernel_dsyrk_nt_l_4x4_vs_lib4(k, &alpha, &pA[(i+4)*sda], &pB[(j+4)*sdb], &beta, &pC[(j+4)*ps+(i+4)*sdc], &pD[(j+4)*ps+(i+4)*sdd], m-i-4, n-j-4);
	return;
#elif defined(TARGET_X64_INTEL_SANDY_BRIDGE)
	left_8:
	j = 0;
	for(; j<i; j+=4)
		{
		kernel_dgemm_nt_8x4_vs_lib4(k, &alpha, &pA[i*sda], sda, &pB[j*sdb], &beta, &pC[j*ps+i*sdc], sdc, &pD[j*ps+i*sdd], sdd, m-i, m-j);
		}
	kernel_dsyrk_nt_l_8x4_vs_lib4(k, &alpha, &pA[i*sda], sda, &pB[j*sdb], &beta, &pC[j*ps+i*sdc], sdc, &pD[j*ps+i*sdd], sdd, m-i, m-j);
	kernel_dsyrk_nt_l_4x4_vs_lib4(k, &alpha, &pA[(i+4)*sda], &pB[(j+4)*sdb], &beta, &pC[(j+4)*ps+(i+4)*sdc], &pD[(j+4)*ps+(i+4)*sdd], m-i-4, m-j-4);
	return;
#endif

#if defined(TARGET_X64_INTEL_HASWELL)
	left_4:
	j = 0;
	for(; j<i-8; j+=12)
		{
		kernel_dgemm_nt_4x12_vs_lib4(k, &alpha, &pA[i*sda], &pB[j*sdb], sdb, &beta, &pC[j*ps+i*sdc], &pD[j*ps+i*sdd], m-i, m-j);
		}
	if(j<i-4)
		{
		kernel_dgemm_nt_4x8_vs_lib4(k, &alpha, &pA[i*sda], &pB[j*sdb], sdb, &beta, &pC[j*ps+i*sdc], &pD[j*ps+i*sdd], m-i, m-j);
		j += 8;
		}
	else if(j<i)
		{
		kernel_dgemm_nt_4x4_vs_lib4(k, &alpha, &pA[i*sda], &pB[j*sdb], &beta, &pC[j*ps+i*sdc], &pD[j*ps+i*sdd], m-i, m-j);
		j += 4;
		}
	kernel_dsyrk_nt_l_4x4_vs_lib4(k, &alpha, &pA[i*sda], &pB[j*sdb], &beta, &pC[j*ps+i*sdc], &pD[j*ps+i*sdd], m-i, m-j);
	return;
#elif defined(TARGET_X86_AMD_BARCELONA)
	left_4:
	j = 0;
	for(; j<i; j+=4)
		{
		kernel_dgemm_nt_4x2_vs_lib4(k, &alpha, &pA[i*sda], &pB[j*sdb], &beta, &pC[j*ps+i*sdc], &pD[j*ps+i*sdd], m-i, m-j);
		kernel_dgemm_nt_4x2_vs_lib4(k, &alpha, &pA[i*sda], &pB[j*sdb+2], &beta, &pC[(j+2)*ps+i*sdc], &pD[(j+2)*ps+i*sdd], m-i, m-(j+2));
		}
	kernel_dsyrk_nt_l_4x2_vs_lib4(k, &alpha, &pA[i*sda], &pB[j*sdb], &beta, &pC[j*ps+i*sdc], &pD[j*ps+i*sdd], m-i, m-j);
	if(j<m-2)
		kernel_dsyrk_nt_l_2x2_vs_lib4(k, &alpha, &pA[i*sda+2], &pB[j*sdb+2], &beta, &pC[(j+2)*ps+i*sdc+2], &pD[(j+2)*ps+i*sdd+2], m-(i+2), m-(j+2));
	return;
#else
	left_4:
	j = 0;
	for(; j<i; j+=4)
		{
		kernel_dgemm_nt_4x4_vs_lib4(k, &alpha, &pA[i*sda], &pB[j*sdb], &beta, &pC[j*ps+i*sdc], &pD[j*ps+i*sdd], m-i, m-j);
		}
	kernel_dsyrk_nt_l_4x4_vs_lib4(k, &alpha, &pA[i*sda], &pB[j*sdb], &beta, &pC[j*ps+i*sdc], &pD[j*ps+i*sdd], m-i, m-j);
	return;
#endif

	left_4_gen:
	j = 0;
	for(; j<i; j+=4)
		{
		kernel_dgemm_nt_4x4_gen_lib4(k, &alpha, &pA[i*sda], &pB[j*sdb], &beta, offsetC, &pC[j*ps+i*sdc], sdc, offsetD, &pD[j*ps+i*sdd], sdd, 0, m-i, 0, m-j);
		}
	kernel_dsyrk_nt_l_4x4_gen_lib4(k, &alpha, &pA[i*sda], &pB[j*sdb], &beta, offsetC, &pC[j*ps+i*sdc], sdc, offsetD, &pD[j*ps+i*sdd], sdd, 0, m-i, 0, m-j);
	return;

	}



void blasfeo_dsyrk_ln_mn(int m, int n, int k, double alpha, struct blasfeo_dmat *sA, int ai, int aj, struct blasfeo_dmat *sB, int bi, int bj, double beta, struct blasfeo_dmat *sC, int ci, int cj, struct blasfeo_dmat *sD, int di, int dj)
	{
	if(m<=0 | n<=0)
		return;

	if(ai!=0 | bi!=0)
		{
		printf("\nblasfeo_dsyrk_ln: feature not implemented yet: ai=%d, bi=%d\n", ai, bi);
		exit(1);
		}

	// invalidate stored inverse diagonal of result matrix
	sD->use_dA = 0;

	const int ps = 4;

	int i, j;

	int sda = sA->cn;
	int sdb = sB->cn;
	int sdc = sC->cn;
	int sdd = sD->cn;
	double *pA = sA->pA + aj*ps;
	double *pB = sB->pA + bj*ps;
	double *pC = sC->pA + cj*ps + (ci-(ci&(ps-1)))*sdc;
	double *pD = sD->pA + dj*ps + (di-(di&(ps-1)))*sdd;

	// TODO ai and bi
	int offsetC;
	int offsetD;
	offsetC = ci&(ps-1);
	offsetD = di&(ps-1);

	// main loop
	i = 0;
	if(offsetC==0 & offsetD==0)
		{
#if defined(TARGET_X64_INTEL_HASWELL)
		for(; i<m-11; i+=12)
			{
			j = 0;
			for(; j<i & j<n-3; j+=4)
				{
				kernel_dgemm_nt_12x4_lib4(k, &alpha, &pA[i*sda], sda, &pB[j*sdb], &beta, &pC[j*ps+i*sdc], sdc, &pD[j*ps+i*sdd], sdd);
				}
			if(j<n)
				{
				if(j<i) // dgemm
					{
					kernel_dgemm_nt_12x4_vs_lib4(k, &alpha, &pA[i*sda], sda, &pB[j*sdb], &beta, &pC[j*ps+i*sdc], sdc, &pD[j*ps+i*sdd], sdd, m-i, n-j);
					}
				else // dsyrk
					{
					if(j<n-11)
						{
						kernel_dsyrk_nt_l_12x4_lib4(k, &alpha, &pA[i*sda], sda, &pB[j*sdb], &beta, &pC[j*ps+i*sdc], sdc, &pD[j*ps+i*sdd], sdd);
						kernel_dsyrk_nt_l_8x8_lib4(k, &alpha, &pA[(i+4)*sda], sda, &pB[(j+4)*sdb], sdb, &beta, &pC[(j+4)*ps+(i+4)*sdc], sdc, &pD[(j+4)*ps+(i+4)*sdd], sdd);
						}
					else
						{
						kernel_dsyrk_nt_l_12x4_vs_lib4(k, &alpha, &pA[i*sda], sda, &pB[j*sdb], &beta, &pC[j*ps+i*sdc], sdc, &pD[j*ps+i*sdd], sdd, m-i, n-j);
						if(j<n-4)
							{
							kernel_dsyrk_nt_l_8x4_vs_lib4(k, &alpha, &pA[(i+4)*sda], sda, &pB[(j+4)*sdb], &beta, &pC[(j+4)*ps+(i+4)*sdc], sdc, &pD[(j+4)*ps+(i+4)*sdd], sdd, m-i-4, n-j-4);
							if(j<n-8)
								{
								kernel_dsyrk_nt_l_4x4_vs_lib4(k, &alpha, &pA[(i+8)*sda], &pB[(j+8)*sdb], &beta, &pC[(j+8)*ps+(i+8)*sdc], &pD[(j+8)*ps+(i+8)*sdd], m-i-8, n-j-8);
								}
							}
						}
					}
				}
			}
		if(m>i)
			{
			if(m-i<=4)
				{
				goto left_4;
				}
			else if(m-i<=8)
				{
				goto left_8;
				}
			else
				{
				goto left_12;
				}
			}
#elif defined(TARGET_X64_INTEL_SANDY_BRIDGE)
		for(; i<m-7; i+=8)
			{
			j = 0;
			for(; j<i & j<n-3; j+=4)
				{
				kernel_dgemm_nt_8x4_lib4(k, &alpha, &pA[i*sda], sda, &pB[j*sdb], &beta, &pC[j*ps+i*sdc], sdc, &pD[j*ps+i*sdd], sdd);
				}
			if(j<n)
				{
				if(j<i) // dgemm
					{
					kernel_dgemm_nt_8x4_vs_lib4(k, &alpha, &pA[i*sda], sda, &pB[j*sdb], &beta, &pC[j*ps+i*sdc], sdc, &pD[j*ps+i*sdd], sdd, m-i, n-j);
					}
				else // dsyrk
					{
					if(j<n-7)
						{
						kernel_dsyrk_nt_l_8x4_lib4(k, &alpha, &pA[i*sda], sda, &pB[j*sdb], &beta, &pC[j*ps+i*sdc], sdc, &pD[j*ps+i*sdd], sdd);
						kernel_dsyrk_nt_l_4x4_lib4(k, &alpha, &pA[(i+4)*sda], &pB[(j+4)*sdb], &beta, &pC[(j+4)*ps+(i+4)*sdc], &pD[(j+4)*ps+(i+4)*sdd]);
						}
					else
						{
						kernel_dsyrk_nt_l_8x4_vs_lib4(k, &alpha, &pA[i*sda], sda, &pB[j*sdb], &beta, &pC[j*ps+i*sdc], sdc, &pD[j*ps+i*sdd], sdd, m-i, n-j);
						if(j<n-4)
							{
							kernel_dsyrk_nt_l_4x4_vs_lib4(k, &alpha, &pA[(i+4)*sda], &pB[(j+4)*sdb], &beta, &pC[(j+4)*ps+(i+4)*sdc], &pD[(j+4)*ps+(i+4)*sdd], m-i-4, n-j-4);
							}
						}
					}
				}
			}
		if(m>i)
			{
			if(m-i<=4)
				{
				goto left_4;
				}
			else
				{
				goto left_8;
				}
			}
#elif defined(TARGET_X86_AMD_BARCELONA)
		for(; i<m-3; i+=4)
			{
			j = 0;
			for(; j<i & j<n-3; j+=4)
				{
				kernel_dgemm_nt_4x2_lib4(k, &alpha, &pA[i*sda], &pB[j*sdb], &beta, &pC[j*ps+i*sdc], &pD[j*ps+i*sdd]);
				kernel_dgemm_nt_4x2_lib4(k, &alpha, &pA[i*sda], &pB[j*sdb+2], &beta, &pC[(j+2)*ps+i*sdc], &pD[(j+2)*ps+i*sdd]);
				}
			if(j<n)
				{
				if(j<i) // dgemm
					{
					kernel_dgemm_nt_4x2_vs_lib4(k, &alpha, &pA[i*sda], &pB[j*sdb], &beta, &pC[j*ps+i*sdc], &pD[j*ps+i*sdd], m-i, n-j);
					if(j<n-2)
						kernel_dgemm_nt_4x2_vs_lib4(k, &alpha, &pA[i*sda], &pB[j*sdb+2], &beta, &pC[(j+2)*ps+i*sdc], &pD[(j+2)*ps+i*sdd], m-i, n-(j+2));
					}
				else // dsyrk
					{
					if(j<n-3)
						{
						kernel_dsyrk_nt_l_4x2_lib4(k, &alpha, &pA[i*sda], &pB[j*sdb], &beta, &pC[j*ps+i*sdc], &pD[j*ps+i*sdd]);
						kernel_dsyrk_nt_l_2x2_lib4(k, &alpha, &pA[i*sda+2], &pB[j*sdb+2], &beta, &pC[(j+2)*ps+i*sdc+2], &pD[(j+2)*ps+i*sdd+2]);
						}
					else
						{
						kernel_dsyrk_nt_l_4x2_vs_lib4(k, &alpha, &pA[i*sda], &pB[j*sdb], &beta, &pC[j*ps+i*sdc], &pD[j*ps+i*sdd], m-i, n-j);
						if(j<n-2)
							kernel_dsyrk_nt_l_2x2_vs_lib4(k, &alpha, &pA[i*sda+2], &pB[j*sdb+2], &beta, &pC[(j+2)*ps+i*sdc+2], &pD[(j+2)*ps+i*sdd+2], m-(i+2), n-(j+2));
						}
					}
				}
			}
		if(m>i)
			{
			goto left_4;
			}
#else
		for(; i<m-3; i+=4)
			{
			j = 0;
			for(; j<i & j<n-3; j+=4)
				{
				kernel_dgemm_nt_4x4_lib4(k, &alpha, &pA[i*sda], &pB[j*sdb], &beta, &pC[j*ps+i*sdc], &pD[j*ps+i*sdd]);
				}
			if(j<n)
				{
				if(j<i) // dgemm
					{
					kernel_dgemm_nt_4x4_vs_lib4(k, &alpha, &pA[i*sda], &pB[j*sdb], &beta, &pC[j*ps+i*sdc], &pD[j*ps+i*sdd], m-i, n-j);
					}
				else // dsyrk
					{
					if(j<n-3)
						{
						kernel_dsyrk_nt_l_4x4_lib4(k, &alpha, &pA[i*sda], &pB[j*sdb], &beta, &pC[j*ps+i*sdc], &pD[j*ps+i*sdd]);
						}
					else
						{
						kernel_dsyrk_nt_l_4x4_vs_lib4(k, &alpha, &pA[i*sda], &pB[j*sdb], &beta, &pC[j*ps+i*sdc], &pD[j*ps+i*sdd], m-i, n-j);
						}
					}
				}
			}
		if(m>i)
			{
			goto left_4;
			}
#endif
		}
	else
		{
#if defined(TARGET_X64_INTEL_HASWELL) | defined(TARGET_X64_INTEL_SANDY_BRIDGE)
		for(; i<m-4; i+=8)
			{
			j = 0;
			for(; j<i & j<n; j+=4)
				{
				kernel_dgemm_nt_8x4_gen_lib4(k, &alpha, &pA[i*sda], sda, &pB[j*sdb], &beta, offsetC, &pC[j*ps+i*sdc], sdc, offsetD, &pD[j*ps+i*sdd], sdd, 0, m-i, 0, n-j);
				}
			if(j<n)
				{
				kernel_dsyrk_nt_l_8x4_gen_lib4(k, &alpha, &pA[i*sda], sda, &pB[j*sdb], &beta, offsetC, &pC[j*ps+i*sdc], sdc, offsetD, &pD[j*ps+i*sdd], sdd, 0, m-i, 0, n-j);
				if(j<n-4)
					{
					kernel_dsyrk_nt_l_4x4_gen_lib4(k, &alpha, &pA[(i+4)*sda], &pB[(j+4)*sdb], &beta, offsetC, &pC[(j+4)*ps+(i+4)*sdc], sdc, offsetD, &pD[(j+4)*ps+(i+4)*sdd], sdd, 0, m-i-4, 0, n-j-4);
					}
				}
			}
		if(m>i)
			{
			goto left_4_gen;
			}
#else
		for(; i<m; i+=4)
			{
			j = 0;
			for(; j<i & j<n; j+=4)
				{
				kernel_dgemm_nt_4x4_gen_lib4(k, &alpha, &pA[i*sda], &pB[j*sdb], &beta, offsetC, &pC[j*ps+i*sdc], sdc, offsetD, &pD[j*ps+i*sdd], sdd, 0, m-i, 0, n-j);
				}
			if(j<n)
				{
				kernel_dsyrk_nt_l_4x4_gen_lib4(k, &alpha, &pA[i*sda], &pB[j*sdb], &beta, offsetC, &pC[j*ps+i*sdc], sdc, offsetD, &pD[j*ps+i*sdd], sdd, 0, m-i, 0, n-j);
				}
			}
#endif
		}

	// common return if i==m
	return;

	// clean up loops definitions

#if defined(TARGET_X64_INTEL_HASWELL)
	left_12:
	j = 0;
	for(; j<i & j<n; j+=4)
		{
		kernel_dgemm_nt_12x4_vs_lib4(k, &alpha, &pA[i*sda], sda, &pB[j*sdb], &beta, &pC[j*ps+i*sdc], sdc, &pD[j*ps+i*sdd], sdd, m-i, n-j);
		}
	if(j<n)
		{
		kernel_dsyrk_nt_l_12x4_vs_lib4(k, &alpha, &pA[i*sda], sda, &pB[j*sdb], &beta, &pC[j*ps+i*sdc], sdc, &pD[j*ps+i*sdd], sdd, m-i, n-j);
		if(j<n-4)
			{
			kernel_dsyrk_nt_l_8x4_vs_lib4(k, &alpha, &pA[(i+4)*sda], sda, &pB[(j+4)*sdb], &beta, &pC[(j+4)*ps+(i+4)*sdc], sdc, &pD[(j+4)*ps+(i+4)*sdd], sdd, m-i-4, n-j-4);
			if(j<n-8)
				{
				kernel_dsyrk_nt_l_4x4_vs_lib4(k, &alpha, &pA[(i+8)*sda], &pB[(j+8)*sdb], &beta, &pC[(j+8)*ps+(i+8)*sdc], &pD[(j+8)*ps+(i+8)*sdd], m-i-8, n-j-8);
				}
			}
		}
	return;
#endif

#if defined(TARGET_X64_INTEL_HASWELL)
	left_8:
	j = 0;
	for(; j<i-8 & j<n-8; j+=12)
		{
		kernel_dgemm_nt_8x8l_vs_lib4(k, &alpha, &pA[i*sda], sda, &pB[j*sdb], sdb, &beta, &pC[j*ps+i*sdc], sdc, &pD[j*ps+i*sdd], sdd, m-i, n-j);
		kernel_dgemm_nt_8x8u_vs_lib4(k, &alpha, &pA[i*sda], sda, &pB[(j+4)*sdb], sdb, &beta, &pC[(j+4)*ps+i*sdc], sdc, &pD[(j+4)*ps+i*sdd], sdd, m-i, n-(j+4));
		}
	if(j<i-4 & j<n-4)
		{
		kernel_dgemm_nt_8x8l_vs_lib4(k, &alpha, &pA[i*sda], sda, &pB[j*sdb], sdb, &beta, &pC[j*ps+i*sdc], sdc, &pD[j*ps+i*sdd], sdd, m-i, n-j);
		kernel_dgemm_nt_4x4_vs_lib4(k, &alpha, &pA[i*sda], &pB[(j+4)*sdb], &beta, &pC[(j+4)*ps+i*sdc], &pD[(j+4)*ps+i*sdd], m-i, n-(j+4));
		j += 8;
		}
	if(j<i & j<n)
		{
		kernel_dgemm_nt_8x4_vs_lib4(k, &alpha, &pA[i*sda], sda, &pB[j*sdb], &beta, &pC[j*ps+i*sdc], sdc, &pD[j*ps+i*sdd], sdd, m-i, n-j);
		j += 4;
		}
	if(j<n)
		{
		kernel_dsyrk_nt_l_8x4_vs_lib4(k, &alpha, &pA[i*sda], sda, &pB[j*sdb], &beta, &pC[j*ps+i*sdc], sdc, &pD[j*ps+i*sdd], sdd, m-i, n-j);
		if(j<n-4)
			{
			kernel_dsyrk_nt_l_4x4_vs_lib4(k, &alpha, &pA[(i+4)*sda], &pB[(j+4)*sdb], &beta, &pC[(j+4)*ps+(i+4)*sdc], &pD[(j+4)*ps+(i+4)*sdd], m-i-4, n-j-4);
			}
		}
	return;
#elif defined(TARGET_X64_INTEL_SANDY_BRIDGE)
	left_8:
	j = 0;
	for(; j<i & j<n; j+=4)
		{
		kernel_dgemm_nt_8x4_vs_lib4(k, &alpha, &pA[i*sda], sda, &pB[j*sdb], &beta, &pC[j*ps+i*sdc], sdc, &pD[j*ps+i*sdd], sdd, m-i, n-j);
		}
	if(j<n)
		{
		kernel_dsyrk_nt_l_8x4_vs_lib4(k, &alpha, &pA[i*sda], sda, &pB[j*sdb], &beta, &pC[j*ps+i*sdc], sdc, &pD[j*ps+i*sdd], sdd, m-i, n-j);
		if(j<n-4)
			{
			kernel_dsyrk_nt_l_4x4_vs_lib4(k, &alpha, &pA[(i+4)*sda], &pB[(j+4)*sdb], &beta, &pC[(j+4)*ps+(i+4)*sdc], &pD[(j+4)*ps+(i+4)*sdd], m-i-4, n-j-4);
			}
		}
	return;
#endif

#if defined(TARGET_X64_INTEL_HASWELL)
	left_4:
	j = 0;
	for(; j<i-8 & j<n-8; j+=12)
		{
		kernel_dgemm_nt_4x12_vs_lib4(k, &alpha, &pA[i*sda], &pB[j*sdb], sdb, &beta, &pC[j*ps+i*sdc], &pD[j*ps+i*sdd], m-i, n-j);
		}
	if(j<i-4 & j<n-4)
		{
		kernel_dgemm_nt_4x8_vs_lib4(k, &alpha, &pA[i*sda], &pB[j*sdb], sdb, &beta, &pC[j*ps+i*sdc], &pD[j*ps+i*sdd], m-i, n-j);
		j += 8;
		}
	else if(j<i & j<n)
		{
		kernel_dgemm_nt_4x4_vs_lib4(k, &alpha, &pA[i*sda], &pB[j*sdb], &beta, &pC[j*ps+i*sdc], &pD[j*ps+i*sdd], m-i, n-j);
		j += 4;
		}
	if(j<n)
		{
		kernel_dsyrk_nt_l_4x4_vs_lib4(k, &alpha, &pA[i*sda], &pB[j*sdb], &beta, &pC[j*ps+i*sdc], &pD[j*ps+i*sdd], m-i, n-j);
		}
	return;
#elif defined(TARGET_X86_AMD_BARCELONA)
	left_4:
	j = 0;
	for(; j<i & j<n; j+=4)
		{
		kernel_dgemm_nt_4x2_vs_lib4(k, &alpha, &pA[i*sda], &pB[j*sdb], &beta, &pC[j*ps+i*sdc], &pD[j*ps+i*sdd], m-i, n-j);
		if(j<n-2)
			kernel_dgemm_nt_4x2_vs_lib4(k, &alpha, &pA[i*sda], &pB[j*sdb+2], &beta, &pC[(j+2)*ps+i*sdc], &pD[(j+2)*ps+i*sdd], m-i, n-(j+2));
		}
	if(j<n)
		{
		kernel_dsyrk_nt_l_4x2_vs_lib4(k, &alpha, &pA[i*sda], &pB[j*sdb], &beta, &pC[j*ps+i*sdc], &pD[j*ps+i*sdd], m-i, n-j);
		if(j<n-2)
			kernel_dsyrk_nt_l_2x2_vs_lib4(k, &alpha, &pA[i*sda+2], &pB[j*sdb+2], &beta, &pC[(j+2)*ps+i*sdc+2], &pD[(j+2)*ps+i*sdd+2], m-(i+2), n-(j+2));
		}
	return;
#else
	left_4:
	j = 0;
	for(; j<i & j<n; j+=4)
		{
		kernel_dgemm_nt_4x4_vs_lib4(k, &alpha, &pA[i*sda], &pB[j*sdb], &beta, &pC[j*ps+i*sdc], &pD[j*ps+i*sdd], m-i, n-j);
		}
	if(j<n)
		{
		kernel_dsyrk_nt_l_4x4_vs_lib4(k, &alpha, &pA[i*sda], &pB[j*sdb], &beta, &pC[j*ps+i*sdc], &pD[j*ps+i*sdd], m-i, n-j);
		}
	return;
#endif

	left_4_gen:
	j = 0;
	for(; j<i & j<n; j+=4)
		{
		kernel_dgemm_nt_4x4_gen_lib4(k, &alpha, &pA[i*sda], &pB[j*sdb], &beta, offsetC, &pC[j*ps+i*sdc], sdc, offsetD, &pD[j*ps+i*sdd], sdd, 0, m-i, 0, n-j);
		}
	if(j<n)
		{
		kernel_dsyrk_nt_l_4x4_gen_lib4(k, &alpha, &pA[i*sda], &pB[j*sdb], &beta, offsetC, &pC[j*ps+i*sdc], sdc, offsetD, &pD[j*ps+i*sdd], sdd, 0, m-i, 0, n-j);
		}
	return;

	}



#else

#error : wrong LA choice

#endif


