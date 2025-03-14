#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

typedef struct{
    void *array;
    size_t len;
    size_t size;
    int(*compare)(const void *, const void *);
}struct_for_merge;


void merge_sort(void *a, size_t len, size_t size, int(*compare)(const void *, const void *)); //a - указ.на массив, len - длина, size - размер одного элем.
int my_compare(const void *a, const void *b);

void merge_blocks(void* a, size_t first_len, size_t second_len, size_t size, int(*compare)(const void*, const void *));

void* merge_sort_structure(void *structure){    //Вспомогательная функция для pthread_create, т.к. нужна функция void*, принимающая void*
    merge_sort( ( (struct_for_merge*)structure) ->array, ((struct_for_merge*)structure) -> len, ((struct_for_merge*)structure) -> size, ((struct_for_merge*)structure) -> compare);
    return NULL;
}

int main(){
    //Тестовый запуск
    int *test = (int*)calloc(10, sizeof(int));
    if (test == NULL) return 1;
    test[0] = 64;
    test[1] = 43;
    test[2] = 22;
    test[3] = 55;
    test[4] = 81;
    test[5] = 48;
    test[6] = 24;
    test[7] = 83;
    test[8] = 74;
    test[9] = 35;
    //
    size_t len = 10;
    size_t cnt_pthreads = 4;//sysconf(_SC_NPROCESSORS_ONLN); //Кол-во потоков на компьютере
    if (cnt_pthreads == -1) return 1;
    pthread_t threads[cnt_pthreads];
    struct_for_merge structures[cnt_pthreads];
    size_t diff = 10 / cnt_pthreads; // Блок какого размера на один поток = 2
    for (int i = 0; i < cnt_pthreads; ++i){
        structures[i] = (struct_for_merge){test+i*diff, diff, sizeof(int), my_compare};
        if (i == cnt_pthreads - 1)  structures[i] = (struct_for_merge){test+i*diff, len-i*diff, sizeof(int), my_compare};   //Обработка последнего потока, чтобы точно доработал до конца (учитываем нецелочисленное деление)
        pthread_create(&(threads[i]), NULL, merge_sort_structure, &(structures[i]) );   //Получим сортировку каждого блока от каждого потока
    }
    struct_for_merge structure = {test, 10, sizeof(int), my_compare}; 
    
    for (int i = 0; i < cnt_pthreads; ++i){
        pthread_join(threads[i], NULL); //Завершаем потоки
    }
    for (int i = 1; i < cnt_pthreads; ++i){
        if (i == cnt_pthreads - 1){
             merge_blocks(test, i*diff, len-i*diff, sizeof(int), my_compare );  //Обработка последнего объединения, чтобы точно доработал до конца (учитываем нецелочисленное деление)
        }else merge_blocks(test, diff*i, diff, sizeof(int), my_compare);
    }
 
    for (int i = 0; i < 10; ++i){
        printf("%d ", test[i]);
    }
    printf("\n");
    free(test);
    
    
    return 0;
}

void swap(int *a, int *b){
    int temp = *a;
    *a = *b;
    *b = temp;
}

void merge_blocks(void* a, size_t first_len, size_t second_len, size_t size, int(*compare)(const void*, const void *)){
    void* result = (void*)malloc((first_len+second_len)*size);
    int pos = 0; //Позиция в результирующем
    int i = 0; //Индекс элементов первого массива
    int j = 0; //Индекс элементов второго массива
    while ( (j != second_len) && (i != first_len) ){
        if (compare(a + i*size, a + (first_len + j)*size) > 0){ //Элемент первого блока > элемента второго блока
            *(int*)(result+pos*size) = *(int*)(a + (first_len + j)*size);
            j++;
            pos++;
        }
        else{
            *(int*)(result+pos*size) = *(int*)(a + i*size);
            i++;
            pos++;
        }
    }
    while (j != second_len){            //Добавление остатков второго блока
        *(int*)(result+pos*size) = *(int*)(a + (first_len + j)*size);
        j++;
        pos++;
    }
    while (i != first_len){             //Добавление остатков первого блока
        *(int*)(result+pos*size) = *(int*)(a + i*size);
        i++;
        pos++;

    }
    for (int k = 0; k < first_len + second_len; ++k)
        *(int*)(a+k*size) = *(int*)(result + k*size);
    free(result);
}



void merge_sort(void *a, size_t len, size_t size, int(*compare)(const void *, const void *)){ //a - указ.на массив, len - длина, size - размер одного элем.
    if (len == 1) return;
    if (len == 2){
        if (compare(a, a+size) > 0 ){
            swap(a, a+size);
        }
        return;
    }
    size_t new_first_len = len / 2;
    size_t new_second_len = len - new_first_len;
    merge_sort(a, new_first_len, size, compare);   //Вызов merge для первой половины
    merge_sort(a+new_first_len*size, new_second_len, size, compare); //Вызов merge для второй
    merge_blocks(a, new_first_len, new_second_len, size, compare); //Слияние блоков

}

int my_compare(const void *a, const void *b){
    int *x = (int*) a;
    int *y = (int*) b;
    return (*x) - (*y);     //Компаратор для сравнения целых чисел, но можно сделать для чего угодно
}

