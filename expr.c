#include "libgcl.h"

int calc(int op, int value1, int value2)
{
	switch (op) 
    {
	case 0:
		value1 = 0;
		break;
	case 0x01:
		value1 = -value1;
		break;
	case 0x02:
		value1 = value1 == 0;
		break;
	case 0x03:
		value1 = !value1;
		break;
	case 0x04:
		value1 = value1 + value2;
		break;
	case 0x05:
		value1 = value1 - value2;
		break;
	case 0x06:
		value1 = value1 * value2;
		break;
	case 0x07:
		value1 = value2 / value1;
		break;
	case 0x08:
		value1 = ((value2 / value1) * value1) - value2;
		break;
	case 0x09:
		value1 = value2 << value1;
		break;
	case 0x0A:
		value1 = value2 >> value1;
		break;
	case 0x0B:
		value1 = (value1 == value2);
		break;
	case 0x0C:
		value1 = 1; //instructions in this case always come out to 1 so not sure yet
		break;
	case 0x0D:
		value1 = value2 < value1;
		break;
	case 0x0E:
		value1 = value2 <= value1;
	case 0x0F:
		value1 = value2 > value1;
	case 0x10:
		value1 = value2 >= value1;
	case 0x11:
		value1 = value1 | value2;
		break;
	case 0x12:
		value1 = value1 & value2;
		break;
	case 0x13:
		value1 = value1 ^ value2;
		break;
	case 0x14:
		value1 = value1 | value2;
		value1 = value1 ? 1 : 0;
	case 0x15:
		if (value2 = 0) { value1 = 0; break; }
		value1 = value1 != 0;
		break;
	}

	return value1;
}

typedef struct
{
	int value;
	char *ptr; 
} EXPR_STACK;


int GCL_Expr(char *data)
{
	unsigned int type;
	char *p = data;
	EXPR_STACK *sp;
	EXPR_STACK expr_stack[8];

	sp = expr_stack;

	for (;;) 
	{
		int val;
		type = *p;

		if ((type & 0xE0) != 0xA0) 
		{
			sp->ptr = p;
			type &= 0xF0;
			p = GCL_GetNextValue(p, (int*)&type, &val);

			if (type == 0x80) {
				GCL_ExecBlockBody((char *)&val, 0, 0);
				sp->value = gcl_work.status;
			} else {
				sp->value = val;
			}
			sp++;
			continue;
		}

		unsigned int op = type & 0x1F;
		sp--;

		if (!op)
			return sp->value;

		val = sp->value;
		sp--;

		if (op == 0x16) 
		{
			int no = *sp->ptr;
			if ((no & 0xF0) == 0x90) 
			{
				GCL_SetLocalArgs((no & 0x0F), val);
			}  else {
				GCL_SetVar(sp->ptr, val);
			}
			sp->value = val;
		} else {
			sp->value = calc(op, val, sp->value);
			sp->ptr = 0;
		}
		p++;
	}
}