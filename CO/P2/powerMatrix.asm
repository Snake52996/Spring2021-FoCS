.data
m:.space 4
n:.space 4
tempMatrix:.space 256
resultMatrix:.space 256
space:.asciiz " "
endl:.asciiz "\n"
.macro get_matrix_element(%matrix,%x,%y,%result)
lw $t9,m
mul $t9,$t9,%x
add $t9,$t9,%y
sll $t9,$t9,2
lw %result,%matrix($t9)
.end_macro
.macro set_matrix_element(%matrix,%x,%y,%value)
lw $t9,m
mul $t9,$t9,%x
add $t9,$t9,%y
sll $t9,$t9,2
sw %value,%matrix($t9)
.end_macro

.text

beq $0,$0,FUNC_MAIN

FUNC_SQRT_TEMP_MATRIX:
lw $t6,m
add $t0,$0,$0
add $t1,$0,$0
add $t2,$0,$0
add $t3,$0,$0
FUNC_SQRT_TEMP_MATRIX_LOOP:
get_matrix_element(tempMatrix,$t0,$t2,$t4)
get_matrix_element(tempMatrix,$t2,$t1,$t5)
mul $t4,$t4,$t5
add $t3,$t3,$t4
addi $t2,$t2,1
blt $t2,$t6,FUNC_SQRT_TEMP_MATRIX_LOOP
set_matrix_element(resultMatrix,$t0,$t1,$t3)
add $t3,$0,$0
add $t2,$0,$0
addi $t1,$t1,1
blt $t1,$t6,FUNC_SQRT_TEMP_MATRIX_LOOP
add $t1,$0,$0
addi $t0,$t0,1
blt $t0,$t6,FUNC_SQRT_TEMP_MATRIX_LOOP
jr $ra

FUNC_REPLACE_TEMP_MATRIX:
lw $t0,m
mul $t0,$t0,$t0
sll $t0,$t0,2
add $t1,$0,$0
FUNC_REPLACE_TEMP_MATRIX_LOOP:
lw $t2,resultMatrix($t1)
sw $t2,tempMatrix($t1)
addi $t1,$t1,4
blt $t1,$t0,FUNC_REPLACE_TEMP_MATRIX_LOOP
jr $ra

FUNC_POWER_TEMP_MATRIX:
lw $t8,n
bne $t8,$0,FUNC_POWER_TEMP_MATRIX_START
jr $ra
FUNC_POWER_TEMP_MATRIX_START:
addi $sp,$sp,-4
sw $ra,4($sp)
add $t7,$0,$0
FUNC_POWER_TEMP_MATRIX_LOOP:
bgezal $0,FUNC_SQRT_TEMP_MATRIX
bgezal $0,FUNC_REPLACE_TEMP_MATRIX
addi $t7,$t7,1
blt $t7,$t8,FUNC_POWER_TEMP_MATRIX_LOOP
lw $ra,4($sp)
addi $sp,$sp,4
jr $ra

FUNC_MAIN:
addi $v0,$0,5
syscall
sw $v0,n
addi $v0,$0,5
syscall
sw $v0,m
add $s0,$0,$v0
add $t0,$0,$0
add $t1,$0,$0
FUNC_MAIN_READ_LOOP:
addi $v0,$0,5
syscall
set_matrix_element(tempMatrix,$t0,$t1,$v0)
addi $t1,$t1,1
blt $t1,$s0,FUNC_MAIN_READ_LOOP
add $t1,$0,$0
addi $t0,$t0,1
blt $t0,$s0,FUNC_MAIN_READ_LOOP

bgezal $0,FUNC_POWER_TEMP_MATRIX

add $t0,$0,$0
add $t1,$0,$0
FUNC_MAIN_WRITE_LOOP:
get_matrix_element(tempMatrix,$t0,$t1,$a0)
addi $v0,$0,1
syscall
addi $v0,$0,4
la $a0,space
syscall
addi $t1,$t1,1
blt $t1,$s0,FUNC_MAIN_WRITE_LOOP
add $t1,$0,$0
addi $t0,$t0,1
addi $v0,$0,4
la $a0,endl
syscall
blt $t0,$s0,FUNC_MAIN_WRITE_LOOP
addi $v0,$0,10
syscall