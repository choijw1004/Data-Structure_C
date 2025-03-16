# Hashing Open Address
부제: C로 구현하는 해싱(Open Address)

## 개요
해시 테이블에서 서로 다른 키(key)가 동일한 해시 값(hash value)을 갖게 되면 충돌(Collision)이 발생합니다. 이를 해결하기 위한 방법 중, 체인(Chaining) 방식과 개방 주소법(Open Address) 중에서 Open Address 기법을 C 언어로 구현하여 추가적으로 학습하고자 하였습니다.


## 문제 정의 및 필요성
'알고리즘 분석' 강의에서 학습한 해싱 충돌(Hash Collision)을 해결하는 방식 중 개방 주소법을 구현하고자 하였습니다.

### 개방 주소법(Open Address)
- 개방 주소법(Open Address): 충돌이 발생하면 빈 슬롯을 탐색하여 해당 위치에 데이터를 삽입하는 기법입니다.
  - 선형 탐사(Linear Probing)를 사용하여 충돌 발생 시 바로 다음 슬롯을 순차적으로 탐색하여 빈 슬롯을 찾고
  - 슬롯이 비어 있거나 삭제된 상태면 해당 위치에 레코드를 저장합니다.

### 개방 주소법의 장단점
| 장점                                    | 단점                                                |
|-----------------------------------------|-------------------------------------------------------|
| 별도의 연결 리스트가 불필요하여 공간 절약 | 테이블이 거의 꽉 차면 탐색 성능이 급격히 저하됨 |
| 간단한 구현 및 관리 용이                 | 삭제 시 상태 관리(EMPTY, DELETED) 필요성 발생 |
| 빠른 슬롯 탐색 가능(낮은 부하율일 때)     | 클러스터링(cluster) 문제로 인해 탐색 성능 저하  |

---

## 기본 조건
- 슬롯 수: Tbl_size = 37533
- 레코드 구조체:
  - name[300]: 회사명
  - monincome: 매출 (단위: 천원)
  - status: 슬롯 상태 (EMPTY, OCCUPIED, DELETED)
- 해시 함수: 문자열을 입력받아 곱셈 기반의 해시 주소를 산출
- 삽입: 홈 주소에서 충돌 발생 시 선형 탐사를 통해 빈 슬롯에 삽입
- 삭제: 상태를 DELETED로 설정하여 직접 삭제를 표현
- 검색: 홈 주소에서부터 상태가 EMPTY일 때까지 탐사하며 동일 키를 검색

---

## 구현 메서드
```
int hash(char recname[]);
int open_address_insert(type_record rec);
int open_address_retrieve(char* key, int* probe);
int open_address_delete(char* key, int* probe);
void print_open_address_statistics();
float compute_average_probes_search(int num_records);
void test_load_factor(float lf);
int main(void);
```
---

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
앞서 체이닝 기법에서와 똑같이 문자열 곱셈 해싱 방법을 적용하여 구현하였습니다.

</details>

<details>
<summary>int open_address_insert(type_record rec)</summary>

```c
int open_address_insert(type_record rec) {
    int HA = hash(rec.name);
    int probes = 1;
    int index = HA;
    while (Hashtable[index].status == OCCUPIED) {
        if (strcmp(Hashtable[index].name, rec.name) == 0)
            return -1;  // 중복 키 존재 시 삽입 실패
        index = (index + 1) % Tbl_size;
        probes++;
        if (index == HA)
            return -1;  // 테이블이 가득 참
    }
    Hashtable[index] = rec;
    Hashtable[index].status = OCCUPIED;
    return probes;
}
```
- 선형 탐사를 통해 빈 슬롯을 찾아 데이터를 삽입합니다.
- 중복 키 존재 시 삽입하지 않고 실패 처리합니다.

</details>

<details>
<summary>int open_address_retrieve(char* key, int* probe)</summary>

```c
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
    return -1;  // 찾지 못함
}
```
- 선형 탐사하여 프로브를 기록합니다.

</details>

<details>
<summary>int open_address_delete(char* key, int* probe)</summary>

```c
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
    return -1;  // 찾지 못함
}
```
- 주어진 키의 데이터를 삭제하여 상태를 DELETED로 설정합니다.

</details>
<details><summary>float compute_average_probes_search(int num_records)</summary>

```c
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
```
</details>
<details>
<summary>void print_open_address_statistics()</summary>

- 전체 슬롯 개수, 사용된 슬롯 개수, 현재의 Load Factor를 출력하여 해시 테이블 상태를 파악합니다.

</details>

<details>
<summary>void test_load_factor(float lf)</summary>

```c

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

```

- 부하율을 지정하여 합성 데이터를 삽입 테스트를 진행합니다.

</details>

---

## 결론
```
Load Factor 0.25: Inserted 9383 records.
전체 슬롯: 37533, 사용된 슬롯: 9383, load factor: 0.25
Load Factor 0.25: Average probes per search = 3.03

Load Factor 0.50: Inserted 18766 records.
전체 슬롯: 37533, 사용된 슬롯: 18766, load factor: 0.50
Load Factor 0.50: Average probes per search = 5.41

Load Factor 0.75: Inserted 28149 records.
전체 슬롯: 37533, 사용된 슬롯: 28149, load factor: 0.75
Load Factor 0.75: Average probes per search = 7.98

Load Factor 1.00: Inserted 37533 records.
전체 슬롯: 37533, 사용된 슬롯: 37533, load factor: 1.00
Load Factor 1.00: Average probes per search = 60.19
```

Open Address 방식은 부하율이 높아질수록 급격히 평균 프로브 수가 증가하여 성능이 크게 저하되는 특성이 있음을 확인할 수 있었습니다. 따라서 일반적으로 해시 테이블의 부하율은 0.75 이하로 관리하는 것이 바람직함을 이해할 수 있었습니다.

---

## 개인적 측면
- 개방 주소법을 직접 구현하면서 상태 관리(OCCUPIED, EMPTY, DELETED)의 중요성을 이해하고 특히 삭제 연산의 구현 복잡성에 대해 깊이 체험할 수 있었습니다.
- 체인 방식과 달리, 개방 주소법은 슬롯 상태 관리에 신경을 써야 하므로 삭제 과정에서 특히 구현 복잡성이 상당히 높았습니다.
- 선형탐색법으로 개발을 진행했지만 이차 탐사법, 이중 해시법이라는 기법이 남아있어 기회가 되면 두 개의 기법도 구현해보고자 합니다.

---

## References
- [Hashing with Open Addressing (Wikipedia)](https://en.wikipedia.org/wiki/Open_addressing)
- [Stack Overflow: Why is load factor 0.75 the default?](https://stackoverflow.com/questions/10901752/why-is-the-default-load-factor-075-in-java-hashmap)

