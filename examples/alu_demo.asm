# Open this file in MARS and assemble/run it.
# Expected: $t0=30, $t3=10, $t4=0, $t5=30, $t6=1.
.text
.globl main
main:
    addi $t1, $zero, 10
    addi $t2, $zero, 20
    add  $t0, $t1, $t2
    sub  $t3, $t2, $t1
    and  $t4, $t1, $t2
    or   $t5, $t1, $t2
    slt  $t6, $t1, $t2
    li   $v0, 10
    syscall
