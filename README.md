電機3A 110501521 徐松廷 HW4 如果助教要測試的話直接執行./main即可!! 感謝助教批改!!!

當前的main.c已經合併前面所有作業:

<img width="736" alt="image" src="https://github.com/2023data-structure/hw4-HsuSungTing/assets/89059171/4eb0758c-91b2-4245-b72d-5514e227d410">

compiling caommand:

gcc -c redis_dll.c -o redis_dll.o

gcc -c redis_str.c -o redis_str.o

gcc -c redis_ss.c -o redis_ss.o

gcc -c redis_json.c -o redis_json.o

gcc -o main main.c redis_dll.o redis_str.o redis_ss.o redis_json.o
