extern crate cc;

fn main() {
    cc::Build::new()
        .file("../cb2_crypto.c")
        .file("../arcfour.c")
        .file("../shs.c")
        .file("../libbig_int/src/basic_funcs.c")
        .file("../libbig_int/src/bitset_funcs.c")
        .file("../libbig_int/src/low_level_funcs/add.c")
        .file("../libbig_int/src/low_level_funcs/and.c")
        .file("../libbig_int/src/low_level_funcs/andnot.c")
        .file("../libbig_int/src/low_level_funcs/cmp.c")
        .file("../libbig_int/src/low_level_funcs/div.c")
        .file("../libbig_int/src/low_level_funcs/mul.c")
        .file("../libbig_int/src/low_level_funcs/or.c")
        .file("../libbig_int/src/low_level_funcs/sqr.c")
        .file("../libbig_int/src/low_level_funcs/sub.c")
        .file("../libbig_int/src/low_level_funcs/xor.c")
        .file("../libbig_int/src/memory_manager.c")
        .file("../libbig_int/src/modular_arithmetic.c")
        .file("../libbig_int/src/number_theory.c")
        .file("../libbig_int/src/service_funcs.c")
        .file("../libbig_int/src/str_funcs.c")
        .include("../libbig_int/include")
        .define("CODE_ENCRYPTION", "1")
        .compile("libcodebreaker.a");
}
