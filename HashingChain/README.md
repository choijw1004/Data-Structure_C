# Hashing Chaining
부제: C로 구현하는 해싱(Chaining)

## 개요
해시 테이블에서 서로 다른 키(key)가 동일한 해시 값(hash value)을 갖게 되면 충돌(Collision)이 발생합니다. 이를 해결하기 위한 방법으로 Hashing Chain 기법과 OpenAdress기법(개방주소법) 중 Hahing Chain 기법을 C언어로 구현하여 추가적으로 학습하고자 하였습니다.


## 문제 정의 및 필요성
'알고리즘 분석'강의에서 학습한 Hahing Collision을 해결하는 방식 중 Hahing Chain 기법을 구현하고자 하였습니다.
### 해싱(Chaining)
- 해싱(Hashing): 키(예: 문자열)를 일정 길이의 해시 값으로 변환하여 빠른 접근을 가능케 하는 기법입니다.  
- 체인(Chaining) 방식: 같은 해시 값이 발생하면 연결 리스트(혹은 동적 배열 등)를 이용해 충돌된 레코드들을 하나의 버킷(슬롯) 아래에 연결합니다. 
  - 각 버킷은 하나의 레코드를 저장하고 충돌이 발생하면 링크를 통해 새 레코드를 연결하고  
  - 오버플로(Overflow)가 발생했을 때 추가로 확보할 공간만 있으면 계속 레코드를 연결할 수 있으므로 단순 선형 탐색과 달리 테이블이 꽉 차더라도 충돌을 해소하기 용이합니다.

### 체인 방식의 장단점
| 장점                                   | 단점                                         |
|-------------------------------------------|--------------------------------------------------|
| 충돌 시 버킷 확장이 용이                | 해시 테이블 외부에 별도 공간 필요(연결 리스트)  |
| 삽입/삭제 시 테이블 재배열 부담 적음    | 최악의 경우 연결 리스트가 길어져 검색 성능 저하 |
| 해시 테이블이 거의 꽉 차도 동작 가능     | 포인터(링크) 관리로 인한 구현 복잡도           |

---
## 기본 조건
- 슬롯 수: Tbl_size = 37533 
- 레코드 구조체:  
  - name[300] : 회사명  
  - monincome : 매출 (단위: 천원)  
  - link : 체인 연결을 위한 다음 슬롯 인덱스 (초기 -1)  
- 해시 함수: 문자열을 입력받아 곱셈 기반의 해시 주소를 산출  
- 삽입: 홈 주소가 비어 있지 않을 경우, 마지막 링크(-1) 직전까지 이동 후 빈 슬롯을 찾아 연결  
- 삭제: 체인 중간에 있는 레코드도 재배치(Chain Reorganization)를 통해 삭제 처리  
- 검색: 체인을 따라가며 동일 키가 발견되면 성공, 못 찾으면 -1 반환  

## 구현 메서드

```
int hash(char recname[]);
int find_empty_location();
int insert_rec(type_record rec);
int retrieve_rec(char* key, int* probe);
int delete_rec(char* dkey, int* chain_split);
int del_middle(int s, int p, int* chain_split);
void delete_multiple(int num_del_req);
void print_hash_statistics();
void test_load_factor(float lf);
int main(void);
```

<details>
<summary>int hash(char recname[])</summary>

```c
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
```

- 문자열 recname을 입력받아, 특정 방식으로 곱셈 연산을 수행하여 Tbl_size로 나눈 나머지를 해시 주소로 반환합니다.  
- 문자 간 곱셈 방식을 이용한 해시 방식을 채택하였습니다.

</details>

<details>
<summary>int find_empty_location()</summary>

```c
int find_empty_location() {
    int curr = LAST;
    while (curr >= 0 && Hashtable[curr].name[0] != '\0')
        curr--;
    return curr;
}
```

- 체인에서 오버플로가 발생했을 때 새 레코드를 저장할 수 있는 빈 슬롯을 뒤에서부터(가장 높은 주소) 탐색합니다.  
- 더 이상 빈 슬롯이 없으면 -1을 반환합니다.

</details>

<details>
<summary>int insert_rec(type_record rec)</summary>

```c
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
```

- 체인 방식으로 레코드를 삽입합니다.  
- 홈 주소(HA)가 비어 있지 않으면 해당 버킷의 링크를 따라가면서 마지막까지 이동한 뒤 새 슬롯을 할당받아 연결합니다.  
- 이미 동일 키가 존재하면 삽입 실패를 return  

</details>

<details>
<summary>int retrieve_rec(char* key, int* probe)</summary>

```c
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
```

- 주어진 key에 대한 홈 주소를 구한 뒤, 체인을 따라가며 비교 
- 찾으면 해당 슬롯 인덱스를 반환하고, 찾지 못하면 -1을 반환
- *probe에는 검색 시 몇 번의 비교(프로빙)가 일어났는지 저장

</details>

<details>
<summary>int delete_rec(char* dkey, int* chain_split)</summary>

```c
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
```

- 주어진 dkey를 체인에서 찾아 제거하고 제거 과정에서 재배치(체인 재구성)가 일어날 경우 이동된 레코드 수nmove를 반환합니다.
- del_start와 del_middle 함수를 통해 체인의 시작 부분 혹은 중간 부분에서 레코드를 삭제하고, 링크를 재정비합니다.

</details>

<details>
<summary>int del_middle(int s, int p, int* chain_split)</summary>

```c
int del_middle(int s, int p, int* chain_split) {
    // 체인 중간에서 s를 홈주소로 하는 마지막 레코드를 찾아 옮기고 재귀적으로 처리
    ...
}
```

- 체인의 중간(버킷 `s`)에 있는 레코드를 제거할 때
  - 그 레코드가 홈 주소가 같은 다른 레코드들의 체인을 끊지 않도록  
  - 필요하면 체인 재배치를 수행합니다.  
- chain_split은 체인이 분리(split)되는 횟수를 계산합니다.

</details>

<details>
<summary>void delete_multiple(int num_del_req)</summary>

```c
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
```

- num_del_req개의 레코드를 연속해서 삭제하고,  
  - 성공적으로 삭제된 레코드 수  
  - 삭제 시 재배치된 레코드 수  
  를 출력합니다.

</details>

<details>
<summary>void print_hash_statistics()</summary>

```c
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
```

- 해시 테이블에서 실제로 사용 중인 슬롯 수, 평균 체인 길이, 최대 체인 길이 등을 계산하여 통계 정보를 출력합니다.

</details>

<details>
<summary>Load Factor 테스트: void test_load_factor(float lf)</summary>

```c
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
```

- 부하율(Load Factor)에 따라 레코드를 삽입하고 체인 길이, 평균 프로브 수 등을 측정합니다.  
- 예: `lf = 0.75`라면 전체 슬롯 37533개의 75%인 약 28150개 레코드를 삽입하여 테스트합니다.

</details>

<details>
<summary>부하율을 조절하여 테스트를 진행하는 int main(void)</summary>

```c
#ifdef TEST_MODE
int main(void) {
    float load_factors[] = {0.25f, 0.50f, 0.75f, 1.0f};
    int num_tests = sizeof(load_factors) / sizeof(load_factors[0]);
    srand((unsigned)time(NULL));
    
    for (int i = 0; i < num_tests; i++) {
        test_load_factor(load_factors[i]);
    }
    
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
```

- TEST_MODE가 정의되어 있으면 체인 해시 테이블에 대해  
  - 서로 다른 Load Factor(0.25, 0.50, 0.75, 1.0)  
  - 삭제 테스트  
  등을 수행하고 결과를 출력합니다.

</details>

---

## 결론
```
Load Factor 0.25: Inserted 9383 records.
전체 슬롯: 37533, 사용된 슬롯: 9383, 평균 체인 길이: 3.20, 최대 체인 길이: 18
Load Factor 0.25: Average probes per search = 3.14

Load Factor 0.50: Inserted 18766 records.
전체 슬롯: 37533, 사용된 슬롯: 18766, 평균 체인 길이: 5.60, 최대 체인 길이: 29
Load Factor 0.50: Average probes per search = 5.51

Load Factor 0.75: Inserted 28149 records.
전체 슬롯: 37533, 사용된 슬롯: 28149, 평균 체인 길이: 8.07, 최대 체인 길이: 43
Load Factor 0.75: Average probes per search = 7.92

Load Factor 1.00: Inserted 37533 records.
전체 슬롯: 37533, 사용된 슬롯: 37533, 평균 체인 길이: 10.63, 최대 체인 길이: 63
Load Factor 1.00: Average probes per search = 10.47
```
Java에서 load factor가 0.75f인 것을 확인하고 왜 0.75f인지 테스트 해보고자했습니다.

전체 슬롯의 개수(37533개) 중 적재율(load factor)을 0.25 0.5 0.75 1.0으로 늘려가며 테스트 해보았을때 
- 부하율의 증가는 체인의 길이의 증가로 이어진다.
- 최대 체인의 길이가 증가한다.
- 탐색시 평균 탐색 횟수가 증가한다.
와 같은 결론을 도출할 수 있었습니다. 

부하율이 낮을 때는 슬롯이 넉넉하여 검색속도가 빠르다는 장점이 있지만 낮은 부하율은 메모리를 과도하게 사용할 수 있다는 결론을 내렸습니다.

0.75f는 포아송 분포이기 때문에 대부분의 hash function에서 default로 사용한다고 합니다. stack over flow에서 11년전에 저와 똑같은 질문을 한 사람이 있었고 답변을 통해 이해할 수 있었습니다. 

#### 개인적 측면
- 삭제 시 체인 재배치 로직을 직접 구현하면서 해시 테이블에서 삭제 연산이 얼마나 까다로운지 체험할 수 있었습니다.
- 다양한 load factor에서의 성능 비교를 통해, 해시 함수와 테이블 크기가 충돌 발생 빈도에 큰 영향을 준다는 사실을 확인했습니다.

---

### References
- Tharp, Alan L. 1988. *File Organization and Processing*. Wiley.  
- [Slideshare: 해싱(Hasing) 기법](https://www.slideshare.net/slideshow/b-and-b-tree-117436342/117436342#2)  