@ =================================================
@ ========= ARRAY FUNCTIONS - ARITHMETIC  =========
@ =================================================

    AREA    array_utils, CODE, READONLY
    THUMB
    PRESERVE8

@ ==============================================
@ = EXPORTED FUNCTIONS (for use in C)
@ ==============================================
    EXPORT  Array_GetSum
    EXPORT  Array_GetAverage
    EXPORT  Array_CountAboveAverage_Byte
    EXPORT  Array_GetMax
    EXPORT  Array_Get2ndMax
    EXPORT  Array_GetMin
    EXPORT  Array_CheckStrictlyMonotonicIncreasing
    EXPORT  Array_CheckStrictlyMonotonicDecreasing
    EXPORT  Array_CheckMonotonicNonIncreasing
    EXPORT  Array_CheckMonotonicNonDecreasing
    EXPORT  Array_CompactByteArrayIntoLittleEndianWords


@ ====================
@ = GET SUM OF ARRAY =
@ ====================
@ Sums all elements of a 32-bit integer array.
@
@ Parameters:
@   R0 = pointer to the start of the array
@   R1 = number of elements in the array (N)
@
@ Returns:
@   R0 = sum of all elements
Array_GetSum FUNCTION
    STMFD sp!, {r4-r8, r10-r11, lr}

    MOV     R5, #0               @ Sum = 0
    CMP     R1, #0
    BLE     exit_sum_loop

sum_loop
    LDR     R4, [R0], #4
    ADD     R5, R5, R4
    SUBS    R1, R1, #1
    BGT     sum_loop

exit_sum_loop
    MOV     R0, R5
    LDMFD sp!, {r4-r8, r10-r11, pc}
	ENDFUNC


@ =========================
@ = GET AVERAGE OF ARRAY =
@ =========================
@ Computes the average (integer division) of a 32-bit integer array.
@
@ Parameters:
@   R0 = pointer to the start of the array
@   R1 = number of elements in the array (N)
@
@ Returns:
@   R0 = average of the array elements (SUM / N)
Array_GetAverage FUNCTION
    STMFD sp!, {r4-r8, r10-r11, lr}

    MOV     R6, R1               @ Save original N
    MOV     R5, #0               @ Sum = 0
    CMP     R1, #0
    BLE     exit_avg_loop

avg_loop
    LDR     R4, [R0], #4
    ADD     R5, R5, R4
    SUBS    R1, R1, #1
    BGT     avg_loop

exit_avg_loop
    CMP     R6, #0
    MOVLE   R0, #0
    SDIVGT  R0, R5, R6
    LDMFD sp!, {r4-r8, r10-r11, pc}
	ENDFUNC


@ =======================================
@ = COUNT HOW MANY VALUES ABOVE AVERAGE =
@ =======================================
@ Counts how many elements in an unsigned char array are strictly greater than the average.
@
@ Parameters:
@   R0 = pointer to the array (unsigned char[])
@   R1 = number of elements in the array (N)
@
@ Returns:
@   R0 = number of elements strictly greater than the average
Array_CountAboveAverage_Byte FUNCTION
    STMFD sp!, {r4-r8, r10-r11, lr}

    MOV     r4, r0               @ Save array base
    MOV     r5, r1               @ Save N
    MOV     r6, #0               @ sum = 0
    MOV     r7, #0               @ i = 0

    CMP     r1, #0
    BLE     exit_count_avg

sum_loop_ca
    LDRB    r8, [r4, r7]
    ADD     r6, r6, r8
    ADD     r7, r7, #1
    CMP     r7, r5
    BLT     sum_loop_ca

    UDIV    r8, r6, r5           @ avg = sum / N

    MOV     r6, #0               @ counter
    MOV     r7, #0               @ i = 0

count_loop
    LDRB    r9, [r4, r7]
    CMP     r9, r8
    ADDGT   r6, r6, #1
    ADD     r7, r7, #1
    CMP     r7, r5
    BLT     count_loop

    MOV     r0, r6
    B       exit_count_avg_byte

exit_count_avg
    MOV     r0, #0
exit_count_avg_byte
    LDMFD sp!, {r4-r8, r10-r11, pc}
	ENDFUNC


@ ==============================
@ = GET MAX OF ARRAY AND INDEX =
@ ==============================
@ Finds the maximum value in a 32-bit integer array and its index.
@
@ Parameters:
@   R0 = pointer to the array
@   R1 = number of elements in the array (N)
@
@ Returns:
@   R0 = maximum value found
@   R1 = index of the maximum value
Array_GetMax FUNCTION
    STMFD sp!, {r4-r8, r10-r11, lr}

    LDR     R6, [R0], #4
    SUBS    R1, R1, #1
    MOV     R5, #0
    MOV     R7, #1
    BLE     exitMax

loopMax
    LDR     R4, [R0], #4
    CMP     R4, R6
    MOVGT   R6, R4
    MOVGT   R5, R7
    ADD     R7, R7, #1
    SUBS    R1, R1, #1
    BGT     loopMax

exitMax
    MOV     R0, R6
    MOV     R1, R5
    LDMFD sp!, {r4-r8, r10-r11, pc}
	ENDFUNC


@ ==================================
@ = GET 2ND MAX IN ARRAY AND INDEX =
@ ==================================
@ Finds the second maximum value (below a given upper limit) and its index.
@
@ Parameters:
@   R0 = pointer to the array
@   R1 = number of elements in the array (N)
@   R2 = upper limit (values >= this are ignored)
@
@ Returns:
@   R0 = second maximum value
@   R1 = index of the second maximum
Array_Get2ndMax FUNCTION
    STMFD sp!, {r4-r8, r10-r11, lr}

    MOV     R6, #-1
    MOV     R7, #0

loopSecondMax
    LDR     R4, [R0], #4
    CMP     R4, R2
    BGE     incr
    CMP     R4, R6
    MOVGT   R6, R4
    MOVGT   R5, R7
incr
    ADD     R7, R7, #1
    SUBS    R1, R1, #1
    BGT     loopSecondMax

exitSecondMax
    MOV     R0, R6
    MOV     R1, R5
    LDMFD sp!, {r4-r8, r10-r11, pc}
	ENDFUNC


@ ==============================
@ = GET MIN OF ARRAY AND INDEX =
@ ==============================
@ Finds the minimum value in a 32-bit integer array and its index.
@
@ Parameters:
@   R0 = pointer to the array
@   R1 = number of elements in the array (N)
@
@ Returns:
@   R0 = minimum value found
@   R1 = index of the minimum value
Array_GetMin FUNCTION
    STMFD sp!, {r4-r8, r10-r11, lr}

    LDR     R6, [R0], #4
    MOV     R5, #0
    MOV     R7, #1
    SUBS    R1, R1, #1
    BLE     exitMin

loopMin
    LDR     R4, [R0], #4
    CMP     R4, R6
    MOVLT   R6, R4
    MOVLT   R5, R7
    ADD     R7, R7, #1
    SUBS    R1, R1, #1
    BGT     loopMin

exitMin
    MOV     R0, R6
    MOV     R1, R5
    LDMFD sp!, {r4-r8, r10-r11, pc}
	ENDFUNC


@ ==============================================
@ = IS ARRAY STRICTLY MONOTONIC INCREASING?    =
@ ==============================================
@ Checks if the array is strictly increasing (each element > previous).
@
@ Parameters:
@   R0 = pointer to the array
@   R1 = number of elements in the array (N)
@
@ Returns:
@   R0 = 1 if strictly increasing
@        0 otherwise
Array_CheckStrictlyMonotonicIncreasing FUNCTION
    STMFD sp!, {r4-r8, r10-r11, lr}

    CMP     r1, #2
    BLT     strict_inc_true

    LDR     r4, [r0], #4
    SUBS    r1, r1, #1

strict_inc_loop
    LDR     r5, [r0], #4
    CMP     r4, r5
    MOVGE   r0, #0
    BGE     strict_inc_exit
    MOV     r4, r5
    SUBS    r1, r1, #1
    BGT     strict_inc_loop

strict_inc_true
    MOV     r0, #1
strict_inc_exit
    LDMFD sp!, {r4-r8, r10-r11, pc}
	ENDFUNC


@ ==============================================
@ = IS ARRAY STRICTLY MONOTONIC DECREASING?    =
@ ==============================================
@ Checks if the array is strictly decreasing (each element < previous).
@
@ Parameters:
@   R0 = pointer to the array
@   R1 = number of elements in the array (N)
@
@ Returns:
@   R0 = 1 if strictly decreasing
@        0 otherwise
Array_CheckStrictlyMonotonicDecreasing FUNCTION
    STMFD sp!, {r4-r8, r10-r11, lr}

    CMP     r1, #2
    BLT     strict_dec_true

    LDR     r4, [r0], #4
    SUBS    r1, r1, #1

strict_dec_loop
    LDR     r5, [r0], #4
    CMP     r4, r5
    MOVLE   r0, #0
    BLE     strict_dec_exit
    MOV     r4, r5
    SUBS    r1, r1, #1
    BGT     strict_dec_loop

strict_dec_true
    MOV     r0, #1
strict_dec_exit
    LDMFD sp!, {r4-r8, r10-r11, pc}
	ENDFUNC


@ ===============================================
@ = IS ARRAY NON-INCREASING (LOOSE DECREASING)? =
@ ===============================================
@ Checks if the array is non-increasing (each element <= previous).
@
@ Parameters:
@   R0 = pointer to the array
@   R1 = number of elements in the array (N)
@
@ Returns:
@   R0 = 1 if non-increasing (loose decreasing)
@        0 otherwise
Array_CheckMonotonicNonIncreasing FUNCTION
    STMFD sp!, {r4-r8, r10-r11, lr}

    CMP     r1, #2
    BLT     non_inc_true

    LDR     r4, [r0], #4
    SUBS    r1, r1, #1

non_inc_loop
    LDR     r5, [r0], #4
    CMP     r4, r5
    MOVLT   r0, #0
    BLT     non_inc_exit
    MOV     r4, r5
    SUBS    r1, r1, #1
    BGT     non_inc_loop

non_inc_true
    MOV     r0, #1
non_inc_exit
    LDMFD sp!, {r4-r8, r10-r11, pc}
	ENDFUNC


@ ===============================================
@ = IS ARRAY NON-DECREASING (LOOSE INCREASING)? =
@ ===============================================
@ Checks if the array is non-decreasing (each element >= previous).
@
@ Parameters:
@   R0 = pointer to the array
@   R1 = number of elements in the array (N)
@
@ Returns:
@   R0 = 1 if non-decreasing (loose increasing)
@        0 otherwise
Array_CheckMonotonicNonDecreasing FUNCTION
    STMFD sp!, {r4-r8, r10-r11, lr}

    CMP     r1, #2
    BLT     non_dec_true

    LDR     r4, [r0], #4
    SUBS    r1, r1, #1

non_dec_loop
    LDR     r5, [r0], #4
    CMP     r4, r5
    MOVGT   r0, #0
    BGT     non_dec_exit
    MOV     r4, r5
    SUBS    r1, r1, #1
    BGT     non_dec_loop

non_dec_true
    MOV     r0, #1
non_dec_exit
    LDMFD sp!, {r4-r8, r10-r11, pc}
	ENDFUNC

@ ========================================
@ = DUTY CYCLE (rescaled 0..127)         =
@ ========================================
@ unsigned char duty_cycle(
@     R0 = unsigned int  *tempi_variazioni  (array pointer)
@     R1 = unsigned int   numero_variazioni (number of elements)
@     R2 = unsigned char  valore_iniziale
@     R3 = unsigned int   tempo_totale)
@ Returns: R0 = (time_at_1 * 127) / tempo_totale
    EXPORT duty_cycle
duty_cycle FUNCTION
    STMFD sp!, {r4-r8, r10-r11, lr}

    CMP   r3, #0              @ guard: tempo_totale == 0 ?
    BEQ   ret_zero            @ avoid divide-by-zero

    MOV   r4, #0             @ r4 = t_high  = 0
    MOV   r5, #0             @ r5 = prev    = 0
    AND   r6, r2, #1         @ r6 = cur     = valore_iniziale & 1

    CMP   r1, #0             @ no variations? jump to last segment
    BEQ   last_segment

loop
    LDR   r8, [r0], #4       @ r8 = t = *p++   (load, then advance pointer)
    CMP   r6, #0             @ is current level 1 ?
    BEQ   skip_add
    SUB   r7, r8, r5         @ r7 = t - prev  (segment duration)
    ADD   r4, r4, r7         @ t_high += duration
skip_add
    MOV   r5, r8            @ prev = t
    EOR   r6, r6, #1         @ cur ^= 1  (toggle level)
    SUBS  r1, r1, #1         @ numero_variazioni--
    BNE   loop

last_segment
    CMP   r6, #0             @ final level == 1 ?
    BEQ   compute
    SUB   r7, r3, r5         @ tempo_totale - prev
    ADD   r4, r4, r7         @ add final segment

compute
    MOV   r0, #127
    MUL   r4, r4, r0        @ r4 = t_high * 127
    UDIV  r0, r4, r3        @ r0 = (t_high * 127) / tempo_totale
    B     done

ret_zero
    MOV   r0, #0

done
    LDMFD sp!, {r4-r8, r10-r11, pc}
    ENDFUNC



@ =====================================================================
@ = Array_CompactByteArrayIntoLittleEndianWords                                                       =
@ =====================================================================
@ Packs the bytes of VETT into 32-bit words of VETT_c (little endian),
@ computes the 32-bit signed sum of VETT, and reports overflow via flag.
@
@ For each output word:  VETT_c[i] = VETT[4i] | VETT[4i+1]<<8
@                                  | VETT[4i+2]<<16 | VETT[4i+3]<<24
@ If dim is not a multiple of 4, the leftover byte slots of the last
@ word are padded with 0xFF (padding bytes do NOT contribute to the sum).
@
@ Parameters:
@   R0 = VETT      (int8_t  *) input byte array
@   R1 = VETT_c    (uint32_t*) output word array (must hold ceil(dim/4) words)
@   R2 = dim       (uint32_t)  number of bytes in VETT
@   R3 = flag      (uint8_t *) <- 0 if no overflow, 0xFF (-1) on overflow
@
@ Returns:
@   R0 = 32-bit signed sum of the values of VETT
@
@ Register usage:
@   r5  = running sum            r6  = sticky overflow flag (0/1)
@   r7  = remaining word count   r8  = current word accumulator
@   r9  = global byte index      r10 = VETT source pointer
@   r11 = VETT_c dest pointer    r4  = byte shift amount (0/8/16/24)
@   r12 = scratch (loaded byte)
Array_CompactByteArrayIntoLittleEndianWords FUNCTION
    STMFD   sp!, {r4-r11, lr}       @ Save callee-saved registers

    MOV     r10, r0                 @ src = VETT
    MOV     r11, r1                 @ dst = VETT_c
    MOV     r5,  #0                 @ sum = 0
    MOV     r6,  #0                 @ overflow flag = 0
    MOV     r9,  #0                 @ idx = 0

    @ --- Early exit if array is empty ---
    CMP     r2, #0
    BLE     write_flag              @ dim <= 0 -> sum = 0, flag = 0

    @ word count = ceil(dim / 4) = (dim + 3) >> 2
    ADD     r7, r2, #3
    LSR     r7, r7, #2

word_loop
    MOV     r8, #0                  @ word accumulator = 0
    MOV     r4, #0                  @ shift amount = 0 (byte position 0)

byte_loop
    CMP     r9, r2                  @ idx < dim ?
    BGE     pad_byte

    @ --- real element: load signed, accumulate, then pack raw byte ---
    LDRSB   r12, [r10], #1          @ sign-extended load, advance src
    ADDS    r5, r5, r12             @ sum += value (updates flags)
    BVC     no_overflow             @ V clear -> no signed overflow
    MOV     r6, #1                  @ sticky overflow = 1
no_overflow
    AND     r12, r12, #0xFF         @ keep only the raw 8 bits for packing
    B       pack_byte

pad_byte
    MOV     r12, #0xFF              @ padding byte (not summed)

pack_byte
    LSL     r12, r12, r4            @ move byte to its position
    ORR     r8, r8, r12             @ merge into the word

    ADD     r9, r9, #1              @ idx++
    ADD     r4, r4, #8              @ next byte position
    CMP     r4, #32
    BLT     byte_loop

    STR     r8, [r11], #4           @ store packed word

    SUBS    r7, r7, #1
    BGT     word_loop

write_flag
    @ flag = 0 (no overflow) or -1 / 0xFF (overflow)
    CMP     r6, #0
    BEQ     flag_zero
    MVN     r12, #0                 @ r12 = 0xFFFFFFFF (-1)
    B       flag_store
flag_zero
    MOV     r12, #0
flag_store
    STRB    r12, [r3]               @ flag is a byte pointer (uint8_t*)

    MOV     r0, r5                  @ return value = sum
    LDMFD   sp!, {r4-r11, pc}
    ENDFUNC




    END

