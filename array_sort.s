@ =============================================
@ ========= ARRAY FUNCTIONS - SORTING =========
@ =============================================

    AREA    array_sort_utils, CODE, READONLY
    THUMB
    PRESERVE8

@ ==============================================
@ = EXPORTED FUNCTIONS
@ ==============================================
    EXPORT  Array_BubbleSortIncreasing_Byte
    EXPORT  Array_BubbleSortIncreasing_Word
    EXPORT  Array_BubbleSortDecreasing_Byte
    EXPORT  Array_BubbleSortDecreasing_Word
    EXPORT  Array_Reverse_Byte
	EXPORT  Array_Reverse_Word


@ ============================================
@ = BUBBLE SORT - INCREASING (BYTE ARRAY)
@ ============================================
@ Sorts an unsigned char array in ascending order.
@
@ Parameters:
@   R0 = pointer to byte array
@   R1 = number of elements (N)
@
@ Returns: None
Array_BubbleSortIncreasing_Byte FUNCTION
    STMFD sp!, {r4-r8, r10-r11, lr}

    SUBS    r1, r1, #1
    BLE     sort_inc_byte_exit

    MOV     r5, #1                     @ swapped flag

while_inc_byte
    CMP     r5, #1
    BNE     sort_inc_byte_exit
    MOV     r5, #0
    MOV     r6, #0                     @ i = 0

for_inc_byte
    ADD 	r8, r0, r6				@ current element index
	LDRB    r4, [r8]
    LDRB    r7, [r8, #1]
    CMP     r7, r4
    STRBLT  r4, [r8, #1]
    STRBLT  r7, [r8]
    MOVLT   r5, #1
    ADD     r6, r6, #1
    CMP     r6, r1
    BLT     for_inc_byte
    B       while_inc_byte

sort_inc_byte_exit
    LDMFD sp!, {r4-r8, r10-r11, pc}
	ENDFUNC


@ ============================================
@ = BUBBLE SORT - INCREASING (WORD ARRAY)
@ ============================================
@ Sorts a 32-bit integer array in ascending order.
@
@ Parameters:
@   R0 = pointer to int array
@   R1 = number of elements (N)
@
@ Returns: None
Array_BubbleSortIncreasing_Word FUNCTION
    STMFD sp!, {r4-r8, r10-r11, lr}

    SUBS    r1, r1, #1
    BLE     sort_inc_exit

    MOV     r11, #1                    @ swapped flag

while_inc
    CMP     r11, #1
    BNE     sort_inc_exit
    MOV     r11, #0
    MOV     r5, r1
    MOV     r6, #0

for_inc
    LDR     r4, [r0, r6, LSL #2]       @ A[i]
    ADD     r8, r0, r6, LSL #2
    LDR     r7, [r8, #4]               @ A[i+1]

    CMP     r7, r4
    STRGT   r4, [r8, #4]
    STRGT   r7, [r0, r6, LSL #2]
    MOVGT   r11, #1

    ADD     r6, r6, #1
    SUBS    r5, r5, #1
    BGT     for_inc
    B       while_inc

sort_inc_exit
    LDMFD sp!, {r4-r8, r10-r11, pc}
	ENDFUNC


@ ============================================
@ = BUBBLE SORT - DECREASING (BYTE ARRAY)
@ ============================================
@ Sorts an unsigned char array in descending order.
@
@ Parameters:
@   R0 = pointer to byte array
@   R1 = number of elements (N)
@
@ Returns: None
Array_BubbleSortDecreasing_Byte FUNCTION
    STMFD sp!, {r4-r8, r10-r11, lr}

    SUBS    r1, r1, #1
    BLE     sort_dec_byte_exit

    MOV     r5, #1                     @ swapped flag

while_dec_byte
    CMP     r5, #1
    BNE     sort_dec_byte_exit
    MOV     r5, #0
    MOV     r6, #0

for_dec_byte
	ADD 	r8, r0, r6				@ current element index
    LDRB    r4, [r8]
    LDRB    r7, [r8, #1]
    CMP     r4, r7
    STRBLT  r4, [r8, #1]
    STRBLT  r7, [r8]
    MOVLT   r5, #1
    ADD     r6, r6, #1
    CMP     r6, r1
    BLT     for_dec_byte
    B       while_dec_byte

sort_dec_byte_exit
    LDMFD sp!, {r4-r8, r10-r11, pc}
	ENDFUNC


@ ============================================
@ = BUBBLE SORT - DECREASING (WORD ARRAY)
@ ============================================
@ Sorts a 32-bit integer array in descending order.
@
@ Parameters:
@   R0 = pointer to int array
@   R1 = number of elements (N)
@
@ Returns: None
Array_BubbleSortDecreasing_Word FUNCTION
    STMFD sp!, {r4-r8, r10-r11, lr}

    SUBS    r1, r1, #1
    BLE     sort_dec_exit

    MOV     r11, #1                    @ swapped flag

while_dec
    CMP     r11, #1
    BNE     sort_dec_exit
    MOV     r11, #0
    MOV     r5, r1
    MOV     r6, #0

for_dec
    LDR     r4, [r0, r6, LSL #2]       @ A[i]
    ADD     r8, r0, r6, LSL #2
    LDR     r7, [r8, #4]               @ A[i+1]

    CMP     r4, r7
    STRGT   r4, [r8, #4]
    STRGT   r7, [r0, r6, LSL #2]
    MOVGT   r11, #1

    ADD     r6, r6, #1
    SUBS    r5, r5, #1
    BGT     for_dec
    B       while_dec

sort_dec_exit
    LDMFD sp!, {r4-r8, r10-r11, pc}
	ENDFUNC

@ =====================================================
@ = REVERSE ARRAY (in place) - byte version           =
@ =====================================================
@ Reverses a byte array in place.
@
@ Parameters:
@   R0 = pointer to the byte array
@   R1 = number of elements in the array (N)
@
@ Returns:
@   None (array is reversed in place)
Array_Reverse_Byte FUNCTION
    STMFD sp!, {r4-r7, lr}

    MOV     r4, #0                 @ left index
    SUB     r5, r1, #1             @ right index = N-1

rev_loop_b
    CMP     r4, r5
    BGE     rev_exit_b
    LDRB    r6, [r0, r4]
    LDRB    r7, [r0, r5]
    STRB    r7, [r0, r4]
    STRB    r6, [r0, r5]
    ADD     r4, r4, #1
    SUB     r5, r5, #1
    B       rev_loop_b

rev_exit_b
    LDMFD sp!, {r4-r7, pc}
	ENDFUNC


@ =====================================================
@ = REVERSE ARRAY (in place) - word version           =
@ =====================================================
@ Reverses a word array in place.
@
@ Parameters:
@   R0 = pointer to the word array
@   R1 = number of elements in the array (N)
@
@ Returns:
@   None (array is reversed in place)
Array_Reverse_Word FUNCTION
    STMFD sp!, {r4-r7, lr}

    MOV     r4, #0                 @ left index
    SUB     r5, r1, #1             @ right index = N-1

rev_loop_w
    CMP     r4, r5
    BGE     rev_exit_w

    LDR     r6, [r0, r4, LSL #2]   @ Load A[left]
    LDR     r7, [r0, r5, LSL #2]   @ Load A[right]

    STR     r7, [r0, r4, LSL #2]   @ Store to A[left]
    STR     r6, [r0, r5, LSL #2]   @ Store to A[right]

    ADD     r4, r4, #1             @ increment index
    SUB     r5, r5, #1             @ decrement index
    B       rev_loop_w

rev_exit_w
    LDMFD sp!, {r4-r7, pc}
	ENDFUNC

    END