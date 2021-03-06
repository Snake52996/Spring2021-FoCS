.data
op1:.asciiz "move disk "
op2:.asciiz " from "
op3:.asciiz " to "
endl:.asciiz "\n"
A:.asciiz "A"
B:.asciiz "B"
C:.asciiz "C"
.text
beq $0,$0,FUNC_MAIN

FUNC_MOVE:
# a0 => id
# a1 => from
# a2 => to
addu $t0,$0,$a0
li $v0,4
la $a0,op1
syscall
addu $a0,$t0,$0
li $v0,1
syscall
li $v0,4
la $a0,op2
syscall
addu $a0,$a1,$0
syscall
la $a0,op3
syscall
addu $a0,$a2,$0
syscall
la $a0,endl
syscall
jr $ra

FUNC_HANOI:
# a0 => base
# a1 => from
# a2 => via
# a3 => to
addi $sp,$sp,-20
sw $ra,20($sp)
sw $a0,16($sp)
sw $a1,12($sp)
sw $a2,8($sp)
sw $a3,4($sp)

bne $a0,$0,FUNC_HANOI_REGULAR
bgezal $0,FUNC_MOVE
lw $a0,16($sp)
lw $a1,8($sp)
lw $a2,4($sp)
bgezal $0,FUNC_MOVE
bgezal $0,FUNC_HANOI_RETURN

FUNC_HANOI_REGULAR:
addi $a0,$a0,-1
bgezal $0,FUNC_HANOI
lw $a0,16($sp)
lw $a1,12($sp)
lw $a2,8($sp)
bgezal $0,FUNC_MOVE
lw $a0,16($sp)
lw $a1,4($sp)
lw $a2,8($sp)
lw $a3,12($sp)
addi $a0,$a0,-1
bgezal $0,FUNC_HANOI
lw $a0,16($sp)
lw $a1,8($sp)
lw $a2,4($sp)
bgezal $0,FUNC_MOVE
lw $a0,16($sp)
lw $a1,12($sp)
lw $a2,8($sp)
lw $a3,4($sp)
addi $a0,$a0,-1
bgezal $0,FUNC_HANOI

FUNC_HANOI_RETURN:
lw $ra,20($sp)
addi $sp,$sp,20
jr $ra

FUNC_MAIN:
li $v0,5
syscall
addu $a0,$v0,$0
la $a1,A
la $a2,B
la $a3,C
bgezal $0,FUNC_HANOI
li $v0,10
syscall