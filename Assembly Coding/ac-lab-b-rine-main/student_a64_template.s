  // This is the general format of an assembly-language program file.
    // Written by: Brian Nguyen - bn6652
    .arch armv8-a
    .text
    // Code for all functions go here.



    // ***** WEEK 1 deliverables *****



    // Every function starts from the .align below this line ...
    .align  2
    .p2align 3,,7
    .global ntz
    .type   ntz, %function

ntz:
    // (STUDENT TODO) Code for ntz goes here.
    // Input parameter n is passed in X0
    // Output value is returned in X0.

    // Shift left, so all trailing zeros will be at the right-side
    ORR X1, X0, X0        
    LSL X0, X0, #1
    ORR X0, X0, X1

    ORR X1, X0, X0        
    LSL X0, X0, #2
    ORR X0, X0, X1

    ORR X1, X0, X0        
    LSL X0, X0, #4
    ORR X0, X0, X1

    ORR X1, X0, X0        
    LSL X0, X0, #8
    ORR X0, X0, X1

    ORR X1, X0, X0        
    LSL X0, X0, #16
    ORR X0, X0, X1

    ORR X1, X0, X0        
    LSL X0, X0, #32
    ORR X0, X0, X1

    MVN X0, X0

    ANDS X2, X0, #0x5555555555555555
    LSR X3, X0, #1
    ANDS X1, X3, #0x5555555555555555
    ADD X4, X1, X2
    ORR X0, X4, X4

    ANDS X2, X0, #0x3333333333333333
    LSR X3, X0, #2
    ANDS X1, X3, #0x3333333333333333
    ADD X4, X1, X2
    ORR X0, X4, X4

    ANDS X2, X0, #0x0F0F0F0F0F0F0F0F
    LSR X3, X0, #4
    ANDS X1, X3, #0x0F0F0F0F0F0F0F0F
    ADD X4, X1, X2
    ORR X0, X4, X4

    ANDS X2, X0, #0x00FF00FF00FF00FF
    LSR X3, X0, #8
    ANDS X1, X3, #0x00FF00FF00FF00FF
    ADD X4, X1, X2
    ORR X0, X4, X4

    ANDS X2, X0, #0x0000FFFF0000FFFF
    LSR X3, X0, #16
    ANDS X1, X3, #0x0000FFFF0000FFFF
    ADD X4, X1, X2
    ORR X0, X4, X4

    ANDS X2, X0, #0x00000000FFFFFFFF
    LSR X3, X0, #32
    ANDS X1, X3, #0x00000000FFFFFFFF
    ADD X4, X1, X2
    ORR X0, X4, X4

    ANDS X0, X0, #0x000000000000007F
    RET 

    .size   ntz, .-ntz
    // ... and ends with the .size above this line.



    // Every function starts from the .align below this line ...
    .align  2
    .p2align 3,,7
    .global aiken_to_long
    .type   aiken_to_long, %function


aiken_to_long:
    // Input parameter buf is passed in X0
    // Output value is returned in X0.

    // starting digit place & times to add to itself when changing digit places
    MOVZ X6, #1 
    
    // our temporary result will be in X2
    MOVZ X2, #0

    // probably check least significant nibbles first

    // while loop until all nibbles checked
    aik_nibble:
        // if all nibbles checked, go to end
        CMP X0, #0
        B.EQ end
    
        // extract least significant nibble, then truncate after each iteration
        ANDS X3, X0, #0xF

        // different adds for valid numbers
        CMP X3, #0x5
        B.LT normal_add
        
        CMP X3, #0xA
        B.GT aiken_add

        // now we know it's invalid, so return -1 here
        MOVZ X0, #0
        SUB X2, X2, #1
        B end
        
    // doing 16 - X1 should give us which nibble we are looking at (LSL 2 times to find where to put?)
    normal_add:
        // 0x0 - 0x4, can just shift as is?
        loop_norm_add: 
            ADD X2, X2, X3
            ADD X8, X8, #1 // X8 will be counter for while loop
            CMP X8, X6
            B.LT loop_norm_add
        ADD X5, X6, #0
        LSL X5, X5, #1
        LSL X6, X6, #3
        ADD X6, X6, X5
        MOVZ X8, #0
        MOVZ X9, #0
        LSR X0, X0, #4
        B aik_nibble

    aiken_add:
        // 0xB - 0xF
        // decode to decimal using Aiken rules
        // complement wise the whole number 
        MVN X3, X3
        ANDS X3, X3, #0xF

        MOVZ X10, #9
        SUB X3, X10, X3

        // add into result
        loop_aik_add: 
            ADD X2, X2, X3
            ADD X8, X8, #1 // X8 will be counter for while loop
            CMP X8, X6
            B.LT loop_aik_add
        ADD X5, X6, #0
        LSL X5, x5, #1
        LSL X6, X6, #3
        ADD X6, X6, X5
        MOVZ X8, #0
        MOVZ X9, #0

        // truncate the bits to move nibbles
        LSR X0, X0, #4
        B aik_nibble

    end:
        SUB X0, X0, X0
        ADD X0, X0, X2
        RET

    .size   aiken_to_long, .-aiken_to_long
    // ... and ends with the .size above this line.



    // Every function starts from the .align below this line ...
    .align  2
    .p2align 3,,7
    .global unicode_to_UTF8
    .type   unicode_to_UTF8, %function

unicode_to_UTF8:
    // (STUDENT TODO) Code for unicode_to_UTF8 goes here.
    // Input parameter a is passed in X0; input parameter utf8 is passed in X1.
    // There are no output values.

    CMP X0, #0x007F
    B.LE uni_one_byte

    CMP X0, #0x07FF
    B.LE uni_two_bytes

    MOVZ X2, #0xFFFF
    CMP X0, X2
    B.LE uni_three_bytes

    MOVZ X2, #0x10, LSL #16
    MOVK X2, #0xFFFF
    CMP X0, X2
    B.LE uni_four_bytes

    B uni_invalid

    // 1 byte encoding - 7 bit cutoff (0x007F)
    // format: 0xxx_xxxx
    uni_one_byte:
        STUR X0, [X1]
        B uni_out

    // 2 byte encoding - 11 bit cutoff (0x07FF)
    // format: 110x_xxxx 10xx_xxxx
    uni_two_bytes:
        LSR X2, X0, #6
        ANDS X3, X2, #0x1F
        ORR X4, X3, #0xC0

        ANDS X5, X0, #0x3F
        ORR X6, X5, #0x80

        STUR X4, [X1]
        STUR X6, [X1, #1]
        B uni_out
    
    // 3 byte encoding - 16 bit cutoff (0xFFFF)
    // format: 1110_xxxx 10xx_xxxx 10xx_xxxx
    uni_three_bytes:
        LSR X2, X0, #12    // Extract bits 15-12
        ANDS X3, X2, #0x0F // Extract the first four bits
        ORR X4, X3, #0xE0  // Set the first byte (1110_xxxx)

        LSR X2, X0, #6     // Extract bits 11-6
        ANDS X5, X2, #0x3F // Extract the middle six bits
        ORR X6, X5, #0x80  // Set the second byte (10xx_xxxx)

        ANDS X7, X0, #0x3F // Extract bits 5-0
        ORR X8, X7, #0x80  // Set the third byte (10xx_xxxx)

        STUR X4, [X1]
        STUR X6, [X1, #1]
        STUR X8, [X1, #2]
        B uni_out

    // 4 byte encoding - 21 bit cutoff (0x10FFFF)
    // format: 1111_0xxx 10xx_xxxx 10xx_xxxx 10xx_xxxx
    uni_four_bytes:
        LSR X2, X0, #18
        ANDS X3, X2, #0x07
        ORR X4, X3, #0xF0

        LSR X2, X0, #12
        ANDS X5, X2, #0x3F
        ORR X6, X5, #0x80

        LSR X2, X0, #6
        ANDS X7, X2, #0x3F
        ORR X8, X7, #0x80

        ANDS X9, X0, #0x3F
        ORR X10, X9, #0x80

        STUR X4, [X1]
        STUR X6, [X1, #1]
        STUR X8, [X1, #2]
        STUR X10, [X1, #3]
        B uni_out

    uni_invalid:
        MOVK X2, #0xFFFF, LSL #16
        MOVK X2, #0xFFFF
        STUR X2, [X1] 
        B uni_out

    uni_out: 
        RET

    .size   unicode_to_UTF8, .-unicode_to_UTF8
    // ... and ends with the .size above this line.



    // Every function starts from the .align below this line ...
    .align  2
    .p2align 3,,7
    .global UTF8_to_unicode
    .type   UTF8_to_unicode, %function

UTF8_to_unicode:
    // (STUDENT TODO) Code for UTF8_to_unicode goes here.
    // Input parameter utf8 is passed in X0.
    // Output value is returned in X0.

    LDUR X1, [X0]
    ADD X2, X1, #0

    ANDS X3, X2, #0xFF // first byte
    LSR X2, X2, #8

    ANDS X4, X2, #0xFF // second byte
    LSR X2, X2, #8

    ANDS X5, X2, #0xFF // third byte
    LSR X2, X2, #8

    ANDS X6, X2, #0xFF // fourth byte

    // Determine the number of bytes needed to represent in unicode
    CMP X3, #0xF0
    B.GE utf_four_bytes
    CMP X3, #0xE0
    B.GE utf_three_bytes
    CMP X3, #0xC0
    B.GE utf_two_bytes
    B utf_one_byte

    utf_four_bytes:
        ANDS X3, X3, #0x07
        ANDS X4, X4, #0x3F
        ANDS X5, X5, #0x3F
        ANDS X6, X6, #0x3F
        LSL X3, X3, #18
        LSL X4, X4, #12
        LSL X5, X5, #6
        ORR X3, X3, X4
        ORR X3, X3, X5
        ORR X3, X3, X6
        ADD X0, X3, #0
        RET

    utf_three_bytes:
        ANDS X3, X3, #0x0F
        ANDS X4, X4, #0x3F
        ANDS X5, X5, #0x3F
        LSL X3, X3, #12
        LSL X4, X4, #6
        ORR X3, X3, X4
        ORR X3, X3, X5
        ADD X0, X3, #0
        RET

    utf_two_bytes:
        ANDS X3, X3, #0x1F
        ANDS X4, X4, #0x3F
        LSL X3, X3, #6
        ORR X3, X3, X4
        ADD X0, X3, #0
        RET

    utf_one_byte:
        ADD X0, X3, #0
        RET

    .size   UTF8_to_unicode, .-UTF8_to_unicode
    // ... and ends with the .size above this line.



    // ***** WEEK 2 deliverables *****



    // Every function starts from the .align below this line ...
    .align  2
    .p2align 3,,7
    .global ustrncmp
    .type   ustrncmp, %function

ustrncmp:
    // (STUDENT TODO) Code for ustrncmp goes here.
    // Input parameter str1 is passed in X0; input parameter str2 is passed in X1;
    //  input parameter num is passed in X2
    // Output value is returned in X0.
    
    LDUR X3, [X0]
    LDUR X4, [X1]
    ADD X10, X3, #0
    ADD X11, X4, #0

    ustrncmp_loop:
        CMP X2, #0
        B.EQ ustrncmp_equal

        SUB X2, X2, #1

        MOVZ X8, #0
        ANDS X8, X10, #0xFF
        LSR X10, X10, #8
        
        MOVZ X9, #0
        ANDS X9, X11, #0xFF
        LSR X11, X11, #8

        CMP X8, #0
        B.EQ ustrncmp_terminator

        CMP X9, #0
        B.EQ ustrncmp_terminator

        CMP X8, X9
        B.LT ustrncmp_unequalLT

        CMP X8, X9
        B.GT ustrncmp_unequalGT
        
        B ustrncmp_loop
    
    ustrncmp_equal:
        MOVZ X0, #2
        RET 
    
    ustrncmp_terminator:
        MOVZ X0, #100
        RET
    
    ustrncmp_unequalLT:
        MOVZ X0, #0
        SUB X0, X0, #1
        RET

    ustrncmp_unequalGT:
        MOVZ X0, #1
        RET        

    .size   ustrncmp, .-ustrncmp
    // ... and ends with the .size above this line.



    // Every function starts from the .align below this line ...
    .align  2
    .p2align 3,,7
    .global gcd_rec
    .type   gcd_rec, %function

gcd_rec:
    // (STUDENT TODO) Code for gcd_rec goes here.
    // Input parameter a is passed in X0; input parameter b is passed in X1.
    // Output value is returned in X0.
    CMP X0, #0
    B.EQ gcd_invalid

    CMP X1, #0
    B.EQ gcd_invalid

    ADD X22, LR, #0
    BL gcd_helper

    gcd_invalid:
        MOVZ X0, #0
        SUB X0, X0, #1
        ADD LR, X22, #0
        RET
    
    gcd_helper:
        CMP X0, X1
        B.EQ gcd_end

        B.GT gcd_a_is_greater

        B gcd_a_is_lesser
    
    gcd_a_is_greater:
        SUB X0, X0, X1
        B gcd_helper

    gcd_a_is_lesser:
        SUB X1, X1, X0
        B gcd_helper

    gcd_end:
        ADD LR, X22, #0
        RET

    .size   gcd_rec, .-gcd_rec
    // ... and ends with the .size above this line.



    // Every function starts from the .align below this line ...
    .align  2
    .p2align 3,,7
    .global tree_traversal
    .type   tree_traversal, %function

tree_traversal:
    // (STUDENT TODO) Code for tree_traversal goes here.
    // Input parameter root is passed in X0; input parameter bit_string is passed in X1;
    //  input parameter bit_string_length is passed in X2
    // Output value is returned in X0.
    
    CMP X2, #0
    B.EQ tree_invalid

    // initialize counter, while loop
    MOVZ X3, #0

    tree_loop:
        CMP X0, #0x0
        B.EQ tree_invalid

        CMP X3, X2
        B.EQ tree_end

        ANDS X4, X1, #1
        LSR X1, X1, #1

        ADD X3, X3, #1

        CMP X4, #0
        B.EQ tree_left

        B tree_right
    
    tree_invalid:
        MOVZ X0, #0
        SUB X0, X0, #1
        RET
    
    tree_left:
        LDUR X0, [X0]
        B tree_loop
    
    tree_right:
        LDUR X0, [X0, #8]
        B tree_loop
    
    tree_end:
        LDUR X0, [X0, #16]
        RET

    .size   tree_traversal, .-tree_traversal
    // ... and ends with the .size above this line.



    // Every function starts from the .align below this line ...
    .align  2
    .p2align 3,,7
    .global collatz_TST
    .type   collatz_TST, %function

collatz_TST:
    // Input parameter n is passed in X0
    // Output value is returned in X0.
    
    // store original LR
    ADD X21, LR, #0
    
    // counter for TST
    MOVZ X10, #0
    
    // stop when n is 1
    collatz_loop:
        CMP X0, #1
        B.EQ collatz_end

        // store n in temporary register
        ADD X20, X0, #0

        // if NTZ = 0, do even function
        BL ntz
        CMP X0, #1
        B.GE collatz_even 

        // we know NTZ = 1 or more, so do odd function
        B collatz_odd

    collatz_even:
        LSR X20, X20, #1
        ADD X0, X20, #0
        ADD X10, X10, #1
        B collatz_loop

    collatz_odd:
        LSL X0, X20, #1
        ADD X0, X0, X20
        ADD X0, X0, #1
        ADD X10, X10, #1
        B collatz_loop

    collatz_end:
        ADD X0, X10, #0
        ADD LR, X21, #0
        RET

    .size   collatz_TST, .-collatz_TST
    // ... and ends with the .size above this line.



    .section    .rodata
    .align  4
    // (STUDENT TODO) Any read-only global variables go here.
    .data
    // (STUDENT TODO) Any modifiable global variables go here.
    .align  3
