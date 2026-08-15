@ ===========================================
@ ========= MISCELLANEOUS FUNCTIONS =========
@ ===========================================

    AREA    misc_utils, CODE, READONLY
    THUMB
    PRESERVE8

@ ==============================================
@ = EXPORTED FUNCTIONS
@ ==============================================
    EXPORT  call_svc


@ ====================
@ = CALL SUPERVISOR  =
@ ====================
@ Triggers an SVC (Supervisor Call) exception with immediate value 0x15.
@ This is typically used to enter supervisor/handler mode for testing
@ or to demonstrate exception handling.
@
@ Parameters:
@   None (this function uses no input parameters)
@
@ Returns:
@   Does not return normally (SVC switches processor mode).
@   The SVC handler is responsible for returning to the caller if needed.
call_svc FUNCTION
    STMFD sp!, {r4-r8, r10-r11, lr}

    MOV     r0, r13              @ Pass current SP (PSP) to SVC handler via R0
    SVC     #0x15                @ Trigger SVC exception with immediate 0x15

    @ Note: After SVC, execution continues in the SVC handler.
    @ If the handler returns properly, this function will continue here.

    LDMFD sp!, {r4-r8, r10-r11, pc}
	ENDFUNC

    END