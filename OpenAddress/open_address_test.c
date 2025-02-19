#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define Tbl_size 37533  // 슬롯 수 (회사명 수의 약 1.1배)

#define EMPTY    0
#define OCCUPIED 1
#define DELETED  2

typedef struct ty_record {
    char name[300];
    int monincome;  // 단위: 천원.
    int status;     // EMPTY, OCCUPIED, DELETED
} type_record;

type_record Hashtable[Tbl_size];

// 해시 함수 (기존과 동일)
int hash(char recname[]) {
    unsigned char u;
    int HA, j, leng, halfleng;
    long sum = 0;
    int A[300];
    leng = strlen(recname);
    for (j = 0; j < leng; j++) {
        u = recname[j];
        A[j] = u;
    }
    halfleng = leng / 2;
    for (j = 0; j < halfleng; j++)
        sum += A[j] * A[leng - 1 - j] * A[(leng - 1) / 2];
    if (leng % 2 == 1)
        sum += A[halfleng] * A[halfleng + 1] * A[(leng - 1) / 2];
    HA = sum % Tbl_size;
    return HA;
}

// 개방 주소법 삽입 (선형 탐사 사용)
// 이미 동일 키가 있으면 -1 반환, 그렇지 않으면 프로브 수를 반환
int open_address_insert(type_record rec) {
    int HA = hash(rec.name);
    int probes = 1;
    int index = HA;
    while (Hashtable[index].status == OCCUPIED) {
        if (strcmp(Hashtable[index].name, rec.name) == 0)
            return -1;  // duplicate key
        index = (index + 1) % Tbl_size;
        probes++;
        if (index == HA)  // 테이블이 가득 찬 경우
            return -1;
    }
    Hashtable[index] = rec;
    Hashtable[index].status = OCCUPIED;
    return probes;
}

// 개방 주소법 검색 (선형 탐사)
// 검색 시 프로브 수를 probe에 저장하고, 찾은 슬롯 인덱스를 반환; 없으면 -1 반환
int open_address_retrieve(char* key, int* probe) {
    int HA = hash(key);
    int index = HA;
    *probe = 0;
    while (Hashtable[index].status != EMPTY) {
        (*probe)++;
        if (Hashtable[index].status == OCCUPIED && strcmp(Hashtable[index].name, key) == 0)
            return index;
        index = (index + 1) % Tbl_size;
        if (index == HA) break;
    }
    return -1;
}

// 개방 주소법 삭제 (선형 탐사)
// 검색 후 찾으면 해당 슬롯을 DELETED 상태로 표시하고, 검색 시 사용한 프로브 수를 probe에 저장함.
int open_address_delete(char* key, int* probe) {
    int HA = hash(key);
    int index = HA;
    *probe = 0;
    while (Hashtable[index].status != EMPTY) {
        (*probe)++;
        if (Hashtable[index].status == OCCUPIED && strcmp(Hashtable[index].name, key) == 0) {
            Hashtable[index].status = DELETED;
            return index;
        }
        index = (index + 1) % Tbl_size;
        if (index == HA) break;
    }
    return -1;
}

// 해시 테이블 통계 출력: 사용된 슬롯 수와 load factor 출력
void print_open_address_statistics() {
    int used = 0;
    for (int i = 0; i < Tbl_size; i++) {
        if (Hashtable[i].status == OCCUPIED)
            used++;
    }
    printf("전체 슬롯: %d, 사용된 슬롯: %d, load factor: %.2f\n", Tbl_size, used, (float)used / Tbl_size);
}

// 평균 프로브 수 계산: 삽입된 num_records 건에 대해 검색
float compute_average_probes_search(int num_records) {
    int total_probes = 0, searches = 0, probe, i;
    char name[300];
    for (i = 0; i < num_records; i++) {
        sprintf(name, "Company%07d", i);
        int pos = open_address_retrieve(name, &probe);
        if (pos != -1) {
            total_probes += probe;
            searches++;
        }
    }
    return (searches ? (float)total_probes / searches : 0);
}

// load factor 테스트 함수: 지정한 load factor에 따라 합성 데이터를 삽입 후 통계 및 평균 프로브 수 출력
void test_load_factor(float lf) {
    int num_records = (int)(Tbl_size * lf);
    int i, probe, ret;
    char name[300];
    type_record rec;
    
    // 해시 테이블 초기화
    for (i = 0; i < Tbl_size; i++) {
        Hashtable[i].status = EMPTY;
        Hashtable[i].name[0] = '\0';
    }
    
    for (i = 0; i < num_records; i++) {
        sprintf(name, "Company%07d", i);
        strcpy(rec.name, name);
        rec.monincome = rand() % 50000;
        rec.status = OCCUPIED;
        ret = open_address_insert(rec);
        if (ret == -1) {
            printf("Insertion failed for %s\n", name);
        }
    }
    printf("Load Factor %.2f: Inserted %d records.\n", lf, num_records);
    print_open_address_statistics();
    
    float avg = compute_average_probes_search(num_records);
    printf("Load Factor %.2f: Average probes per search = %.2f\n\n", lf, avg);
}

#ifdef TEST_MODE
int main(void) {
    float load_factors[] = {0.25f, 0.50f, 0.75f, 1.00f};
    int num_tests = sizeof(load_factors) / sizeof(load_factors[0]);
    srand((unsigned)time(NULL));
    
    for (int i = 0; i < num_tests; i++) {
        test_load_factor(load_factors[i]);
    }
    
    // 추가: load factor 0.75 상태에서 일부 삭제 테스트
    printf("삭제 테스트 (Load Factor 0.75 상태):\n");
    test_load_factor(0.75f);
    for (int i = 0; i < 100; i++) {
        char name[300];
        int probe;
        sprintf(name, "Company%07d", i);
        int pos = open_address_delete(name, &probe);
        if (pos != -1)
            printf("Deleted %s at index %d, probes=%d\n", name, pos, probe);
        else
            printf("Failed to delete %s\n", name);
    }
    print_open_address_statistics();
    
    return 0;
}
#else
int main(void) {
    printf("TEST_MODE가 정의되지 않았습니다.\n");
    return 0;
}
#endif
