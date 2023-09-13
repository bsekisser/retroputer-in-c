typedef struct task_t {
	unsigned reg[_TASK_RCOUNT];
	unsigned stack[256];
//
	unsigned *flags;
}task_t;

static void task_clear_flag_imm(task_t t, unsigned arg) {
	BCLR(flags, bit);
}
