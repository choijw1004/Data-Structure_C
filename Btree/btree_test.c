#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h> 

#define T 2
#define MAXK (2 * T - 1)       // 최대 키 수, T=2이면 3
#define MIN_KEYS (T - 1)       // 최소 키 수, T=2이면 1
#define false 0
#define true 1

// 레코드 구조체: 회사명과 그 길이
typedef struct element {
    char name[100];
    int nleng;
} ele;

// B-트리 노드 구조체
typedef struct node* nodeptr;
typedef struct node {
    int n;                   // 노드에 저장된 키의 수
    ele keys[MAXK];          // 키 배열 (정렬되어 있음)
    nodeptr children[MAXK+1]; // 자식 포인터 배열 (leaf이면 모두 NULL)
} node;

nodeptr root = NULL;

// 새로운 노드 생성 함수
nodeptr create_node() {
    nodeptr x = (nodeptr)malloc(sizeof(node));
    if(x == NULL){ perror("malloc"); exit(1); }
    x->n = 0;
    for(int i = 0; i < MAXK+1; i++){
        x->children[i] = NULL;
    }
    return x;
}

// 부모의 i번째 자식 y가 꽉 찼을 때, y를 분할하고 새 노드 z를 생성하여 부모 x에 중간 키와 함께 삽입
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

// x 노드가 비어있지 않은 상태에서 새 키 k를 삽입하는 함수
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

// B-트리 삽입 함수: 루트가 꽉 찼다면 새로운 루트를 생성하고, 비어있는 노드에 삽입
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

// 재귀 탐색 함수
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

// 헬퍼 함수: x에서 key보다 크거나 같은 첫 번째 인덱스 반환
int find_key(nodeptr x, char *key) {
    int idx = 0;
    while(idx < x->n && strcmp(x->keys[idx].name, key) < 0)
         idx++;
    return idx;
}

// 헬퍼 함수: x의 전임자 키 (왼쪽 서브트리의 가장 오른쪽 leaf의 마지막 키)
ele get_predecessor(nodeptr x) {
    while(x->children[x->n] != NULL)
         x = x->children[x->n];
    return x->keys[x->n - 1];
}

// 헬퍼 함수: x의 후임자 키 (오른쪽 서브트리의 가장 왼쪽 leaf의 첫 키)
ele get_successor(nodeptr x) {
    while(x->children[0] != NULL)
         x = x->children[0];
    return x->keys[0];
}

// 두 자식 노드를 병합: 부모의 keys[idx]를 끌어와 병합
void merge_nodes(nodeptr parent, int idx) {
    nodeptr child = parent->children[idx];
    nodeptr sibling = parent->children[idx+1];
    child->keys[child->n] = parent->keys[idx];
    for (int i = 0; i < sibling->n; i++){
         child->keys[child->n + 1 + i] = sibling->keys[i];
    }
    if(child->children[0] != NULL){
         for (int i = 0; i <= sibling->n; i++){
              child->children[child->n + 1 + i] = sibling->children[i];
         }
    }
    child->n += sibling->n + 1;
    for (int i = idx+1; i < parent->n; i++){
         parent->keys[i-1] = parent->keys[i];
         parent->children[i] = parent->children[i+1];
    }
    parent->n--;
    free(sibling);
}

// 왼쪽 형제로부터 키 빌리기
void borrow_from_prev(nodeptr parent, int idx) {
    nodeptr child = parent->children[idx];
    nodeptr sibling = parent->children[idx-1];
    for (int i = child->n - 1; i >= 0; i--){
         child->keys[i+1] = child->keys[i];
    }
    if(child->children[0] != NULL){
         for (int i = child->n; i >= 0; i--){
              child->children[i+1] = child->children[i];
         }
    }
    child->keys[0] = parent->keys[idx-1];
    if(child->children[0] != NULL)
         child->children[0] = sibling->children[sibling->n];
    parent->keys[idx-1] = sibling->keys[sibling->n - 1];
    sibling->n--;
    child->n++;
}

// 오른쪽 형제로부터 키 빌리기
void borrow_from_next(nodeptr parent, int idx) {
    nodeptr child = parent->children[idx];
    nodeptr sibling = parent->children[idx+1];
    child->keys[child->n] = parent->keys[idx];
    if(child->children[0] != NULL)
         child->children[child->n+1] = sibling->children[0];
    child->n++;
    parent->keys[idx] = sibling->keys[0];
    for (int i = 1; i < sibling->n; i++){
         sibling->keys[i-1] = sibling->keys[i];
    }
    if(sibling->children[0] != NULL){
         for (int i = 1; i <= sibling->n; i++){
              sibling->children[i-1] = sibling->children[i];
         }
    }
    sibling->n--;
}

// 재귀적으로 x에서 key를 삭제하는 함수
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

// 삭제 함수의 래퍼: key 삭제 후 루트 조정
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

// 재귀 탐색 함수 (래퍼)
nodeptr BTree_search_wrapper(char *key, int *idx) {
    return BTree_search(root, key, idx);
}

//--------------------------------------
// 성능 측정 테스트 함수: 10만, 100만, 1000만 건 테스트
void test_btree_for_size(int totalTests) {
    int searchCount = totalTests / 100;    // 100 간격 탐색
    int deletionCount = totalTests / 100;    // 100 간격 삭제
    ele testRec;
    char companyName[100];
    int idx;
    nodeptr tp;
    clock_t start, end;
    double insertion_time, search_time, deletion_time;
    
    // 초기화
    root = NULL;
    
    // 삽입 테스트
    start = clock();
    for (int i = 0; i < totalTests; i++) {
         sprintf(companyName, "Company%07d", i);
         strcpy(testRec.name, companyName);
         testRec.nleng = (int)strlen(companyName);
         BTree_insert(testRec);
    }
    end = clock();
    insertion_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("총 %d건 삽입, 걸린 시간: %f 초\n", totalTests, insertion_time);
    
    // 탐색 테스트 (100 간격)
    start = clock();
    int foundCount = 0;
    for (int i = 0; i < searchCount; i++) {
         sprintf(companyName, "Company%07d", i * 100);
         tp = BTree_search(root, companyName, &idx);
         if(tp != NULL)
              foundCount++;
    }
    end = clock();
    search_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("총 %d건 탐색, 걸린 시간: %f 초, 성공 건수: %d\n", searchCount, search_time, foundCount);
    
    // 삭제 테스트 (100 간격)
    start = clock();
    int deletionSuccess = 0;
    for (int i = 0; i < deletionCount; i++) {
         sprintf(companyName, "Company%07d", i * 100);
         if(B_tree_deletion(companyName) != 0)
              deletionSuccess++;
    }
    end = clock();
    deletion_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("총 %d건 삭제, 걸린 시간: %f 초, 삭제 성공 건수: %d\n", deletionCount, deletion_time, deletionSuccess);
    
}

int main(void) {
    printf("==== 10만 건 테스트 ====\n");
    test_btree_for_size(100000);
    printf("\n==== 100만 건 테스트 ====\n");
    test_btree_for_size(1000000);
    printf("\n==== 1000만 건 테스트 ====\n");
    test_btree_for_size(10000000);
    return 0;
}
