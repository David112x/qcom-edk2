//
// Copyright (c) 2016, Linaro Limited
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions are met:
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of the Linaro nor the
//       names of its contributors may be used to endorse or promote products
//       derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT // 
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR 
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
// HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
INCIDENTAL, // SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
BUT NOT // LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, // DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY // THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
LIABILITY, OR TORT // (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN 
ANY WAY OUT OF THE USE // OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

    .text
    .thumb
    .syntax unified
    .align  5
ASM_GLOBAL ASM_PFX(InternalMemCompareGuid)
ASM_PFX(InternalMemCompareGuid):
    push    {r4-r6, lr}
    ldr     r2, [r0]
    ldr     r3, [r0, #4]
    ldr     r4, [r0, #8]
    ldr     r0, [r0, #12]
    cbz     r1, 1f
    ldr     r5, [r1]
    ldr     r6, [r1, #4]
    cmp     r2, r5
    it      eq
    cmpeq   r3, r6
    beq     2f

0:  movs    r0, #0
    pop     {r4-r6, pc}

1:  movs    r0, #0
    cbnz    r2, 4f
    cbnz    r3, 4f
    cbnz    r4, 4f
    cbnz    r0, 4f
    b       3f

2:  ldr     r5, [r1, #8]
    ldr     r1, [r1, #12]
    cmp     r4, r5
    it      eq
    cmpeq   r0, r1
    bne     0b

3:  movs    r0, #1
4:  pop     {r4-r6, pc}
