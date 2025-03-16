# B+Tree  
부제: C로 구현하는 B+Tree
## 개요
데이터베이스에서 빠른 데이터 접근과 효율적인 검색 성능을 보장하려면 인덱스(Index)가 필수적입니다. 특히 대용량 데이터를 다룰 때 검색 성능 향상을 위해 자주 사용하는 B+Tree를 직접 C언어로 구현하여 인덱싱 원리를 이해하고자 진행했습니다.

## 문제 정의 및 필요성
### B-Tree 란?
B+Tree는 데이터베이스나 파일 시스템에서 사용되는 균형 잡힌 다중 노드 트리 구조입니다. 내부 노드에는 키와 자식 포인터만 저장하고, 실제 데이터는 리프 노드에만 저장하는 특징을 가지고 있습니다. 모든 연산이 O(log n)의 시간 복잡도를 보장하며, 리프 노드들 간의 링크 연결을 통해 순차 및 범위 검색이 효율적으로 이루어집니다.

'알고리즘 분석'강의에서 학습한 B+Tree 구조를 C언어로 직접 구현하여 데이터베이스 시스템에서의 B+Tree는 어떻게 이루어져있을까에 대한 호기심으로 시작한 프로젝트입니다.

### B-Tree 와 B+Tree
| **특징**           | **B Tree**                                                      | **B+ Tree**                                                      |
|--------------------|-------------------------------------------------------------------|-------------------------------------------------------------------|
| **데이터 저장 위치** | 내부 노드와 리프 노드 모두에 실제 데이터(또는 데이터 포인터) 저장      | 내부 노드에는 키와 포인터만 저장, 실제 데이터는 리프 노드에만 저장     |
| **순차 검색 효율성**  | 리프 노드 간 직접적인 링크가 없어 순차 검색 시 전체 트리를 탐색해야 할 수 있음 | 리프 노드들이 연결되어 있어 순차 및 범위 검색이 효율적임            |
| **검색 경로**        | 내부 노드에서 데이터 검색 후 조기 종료 가능                        | 항상 리프 노드까지 내려가야 하므로 검색 경로가 일정함                    |
| **구조의 단순성**    | 내부 노드와 리프 노드 모두 데이터 저장로 인해 구조가 상대적으로 복잡함   | 데이터가 리프 노드에 집중되어 있어 구조가 단순하고 관리가 용이함         |
| **응용 분야**       | 메모리 내 데이터베이스나 파일 시스템 등에서 사용                     | 대용량 데이터베이스 인덱싱 및 파일 시스템에서 범위 검색에 최적화되어 사용됨 |



---

## 구현 메서드

```
void push(type_ptr_idxnode node);
type_ptr_idxnode pop();
int insert_arec_b_plus_tree(type_rec in_rec);
type_ptr_datanode get_datanode(char* key);
void retrieve(char* name);
int range_search(char* key1, char* key2);
void test_bplus_tree_search(int totalSearches);
void test_bplus_tree_range_search();
void test_bplus_tree_for_size(int totalTests);
int main(void);
```

---

<details>
<summary>void push(type_ptr_idxnode node)</summary>

```c
void push(type_ptr_idxnode node) {
    if(top >= MAX-1) { 
        printf("stack is full\n"); 
        return; 
    }
    stack[++top] = node;
}
```

</details>

<details>
<summary>type_ptr_idxnode pop()</summary>

```c
type_ptr_idxnode pop() {
    if(top < 0) { 
        printf("stack is empty.\n"); 
        getchar(); 
        return NULL; 
    }
    return stack[top--];
}
```

</details>

<details>
<summary>int insert_arec_b_plus_tree(type_rec in_rec)</summary>

```c
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
```

</details>

<details>
<summary>type_ptr_datanode get_datanode(char* key)</summary>

```c
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
```

</details>

<details>
<summary>void retrieve(char* name)</summary>

```c
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
```

</details>

<details>
<summary>int range_search(char* key1, char* key2)</summary>

```c
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
```

</details>

<details>
<summary>void test_bplus_tree_search(int totalSearches)</summary>

```c
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
```

</details>

<details>
<summary>void test_bplus_tree_range_search()</summary>

```c
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
```

</details>

<details>
<summary>void test_bplus_tree_for_size(int totalTests)</summary>

```c
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
```

</details>

---

## 기본 조건  
- **한 노드에 저장 가능한 키 수**  
  - 인덱스 노드: 최대 4개의 키 (MAXD_idx)  
  - 데이터 노드: 최대 4개의 레코드 (MAXD_data)  
- **분할 기준**  
  - 내부 노드: D_idx(=2)를 기준으로 분할  
  - 데이터 노드: D_data(=2)를 기준으로 분할  
- **레코드 구조체(type_rec)**  
  - 회사명을 저장하는 문자 배열 (크기 100)  
  - 회사명의 길이를 저장하는 정수 leng  
- **인덱스 노드(type_idx_node)**  
  - 정렬된 키 배열과, 자식 포인터(내부 노드) 및 데이터 포인터(리프 노드)를 저장  
  - fill_cnt: 현재 저장된 키 개수  
- **데이터 노드(type_data_node)**  
  - 레코드 배열을 저장하며, 다음 데이터 노드를 가리키는 링크 포인터가 있음  
  - fill_cnt: 현재 저장된 레코드 개수

---

## 주요 기능 및 함수 설명

**void push(type_ptr_idxnode node)**  
- 스택에 인덱스 노드를 삽입  
- 스택이 가득 찼으면 return

**type_ptr_idxnode pop()**  
- 스택에서 인덱스 노드를 꺼내 반환  
- 스택이 비어 있을 경우 NULL을 반환

**int insert_arec_b_plus_tree(type_rec in_rec)**  
- B+Tree에 레코드를 삽입하는 함수  
- 초기 상태에서 ROOT와 HEAD가 없으면, 새 인덱스 노드와 데이터 노드를 생성하여 초기화
- 데이터 노드(HEAD)에 빈 공간이 있으면 해당 위치에 삽입하고  
  데이터 노드가 꽉 찼을 경우 분할을 통해 새 데이터 노드를 생성한 후  
  인덱스 노드의 분할 및 승격 과정을 통해 트리의 균형을 유지합니다.  
- 동일 키가 존재하면 삽입 실패를 반환

***type_ptr_datanode get_datanode(char* key)**  
- 주어진 키에 해당하는 데이터 노드를 찾아 반환
- ROOT부터 리프 노드까지 내려가면서 해당 키가 포함된 데이터 노드 포인터를 반환

***void retrieve(char* name)**  
- 주어진 이름으로 데이터 노드 내에서 레코드를 탐색
- 해당 레코드가 존재하면 "탐색 성공" 존재하지 않으면 "탐색 실패"
**int range_search(char* key1, char* key2)**  
- 시작 키부터 종료 키까지의 범위 내 레코드를 순차적으로 탐색하여 레코드의 개수를 반환

**void test_bplus_tree_search(int totalSearches)**  
- 단일 키 탐색 테스트를 수행하여 총 totalSearches 건의 탐색에 걸린 시간을 측정하고 성공 건수 return

---

### 결론

**void test_bplustree_for_size(int totalTests)**  
총 10만 건, 100만 건, 1000만 건의 레코드에 대해 B+트리에서 삽입, 탐색(단일 및 범위) 테스트를 진행하였습니다.  
각 구간별로 `clock()` 함수를 이용해 시작과 끝의 CPU 시간을 측정하고, 이를 `CLOCKS_PER_SEC`로 나누어 초 단위로 변환하여 출력하도록 구현하였습니다.

결과는 다음과 같습니다. 삽입 테스트에서는 10만, 100만, 1000만으로 선형적으로 데이터 규모를 증가시켰을 때, 이론적인 O(log n) 시간 복잡도에 근접한 성능을 보여주고 있습니다. 또한 B+트리 특성상 리프 노드 간 링크를 통해 범위 탐색하므로 B-Tree보다 빠르게 탐색되는 것을 확인할 수 있었습니다. 

```
==== 10만 건 테스트 ====
총 100000건 삽입, 걸린 시간: 0.024377 초
단일 탐색 테스트: 총 10000건 탐색, 걸린 시간: 0.001355 초, 성공 건수: 10000
범위 탐색 테스트: Company0000000 부터 Company0000100 까지, 출력 레코드 수: 101, 걸린 시간: 0.000179 초

==== 100만 건 테스트 ====
총 1000000건 삽입, 걸린 시간: 0.271533 초
단일 탐색 테스트: 총 10000건 탐색, 걸린 시간: 0.001521 초, 성공 건수: 10000
범위 탐색 테스트: Company0000000 부터 Company0000100 까지, 출력 레코드 수: 101, 걸린 시간: 0.000257 초

==== 1000만 건 테스트 ====
총 10000000건 삽입, 걸린 시간: 2.630727 초
단일 탐색 테스트: 총 10000건 탐색, 걸린 시간: 0.002781 초, 성공 건수: 10000
범위 탐색 테스트: Company0000000 부터 Company0000100 까지, 출력 레코드 수: 101, 걸린 시간: 0.000205 초
```

#### 개인적 측면
B+트리 구현을 통해 인덱스로서의 B+트리 구조와 동작 원리를 직접 체험해볼 수 있었습니다.  
특히 리프 노드의 연결을 활용한 범위 탐색이 매우 빠른 것을 직접 확인할 수 있었습니다.
비록 모든 연산이 메모리 상에서만 수행되므로 실제 디스크 I/O가 포함된 상황과는 다를 수 있다는 한계가 있지만 강의에서 배우지 않았던 삭제 경우를 추가적으로 학습할 수 있었습니다. 


### References
- Tharp, Alan L. 1988. *File Organization and Processing*. Wiley.
https://www.slideshare.net/slideshow/b-and-b-tree-117436342/117436342#2

