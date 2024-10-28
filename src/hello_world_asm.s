 .section .rodata
HelloMessage:
    .asciz "HELLO WORLD!" 
 
 .section .text
    .global hello_world_asm


hello_world_asm:
    move.l  #10,-(%sp)
    move.l  #1,-(%sp)
    move.l  #HelloMessage,-(%sp)
    jsr VDP_drawText
    lea  4*3(%sp), %sp
    rts