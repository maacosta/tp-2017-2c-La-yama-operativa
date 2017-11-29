# tp-2017-2c-La-yama-operativa

Requisitos para instalar
> sudo apt-get install libreadline6 libreadline6-dev

Ayuditas
* Para crear los data.bin de los datanodes (ejemplo para un data.bin de 1 mb con bloques de 1024)
 > dd if=/dev/zero of=data.bin bs=1024 count=1024 
 Ejemplo para un data.bin de 10 mb con bloques de 1 mb
 > dd if=/dev/zero of=data.bin bs=1048576 count=10