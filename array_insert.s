@ ==================================================
@ ========= ARRAY FUNCTIONS - INSERT VALUE =========
@ ==================================================

    AREA    array_insert_utils, CODE, READONLY
    THUMB
    PRESERVE8

@ ==============================================
@ = EXPORTED FUNCTIONS
@ ==============================================
    EXPORT  Array_InsertOrderedIncreasing_Byte
    EXPORT  Array_InsertOrderedIncreasing_Word
    EXPORT  Array_InsertOrderedDecreasing_Byte
    EXPORT  Array_InsertOrderedDecreasing_Word
    EXPORT  Array_InsertAtBeginning_Byte
    EXPORT  Array_InsertAtBeginning_Word
    EXPORT  Array_InsertAtEnd_Byte
    EXPORT  Array_InsertAtEnd_Word
    EXPORT  Array_InsertAtIndex_Byte
    EXPORT  Array_InsertAtIndex_Word


@ ===========================
@ = INSERT VALUE INTO ARRAY =
@ ===========================
@ Inserts a new value into a sorted (ascending) byte array while maintaining order.
@ Elements greater than the new value are shifted right.
@
@ Parameters:
@   R0 = pointer to the sorted byte array (must have space for at least N+1 elements)
@   R1 = current number of elements (N)
@   R2 = new value to insert (byte)
@
@ Returns:
@   None (array is modified in place)
Array_InsertOrderedIncreasing_Byte FUNCTION
    STMFD sp!, {r4-r7, lr}

    SUB     r4, r1, #1           @ i = N-1 (start from last element)

loop_insert
    LDRB    r5, [r0, r4]         @ load A[i]
    CMP     r5, r2
    BLE     insert_here          @ if A[i] <= new value, stop shifting

    ADD     r6, r4, #1
    STRB    r5, [r0, r6]         @ shift A[i] to A[i+1]
    SUBS    r4, r4, #1
    BGE     loop_insert

insert_here
    ADD     r4, r4, #1
    STRB    r2, [r0, r4]         @ insert new value at correct position

    LDMFD sp!, {r4-r7, pc}
	ENDFUNC


@ ===========================
@ = INSERT VALUE INTO ARRAY (WORD, ASCENDING) =
@ ===========================
@ Inserts a new value into a sorted (ascending) word array while maintaining order.
@ Elements greater than the new value are shifted right.
@
@ Parameters:
@   R0 = pointer to the sorted word array (must have space for at least N+1 elements)
@   R1 = current number of elements (N)
@   R2 = new value to insert (word)
@
@ Returns:
@   None (array is modified in place)
Array_InsertOrderedIncreasing_Word FUNCTION
    STMFD sp!, {r4-r7, lr}

    SUB     r4, r1, #1           @ i = N-1 (start from last element)

loop_insert_inc_w
    LDR     r5, [r0, r4, LSL #2] @ load A[i]
    CMP     r5, r2
    BLE     insert_here_inc_w    @ if A[i] <= new value, stop shifting

    ADD     r6, r4, #1
    STR     r5, [r0, r6, LSL #2] @ shift A[i] to A[i+1]
    SUBS    r4, r4, #1
    BGE     loop_insert_inc_w

insert_here_inc_w
    ADD     r4, r4, #1
    STR     r2, [r0, r4, LSL #2] @ insert new value at correct position

    LDMFD sp!, {r4-r7, pc}
	ENDFUNC


@ ===========================
@ = INSERT VALUE INTO ARRAY (BYTE, DESCENDING) =
@ ===========================
@ Inserts a new value into a sorted (descending) byte array while maintaining order.
@ Elements smaller than the new value are shifted right.
@
@ Parameters:
@   R0 = pointer to the sorted byte array (must have space for at least N+1 elements)
@   R1 = current number of elements (N)
@   R2 = new value to insert (byte)
@
@ Returns:
@   None (array is modified in place)
Array_InsertOrderedDecreasing_Byte FUNCTION
    STMFD sp!, {r4-r7, lr}

    SUB     r4, r1, #1           @ i = N-1 (start from last element)

loop_insert_dec_b
    LDRB    r5, [r0, r4]         @ load A[i]
    CMP     r5, r2
    BGE     insert_here_dec_b    @ if A[i] >= new value, stop shifting

    ADD     r6, r4, #1
    STRB    r5, [r0, r6]         @ shift A[i] to A[i+1]
    SUBS    r4, r4, #1
    BGE     loop_insert_dec_b

insert_here_dec_b
    ADD     r4, r4, #1
    STRB    r2, [r0, r4]         @ insert new value at correct position

    LDMFD sp!, {r4-r7, pc}
	ENDFUNC


@ ===========================
@ = INSERT VALUE INTO ARRAY (WORD, DESCENDING) =
@ ===========================
@ Inserts a new value into a sorted (descending) word array while maintaining order.
@ Elements smaller than the new value are shifted right.
@
@ Parameters:
@   R0 = pointer to the sorted word array (must have space for at least N+1 elements)
@   R1 = current number of elements (N)
@   R2 = new value to insert (word)
@
@ Returns:
@   None (array is modified in place)
Array_InsertOrderedDecreasing_Word FUNCTION
    STMFD sp!, {r4-r7, lr}

    SUB     r4, r1, #1           @ i = N-1 (start from last element)

loop_insert_dec_w
    LDR     r5, [r0, r4, LSL #2] @ load A[i]
    CMP     r5, r2
    BGE     insert_here_dec_w    @ if A[i] >= new value, stop shifting

    ADD     r6, r4, #1
    STR     r5, [r0, r6, LSL #2] @ shift A[i] to A[i+1]
    SUBS    r4, r4, #1
    BGE     loop_insert_dec_w

insert_here_dec_w
    ADD     r4, r4, #1
    STR     r2, [r0, r4, LSL #2] @ insert new value at correct position

    LDMFD sp!, {r4-r7, pc}
	ENDFUNC


@ =====================================================
@ = INSERT AT BEGINNING (shifts all elements right)   =
@ =====================================================
@ Inserts a value at the beginning of a byte array by shifting all elements right.
@
@ Parameters:
@   R0 = pointer to the byte array
@   R1 = current number of elements (N)
@   R2 = value to insert at the beginning
@
@ Returns:
@   None (array is modified in place)
Array_InsertAtBeginning_Byte FUNCTION
    STMFD sp!, {r4-r7, lr}

    SUBS    r4, r1, #1           @ i = N-1
    BLT     insert_begin_exit    @ if N == 0, just insert at [0]

shift_right
    LDRB    r5, [r0, r4]
    ADD     r6, r4, #1
    STRB    r5, [r0, r6]         @ shift element right
    SUBS    r4, r4, #1
    BGE     shift_right

    STRB    r2, [r0]             @ insert new value at position 0

insert_begin_exit
    LDMFD sp!, {r4-r7, pc}
	ENDFUNC


@ =====================================================
@ = INSERT AT BEGINNING (WORD, shifts all right)      =
@ =====================================================
@ Inserts a value at the beginning of a word array by shifting all elements right.
@
@ Parameters:
@   R0 = pointer to the word array
@   R1 = current number of elements (N)
@   R2 = value to insert at the beginning
@
@ Returns:
@   None (array is modified in place)
Array_InsertAtBeginning_Word FUNCTION
    STMFD sp!, {r4-r7, lr}

    SUBS    r4, r1, #1           @ i = N-1
    BLT     insert_begin_w_exit  @ if N == 0, just insert at [0]

shift_right_w
    LDR     r5, [r0, r4, LSL #2]
    ADD     r6, r4, #1
    STR     r5, [r0, r6, LSL #2] @ shift element right
    SUBS    r4, r4, #1
    BGE     shift_right_w

    STR     r2, [r0]             @ insert new value at position 0

insert_begin_w_exit
    LDMFD sp!, {r4-r7, pc}
	ENDFUNC


@ =====================================================
@ = INSERT AT END (if space available)                =
@ =====================================================
@ Inserts a value at the end of a byte array.
@
@ Parameters:
@   R0 = pointer to the byte array
@   R1 = current number of elements (N) → insertion index
@   R2 = value to insert at the end
@
@ Returns:
@   None (array is modified in place)
Array_InsertAtEnd_Byte FUNCTION
    STMFD sp!, {r4, lr}

    STRB    r2, [r0, r1]         @ VETT[N] = val

    LDMFD sp!, {r4, pc}
	ENDFUNC


@ =====================================================
@ = INSERT AT END (WORD, if space available)          =
@ =====================================================
@ Inserts a value at the end of a word array.
@
@ Parameters:
@   R0 = pointer to the word array
@   R1 = current number of elements (N) → insertion index
@   R2 = value to insert at the end
@
@ Returns:
@   None (array is modified in place)
Array_InsertAtEnd_Word FUNCTION
    STMFD sp!, {r4, lr}

    STR     r2, [r0, r1, LSL #2] @ VETT[N] = val

    LDMFD sp!, {r4, pc}
	ENDFUNC


@ =====================================================
@ = INSERT AT INDEX (BYTE)                            =
@ =====================================================
@ Inserts a value at an arbitrary index, shifting elements
@ from that index onward one position to the right.
@
@ Parameters:
@   R0 = pointer to the byte array (must have space for N+1 elements)
@   R1 = current number of elements (N)
@   R2 = value to insert
@   R3 = index at which to insert (assumed 0 <= index <= N)
@
@ Returns:
@   None (array is modified in place)
Array_InsertAtIndex_Byte FUNCTION
    STMFD sp!, {r4-r6, lr}

    SUBS    r4, r1, #1           @ i = N-1

ins_idx_b_loop
    CMP     r4, r3
    BLT     ins_idx_b_place      @ stop once below target index
    LDRB    r5, [r0, r4]
    ADD     r6, r4, #1
    STRB    r5, [r0, r6]         @ shift A[i] to A[i+1]
    SUBS    r4, r4, #1
    B       ins_idx_b_loop

ins_idx_b_place
    STRB    r2, [r0, r3]         @ insert value at index

    LDMFD sp!, {r4-r6, pc}
	ENDFUNC


@ =====================================================
@ = INSERT AT INDEX (WORD)                            =
@ =====================================================
@ Inserts a value at an arbitrary index, shifting elements
@ from that index onward one position to the right.
@
@ Parameters:
@   R0 = pointer to the word array (must have space for N+1 elements)
@   R1 = current number of elements (N)
@   R2 = value to insert
@   R3 = index at which to insert (assumed 0 <= index <= N)
@
@ Returns:
@   None (array is modified in place)
Array_InsertAtIndex_Word FUNCTION
    STMFD sp!, {r4-r6, lr}

    SUBS    r4, r1, #1           @ i = N-1

ins_idx_w_loop
    CMP     r4, r3
    BLT     ins_idx_w_place      @ stop once below target index
    LDR     r5, [r0, r4, LSL #2]
    ADD     r6, r4, #1
    STR     r5, [r0, r6, LSL #2] @ shift A[i] to A[i+1]
    SUBS    r4, r4, #1
    B       ins_idx_w_loop

ins_idx_w_place
    STR     r2, [r0, r3, LSL #2] @ insert value at index

    LDMFD sp!, {r4-r6, pc}
	ENDFUNC

    END