#include <stdarg.h> //变长参数函数所需的头文件
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "Ast.h"
#include "Pass.h"
#include "bblock.h"
#include "c_container_auxiliary.h"
#include "cds.h"
#include "symbol_table.h"

extern List *ins_list;
extern List *global_func_list;
extern List *global_var_list;

SymbolTable *cur_symboltable = NULL;

bool is_functional_test = true;

int yyparse(void);

int parser(char *input);

char *tty_path;

char *read_code_from_file(const char *);
void register_replace(Function *func_list);

char *test_cases[] = {"./test_cases/00_main.c",
                      "./test_cases/01_var_defn2.c",
                      "./test_cases/02_var_defn3.c",
                      "./test_cases/03_arr_defn2.c",
                      "./test_cases/04_arr_defn3.c",
                      "./test_cases/05_arr_defn4.c",
                      "./test_cases/06_const_var_defn2.c",
                      "./test_cases/07_const_var_defn3.c",
                      "./test_cases/08_const_array_defn.c",
                      "./test_cases/09_func_defn.c",
                      "./test_cases/10_var_defn_func.c",
                      "./test_cases/11_add2.c",
                      "./test_cases/12_addc.c",
                      "./test_cases/13_sub2.c",
                      "./test_cases/14_subc.c",
                      "./test_cases/15_mul.c",
                      "./test_cases/16_mulc.c",
                      "./test_cases/17_div.c",
                      "./test_cases/18_divc.c",
                      "./test_cases/19_mod.c",
                      "./test_cases/20_rem.c",
                      "./test_cases/21_if_test2.c",
                      "./test_cases/22_if_test3.c",
                      "./test_cases/23_if_test4.c",
                      "./test_cases/24_if_test5.c",
                      "./test_cases/25_while_if.c",
                      "./test_cases/26_while_test1.c",
                      "./test_cases/27_while_test2.c",
                      "./test_cases/28_while_test3.c",
                      "./test_cases/29_break.c",
                      "./test_cases/30_continue.c",
                      "./test_cases/31_while_if_test1.c",
                      "./test_cases/32_while_if_test2.c",
                      "./test_cases/33_while_if_test3.c",
                      "./test_cases/34_arr_expr_len.c",
                      "./test_cases/35_op_priority1.c",
                      "./test_cases/36_op_priority2.c",
                      "./test_cases/37_op_priority3.c",
                      "./test_cases/38_op_priority4.c",
                      "./test_cases/39_op_priority5.c",
                      "./test_cases/40_unary_op.c",
                      "./test_cases/41_unary_op2.c",
                      "./test_cases/42_empty_stmt.c",
                      "./test_cases/43_logi_assign.c",
                      "./test_cases/44_stmt_expr.c",
                      "./test_cases/45_comment1.c",
                      "./test_cases/46_hex_defn.c",
                      "./test_cases/47_hex_oct_add.c",
                      "./test_cases/48_assign_complex_expr.c",
                      "./test_cases/49_if_complex_expr.c",
                      "./test_cases/50_short_circuit.c",
                      "./test_cases/51_short_circuit3.c",
                      "./test_cases/52_scope.c",
                      "./test_cases/53_scope2.c",
                      "./test_cases/54_hidden_var.c",
                      "./test_cases/55_sort_test1.c",
                      "./test_cases/56_sort_test2.c",
                      "./test_cases/57_sort_test3.c",
                      "./test_cases/58_sort_test4.c",
                      "./test_cases/59_sort_test5.c",
                      "./test_cases/60_sort_test6.c",
                      "./test_cases/61_sort_test7.c",
                      "./test_cases/62_percolation.c",
                      "./test_cases/63_big_int_mul.c",
                      "./test_cases/64_calculator.c",
                      "./test_cases/65_color.c",
                      "./test_cases/66_exgcd.c",
                      "./test_cases/67_reverse_output.c",
                      "./test_cases/68_brainfk.c",
                      "./test_cases/69_expr_eval.c",
                      "./test_cases/70_dijkstra.c",
                      "./test_cases/71_full_conn.c",
                      "./test_cases/72_hanoi.c",
                      "./test_cases/73_int_io.c",
                      "./test_cases/74_kmp.c",
                      "./test_cases/75_max_flow.c",
                      "./test_cases/76_n_queens.c",
                      "./test_cases/77_substr.c",
                      "./test_cases/78_side_effect.c",
                      "./test_cases/79_var_name.c",
                      "./test_cases/80_chaos_token.c",
                      "./test_cases/81_skip_spaces.c",
                      "./test_cases/82_long_func.c",
                      "./test_cases/83_long_array.c",
                      "./test_cases/84_long_array2.c",
                      "./test_cases/85_long_code.c",
                      "./test_cases/86_long_code2.c",
                      "./test_cases/87_many_params.c",
                      "./test_cases/88_many_params2.c",
                      "./test_cases/89_many_globals.c",
                      "./test_cases/90_many_locals.c",
                      "./test_cases/91_many_locals2.c",
                      "./test_cases/92_register_alloc.c",
                      "./test_cases/93_nested_calls.c",
                      "./test_cases/94_nested_loops.c",
                      "./test_cases/95_float.c",
                      "./test_cases/96_matrix_add.c",
                      "./test_cases/97_matrix_sub.c",
                      "./test_cases/98_matrix_mul.c",
                      "./test_cases/99_matrix_tran.c"};

// --------------------------------------------------
char *hidden_cases[] = {
    "./hidden_cases/00_comment2.c",
    "./hidden_cases/01_multiple_returns.c",
    "./hidden_cases/02_ret_in_block.c",
    "./hidden_cases/03_branch.c",
    "./hidden_cases/04_break_continue.c",
    "./hidden_cases/05_param_name.c",
    "./hidden_cases/06_func_name.c",
    "./hidden_cases/07_arr_init_nd.c",
    "./hidden_cases/08_global_arr_init.c",
    "./hidden_cases/09_BFS.c",
    "./hidden_cases/10_DFS.c",
    "./hidden_cases/11_BST.c",
    "./hidden_cases/12_DSU.c",
    "./hidden_cases/13_LCA.c",
    "./hidden_cases/14_dp.c",
    "./hidden_cases/15_graph_coloring.c",
    "./hidden_cases/16_k_smallest.c",
    "./hidden_cases/17_maximal_clique.c",
    "./hidden_cases/18_prim.c",
    "./hidden_cases/19_search.c",
    "./hidden_cases/20_sort.c",
    "./hidden_cases/21_union_find.c",
    "./hidden_cases/22_matrix_multiply.c",
    "./hidden_cases/23_json.c",
    "./hidden_cases/24_array_only.c",
    "./hidden_cases/25_scope3.c",
    "./hidden_cases/26_scope4.c",
    "./hidden_cases/27_scope5.c",
    "./hidden_cases/28_side_effect2.c",
    "./hidden_cases/29_long_line.c",
    "./hidden_cases/30_many_dimensions.c",
    "./hidden_cases/31_many_indirections.c",
    "./hidden_cases/32_many_params3.c",
    "./hidden_cases/33_multi_branch.c",
    "./hidden_cases/34_multi_loop.c",
    "./hidden_cases/35_math.c",
    "./hidden_cases/36_rotate.c",
    "./hidden_cases/37_dct.c",
    "./hidden_cases/38_light2d.c",
    "./hidden_cases/39_fp_params.c",
};
// --------------------------------------------------
char *performance_test[] = {
    "./performance_test/00_bitset1.c",
    "./performance_test/00_bitset2.c",
    "./performance_test/00_bitset3.c",
    "./performance_test/01_mm1.c",
    "./performance_test/01_mm2.c",
    "./performance_test/01_mm3.c",
    "./performance_test/02_mv1.c",
    "./performance_test/02_mv2.c",
    "./performance_test/02_mv3.c",
    "./performance_test/03_sort1.c",
    "./performance_test/03_sort2.c",
    "./performance_test/03_sort3.c",
    "./performance_test/04_spmv1.c",
    "./performance_test/04_spmv2.c",
    "./performance_test/04_spmv3.c",
    "./performance_test/brainfuck-bootstrap.c",
    "./performance_test/brainfuck-mandelbrot-nerf.c",
    "./performance_test/brainfuck-pi-nerf.c",
    "./performance_test/conv0.c",
    "./performance_test/conv1.c",
    "./performance_test/conv2.c",
    "./performance_test/crypto-1.c",
    "./performance_test/crypto-2.c",
    "./performance_test/crypto-3.c",
    "./performance_test/dead-code-elimination-1.c",
    "./performance_test/dead-code-elimination-2.c",
    "./performance_test/dead-code-elimination-3.c",
    "./performance_test/fft0.c",
    "./performance_test/fft1.c",
    "./performance_test/fft2.c",
    "./performance_test/floyd-0.c",
    "./performance_test/floyd-1.c",
    "./performance_test/floyd-2.c",
    "./performance_test/gameoflife-gosper.c",
    "./performance_test/gameoflife-oscillator.c",
    "./performance_test/gameoflife-p61glidergun.c",
    "./performance_test/hoist-1.c",
    "./performance_test/hoist-2.c",
    "./performance_test/hoist-3.c",
    "./performance_test/instruction-combining-1.c",
    "./performance_test/instruction-combining-2.c",
    "./performance_test/instruction-combining-3.c",
    "./performance_test/integer-divide-optimization-1.c",
    "./performance_test/integer-divide-optimization-2.c",
    "./performance_test/integer-divide-optimization-3.c",
    "./performance_test/median0.c",
    "./performance_test/median1.c",
    "./performance_test/median2.c",
    "./performance_test/shuffle0.c",
    "./performance_test/shuffle1.c",
    "./performance_test/shuffle2.c",
    "./performance_test/sl1.c",
    "./performance_test/sl2.c",
    "./performance_test/sl3.c",
    "./performance_test/stencil0.c",
    "./performance_test/stencil1.c",
    "./performance_test/transpose0.c",
    "./performance_test/transpose1.c",
    "./performance_test/transpose2.c",
};

// --------------------------------------------------
int main(int argc, char **argv) {
  // freopen("/dev/null", "w", stdout);

#ifdef PRINT_OK
  printf("hello world\n");
#endif

  tty_path = ttyname(STDOUT_FILENO);

  AllInit();

#ifdef PRINT_OK
  printf("%%begin the pass\n");
#endif
  char *choose_case = NULL;
  if (argc == 5) {
    is_functional_test = true;
    choose_case = read_code_from_file(argv[4]);
  } else if (argc == 6) {
    is_functional_test = false;
    choose_case = read_code_from_file(argv[4]);
  } else if (argc == 2) {
    is_functional_test = true;
    choose_case = read_code_from_file(argv[1]);
  } else if (argc == 3) {
    is_functional_test = false;
    choose_case = read_code_from_file(argv[1]);
  } else {
    is_functional_test = false;
    // choose_case = read_code_from_file(test_cases[77]);
    // choose_case = read_code_from_file(hidden_cases[4]);
    choose_case = read_code_from_file("./my_cases/if_and_or.c");
  }
  if (choose_case == NULL)
    return 1;

#ifdef PRINT_OK
  freopen("./output/printf_ast.txt", "w", stdout);
#endif

#define PARSER
  parser(choose_case);

#ifdef PRINT_OK
  freopen(tty_path, "w", stdout);
  freopen("./output/out.txt", "w", stdout);
  setvbuf(stdout, NULL, _IOLBF, BUFSIZ);
#endif

#ifdef PRINT_OK
  print_ins_pass(ins_list);
  fflush(stdout);
  printf("\n\n\n\n");
#endif

#ifdef PARSER

  delete_return_deadcode_pass(ins_list);

#ifdef PRINT_OK
  print_ins_pass(ins_list);
#endif

  ins_toBBlock_pass(ins_list);

#ifdef PRINT_OK
  print_ins_pass(global_var_list);
#endif

  ListFirst(global_func_list, false);
  void *element;
  while (ListNext(global_func_list, &element)) {
#ifdef PRINT_OK
    puts(((Function *)element)->label->name);
#endif
    bblock_to_dom_graph_pass((Function *)element);
    register_replace((Function *)element);
  }

#endif

  free(tty_path);
  printf("%s test All over!\n",
         is_functional_test ? "functional" : "performance");
  return 0;
}

char *read_code_from_file(const char *file_path) {
  puts(file_path);
  FILE *fd = fopen(file_path, "r");

  if (fd == NULL) {
    perror("fopen()");
    return NULL;
  }

  fseek(fd, 0, SEEK_END);
  long file_size = ftell(fd);
  fseek(fd, 0, SEEK_SET);

  char *buffer = (char *)malloc(file_size + 1);
  if (buffer == NULL) {
    printf("malloc() error\n");
    fclose(fd);
    return NULL;
  }
  size_t bytes_read = fread(buffer, 1, file_size, fd);
  buffer[bytes_read] = '\0';
  fclose(fd);
  return buffer;
}
