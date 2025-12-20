const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const version = v: {
        const git_describe = std.mem.trim(u8, b.run(&[_][]const u8{
            "git",
            "-C",
            b.build_root.path orelse ".",
            "describe",
            "--tags",
            "--match",
            "v[0-9]*",
            "--abbrev=4",
            "--dirty",
        }), " \n\r");
        const version = b.allocator.dupeZ(u8, git_describe[1..]) catch @panic("OOM");
        std.mem.replaceScalar(u8, version, '-', '.');
        break :v version;
    };

    const libbig_int = b.addLibrary(.{
        .linkage = .static,
        .name = "big_int",
        .root_module = b.createModule(.{
            .target = target,
            .optimize = optimize,
            .link_libc = true,
            .sanitize_c = .off,
        }),
    });
    libbig_int.addCSourceFiles(.{ .files = &.{
        "libbig_int/src/basic_funcs.c",
        "libbig_int/src/bitset_funcs.c",
        "libbig_int/src/low_level_funcs/add.c",
        "libbig_int/src/low_level_funcs/and.c",
        "libbig_int/src/low_level_funcs/andnot.c",
        "libbig_int/src/low_level_funcs/cmp.c",
        "libbig_int/src/low_level_funcs/div.c",
        "libbig_int/src/low_level_funcs/mul.c",
        "libbig_int/src/low_level_funcs/or.c",
        "libbig_int/src/low_level_funcs/sqr.c",
        "libbig_int/src/low_level_funcs/sub.c",
        "libbig_int/src/low_level_funcs/xor.c",
        "libbig_int/src/memory_manager.c",
        "libbig_int/src/modular_arithmetic.c",
        "libbig_int/src/number_theory.c",
        "libbig_int/src/service_funcs.c",
        "libbig_int/src/str_funcs.c",
    }, .flags = &cflags });
    libbig_int.addIncludePath(b.path("libbig_int/include"));

    const libcheats = b.addLibrary(.{
        .linkage = .static,
        .name = "cheats",
        .root_module = b.createModule(.{
            .target = target,
            .optimize = optimize,
            .link_libc = true,
            .sanitize_c = .off,
        }),
    });
    libcheats.addCSourceFiles(.{ .files = &.{
        "libcheats/src/cheatlist.c",
        "libcheats/src/libcheats.c",
        "libcheats/src/mystring.c",
        "libcheats/src/parser.c",
    }, .flags = &cflags });
    libcheats.addIncludePath(b.path("libcheats/include"));
    libcheats.root_module.addCMacro("HAVE_STDINT_H", "1");

    const zlib = b.dependency("zlib", .{ .target = target, .optimize = optimize });
    zlib.artifact("z").root_module.sanitize_c = .off;

    const exe = b.addExecutable(.{
        .name = "cb2util",
        .root_module = b.createModule(.{
            .target = target,
            .optimize = optimize,
            .sanitize_c = .off,
        }),
    });
    exe.addCSourceFiles(.{ .files = &.{
        "arcfour.c",
        "cb2_crypto.c",
        "cb2util.c",
        "cbc.c",
        "cheats.c",
        "cheats_common.c",
        "compress.c",
        "fileio.c",
        "pcb.c",
        "shs.c",
    }, .flags = &cflags });
    exe.addIncludePath(b.path("include"));
    exe.addIncludePath(b.path("libbig_int/include"));
    exe.addIncludePath(b.path("libcheats/include"));
    exe.linkLibrary(libbig_int);
    exe.linkLibrary(libcheats);
    exe.linkLibrary(zlib.artifact("z"));
    exe.linkLibC();
    exe.root_module.addCMacro("CB2UTIL_VERSION", b.fmt("\"{s}\"", .{version}));
    exe.root_module.addCMacro("HAVE_STDINT_H", "1");

    b.installArtifact(exe);

    const run_exe = b.addRunArtifact(exe);
    if (b.args) |args| {
        run_exe.addArgs(args);
    }
    const run_step = b.step("run", "Run the tool");
    run_step.dependOn(&run_exe.step);
}

const cflags = [_][]const u8{ "-Wall", "-Werror" };
