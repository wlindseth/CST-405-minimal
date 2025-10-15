.data

.text
.globl main
main:
    # Allocate stack space
    addi $sp, $sp, -400

    # Declared x at offset 0
    # Declared y at offset 4
    li $t0, 10
    sw $t0, 0($sp)
    li $t0, 20
    sw $t0, 4($sp)
    lw $t0, 0($sp)
    lw $t1, 4($sp)
    add $t0, $t0, $t1
    # Print integer
    move $a0, $t0
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    # Declared array arr[3] at offset 8
    # Declared i at offset 20
    li $t0, 0
    sw $t0, 20($sp)
    lw $t0, 20($sp)
    sll $t0, $t0, 2
    addi $t1, $sp, 8
    add $t1, $t1, $t0
    li $t2, 100
    sw $t2, 0($t1)
    li $t0, 1
    sw $t0, 20($sp)
    lw $t0, 20($sp)
    sll $t0, $t0, 2
    addi $t1, $sp, 8
    add $t1, $t1, $t0
    li $t2, 200
    sw $t2, 0($t1)
    li $t0, 0
    sll $t0, $t0, 2
    addi $t1, $sp, 8
    add $t1, $t1, $t0
    lw $t0, 0($t1)
    # Print integer
    move $a0, $t0
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    li $t0, 1
    sll $t0, $t0, 2
    addi $t1, $sp, 8
    add $t1, $t1, $t0
    lw $t0, 0($t1)
    # Print integer
    move $a0, $t0
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    # Declared 2D array matrix[2][3] at offset 24
    # Declared row at offset 48
    # Declared col at offset 52
    li $t0, 0
    sw $t0, 48($sp)
    li $t0, 0
    sw $t0, 52($sp)
    lw $t0, 48($sp)
    lw $t1, 52($sp)
    li $t2, 3
    mul $t0, $t0, $t2
    add $t0, $t0, $t1
    sll $t0, $t0, 2
    addi $t2, $sp, 24
    add $t2, $t2, $t0
    li $t3, 5
    sw $t3, 0($t2)
    li $t0, 0
    sw $t0, 48($sp)
    li $t0, 1
    sw $t0, 52($sp)
    lw $t0, 48($sp)
    lw $t1, 52($sp)
    li $t2, 3
    mul $t0, $t0, $t2
    add $t0, $t0, $t1
    sll $t0, $t0, 2
    addi $t2, $sp, 24
    add $t2, $t2, $t0
    li $t3, 10
    sw $t3, 0($t2)
    li $t0, 0
    sw $t0, 48($sp)
    li $t0, 2
    sw $t0, 52($sp)
    lw $t0, 48($sp)
    lw $t1, 52($sp)
    li $t2, 3
    mul $t0, $t0, $t2
    add $t0, $t0, $t1
    sll $t0, $t0, 2
    addi $t2, $sp, 24
    add $t2, $t2, $t0
    li $t3, 15
    sw $t3, 0($t2)
    li $t0, 1
    sw $t0, 48($sp)
    li $t0, 0
    sw $t0, 52($sp)
    lw $t0, 48($sp)
    lw $t1, 52($sp)
    li $t2, 3
    mul $t0, $t0, $t2
    add $t0, $t0, $t1
    sll $t0, $t0, 2
    addi $t2, $sp, 24
    add $t2, $t2, $t0
    li $t3, 20
    sw $t3, 0($t2)
    li $t0, 1
    sw $t0, 48($sp)
    li $t0, 1
    sw $t0, 52($sp)
    lw $t0, 48($sp)
    lw $t1, 52($sp)
    li $t2, 3
    mul $t0, $t0, $t2
    add $t0, $t0, $t1
    sll $t0, $t0, 2
    addi $t2, $sp, 24
    add $t2, $t2, $t0
    li $t3, 25
    sw $t3, 0($t2)
    li $t0, 1
    sw $t0, 48($sp)
    li $t0, 2
    sw $t0, 52($sp)
    lw $t0, 48($sp)
    lw $t1, 52($sp)
    li $t2, 3
    mul $t0, $t0, $t2
    add $t0, $t0, $t1
    sll $t0, $t0, 2
    addi $t2, $sp, 24
    add $t2, $t2, $t0
    li $t3, 30
    sw $t3, 0($t2)
    li $t0, 0
    li $t1, 0
    li $t2, 3
    mul $t0, $t0, $t2
    add $t0, $t0, $t1
    sll $t0, $t0, 2
    addi $t2, $sp, 24
    add $t2, $t2, $t0
    lw $t0, 0($t2)
    # Print integer
    move $a0, $t0
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    li $t0, 0
    li $t1, 1
    li $t2, 3
    mul $t0, $t0, $t2
    add $t0, $t0, $t1
    sll $t0, $t0, 2
    addi $t2, $sp, 24
    add $t2, $t2, $t0
    lw $t0, 0($t2)
    # Print integer
    move $a0, $t0
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    li $t0, 0
    li $t1, 2
    li $t2, 3
    mul $t0, $t0, $t2
    add $t0, $t0, $t1
    sll $t0, $t0, 2
    addi $t2, $sp, 24
    add $t2, $t2, $t0
    lw $t0, 0($t2)
    # Print integer
    move $a0, $t0
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    li $t0, 1
    li $t1, 0
    li $t2, 3
    mul $t0, $t0, $t2
    add $t0, $t0, $t1
    sll $t0, $t0, 2
    addi $t2, $sp, 24
    add $t2, $t2, $t0
    lw $t0, 0($t2)
    # Print integer
    move $a0, $t0
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    li $t0, 1
    li $t1, 1
    li $t2, 3
    mul $t0, $t0, $t2
    add $t0, $t0, $t1
    sll $t0, $t0, 2
    addi $t2, $sp, 24
    add $t2, $t2, $t0
    lw $t0, 0($t2)
    # Print integer
    move $a0, $t0
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    li $t0, 1
    li $t1, 2
    li $t2, 3
    mul $t0, $t0, $t2
    add $t0, $t0, $t1
    sll $t0, $t0, 2
    addi $t2, $sp, 24
    add $t2, $t2, $t0
    lw $t0, 0($t2)
    # Print integer
    move $a0, $t0
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    li $t0, 0
    li $t1, 0
    li $t2, 3
    mul $t0, $t0, $t2
    add $t0, $t0, $t1
    sll $t0, $t0, 2
    addi $t2, $sp, 24
    add $t2, $t2, $t0
    lw $t0, 0($t2)
    li $t1, 1
    li $t2, 2
    li $t3, 3
    mul $t1, $t1, $t3
    add $t1, $t1, $t2
    sll $t1, $t1, 2
    addi $t3, $sp, 24
    add $t3, $t3, $t1
    lw $t1, 0($t3)
    add $t0, $t0, $t1
    sw $t0, 0($sp)
    lw $t0, 0($sp)
    # Print integer
    move $a0, $t0
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall

    # Exit program
    addi $sp, $sp, 400
    li $v0, 10
    syscall
