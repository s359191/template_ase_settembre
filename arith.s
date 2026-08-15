@ ========================================
@ ========= ARITHMETIC FUNCTIONS =========
@ ========================================

    AREA    arithmetic_utils, CODE, READONLY
    THUMB
    PRESERVE8

@ ==============================================
@ = EXPORTED FUNCTIONS
@ ==============================================
    EXPORT  Arith_GetModulus
    EXPORT  Arith_GetAbs
    EXPORT  Arith_CheckValueIsInRange
    EXPORT  Arith_FPDIV


@ ====================
@ = MODULUS OPERATOR =
@ ====================
@ Computes remainder using hardware division (udiv + mls)
@
@ Parameters:
@   R0 = dividend (a)
@   R1 = divisor  (b)
@
@ Returns:
@   R0 = remainder (a % b)
Arith_GetModulus FUNCTION
    STMFD sp!, {r4-r8, r10-r11, lr}

    UDIV    r3, r0, r1           @ r3 = a / b
    MLS     r0, r3, r1, r0       @ r0 = a - (r3 * b)  → remainder

    LDMFD sp!, {r4-r8, r10-r11, pc}
	ENDFUNC


@ ======================
@ = GET ABSOLUTE VALUE =
@ ======================
@ Computes |R0 - R1|
@
@ Parameters:
@   R0 = first value
@   R1 = second value
@
@ Returns:
@   R0 = absolute difference |R0 - R1|
Arith_GetAbs FUNCTION
    STMFD sp!, {r4-r8, r10-r11, lr}

    CMP     r0, r1
    RSBLT   r0, r0, r1           @ if r0 < r1 → r0 = r1 - r0
    SUBGE   r0, r0, r1           @ if r0 >= r1 → r0 = r0 - r1

    LDMFD sp!, {r4-r8, r10-r11, pc}
	ENDFUNC


@ ========================
@ = CHECK VALUE IN RANGE =
@ ========================
@ Checks if a value is within [MIN, MAX]
@
@ Parameters:
@   R0 = VALUE to check
@   R1 = MIN value
@   R2 = MAX value
@
@ Returns:
@   R0 = 1 if MIN <= VALUE <= MAX
@        0 otherwise
Arith_CheckValueIsInRange FUNCTION
    STMFD sp!, {r4-r8, r10-r11, lr}

    CMP     r0, r1
    BLO     out_of_range
    CMP     r0, r2
    BHI     out_of_range

    MOV     r0, #1               @ inside range
    B       exit_range_check

out_of_range
    MOV     r0, #0               @ outside range

exit_range_check
    LDMFD sp!, {r4-r8, r10-r11, pc}
	ENDFUNC


@ ===============
@ = FP DIVISION =
@ ===============
@ Floating-point division using __aeabi_fdiv
@
@ Parameters:
@   R0 = pointer to first float (e.g. area)
@   R1 = pointer to second float (e.g. radius^2)
@
@ Returns:
@   R0 = result of division (float)
	IMPORT __aeabi_fdiv
Arith_FPDIV FUNCTION
    STMFD sp!, {r4-r8, r10-r11, lr}

    LDR     r4, [r0]             @ load first float
    LDR     r5, [r1]             @ load second float

    MOV     r0, r4
    MOV     r1, r5

    BL      __aeabi_fdiv         @ R0 = r0 / r1 (float result)

    LDMFD sp!, {r4-r8, r10-r11, pc}
	ENDFUNC

    END