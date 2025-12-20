use std::process::Command;

#[test]
fn sharness() {
    let manifest_dir = env!("CARGO_MANIFEST_DIR");
    let profile = if cfg!(debug_assertions) { "debug" } else { "release" };
    let target_dir = format!("{}/target/{}", manifest_dir, profile);

    let status = Command::new("make")
        .args(["-C", "test"])
        .env("SHARNESS_BUILD_DIRECTORY", &target_dir)
        .current_dir(manifest_dir)
        .status()
        .expect("failed to run make");

    assert!(status.success(), "sharness tests failed");
}
