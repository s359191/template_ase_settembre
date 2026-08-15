@ =======================================
@ ========= CHARACTER FUNCTIONS =========
@ =======================================

    AREA    char_utils, CODE, READONLY
    THUMB
    PRESERVE8

@ ==============================================
@ = EXPORTED FUNCTIONS
@ ==============================================
    EXPORT  Char_CheckLowercase
    EXPORT  Char_CheckUppercase


@ ====================
@ = CHECK LOWERCASE  =
@ ====================
@ Checks if the input character is a lowercase letter (a-z)
@
@ Parameters:
@   R0 = character to check (ASCII value)
@
@ Returns:
@   R0 = 1 if the character is lowercase (a-z)
@        0 otherwise
Char_CheckLowercase FUNCTION
    STMFD sp!, {r4-r8, r10-r11, lr}

    CMP     r0, #'a'
    BLT     not_lowercase
    CMP     r0, #'z'
    BGT     not_lowercase

    MOV     r0, #1               @ Return 1 (true)
    B       exit_char_lower

not_lowercase
    MOV     r0, #0               @ Return 0 (false)

exit_char_lower
    LDMFD sp!, {r4-r8, r10-r11, pc}
	ENDFUNC


@ ====================
@ = CHECK UPPERCASE  =
@ ====================
@ Checks if the input character is an uppercase letter (A-Z)
@
@ Parameters:
@   R0 = character to check (ASCII value)
@
@ Returns:
@   R0 = 1 if the character is uppercase (A-Z)
@        0 otherwise
Char_CheckUppercase FUNCTION
    STMFD sp!, {r4-r8, r10-r11, lr}

    CMP     r0, #'A'
    BLT     not_uppercase
    CMP     r0, #'Z'
    BGT     not_uppercase

    MOV     r0, #1               @ Return 1 (true)
    B       exit_char_upper

not_uppercase
    MOV     r0, #0               @ Return 0 (false)

exit_char_upper
    LDMFD sp!, {r4-r8, r10-r11, pc}
	ENDFUNC

    END