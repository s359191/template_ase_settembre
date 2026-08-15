@ =====================================
@ ========= BITWISE FUNCTIONS =========
@ =====================================

    AREA    bitwise_utils, CODE, READONLY
    THUMB
    PRESERVE8

@ ==============================================
@ = EXPORTED FUNCTIONS
@ ==============================================
    EXPORT  Bitwise_GetCountSetBits_V1
    EXPORT  Bitwise_GetCountSetBits_V2
    EXPORT  Bitwise_GetC2_32bit
    EXPORT  Bitwise_GetC2_64bit
    EXPORT  Bitwise_GetCountLeadingZero


@ ====================
@ = COUNT SET BITS   =
@ ====================
@ Brian Kernighan's algorithm (fast and efficient)
@
@ Parameters:
@   R0 = input number (32-bit)
@
@ Returns:
@   R0 = number of set bits (1s) in the input
Bitwise_GetCountSetBits_V1 FUNCTION
    STMFD sp!, {r4-r5, lr}

    MOV     r4, #0               @ counter = 0

count_loop_v1
    CMP     r0, #0
    BEQ     exit_count_v1

    SUB     r5, r0, #1
    AND     r0, r0, r5           @ n = n & (n-1)
    ADD     r4, r4, #1
    B       count_loop_v1

exit_count_v1
    MOV     r0, r4               @ return count
    LDMFD sp!, {r4-r5, pc}
	ENDFUNC


@ ==================
@ = COUNT SET BITS =
@ ==================
@ Simple bit-by-bit counting method (always 32 iterations)
@
@ Parameters:
@   R0 = input number (32-bit)
@
@ Returns:
@   R0 = number of set bits (1s) in the input
Bitwise_GetCountSetBits_V2 FUNCTION
    STMFD sp!, {r4-r8, r10-r11, lr}

    MOV     r1, #32              @ 32 bits to check
    MOV     r2, #0               @ counter = 0

loop_count_bits
    LSLS    r0, r0, #1
    ADDCS   r2, r2, #1           @ if carry flag is set, increment counter
    SUBS    r1, r1, #1
    BNE     loop_count_bits

    MOV     r0, r2               @ return count
    LDMFD sp!, {r4-r8, r10-r11, pc}
	ENDFUNC


@ =========================
@ = 32-BIT 2'S COMPLEMENT =
@ =========================
@ Computes the two's complement of a 32-bit number
@
@ Parameters:
@   R0 = input number (32-bit)
@
@ Returns:
@   R0 = two's complement of the input number
Bitwise_GetC2_32bit FUNCTION
    STMFD sp!, {r4-r8, r10-r11, lr}

    MVN     r0, r0               @ bitwise NOT
    ADD     r0, r0, #1           @ add 1

    LDMFD sp!, {r4-r8, r10-r11, pc}
	ENDFUNC


@ =========================
@ = 64-BIT 2'S COMPLEMENT =
@ =========================
@ Computes the two's complement of a 64-bit number
@
@ Parameters:
@   R0 = upper 32 bits of the input number
@   R1 = lower 32 bits of the input number
@
@ Returns:
@   R0 = upper 32 bits of the result
@   R1 = lower 32 bits of the result
Bitwise_GetC2_64bit FUNCTION
    STMFD sp!, {r4-r8, r10-r11, lr}

    MVN     r0, r0               @ invert upper bits
    MVN     r1, r1               @ invert lower bits

    ADDS    r1, r1, #1           @ add 1 to lower 32 bits
    BVC     no_carry
    ADD     r0, r0, #1           @ propagate carry to upper bits if needed

no_carry
    LDMFD sp!, {r4-r8, r10-r11, pc}
	ENDFUNC


@ ======================
@ = COUNT LEADING ZERO =
@ ======================
@ Counts the number of leading zeros in a 32-bit value
@
@ Parameters:
@   R0 = input value (32-bit)
@
@ Returns:
@   R0 = number of leading zeros (0 to 32)
Bitwise_GetCountLeadingZero FUNCTION
    STMFD sp!, {r4-r8, r10-r11, lr}

    CLZ     r0, r0

    LDMFD sp!, {r4-r8, r10-r11, pc}
	ENDFUNC

    END