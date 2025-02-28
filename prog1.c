#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>


void merge_sort(void **a, size_t len, size_t size, int(*compare)(const void *, const void *)); //a - указ.на массив, len - длина, size - размер одного элем.
int my_compare(const void *a, const void *b);

int main(){
    int *test = (int*)calloc(5, sizeof(int));
    test[0] = 6;
    test[1] = 4;
    test[2] = 2;
    test[3] = 5;
    test[4] = 3;
    merge_sort(&test, 5, sizeof(int), my_compare);
    for (int i = 0; i < 5; i++){
        printf("%d ", test[i]);
    }
    printf("\n");
    return 0;
}

void swap(int *a, int *b){
    int temp = *a;
    *a = *b;
    *b = temp;
}

void* merge_blocks(void* a, size_t first_len, size_t second_len, size_t size, int(*compare)(const void*, const void *)){
    void* result = (void*)malloc((first_len+second_len)*size);
    int pos = 0; //Позиция в результирующем
    int i = 0; //Индекс элементов первого массива
    int j = 0; //Индекс элементов второго массива
    while ( (j != second_len) && (i != first_len) ){
        if (compare(a + i*size, a + (first_len + j)*size) > 0){ //Элемент первого блока > элемента второго блока
            *(int*)(result+pos*size) = *(int*)(a + (first_len + j)*size);
            printf("Первый элемент %d \n", *(int*)(result+pos*size));
            j++;
            pos++;
        }
        else{
            *(int*)(result+pos*size) = *(int*)(a + i*size);
            printf("Второй элемент %d \n", *(int*)(result+pos*size));
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
    
    return result;
}



void merge_sort(void **a, size_t len, size_t size, int(*compare)(const void *, const void *)){ //a - указ.на массив, len - длина, size - размер одного элем.
        //Поменять ВСЕ a на *а
    if (len == 1) return;
    if (len == 2){
        if (compare(a, a+size) > 0 ){
            swap(a, a+size);
    for (int i = 0; i < len; i++){
        printf("%d ",  *((int*)(a+i*size)));
    }
    printf("\n");
        }
        return;
    }
    size_t new_first_len = len / 2;
    size_t new_second_len = len - new_first_len;
    merge_sort(a, new_first_len, size, compare);   //Вызов merge для первой половины
    merge_sort(a+new_first_len*size, new_second_len, size, compare); //Вызов merge для второй
    a = merge_blocks(a, new_first_len, new_second_len, size, compare); //Слияние блоков

    /*if (new_second_len > new_first_len){
        if (compare(a + (new_first_len - 1)*size, a + (new_first_len + new_second_len - 1)*size) > 0){
        swap(a + (new_first_len - 1)*size, a + (new_first_len + new_second_len - 1)*size);
        }
    }
    */
    for (int i = 0; i < len; i++){
        printf("%d ",  *((int*)(a+i*size)));
    }
    printf("\n");
}

int my_compare(const void *a, const void *b){
    int *x = (int*) a;
    int *y = (int*) b;
    return (*x) - (*y);     //Компаратор для сравнения целых чисел, но можно сделать для чего угодно
}

