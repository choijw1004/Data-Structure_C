# B-Tree
부제: C로 구현하는 B-Tree

## 개요
데이터베이스에서 빠른 데이터 접근과 효율적인 검색 성능을 보장하려면 인덱스(Index)가 필수적입니다. 특히 대용량 데이터를 다룰 때 검색 성능 향상을 위해 자주 사용하는 B-Tree를 직접 C언어로 구현하여 인덱싱 원리를 이해하고자 진행했습니다.

## 문제 정의 및 필요성
### B-Tree 란?
B-Tree는 데이터베이스나 파일 시스템에서 사용되는 균형 잡힌 다중 노드 트리 구조입니다. 삽입, 삭제, 검색의 모든 연산이 O(log n)의 시간 복잡도를 보장하여 빠른 성능을 제공합니다.

'알고리즘 분석', '데이터 베이스'강의에서 학습한 B-Tree 구조를 C언어로 직접 구현하여 데이터베이스 시스템에서의 B-Tree는 어떻게 이루어져있을까에 대한 호기심으로 시작한 프로젝트입니다.

---

## 구현 메서드

```
void create_node();
void BTree_split_child(nodeptr x, int i);
void BTree_insert_nonfull(nodeptr x, ele k);
int BTree_insert(ele k);
nodeptr BTree_search(nodeptr x, char *k, int *idx);
void BTree_delete(nodeptr x, char *k);
int B_tree_deletion(char *key);
nodeptr BTree_search_wrapper(char *key, int *idx);
```
---
<details>
<summary>nodeptr create_node()</summary>

```c
nodeptr create_node() {
    nodeptr x = (nodeptr)malloc(sizeof(node));
    if(x == NULL){ 
        perror("malloc"); 
        exit(1); 
    }
    x->n = 0;
    for(int i = 0; i < MAXK+1; i++){
        x->children[i] = NULL;
    }
    return x;
}
```

</details>

<details>
<summary>void BTree_split_child(nodeptr x, int i)</summary>

```c
void BTree_split_child(nodeptr x, int i) {
    nodeptr y = x->children[i];
    nodeptr z = create_node();
    z->n = T - 1;  // z에는 T-1개의 키가 저장됨 (T=2이면 1개)
    for(int j = 0; j < T - 1; j++){
         z->keys[j] = y->keys[j + T];
    }
    if(y->children[0] != NULL){
         for(int j = 0; j < T; j++){
              z->children[j] = y->children[j + T];
         }
    }
    y->n = T - 1;  // y는 T-1개의 키만 남김
    for(int j = x->n; j >= i + 1; j--){
         x->children[j+1] = x->children[j];
    }
    x->children[i+1] = z;
    for(int j = x->n - 1; j >= i; j--){
         x->keys[j+1] = x->keys[j];
    }
    x->keys[i] = y->keys[T-1];  // y의 중간 키 승격
    x->n++;
}
```

</details>

<details>
<summary>void BTree_insert_nonfull(nodeptr x, ele k)</summary>

```c
void BTree_insert_nonfull(nodeptr x, ele k) {
    int i = x->n - 1;
    if(x->children[0] == NULL) {  // leaf인 경우
         while(i >= 0 && strcmp(k.name, x->keys[i].name) < 0) {
              x->keys[i+1] = x->keys[i];
              i--;
         }
         x->keys[i+1] = k;
         x->n++;
    } else {
         while(i >= 0 && strcmp(k.name, x->keys[i].name) < 0) {
              i--;
         }
         i++;
         if(x->children[i]->n == MAXK) {  // 자식이 꽉 찼으면 먼저 분할
              BTree_split_child(x, i);
              if(strcmp(k.name, x->keys[i].name) > 0)
                  i++;
         }
         BTree_insert_nonfull(x->children[i], k);
    }
}
```

</details>

<details>
<summary>int BTree_insert(ele k)</summary>

```c
int BTree_insert(ele k) {
    if(root == NULL) {
         root = create_node();
         root->keys[0] = k;
         root->n = 1;
         return 1;
    }
    if(root->n == MAXK) {
         nodeptr s = create_node();
         s->children[0] = root;
         BTree_split_child(s, 0);
         int i = 0;
         if(strcmp(k.name, s->keys[0].name) > 0)
              i++;
         BTree_insert_nonfull(s->children[i], k);
         root = s;
         return 2;
    } else {
         BTree_insert_nonfull(root, k);
         return 1;
    }
}
```

</details>

<details>
<summary>nodeptr BTree_search(nodeptr x, char *k, int *idx)</summary>

```c
nodeptr BTree_search(nodeptr x, char *k, int *idx) {
    int i = 0;
    while(i < x->n && strcmp(k, x->keys[i].name) > 0)
         i++;
    if(i < x->n && strcmp(k, x->keys[i].name) == 0) {
         *idx = i;
         return x;
    }
    if(x->children[0] == NULL)
         return NULL;
    return BTree_search(x->children[i], k, idx);
}
```

</details>

<details>
<summary>void BTree_delete(nodeptr x, char *k)</summary>

```c
void BTree_delete(nodeptr x, char *k) {
    int idx = find_key(x, k);
    if(idx < x->n && strcmp(x->keys[idx].name, k) == 0) {
         // 키가 x에 있음
         if(x->children[0] == NULL) {
              for (int i = idx+1; i < x->n; i++)
                   x->keys[i-1] = x->keys[i];
              x->n--;
         } else {
              if(x->children[idx]->n >= T) {
                   ele pred = get_predecessor(x->children[idx]);
                   strcpy(x->keys[idx].name, pred.name);
                   x->keys[idx].nleng = pred.nleng;
                   BTree_delete(x->children[idx], pred.name);
              } else if(x->children[idx+1]->n >= T) {
                   ele succ = get_successor(x->children[idx+1]);
                   strcpy(x->keys[idx].name, succ.name);
                   x->keys[idx].nleng = succ.nleng;
                   BTree_delete(x->children[idx+1], succ.name);
              } else {
                   merge_nodes(x, idx);
                   BTree_delete(x->children[idx], k);
              }
         }
    } else {
         if(x->children[0] == NULL)
              return;
         int flag = (idx == x->n);
         if(x->children[idx]->n < T) {
              if(idx > 0 && x->children[idx-1]->n >= T)
                   borrow_from_prev(x, idx);
              else if(idx < x->n && x->children[idx+1]->n >= T)
                   borrow_from_next(x, idx);
              else {
                   if(idx < x->n)
                        merge_nodes(x, idx);
                   else
                        merge_nodes(x, idx-1);
              }
         }
         if(flag && idx > x->n)
              BTree_delete(x, k);
         else
              BTree_delete(x->children[idx], k);
    }
}
```

</details>

<details>
<summary>int B_tree_deletion(char *key)</summary>

```c
int B_tree_deletion(char *key) {
    if(root == NULL) return 0;
    BTree_delete(root, key);
    if(root->n == 0) {
         nodeptr tmp = root;
         root = root->children[0];
         free(tmp);
    }
    return 1;
}
```

</details>

<details>
<summary>nodeptr BTree_search_wrapper(char *key, int *idx)</summary>

```c
nodeptr BTree_search_wrapper(char *key, int *idx) {
    return BTree_search(root, key, idx);
}
```

</details>

---

## 기본 조건  
한 노드에 최대 3개의 키를 담을 수 있도록 구현하였습니다.
- 레코드 구조체(ele):  
  - 회사명을 저장하는 문자 배열 (크기 100)  
  - 회사명의 길이를 저장하는 정수 nleng  
- B-트리 노드 구조체(node):  
  - 노드에 저장된 키의 개수 n  
  - 정렬된 상태로 최대 3개의 키를 저장하는 키 배열  
  - 최대 4개의 자식 포인터를 저장하는 자식 배열 (leaf이면 모두 NULL)


**nodeptr create_node()**
- 동적 메모리 할당으로 새 노드를 생성  
- 노드의 키 개수를 0으로 초기화  
- 모든 자식 포인터를 NULL로 설정하여 빈 노드임을 명확하게 구분

**void BTree_split_child(nodeptr x, int i)**
- 부모 노드 x의 i번째 자식 노드 y가 꽉 찼을 때 분할 실행  
- 새 노드 z를 생성하여 y의 상위 T개의 키 중 T-1개를 옮김 (T=2이면 1개)  
- y의 중간 키를 부모 x로 승격  
- 부모 노드의 자식 포인터와 키 배열을 재정렬하여 트리 균형 유지

**void BTree_insert_nonfull(nodeptr x, ele k)**

- 노드 x가 꽉 차지 않은 상태에서 올바른 위치에 키 k 삽입  
- 리프 노드이면 오른쪽으로 키를 밀어 넣어 삽입  
- 내부 노드이면 적절한 자식 노드로 내려감  
- 자식 노드가 꽉 차 있으면 먼저 분할 후 재귀적으로 삽입

**int BTree_insert(ele k)**
- 루트 노드부터 키 삽입 시작  
- 루트가 NULL이면 새 노드를 생성하여 삽입  
- 루트가 꽉 차 있으면 새로운 루트를 생성하고 기존 루트를 자식으로 연결한 후 분할하여 삽입  
- 트리의 균형과 정렬 상태 유지

**nodeptr BTree_search(nodeptr x, char *k, int *idx)****
- 루트부터 시작하여 정렬된 키 배열에서 순차적으로 비교하며 탐색  
- 키가 존재하면 해당 인덱스를 *idx에 저장하고 노드 반환  
- 키가 없으면 적절한 자식 노드로 내려가 재귀적으로 탐색

**void BTree_delete(nodeptr x, char *k) 와 int B_tree_deletion(char *key)****
- 리프 노드이면 단순히 키를 제거하고 나머지 키를 당김  
- 내부 노드이면 전임자(predecessor) 또는 후임자(successor)를 찾아 삭제할 자리로 승격  
- 자식 노드의 키가 부족하면 형제 노드로부터 키를 빌리거나 두 자식을 병합  
- B_tree_deletion 함수는 삭제 후 루트 노드의 키가 0이면 자식 노드를 새로운 루트로 설정하여 전체 트리의 정리를 수행


**nodeptr BTree_search_wrapper(char *key, int *idx)****
- 루트 노드부터 키 탐색을 시작하도록 단순하게 호출  
- 사용자가 별도의 노드 인자 신경 쓰지 않고 간편하게 키를 검색할 수 있도록 제공
---
### 결론
**void test_btree_for_size(int totalTests)**
총 10만건, 100만건, 1000만건의 레코드의 삽입, 삭제, 탐색 테스트를 진행하였습니다.
삭제와 탐색 과정에서는 100개의 간격으로 줄여 시간이 과도하게 오래 걸리는 것을 방지하였습니다.

각 구간마다 `clock()`을 통하여 시작과 끝의 CPU 시간을 측정하고 소요시간을 `CLOCKS_PER_SEC`으로 나누어 초 단위로 변환하여 출력하도록 구현하였습니다. 

결과는 다음과 같습니다. 삽입 과정에서는 10만 100만 1000만으로 선형적으로 테스트 크기를 키웠을 때 O(log n)에 가까운 시간복잡도를 보이는 것으로 이론적으로 최대한 가깝게 구현했다는 것을 확인할 수 있었습니다. 
반면 삭제, 탐색 과정에서는 삽입 과정에서만큼 안정적으로 시간이 증가하지는 않지만 전체 연산횟수가 누적되어 측정 된 점을 고려하면 최대한 이론적인 구현이 되었다는 것을 확인할 수 있었습니다.
```
==== 10만 건 테스트 ====
총 100000건 삽입, 걸린 시간: 0.017582 초
총 1000건 탐색, 걸린 시간: 0.001648 초, 성공 건수: 1000
총 1000건 삭제, 걸린 시간: 0.001379 초, 삭제 성공 건수: 1000

==== 100만 건 테스트 ====
총 1000000건 삽입, 걸린 시간: 0.196161 초
총 10000건 탐색, 걸린 시간: 0.023055 초, 성공 건수: 10000
총 10000건 삭제, 걸린 시간: 0.015259 초, 삭제 성공 건수: 10000

==== 1000만 건 테스트 ====
총 10000000건 삽입, 걸린 시간: 1.992676 초
총 100000건 탐색, 걸린 시간: 0.477605 초, 성공 건수: 100000
총 100000건 삭제, 걸린 시간: 0.294188 초, 삭제 성공 건수: 100000
```
#### 개인적 측면
비록 모든 연산이 메모리 상에서만 수행되므로 실제 디스크 I/O가 포함된 상황과는 다를 수 있다는 한계가 있지만 강의에서 배우지 않았던 삭제 경우를 추가적으로 학습할 수 있었습니다. 

### References
Building a B-tree in JavaScript
https://levelup.gitconnected.com/building-a-b-tree-in-javascript-4482dee083cb

Tharp, Alan L. 1988. File Organization and Processing. Wiley.