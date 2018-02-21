echo off

rem SET BOARD=edu_cia_nxp
SET NOMBRE=main

echo,
echo - Generando codigo assembly

arm-none-eabi-gcc -S -O2 -Wall -fdata-sections --function-sections -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -mthumb -I"./inc/" -DCORE_M4 ./src/%NOMBRE%.c -o ./%NOMBRE%.s

pause