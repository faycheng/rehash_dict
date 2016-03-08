# rehash_dict

标签（空格分隔）： rehash_dict

---
rehash_dict是一个使用C语言实现的动态词典（也称为符号表，关联数组，或映射），该模块模仿于redis的dict。

## 特性
>* 完整支持常规的dict操作
>* 通过函数指针（钩子）实现简单的多态
>* 使用最新的hash算法murmurhash3
>* 使用链地址法解决键冲突
>* 使用单步渐进式rehash进行表中数据迁移，避免一次性rehash造成的阻塞

## 问题
>* 不支持嵌套，如dict的value为dict

## 原始使用示例
```
// 自定义value的释放函数
void value_free_custom(DictEntry *ptr)
{
	if (ptr->value_type == STRINGTYPE && ptr->value.val_void)
	{
		free(ptr->value.val_void);
		ptr->value.val_void = NULL;
	}
}
// 自定义dict附带的private data的释放函数
void private_data_free_custom(void *d)
{
	Dict *d_ptr = (Dict *)d;
	if (d_ptr->private_data != NULL)
		free(d_ptr->private_data);
	d_ptr->private_data = NULL;
}

// 将定义的函数添加给dict，并创建dict
DictFunction *function =  (DictFunction *)calloc(1, sizeof(DictFunction));
function->private_data_free = private_data_free_custom;
function->value_free = value_free_custom;
Dict *d = dictCreateWithArgs(function, NULL);

// 创建一个带值DictEntry，并添加到dict
DictEntry *entry = (DictEntry *)calloc(1, sizeof(DictEntry));
char *entry_key = (char *)calloc(64, sizeof(char));
memcpy(entry_key, "key_1", strlen("key_1"));
entry->key = entry_key;
entry->next = NULL;
entry->value_type = BOOL_FALSE;
entry->value.val_bool = false;
dictAddEntry(d, entry);

DictEntry *entry_str = (DictEntry *)calloc(1, sizeof(DictEntry));
char *entry_key_str = (char *)calloc(64, sizeof(char));
memcpy(entry_key_str, "key_2", strlen("key_2"));
entry_str->key = entry_key_str;
entry_str->next = NULL;
entry_str->value_type = STRINGTYPE;
entry_str->value.val_void = "value_string";
dictAddEntry(d, entry_str);
//判断dict中是否存在key_1
if (dictExistKey(d, "key_1") == NULL)
		printf("Not exist key_1");

// 打印信息
printf("key_1 value:%d\n", dictSearchEntry(d, "key_1")->value.val_bool);
printf("key_1 value:%s\n", dictSearchEntry(d, "key_2")->value.val_void);

//释放并销毁dict
dictRelease(d);

```

## 封装后使用示例
```
Dict *d = createDict();
ASSERT(d != NULL);
uint8_t count = 0;
for ( ; count < 10; count ++)
{
	char buf[8] = {0};
	sprintf(buf, "%d", count);
	ASSERT(addDict(d, buf, INTTYPE, count));
	ASSERT(existKey(d, buf));
}
DictIterator *iterator = dictCreateIterator(d);
ASSERT(iterator != NULL);
DictEntry *entry = NULL;
while ((entry = dictIteratorNext(iterator)) != NULL)
{
	printf("key:%s\tvalue:%d\n", entry->key, entry->value.num_value);
}
dictDestoryIterator(&iterator);
releaseDict(d);
```
>* 封装使用需要引用dict_manipulation.h，dict_manipulation.c两个文件
>* 支持简单的迭代器，以便方便迭代dict元素
