#include <stdio.h>
#include<string.h>
#include <stdlib.h> 
#include <windows.h>
#include<process.h>

#pragma comment(lib,"Winmm.lib")
#pragma warning(disable:4996)
#define SWAP(a,b) tmp=(a);(a)=(b);(b)=tmp;
#define M 32
#define NSTACK 50
#define sepN 4
void sort(int nameNum);
void merge(int n1,int n2, int newNameNum);
char Name[4][15];
int f = 1;
char input_array[110];
char input_array2[110];
typedef struct data {
	long data;
	char name[15];
	struct data *next;
}Data;
Data *root, *v,*subRoot;
Data* move(int n,Data* r);
void newData(Data **w);
void insert(int n, long l, char c[15], Data* r);
struct thread_info {
	int nameNum;
};
struct merge_thread_info{
	int n1;
	int n2;
	int newNameNum;
};

unsigned __stdcall qsortThreadEntry(void * arg) {
	struct thread_info *context = (struct thread_info *) arg;
	sort(context->nameNum);
	return 0;
}
unsigned __stdcall mergeThreadEntry(void * arg) {
	struct merge_thread_info *context = (struct merge_thread_info *) arg;
	merge(context->n1, context->n2, context->newNameNum);
	return 0;
}

void firstSort(int c){
	int newNameNum = 0;
	HANDLE A[sepN];
	HANDLE B[sepN];
	struct thread_info para[sepN];
	struct merge_thread_info parb[sepN];
	int i,j,k,l;
	printf("quick\n");
	for (i = 0; i < c / sepN; i++) {
		for (j = 0; j < sepN; j++) {
			para[j].nameNum = i*4+j;
			A[j] = (HANDLE)_beginthreadex(NULL, 0, &qsortThreadEntry, (void*)&para[j], 0, NULL);
			if (A[j] == NULL) {
				printf("_beginthreadex() failed, error: %d\n", GetLastError());
				return;
			}
		}
		WaitForMultipleObjects(4, A, TRUE, INFINITE);
		CloseHandle(A[0]);
		CloseHandle(A[1]);
		CloseHandle(A[2]);
		CloseHandle(A[3]);
	}
	printf("\n\n%s\n\n", root->name);
	printf("merge\n");
	for (i = c;i>1;) {
		f++;
		int num = 0;
		j = 0;
		int number[4];
		for (; j+8 < i&&i>=8; j += 8) {
			for (k = 0; k < sepN; k++) {
				sprintf(Name[k], "%s%d%d%s", "s", f, num, ".dat");
				insert(num, 0, Name[k], subRoot);
				number[k] = num;
				num++;
			}
			for (k = 0; k < sepN; k++) { 
				parb[k].n1 = j + 2 * k;
				parb[k].n2 = j + 2 * k + 1;
				parb[k].newNameNum = number[k]; 
				B[k] = (HANDLE)_beginthreadex(NULL, 0, &mergeThreadEntry, (void*)&parb[k], 0, NULL);
				if (B[k] == NULL) {
					printf("_beginthreadex() failed, error: %d\n", GetLastError());
					return;
				}
			}
			
			WaitForMultipleObjects(4, B, TRUE, INFINITE);
			CloseHandle(B[0]);
			CloseHandle(B[1]);
			CloseHandle(B[2]);
			CloseHandle(B[3]);
		}
		
		l = i - j;
		if (l % 2 == 0) { 
			l = l / 2; 
			for (k = 0; k < l; k++) {
				sprintf(Name[k], "%s%d%d%s", "s", f, num, ".dat");
				insert(num, 0, Name[k], subRoot);
				number[k] = num;
				num++;
			}
			for (k = 0; k < l; k++) {
				parb[k].n1 = j + 2 * k;
				parb[k].n2 = j + 2 * k + 1;
				parb[k].newNameNum = number[k];
				B[k] = (HANDLE)_beginthreadex(NULL, 0, &mergeThreadEntry, (void*)&parb[k], 0, NULL);
				if (B[k] == NULL) {
					return;
				}
			}
			WaitForMultipleObjects(l, B, TRUE, INFINITE);
			for (k = 0; k < l; k++) {
				CloseHandle(B[k]);
			}
		}
		else {
			l = l / 2 ; 
			for (k = 0; k <l ; k++) {
				sprintf(Name[k], "%s%d%d%s", "s", f, num, ".dat");
				insert(num, 0, Name[k], subRoot);
				number[k] = num;
				num++;
			}
			for (k = 0; k < l; k++) {
				parb[k].n1 = j + 2 * k;
				parb[k].n2 = j + 2 * k + 1;
				parb[k].newNameNum = number[k];
				B[k] = (HANDLE)_beginthreadex(NULL, 0, &mergeThreadEntry, (void*)&parb[k], 0, NULL);
				if (B[k] == NULL) {
					return;
				}
			}
			WaitForMultipleObjects(l, B, TRUE, INFINITE);
			for (k = 0; k < l; k++) {
				CloseHandle(B[k]);
			}
			sprintf(Name[0], "%s%d%d%s", "s", f, num, ".dat");
			insert(num, 0, Name[0], subRoot);
			Data *f = move(i-1, root); 
			FILE* f1;
			f1 = fopen(f->name, "r");
			if (f1 != NULL) {
				FILE* f2 = fopen(Name[0], "w");
				while (fgets(input_array, 100, f1) != NULL) {
					fputs(input_array, f2);
				}
				fclose(f1);
				fclose(f2);
			}
		}
		
		while (root->next!=NULL) {
			if (!remove(root->name)) {
			}
			root = root->next;
		}
		root = subRoot;
		newData(&subRoot);
		if (i % 2 == 0) { i = i / 2; }
		else { i = i / 2 + 1; }
	}
	printf("fin");
}

void sort(int nameNum) {
	printf("quick->%d\n",nameNum);
	char *tmp;
	Data * d;
	d = move(nameNum, root);
	unsigned long n = d->data;
	char**arr;
	arr = malloc(sizeof(char*)*(n+2));
	FILE* fp = fopen(d->name,"r");
	FILE* fc = fopen(d->name, "r");
	if (fgets(input_array, 100, fc) == NULL) { 
		fclose(fp);
		fclose(fc);
		return; 
	}
	for (unsigned int ar = 0; ar <= n; ar++) {
		arr[ar] = malloc(sizeof(char)*(100+2));
	}
	for (unsigned int a =1; a <= n; a++) {
		if (fgets(arr[a], 100, fp) == NULL) {
			break;
		}
	}
	unsigned long i, j, k;
	unsigned long l = 1;
	unsigned long r = n;
	int *stack, nstack = 0;
	char* a;
	stack = (int *)malloc(NSTACK * sizeof(int)) - 1;
	for (;;) {
		if (r - l < M) {
			for (j = l + 1; j <= r; j++) {
				a = arr[j];
				for (i = j - 1; i >= l; i--) {
					if (memcmp(a, arr[i], 10) == 1 || memcmp(a, arr[i], 10) == 0)break;
					arr[i + 1] = arr[i];
				}     arr[i + 1] = a;
			}
			if (nstack == 0)break;
			r = stack[nstack--];
			l = stack[nstack--];
		}
		else {
			
			k = (l + r) >> 1;
			SWAP(arr[k], arr[l + 1])
			if (memcmp(arr[l + 1],arr[r], 10) == 1) {
				SWAP(arr[l + 1], arr[r])
			}
			if (memcmp(arr[l], arr[r], 10) == 1) {//
				SWAP(arr[l], arr[r])
			}
			if (memcmp(arr[l+1], arr[l], 10) == 1) {
				SWAP(arr[l + 1], arr[l])
			}
			i = l + 1; j = r;
			a = arr[l];
			for (;;) {
				do {
					i++;
				} while (memcmp(a, arr[i], 10) == 1);
				do {
					
					j--;
				}while (memcmp(arr[j], a, 10) == 1);
				if (j < i)break;
				SWAP(arr[i], arr[j])
					
			}
			arr[l] = arr[j];
			arr[j] = a;
			nstack += 2;
			if (nstack > NSTACK)printf("Error, NSTACK too small\n");
			if (r - i + 1 >= j - l) {
				stack[nstack] = r;
				stack[nstack - 1] = i;
				r = j - 1;
			}
			else {
				stack[nstack] = j - 1;
				stack[nstack - 1] = l;
				l = i;
			}
		}
	}
	FILE* fw = fopen(d->name, "w");
	for (i = 1; i <= n; i++) {
		fputs(arr[i], fw);
	}
	fclose(fw);
	free(stack + 1);
	for (unsigned int ar = 0; ar <= n; ar++) {
		free(arr[ar]);
	}
	free(arr);
	fclose(fc);
	fclose(fp);
}

void merge(int n1, int n2,int newNameNum) {
	printf("merge->%d\n",newNameNum);
	FILE* fi[2];
	Data* d1;
	Data* d2;
	Data* newD;
	d1 = move(n1, root);
	d2 = move(n2, root);
	newD = move(newNameNum, subRoot);
	FILE*fc1; FILE*fc2;
	if ((fc1 = fopen(d1->name, "r")) == NULL)return;
	if ((fc2 = fopen(d2->name, "r")) == NULL)return;
	if (fgets(input_array, 100, fc1) == NULL&& fgets(input_array2, 100, fc2) == NULL) {
		fclose(fc1);
		fclose(fc2);
		return;
	}fclose(fc1);
	fclose(fc2);
	fi[0] = fopen(d1->name,"r");
	fi[1] = fopen(d2->name, "r");
	FILE* fm = fopen(newD->name, "a");
	char *input_array[2];
	input_array[0] = (char*)malloc(100);
	input_array[1] = (char*)malloc(100);
	int a = 0;
	fgets(input_array[1], 100, fi[1]);
	while (fgets(input_array[a], 100, fi[a])!=NULL) {

		if (memcmp(input_array[1], input_array[0], 10) == 1) {
			fputs(input_array[0], fm);
			a = 0;
		}
		else {
			fputs(input_array[1], fm);
			a = 1;
		}
	}
	if (a == 0) {
		fputs(input_array[1], fm);
	}
	else if (a == 1) {
		fputs(input_array[0], fm);
	}
	a = 2;
	while (a==2) {
		a = 0;
		if (fgets(input_array[0], 100, fi[0]) != NULL) {
			fputs(input_array[0], fm);
			a = 2;
		}
		else if (fgets(input_array[1], 100, fi[1]) != NULL) {
			fputs(input_array[1], fm);
			a = 2;
		}
	}
	fclose(fi[0]);
	fclose(fi[1]);
	fclose(fm);
}

void newData(Data **w){
	*w = (Data *)malloc(sizeof(Data));
	(*w)->next = NULL;
}
void insert(int n, long l, char c[15],Data* r) {
	v = r;
	int i = 0;
	Data *w;
	while (i<n) {
		v = v->next;
		i++;
	}
	newData(&w);
	
	v->data = l;
	strcpy(v->name, c);
	v->next = w;
}
void change(int n, long l, Data* r) {
	v = r;
	int i = 0;
	while (i < n) {
		v = v->next;
		i++;
	}
	v->data = l;
}

Data* move(int n, Data* r) {
	v = r;
	int i = 0;
	while (i < n) {
		v = v->next;
		i++;
	}
	return v;
}

int main() {
	char *file = "data.dat";      //ì¸óÕÉfÅ[É^
	unsigned long startTime, deff;
	newData(&root);
	newData(&subRoot);
	int i = 0;
	int j = 0;
	int fn = 0;
	int c = 0;
	long snum[4] = {0,0,0,0};
	startTime = timeGetTime();
	FILE *fp = fopen(file, "r");

	while (fn==0) {
		FILE *fs[sepN];
		for (i = 0; i < sepN; i++) {
			printf("file->%d\n", i + j);
			sprintf(Name[i], "%s%d%d%s", "s",f, i+j, ".dat");
			fs[i] = fopen(Name[i], "a+");
			insert(i+j,0,Name[i], root);
			c++;
		}
		i = 0;
		do{
			if (fgets(input_array, 100, fp) != NULL) {
				fputs(input_array, fs[i%sepN]);
				snum[i%sepN]++;
				i++;
			}
			else {
				fn = 1;
				break;
			}
		} while (i < 10000000);
		for (i = 0; i < sepN; i++) {
			change(i+j, snum[i], root);
			fclose(fs[i]);
			snum[i] = 0;
		}
		if (fn != 1) {
			j += sepN;
		}
	}
	Data *w;
	w = root;
	for (i = 0; i < c; i++) {
		w = w->next;
	}
	printf("d\n");
	firstSort(c);
	deff = timeGetTime() - startTime;
	printf("sortTime:total: %dmsec\n", deff);
	return 0;
}