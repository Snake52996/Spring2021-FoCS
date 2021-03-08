.text
addi $v0,$0,9
addi $a0,$0,4
syscall
add $s0,$v0,$0
addi $v0,$0,5
syscall
sh $v0,0($s0)
add $t0,$0,$v0
addi $v0,$0,9
add $a0,$0,$t0
sll $a0,$a0,2
syscall
add $s1,$0,$v0
add $t1,$a0,$s1
addi $t1,$t1,-4
add $t0,$a0,$s1
READ_LOOP_1:
addi $v0,$0,5
syscall
sw $v0,0($t1)
addi $t1,$t1,-4
bge $t1,$s1,READ_LOOP_1
addi $v0,$0,5
syscall
sh $v0,2($s0)
add $t0,$0,$v0
addi $v0,$0,9
add $a0,$0,$t0
sll $a0,$a0,2
syscall
add $s2,$0,$v0
add $t1,$a0,$s2
addi $t1,$t1,-4
add $t0,$a0,$s2
READ_LOOP_2:
addi $v0,$0,5
syscall
sb $v0,0($t1)
addi $t1,$t1,-4
bge $t1,$s2,READ_LOOP_2

lh $t0,0($s0)
lh $t1,2($s0)
add $t0,$t0,$t1
sll $a0,$t0,2
addi $v0,$0,9
syscall
add $s3,$v0,$0

add $t0,$s3,$a0
add $t1,$0,$s3
INIT_LOOP:
sw $0,0($t1)
addi $t1,$t1,4
blt $t1,$t0,INIT_LOOP

lh $t7,0($s0)
lh $t8,2($s0)
sll $t7,$t7,2
sll $t8,$t8,2
add $t0,$0,$0
add $t1,$0,$0
CALCULATE_LOOP:
add $t2,$t0,$t1
add $t2,$s3,$t2
lw $t3,0($t2)
add $t4,$t0,$s1
lw $t5,0($t4)
add $t4,$t1,$s2
lw $t6,0($t4)
mul $t5,$t5,$t6
add $t3,$t3,$t5
sw $t3,0($t2)
addi $t1,$t1,4
blt $t1,$t8,CALCULATE_LOOP
add $t1,$0,$0
addi $t0,$t0,4
blt $t0,$t7,CALCULATE_LOOP

add $t0,$s3,$t7
add $t0,$t0,$t8
add $t1,$s3,$0
CARRY_LOOP:
lw $t2,0($t1)
divu $t2,$t2,10
mfhi $t2
sw $t2,0($t1)
lw $t3,4($t1)
mflo $t2
add $t2,$t2,$t3
sw $t2,4($t1)
addi $t1,$t1,4
blt $t1,$t0,CARRY_LOOP


add $t0,$s3,$t7
add $t0,$t0,$t8
addi $t0,$t0,-4
add $t1,$0,$0
addi $v0,$0,1
PRINT_LOOP:
lw $a0,0($t0)
beq $a0,$0,ZERO
OUT:
addi $t1,$0,1
syscall
beq $0,$0,NEXT
ZERO:
bne $t1,$0,OUT
NEXT:
addi $t0,$t0,-4
bgt $t0,$s3,PRINT_LOOP
lw $a0,0($s3)
syscall
addi $v0,$0,10
syscall