#include "dict_manipulation.h"

void value_free_custom(DictEntry *ptr)
{
	if (ptr->value_type == STRINGTYPE && ptr->value.val_void)
	{
		free(ptr->value.val_void);
		ptr->value.val_void = NULL;
	}
}

void private_data_free_custom(void *d)
{
	Dict *d_ptr = (Dict *)d;
	if (d_ptr->private_data != NULL)
		free(d_ptr->private_data);
	d_ptr->private_data = NULL;
}

bool addDict(dict *d, char *key, int type, ...)
{
	if (stringIsEmpty(key))
	{
		printf("Enpty key.\n");
		return false;
	}
	va_list value_ptr;
	va_start(value_ptr, type);
	DictEntry *entry = (DictEntry *)calloc(1, sizeof(DictEntry));
	char *entry_key = (char *)calloc(64, sizeof(char));
	memcpy(entry_key, key, strlen(key));
	entry->key = entry_key;
	entry->next = NULL;
	entry->value_type = type;
	switch (type) {
	case BOOL_FALSE:
	{
		entry->value.val_bool = false;
		dictAddEntry(d, entry);
	}
		break;
	case BOOL_TRUE:
	{
		entry->value.val_bool = true;
		dictAddEntry(d, entry);
	}
		break;
	case INTTYPE:
	{
		entry->value.val_int64 = va_arg(value_ptr, long);
		dictAddEntry(d, entry);
	}
		break;
	case DECIMALTYPE:
	{
		entry->value.val_double = va_arg(value_ptr, double);
		dictAddEntry(d, entry);
	}
		break;
	case STRINGTYPE:
	{
		char *value_tmp = va_arg(value_ptr, char *);
		uint16_t value_length = strlen(value_tmp);
		char *value = (char *)calloc(value_length + 1, sizeof(char));
		memcpy(value, value_tmp, value_length);
		value[value_length] = '\0';
		entry->value.val_void = value;
		dictAddEntry(d, entry);
	}
		break;
	case OBJECTTYPE:
	{
		entry->value.val_void = va_arg(value_ptr, void *);
		dictAddEntry(d, entry);
	}
		break;
	default:
	{
		printf("value type is error.\n");
		return false;
	}
		break;
	}
	return true;
}






