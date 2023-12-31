#include <TabosVM.h>
#include <atd/list.h>
#include <atd/mem.h>

ATD_LINK_NODE(uintptr_t);
ATD_LIST_CREATE(uintptr_t, 0);

#pragma region INSTRUCTIONS

#pragma region NULL_INSTR
bool NULL_OPERATION(TVM_engine_processor_t *proc)
{
    return true;
}
#pragma endregion
#pragma region FLOW_INSTR
#pragma region CALL
bool CALLMS(TVM_engine_processor_t *proc)
{
    const char *IDENTIFIER = (proc->module.base + proc->module.body.code_start + proc->code.bytecode_start) + proc->IP;
    TVM_method_t method = TVM_get_method_n(proc->module, proc->code, IDENTIFIER);

    // push current instruction pointer and
    // jump to the destination
    *ATD_LIST_uintptr_t_PUSH(proc->call_stack) = proc->IP + ATD_strlen(IDENTIFIER) + 1;
    proc->IP = method.offset;

    return true;
}
bool CALLM8(TVM_engine_processor_t *proc)
{
    uint8_t IDENTIFIER = TVM_ENGINE_PROCESSOR_CYCLE8(proc);
    TVM_method_t method = TVM_get_method_i(proc->module, proc->code, IDENTIFIER);

    // push current instruction pointer and
    // jump to the destination
    *ATD_LIST_uintptr_t_PUSH(proc->call_stack) = proc->IP;
    proc->IP = method.offset;

    return true;
}
bool CALLM16(TVM_engine_processor_t *proc)
{
    uint16_t IDENTIFIER = TVM_ENGINE_PROCESSOR_CYCLE16(proc);
    TVM_method_t method = TVM_get_method_i(proc->module, proc->code, IDENTIFIER);

    // push current instruction pointer and
    // jump to the destination
    *ATD_LIST_uintptr_t_PUSH(proc->call_stack) = proc->IP;
    proc->IP = method.offset;

    return true;
}
bool CALLM32(TVM_engine_processor_t *proc)
{
    uint32_t IDENTIFIER = TVM_ENGINE_PROCESSOR_CYCLE32(proc);
    TVM_method_t method = TVM_get_method_i(proc->module, proc->code, IDENTIFIER);

    // push current instruction pointer and
    // jump to the destination
    *ATD_LIST_uintptr_t_PUSH(proc->call_stack) = proc->IP;
    proc->IP = method.offset;

    return true;
}
bool RET(TVM_engine_processor_t *proc)
{
    uintptr_t destIP = ATD_LIST_uintptr_t_POP(proc->call_stack);

    // jump back to caller
    proc->IP = (uint32_t)destIP;
    return true;
}
#pragma endregion
#pragma endregion
#pragma region TEST_INSTR
bool DEBUG(TVM_engine_processor_t *proc)
{
    ATD_printf("DEBUG\n");
    return true;
}
#pragma endregion

#pragma endregion

TVM_instruction_callback_t __TVM_INSTR_MAP__[65535] = { 0 };
bool __TVM_INSTR_MAP_INIT__ = false;

void TVM_register_bytecode(uint16_t b, TVM_instruction_callback_t instr) { __TVM_INSTR_MAP__[b] = instr; }

TVM_engine_processor_t TVM_build(TVM_module_t m, TVM_code_t c)
{
    TVM_engine_processor_t result = {
        .module = m,
        .code = c,
        .IP = TVM_get_method_n(m, c, "main").offset,
        .type_names_head = ATD_LIST_uintptr_t_MAKE(),
        .types_head = ATD_LIST_uintptr_t_MAKE(),
        .call_stack = ATD_LIST_uintptr_t_MAKE(),
        .vars_head = ATD_LIST_uintptr_t_MAKE(),
    };
	size_t vars = TVM_get_variables_num(m);
	for (int i = 0; i < vars; i++) ATD_LIST_uintptr_t_PUSH(vars);
    return result;
}

void TVM_dispose(TVM_engine_processor_t *proc)
{
    ATD_LIST_uintptr_t_DISPOSE(proc->type_names_head);
    ATD_LIST_uintptr_t_DISPOSE(proc->types_head);
    ATD_LIST_uintptr_t_DISPOSE(proc->call_stack);
    ATD_LIST_uintptr_t_DISPOSE(proc->vars_head);
}

void TVM_init()
{
    if (__TVM_INSTR_MAP_INIT__) return;

    TVM_register_bytecode(0, NULL_OPERATION);
    TVM_register_bytecode(1, CALLMS);
    TVM_register_bytecode(2, CALLM8);
    TVM_register_bytecode(3, CALLM16);
    TVM_register_bytecode(4, CALLM32);
    TVM_register_bytecode(5, DEBUG);
    TVM_register_bytecode(6, RET);

__TVM_INSTR_MAP_INIT__ = true;
}

bool TVM_exec_one(TVM_engine_processor_t *proc)
{
    TVM_instruction_callback_t callback = NULL;
    uint16_t byc = 0;

    // get the bytecode
    byc = TVM_ENGINE_PROCESSOR_CYCLE16(proc);
    callback = __TVM_INSTR_MAP__[byc];

    // end instruction
    if (byc == 0xffff) return false;

    // execute the instruction
    if (callback) return callback(proc);
}
void TVM_exec(TVM_engine_processor_t *proc)
{
    // execute the program until interrupted
    while (TVM_exec_one(proc)) ;
}

bool TVM_register_type(TVM_engine_processor_t *proc, const char *name, TVM_type_t *type)
{
    *ATD_LIST_uintptr_t_PUSH(proc->type_names_head) = name;
    *ATD_LIST_uintptr_t_PUSH(proc->types_head) = type;
    return true;
}
TVM_type_t * TVM_resolve_typeexpr(TVM_engine_processor_t *processor, const char *expr)
{
    size_t i, sz = ATD_LIST_uintptr_t_LENGTH(processor->type_names_head);
    
    for (i = 1; i < sz; i++)
    {
        if (ATD_strcmp(((char*)*ATD_LIST_uintptr_t_AT(processor->type_names_head, i)), expr) == 0)
            return ((TVM_type_t *)*ATD_LIST_uintptr_t_AT(processor->types_head, i));
    }

    return NULL;
}

TVM_engine_var_t TVM_ENGINE_VAR_BUILD(TVM_engine_processor_t *processor, TVM_variable_t var) {
	// check if variable is valid
	if (var.index == -1) return (TVM_engine_var_t) {
		.pointer = NULL
	};
	// find pointer
	uintptr_t ptr = *ATD_LIST_uintptr_t_AT(processor->vars_head, var.index);

	// return the var with its pointer
	return (TVM_engine_var_t) {
		.name = var.name,
		.type = var.type,

		.pointer = ptr
	};
}

TVM_engine_var_t TVM_variable_n(TVM_engine_processor_t *processor, const char name[]) {
	// resolve var name and build the engine variable
	TVM_variable_t var = TVM_get_var_n(processor->module, name);
	return TVM_ENGINE_VAR_BUILD(processor, var);
}
TVM_engine_var_t TVM_variable_i(TVM_engine_processor_t *processor, size_t index) {
	// resolve var name and build the engine variable
	TVM_variable_t var = TVM_get_var_i(processor->module, index);
	return TVM_ENGINE_VAR_BUILD(processor, var);
}
