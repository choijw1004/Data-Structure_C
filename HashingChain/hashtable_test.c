#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define Tbl_size 37533  // 슬롯 수 (회사명 수의 약 1.1배)

typedef struct ty_record {
    char name[300];
    int monincome;  // 단위: 천원.
    int link;
} type_record;

type_record Hashtable[Tbl_size];
int LAST = Tbl_size - 1;
float average_num_probe;

int del_middle(int s, int p, int* chain_split);

// 해시 함수
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

// 빈 슬롯 찾기: LAST 이하의 빈 자리 중 가장 높은 주소 반환
int find_empty_location() {
    int curr = LAST;
    while (curr >= 0 && Hashtable[curr].name[0] != '\0')
        curr--;
    return curr;
}

// 해시 테이블에 레코드 삽입 (체이닝 방식)
// 동일 키가 있으면 삽입 실패(-1 반환)
int insert_rec(type_record rec) {
    int HA, curr, nprove = 0, empty_loc;
    HA = hash(rec.name);
    if (Hashtable[HA].name[0] == '\0') {
        Hashtable[HA] = rec;
        return 1;
    } else {
        curr = HA;
        do {
            if (strcmp(Hashtable[curr].name, rec.name) == 0) {
                // 동일 키 존재 시 삽입 실패
                return -1;
            }
            nprove++;
            if (Hashtable[curr].link == -1)
                break;
            else
                curr = Hashtable[curr].link;
        } while (1);
        empty_loc = find_empty_location();
        if (empty_loc < 0) {
            printf("No empty slot available!\n");
            return -1;
        }
        Hashtable[empty_loc] = rec;
        Hashtable[curr].link = empty_loc;
        return nprove;
    }
}

// 검색: 주어진 key에 대해 체인을 따라가며 검색, 프로브 수를 probe에 저장
// 검색 실패 시 -1 반환
int retrieve_rec(char* key, int* probe) {
    int curr;
    curr = hash(key);
    if (Hashtable[curr].name[0] == '\0')
        return -1;
    *probe = 0;
    do {
        (*probe)++;
        if (strcmp(Hashtable[curr].name, key) == 0)
            return curr;
        else
            curr = Hashtable[curr].link;
    } while (curr != -1);
    return -1;
}

// 평균 프로브 수 계산: 합성 데이터(해당 load factor에 따른 레코드 수)에 대해 검색
float compute_average_number_of_probes_per_search(int num_records) {
    int total_probes = 0, searches = 0, nprobe, i;
    char name[300];
    for (i = 0; i < num_records; i++) {
        sprintf(name, "Company%07d", i);
        if (retrieve_rec(name, &nprobe) != -1) {
            total_probes += nprobe;
            searches++;
        }
    }
    return (searches ? (float)total_probes / searches : 0);
}

// del_start: 체인의 시작(s)의 다음부터, s의 홈주소와 일치하는 마지막 레코드를 찾아 s로 옮기고 재귀적으로 처리
int del_start(int s, int* chain_split) {
    int curr, prev, nmove, found = -1, prev_found;
    curr = Hashtable[s].link;
    prev = s;
    while (curr != -1) {
        if (hash(Hashtable[curr].name) == s) {
            prev_found = prev;
            found = curr;
        }
        prev = curr;
        curr = Hashtable[curr].link;
    }
    if (found == -1) {
        Hashtable[s].name[0] = '\0';
        return 0;
    } else {
        strcpy(Hashtable[s].name, Hashtable[found].name);
        Hashtable[s].monincome = Hashtable[found].monincome;
        nmove = 1 + del_middle(found, prev_found, chain_split);
        return nmove;
    }
}

// del_middle: 체인 중간에서 s를 홈주소로 하는 마지막 레코드를 찾아 옮기고 재귀적으로 처리
int del_middle(int s, int p, int* chain_split) {
    int curr, prev, HA_curr, HA_curr_belongs_to_D_s, element_of_D_s, nmove, found, prev_found;
    curr = Hashtable[s].link;
    prev = s;
    found = -1;
    while (curr != -1) {
        if (hash(Hashtable[curr].name) == s) {
            prev_found = prev;
            found = curr;
        }
        prev = curr;
        curr = Hashtable[curr].link;
    }
    if (found == -1) {
        Hashtable[p].link = Hashtable[s].link;
        Hashtable[s].name[0] = '\0';
        return 0;
    } else {
        found = -1;
        curr = Hashtable[s].link;
        prev = s;
        while (curr != -1) {
            HA_curr = hash(Hashtable[curr].name);
            HA_curr_belongs_to_D_s = 0;
            element_of_D_s = s;
            do {
                if (element_of_D_s == HA_curr) {
                    HA_curr_belongs_to_D_s = 1;
                    break;
                } else
                    element_of_D_s = Hashtable[element_of_D_s].link;
            } while (element_of_D_s != -1);
            if (HA_curr_belongs_to_D_s == 0) {
                found = curr;
                prev_found = prev;
            }
            prev = curr;
           	curr = Hashtable[curr].link;
        }
        if (found != -1) {
            strcpy(Hashtable[s].name, Hashtable[found].name);
            Hashtable[s].monincome = Hashtable[found].monincome;
            nmove = 1 + del_middle(found, prev_found, chain_split);
            return nmove;
        } else {
            Hashtable[p].link = -1;
            (*chain_split)++;
            nmove = del_start(s, chain_split);
            return nmove;
        }
    }
}

// delete_rec: 주어진 key의 레코드를 삭제하고, 이동한 레코드 수 반환
int delete_rec(char* dkey, int* chain_split) {
    int found = -1, h, curr, prev, nmove = 0;
    h = hash(dkey);
    if (Hashtable[h].name[0] == '\0') {
        printf("There are no records with such key.\n");
        return -1;
    }
    if (strcmp(dkey, Hashtable[h].name) == 0)
        nmove = del_start(h, chain_split);
    else {
        curr = Hashtable[h].link;
        prev = h;
        found = -1;
        while (curr != -1) {
            if (strcmp(Hashtable[curr].name, dkey) == 0) {
                found = curr;
                break;
            } else {
                prev = curr;
                curr = Hashtable[curr].link;
            }
        }
        if (found == -1) {
            printf("A record with such key does not exist.\n");
            return -1;
        } else
            nmove = del_middle(curr, prev, chain_split);
    }
    return nmove;
}

// delete_multiple: 첫 num_del_req 건의 레코드를 삭제
void delete_multiple(int num_del_req) {
    char line[300];
    int i, num_deletion_success = 0, num_relocated_deletions = 0, nmove, num_split = 0;
    for (i = 0; i < num_del_req; i++) {
        sprintf(line, "Company%07d", i);
        nmove = delete_rec(line, &num_split);
        if (nmove >= 0)
            num_deletion_success++;
        if (nmove > 0)
            num_relocated_deletions += nmove;
    }
    printf("삭제 성공 레코드 수=%d, 이동된 레코드 수=%d\n", num_deletion_success, num_relocated_deletions);
}

// 해시 테이블 체인 통계 출력: 사용된 슬롯 수, 평균 체인 길이, 최대 체인 길이
void print_hash_statistics() {
    int i, chain_length, used_slots = 0, total_chain = 0, max_chain = 0;
    for (i = 0; i < Tbl_size; i++) {
        if (Hashtable[i].name[0] != '\0') {
            used_slots++;
            chain_length = 0;
            int idx = i;
            while (idx != -1) {
                chain_length++;
                idx = Hashtable[idx].link;
            }
            total_chain += chain_length;
            if (chain_length > max_chain)
                max_chain = chain_length;
        }
    }
    printf("전체 슬롯: %d, 사용된 슬롯: %d, 평균 체인 길이: %.2f, 최대 체인 길이: %d\n",
           Tbl_size, used_slots, (used_slots ? (float)total_chain / used_slots : 0), max_chain);
}

// load factor 테스트 함수: 지정한 load factor에 따라 레코드를 삽입하고 통계 출력
void test_load_factor(float lf) {
    int num_records = (int)(Tbl_size * lf);
    int i, n_probe, pos;
    char name[300];
    type_record rec;
    
    // 해시 테이블 초기화
    for (i = 0; i < Tbl_size; i++) {
        Hashtable[i].name[0] = '\0';
        Hashtable[i].link = -1;
    }
    
    // 합성 데이터 삽입
    for (i = 0; i < num_records; i++) {
        sprintf(name, "Company%07d", i);
        strcpy(rec.name, name);
        rec.monincome = rand() % 50000;
        rec.link = -1;
        if (insert_rec(rec) == -1) {
            printf("Insertion failed for %s\n", name);
        }
    }
    printf("Load Factor %.2f: Inserted %d records.\n", lf, num_records);
    print_hash_statistics();
    
    // 전체 데이터에 대한 평균 프로브 수 계산
    int total_probes = 0, searches = 0;
    for (i = 0; i < num_records; i++) {
        sprintf(name, "Company%07d", i);
        pos = retrieve_rec(name, &n_probe);
        if (pos != -1) {
            total_probes += n_probe;
            searches++;
        }
    }
    printf("Load Factor %.2f: Average probes per search = %.2f\n\n", lf, (float)total_probes / searches);
}

#ifdef TEST_MODE
int main(void) {
    float load_factors[] = {0.25f, 0.50f, 0.75f, 1.0f};
    int num_tests = sizeof(load_factors) / sizeof(load_factors[0]);
    srand((unsigned)time(NULL));
    
    for (int i = 0; i < num_tests; i++) {
        test_load_factor(load_factors[i]);
    }
    
    // 추가: 삭제 테스트 (예: load factor 0.75 상태에서 처음 100건 삭제)
    printf("삭제 테스트 (Load Factor 0.75 상태):\n");
    test_load_factor(0.75f);  // load factor 0.75 상태에서 테이블을 재구성
    int chain_split = 0, n_move;
    char name[300];
    for (int i = 0; i < 100; i++) {
        sprintf(name, "Company%07d", i);
        n_move = delete_rec(name, &chain_split);
        if (n_move >= 0)
            printf("Deleted %s, relocations=%d, chain splits=%d\n", name, n_move, chain_split);
        else
            printf("Failed to delete %s\n", name);
    }
    print_hash_statistics();
    
    return 0;
}
#else
int main(void) {
    printf("TEST_MODE가 정의되지 않았습니다.\n");
    return 0;
}
#endif
