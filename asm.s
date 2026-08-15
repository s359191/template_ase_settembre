    AREA    my_asm_function, CODE, READONLY
    THUMB
    PRESERVE8

@ ==============================================
@ = IMPORTS AND EXPORTS
@ ==============================================
	@ <<< IMPORTs HERE >>>
    EXPORT  EXAM_FUNCTION_NAME


@ =====================================
@ = GENERAL PURPOSE ASSEMBLY FUNCTION =
@ =====================================
@ Use this as your main function template for exams.
@
@ Parameters:
@   R0 = first parameter
@   R1 = second parameter
@   R2 = third parameter
@   R3 = fourth parameter
@   [stack] = 5th and further parameters (if needed)
@
@ Returns:
@   R0 = return value
EXAM_FUNCTION_NAME FUNCTION
    STMFD sp!, {r4-r8, r10-r11, lr}     @ Save callee-saved registers

    @ If you need more than 4 parameters, access them from the stack:
    @ LDR   r4, [sp, #32]                @ 5th parameter
    @ LDR   r5, [sp, #36]                @ 6th parameter


    @ --- Early exit if array is empty ---
    @ CMP NUM_ELEMENTS, #0
    BEQ exit

	@ --- Local Variables ---
	@ MOV r10, ARRAY_PTR					@ Current element address


main_loop
	@ --- Elements Loading and bookkeeping ---
    LDR     r7, [r10], #4        @ Load current element
    @ LDR     r8, [r10]        @ Load next element


    @ ================= YOUR LOGIC HERE =================


	@ --- main_loop iterations check ---
    @ SUBS    NUM_ELEMENTS, NUM_ELEMENTS, #1
    BGT     main_loop

exit
    @ Return value @ R0
    @ MOV   r0, RESULT_REG

    LDMFD sp!, {r4-r8, r10-r11, pc}
    ENDFUNC

    END