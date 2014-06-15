#ifndef _HASHTABLE_H_
#define _HASHTABLE_H_

#ifndef HASH_EXTEND_MULTIPLIER
#define HASH_EXTEND_MULTIPLIER ( 1.75F )
#endif

#ifdef __cplusplus
extern "C" {
#endif


typedef struct  HashtableNode HashtableNode;
struct  HashtableNode { 
	char			*key;
	unsigned int	hashA;
	unsigned int	hashB;
	void			*value;
	HashtableNode	*next;
};

typedef struct Hashtable Hashtable;
struct Hashtable {
	int				capacity;
	int				real_capacity;
	int				count;
	HashtableNode	**table;
	void (*free_value)(void *);
	int				ignore_case;
};

typedef struct HashtableIterater {
	Hashtable		*ht;
	int				index;
	HashtableNode	*p;
} HashtableIterater;

/*
 * ����һ����ϣ��
 * capacity ��������������ڲ��ڵ����Ԫ������
 * ignore_case  - Key�Ƿ���Դ�Сд��0-��ʾ�����ԣ���0ֵ��ʾ���Դ�Сд
 * free_value   - һ����������չ�ϣ��ʱ����ʹ��ÿһ���ڵ��valueֵ�����øú���
*/
Hashtable *ht_create(int capacity, int ignore_case, void (*free_value)(void *));
/*�ͷŵ���ϣ��*/
void ht_destroy(Hashtable *ht);
/*���ݹ�ϣ���ɹ�����0��ʧ�ܷ��ط�0ֵ��*/
int ht_expand(Hashtable *ht, int capacity);
/*���һ���ϣ���С����ʧ�ܻ��Ѿ����ڣ��򷵻ط�0ֵ�����򷵻�0*/
int ht_add(Hashtable *ht, const char *key, void *value);
/*
 * �ӹ�ϣ�����Ƴ�һ����ʧ�ܻ򲻴��ڣ��򷵻ط�0ֵ�����򷵻�0��
 * ��� pVal ֵ��ΪNULL������ִ�гɹ�ʱ�����ѱ��Ƴ����valueֵд��pValָ���ĵ�ַ�С�
 * �������Ҫ���ձ��Ƴ����ֵ������ pVal ��ֵΪ NULL��
*/
int ht_remove(Hashtable *ht, const char *key, void **pVal);
/*���ع�ϣ�����Ƿ���ڸ�������ڷ���0�����ڷ���1��*/
int ht_has(Hashtable *ht, const char *key);
/*���ع�ϣ����keyָ�����ֵ������������򷵻�NULL�������򷵻�ʵ��ֵ*/
void *ht_get(Hashtable *ht, const char *key);
/*���ع�ϣ����keyָ���Ľڵ������������򷵻�NULL�������򷵻ظýڵ�*/
HashtableNode *ht_get_node(Hashtable *ht, const char *key);
/*��չ�ϣ��ע�⺯�������Զ��ͷ�ÿ���ֵ�����ǰ�����ÿһ�����ͷ���ֵ��*/
int ht_clear(Hashtable *ht);

/*����һ����ϣ��ĵ�����*/
HashtableIterater *ht_it_create(Hashtable *ht);
/*�ͷŵ�����*/
void ht_it_destroy(HashtableIterater *iterater);
/*���õ�����*/
void ht_it_reset(HashtableIterater *iterater);
/*�������ƶ�����һ�����ƶ��ɹ��򷵻�1�����򷵻�0*/
int ht_it_next(HashtableIterater *iterater);
/*���ص�������ǰ���ֵ*/
void *ht_it_current(HashtableIterater *iterater);

#ifdef __cplusplus
}
#endif

#endif
