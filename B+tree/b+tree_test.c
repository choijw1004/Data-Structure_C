#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define MAXD_idx 4		
#define MAXD_data 4
#define D_idx 2	
#define D_data 2  
#define MAX 100

typedef char type_key[100];

typedef struct record {
    type_key name;
    int leng;
} type_rec;

typedef struct idxnode* type_ptr_idxnode;
typedef struct datanode* type_ptr_datanode;

typedef struct idxnode {
    type_key key[MAXD_idx];
    type_ptr_idxnode ptri[MAXD_idx+1];
    type_ptr_datanode ptrd[MAXD_idx+1];
    int fill_cnt;
} type_idx_node;

typedef struct datanode {
    type_rec rec[MAXD_data];
    type_ptr_datanode link;
    int fill_cnt;
} type_data_node;

typedef struct big_node_index {
    type_key key[MAXD_idx+1];
    type_ptr_idxnode ptri[MAXD_idx+2];
    type_ptr_datanode ptrd[MAXD_idx+2];
} big_node_index;

typedef struct big_node_data {
    type_rec rec[MAXD_data+1];
} big_node_data;

type_ptr_idxnode ROOT = NULL;
type_ptr_datanode HEAD = NULL;
type_ptr_idxnode stack[MAX];
int top = -1;

void push(type_ptr_idxnode node) {
    if(top >= MAX-1) { printf("stack is full\n"); return; }
    stack[++top] = node;
}

type_ptr_idxnode pop() {
    if(top < 0) { printf("stack is empty.\n"); getchar(); return NULL; }
    return stack[top--];
}

int insert_arec_b_plus_tree(type_rec in_rec) {
    int i, j;
    type_ptr_idxnode parent = NULL, curr = NULL, child = NULL, new_ptri = NULL, tptr = NULL;
    type_ptr_datanode curr_d = NULL, new_ptrd = NULL, child_d = NULL;
    big_node_index bnode_index;
    big_node_data bnode_data;
    int fc, down_idx;
    type_key in_key;
    strcpy(in_key, in_rec.name);
    if(!ROOT) {
        ROOT = (type_ptr_idxnode)malloc(sizeof(type_idx_node));
        HEAD = (type_ptr_datanode)malloc(sizeof(type_data_node));
        HEAD->rec[0] = in_rec;
        HEAD->fill_cnt = 1;
        HEAD->link = NULL;
        ROOT->ptri[0] = NULL;
        ROOT->ptrd[0] = HEAD;
        ROOT->ptrd[1] = NULL;
        strcpy(ROOT->key[0], in_rec.name);
        ROOT->fill_cnt = 1;
        return 1;
    }
    else if(!ROOT->ptri[0] && !ROOT->ptrd[1]) {
        fc = HEAD->fill_cnt;
        if(fc < MAXD_data) {
            for(i = 0; i < fc; i++) {
                if(strcmp(in_key, HEAD->rec[i].name) < 0)
                    break;
                else if(strcmp(in_key, HEAD->rec[i].name) == 0) {
                    printf("동일키 이미 존재하여 삽입 실패!\n");
                    return 0;
                }
            }
            for(j = fc; j > i; j--)
                HEAD->rec[j] = HEAD->rec[j-1];
            HEAD->rec[i] = in_rec;
            HEAD->fill_cnt++;
            strcpy(ROOT->key[0], HEAD->rec[fc].name);
            return 1;
        }
        else {
            for(i = 0; i < MAXD_data; i++) {
                if(strcmp(in_key, HEAD->rec[i].name) < 0)
                    break;
                else if(strcmp(in_key, HEAD->rec[i].name) == 0) { 
                    printf("동일키 이미 존재하여 삽입 실패!\n");
                    return 0;
                }
            }
            for(j = 0; j < i; j++)
                bnode_data.rec[j] = HEAD->rec[j];
            bnode_data.rec[j] = in_rec;
            j++;
            while(i < MAXD_data) {
                bnode_data.rec[j] = HEAD->rec[i];
                j++;
                i++;
            }
            for(i = 0; i <= D_data; i++)
                HEAD->rec[i] = bnode_data.rec[i];
            HEAD->fill_cnt = D_data+1;
            new_ptrd = (type_ptr_datanode)malloc(sizeof(type_data_node));
            for(i = 0; i < D_data; i++)
                new_ptrd->rec[i] = bnode_data.rec[i+1+D_data];
            new_ptrd->fill_cnt = D_data;
            strcpy(ROOT->key[0], HEAD->rec[D_data].name);
            ROOT->ptrd[1] = new_ptrd;
            HEAD->link = new_ptrd;
            new_ptrd->link = NULL;
            return 1;
        }
    }
    curr = ROOT;
    top = -1;
    if(ROOT->ptri[0] != NULL) {
        do {
            for(i = 0; i < curr->fill_cnt; i++) {
                if(strcmp(in_key, curr->key[i]) <= 0)
                    break;
            }
            push(curr);
            curr = curr->ptri[i];
            if(curr->ptri[0] == NULL)
                break;
        } while(1);
    }
    for(i = 0; i < curr->fill_cnt; i++)
        if(strcmp(in_key, curr->key[i]) <= 0)
            break;
    parent = curr;
    curr_d = curr->ptrd[i];
    down_idx = i;
    fc = curr_d->fill_cnt;
    if(fc < MAXD_data) {
        for(i = 0; i < fc; i++) {
            if(strcmp(in_key, curr_d->rec[i].name) < 0)
                break;
            else if(strcmp(in_key, curr_d->rec[i].name)==0) {
                printf("동일키 이미 존재하여 삽입 실패!\n");
                return 0;
            }
        }
        for(j = fc; j > i; j--)
            curr_d->rec[j] = curr_d->rec[j-1];
        curr_d->rec[i] = in_rec;
        curr_d->fill_cnt++;
        if(down_idx < parent->fill_cnt)
            strcpy(parent->key[down_idx], curr_d->rec[fc].name);
        return 1;
    }
    else {
        for(i = 0; i < MAXD_data; i++) {
            if(strcmp(in_key, curr_d->rec[i].name) < 0)
                break;
            else if(strcmp(in_key, curr_d->rec[i].name)==0) {
                printf("동일키 이미 존재하여 삽입 실패!\n");
                return 0;
            }
        }
        for(j = 0; j < i; j++)
            bnode_data.rec[j] = curr_d->rec[j];
        bnode_data.rec[j] = in_rec;
        j++;
        while(i < MAXD_data) {
            bnode_data.rec[j] = curr_d->rec[i];
            j++;
            i++;
        }
        for(i = 0; i <= D_data; i++)
            curr_d->rec[i] = bnode_data.rec[i];
        curr_d->fill_cnt = D_data+1;
        new_ptrd = (type_ptr_datanode)malloc(sizeof(type_data_node));
        for(i = 0; i < D_data; i++)
            new_ptrd->rec[i] = bnode_data.rec[i+1+D_data];
        new_ptrd->fill_cnt = D_data;
        new_ptrd->link = NULL;
        new_ptrd->link = curr_d->link;
        curr_d->link = new_ptrd;
        curr = parent;
        strcpy(in_key, curr_d->rec[D_data].name);
        child_d = new_ptrd;
    }
    do {
        if(curr->fill_cnt < MAXD_idx) {
            for(i = 0; i < curr->fill_cnt; i++)
                if(strcmp(in_key, curr->key[i]) < 0)
                    break;
            for(j = curr->fill_cnt; j > i; j--) {
                curr->ptrd[j+1] = curr->ptrd[j];
                curr->ptri[j+1] = curr->ptri[j];
                strcpy(curr->key[j], curr->key[j-1]);
            }
            strcpy(curr->key[i], in_key);
            if(curr->ptri[0] == NULL) {
                curr->ptrd[i+1] = child_d;
                curr->ptri[i+1] = NULL;
            } else {
                curr->ptri[i+1] = child;
               	curr->ptrd[i+1] = NULL;
            }
            curr->fill_cnt++;
            return 1;
        } else {
            for(i = 0; i < MAXD_idx; i++) {
                if(strcmp(in_key, curr->key[i]) < 0)
                    break;
            }
            bnode_index.ptri[0] = curr->ptri[0];
            bnode_index.ptrd[0] = curr->ptrd[0];
            for(j = 0; j < i; j++) {
                strcpy(bnode_index.key[j], curr->key[j]);
                bnode_index.ptri[j+1] = curr->ptri[j+1];
                bnode_index.ptrd[j+1] = curr->ptrd[j+1];
            }
            strcpy(bnode_index.key[j], in_key);
            if(curr->ptri[0] == NULL) {
                bnode_index.ptrd[j+1] = child_d;
                bnode_index.ptri[j+1] = NULL;
            } else {
                bnode_index.ptri[j+1] = child;
                bnode_index.ptrd[j+1] = NULL;
            }
            j++;
            while(i < MAXD_idx) {
                strcpy(bnode_index.key[j], curr->key[i]);
                bnode_index.ptri[j+1] = curr->ptri[i+1];
                bnode_index.ptrd[j+1] = curr->ptrd[i+1];
                j++;
                i++;
            }
            for(i = 0; i < D_idx; i++) {
                curr->ptri[i] = bnode_index.ptri[i];
                curr->ptrd[i] = bnode_index.ptrd[i];
                strcpy(curr->key[i], bnode_index.key[i]);
            }
            curr->ptri[i] = bnode_index.ptri[i];
            curr->ptrd[i] = bnode_index.ptrd[i];
            curr->fill_cnt = D_idx;
            new_ptri = (type_ptr_idxnode)malloc(sizeof(type_idx_node));
            for(i = 0; i < D_idx+1; i++) {
                new_ptri->ptri[i] = bnode_index.ptri[i+1+D_idx];
                new_ptri->ptrd[i] = bnode_index.ptrd[i+1+D_idx];
                strcpy(new_ptri->key[i], bnode_index.key[i+1+D_idx]);
            }
            new_ptri->ptri[i] = bnode_index.ptri[i+1+D_idx];
            new_ptri->ptrd[i] = bnode_index.ptrd[i+1+D_idx];
            new_ptri->fill_cnt = D_idx;
            strcpy(in_key, bnode_index.key[D_idx]);
            child = new_ptri;
            if(top >= 0)
                curr = pop();
            else {
                tptr = (type_ptr_idxnode)malloc(sizeof(type_idx_node));
                strcpy(tptr->key[0], in_key);
                tptr->ptri[0] = curr;
                tptr->ptri[1] = child;
                tptr->ptrd[0] = NULL;
                tptr->fill_cnt = 1;
                ROOT = tptr;
                return 1;
            }
        }
    } while(1);
}

type_ptr_datanode get_datanode(char* key) {
    int i;
    type_ptr_idxnode curr = ROOT;
    if(curr == NULL)
        return NULL;
    while(curr->ptri[0] != NULL) {
        for(i = 0; i < curr->fill_cnt; i++) {
            if(strcmp(key, curr->key[i]) <= 0)
                break;
        }
        curr = curr->ptri[i];
    }
    return curr->ptrd[i];
}

void retrieve(char* name) {
    int i;
    type_ptr_datanode dnode = get_datanode(name);
    if(dnode == NULL) {
        printf("데이터 노드 없음\n");
        return;
    }
    for(i = 0; i < dnode->fill_cnt; i++) {
        if(strcmp(name, dnode->rec[i].name)==0) {
            printf("탐색 성공. 이름 = %s, 길이 = %d\n", dnode->rec[i].name, dnode->rec[i].leng);
            return;
        }
    }
    printf("탐색 실패: %s 없음\n", name);
}

int range_search(char* key1, char* key2) {
    int count = 0, i;
    FILE* fp = fopen("range_search_output.txt", "w");
    if(fp == NULL) {
        printf("파일 열기 실패\n");
        return 0;
    }
    type_ptr_datanode startNode = get_datanode(key1);
    type_ptr_datanode endNode = get_datanode(key2);
    if(startNode == NULL) {
        fclose(fp);
        return 0;
    }
    int startIndex = 0;
    for(i = 0; i < startNode->fill_cnt; i++) {
        if(strcmp(startNode->rec[i].name, key1) >= 0) {
            startIndex = i;
            break;
        }
    }
    type_ptr_datanode curr = startNode;
    while(curr != NULL) {
        for(i = (curr == startNode ? startIndex : 0); i < curr->fill_cnt; i++) {
            if(strcmp(curr->rec[i].name, key2) > 0) {
                fclose(fp);
                return count;
            }
            fprintf(fp, "%s %d\n", curr->rec[i].name, curr->rec[i].leng);
            count++;
        }
        if(curr == endNode)
            break;
        curr = curr->link;
    }
    fclose(fp);
    return count;
}

void test_bplus_tree_search(int totalSearches) {
    int i, foundCount = 0;
    char companyName[100];
    type_ptr_datanode result;
    clock_t start, end;
    double search_time;
    start = clock();
    for(i = 0; i < totalSearches; i++) {
        sprintf(companyName, "Company%07d", i);
        result = get_datanode(companyName);
        if(result != NULL)
            foundCount++;
    }
    end = clock();
    search_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("단일 탐색 테스트: 총 %d건 탐색, 걸린 시간: %f 초, 성공 건수: %d\n", totalSearches, search_time, foundCount);
}

void test_bplus_tree_range_search() {
    char startKey[100], endKey[100];
    int count;
    clock_t start, end;
    double search_time;
    strcpy(startKey, "Company0000000");
    strcpy(endKey, "Company0000100");
    start = clock();
    count = range_search(startKey, endKey);
    end = clock();
    search_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("범위 탐색 테스트: %s 부터 %s 까지, 출력 레코드 수: %d, 걸린 시간: %f 초\n", startKey, endKey, count, search_time);
}

void test_bplus_tree_for_size(int totalTests) {
    int i;
    type_rec testRec;
    char companyName[100];
    clock_t start, end;
    double insertion_time;
    ROOT = NULL;
    HEAD = NULL;
    top = -1;
    start = clock();
    for(i = 0; i < totalTests; i++) {
        sprintf(companyName, "Company%07d", i);
        strcpy(testRec.name, companyName);
        testRec.leng = (int)strlen(companyName);
        if(!insert_arec_b_plus_tree(testRec)) { }
    }
    end = clock();
    insertion_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("총 %d건 삽입, 걸린 시간: %f 초\n", totalTests, insertion_time);
    test_bplus_tree_search(10000);
    test_bplus_tree_range_search();
}

int main(void) {
    printf("==== 10만 건 테스트 ====\n");
    test_bplus_tree_for_size(100000);
    ROOT = NULL; HEAD = NULL; top = -1;
    printf("\n==== 100만 건 테스트 ====\n");
    test_bplus_tree_for_size(1000000);
    ROOT = NULL; HEAD = NULL; top = -1;
    printf("\n==== 1000만 건 테스트 ====\n");
    test_bplus_tree_for_size(10000000);
    return 0;
}
