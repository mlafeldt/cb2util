// Encrypt and decrypt codes using CB v7 scheme

#![allow(dead_code)]

use super::{beefcodf, key, oldkey, seeds};
use super::{slice_to_u8, slice_to_u8_mut};
use rc4::Rc4;
use rsa;
use std::slice;

// Default seed tables (1280 bytes total)
#[cfg_attr(rustfmt, rustfmt_skip)]
const SEEDS: [[u8; 256]; 5] = [
    [
        0x84, 0x01, 0x21, 0xa4, 0xfa, 0x4d, 0x50, 0x8d, 0x75, 0x33, 0xc5, 0xf7, 0x4a, 0x6d, 0x7c, 0xa6,
        0x1c, 0xf8, 0x40, 0x18, 0xa1, 0xb3, 0xa2, 0xf9, 0x6a, 0x19, 0x63, 0x66, 0x29, 0xae, 0x10, 0x75,
        0x84, 0x7d, 0xec, 0x6a, 0xf9, 0x2d, 0x8e, 0x33, 0x44, 0x5c, 0x33, 0x6d, 0x78, 0x3e, 0x1b, 0x6c,
        0x02, 0xe0, 0x7d, 0x77, 0x1d, 0xb1, 0x61, 0x2a, 0xcd, 0xc1, 0x38, 0x53, 0x1f, 0xa1, 0x6e, 0x3d,
        0x03, 0x0d, 0x05, 0xdc, 0x50, 0x19, 0x85, 0x89, 0x9b, 0xf1, 0x8a, 0xc2, 0xd1, 0x5c, 0x22, 0xc4,
        0x11, 0x29, 0xf6, 0x13, 0xec, 0x06, 0xe4, 0xbd, 0x08, 0x9e, 0xb7, 0x8d, 0x72, 0x92, 0x10, 0x3c,
        0x41, 0x4e, 0x81, 0x55, 0x08, 0x9c, 0xa3, 0xbc, 0xa1, 0x79, 0xb0, 0x7a, 0x94, 0x3a, 0x39, 0x95,
        0x7a, 0xc6, 0x96, 0x21, 0xb0, 0x07, 0x17, 0x5e, 0x53, 0x54, 0x08, 0xcf, 0x85, 0x6c, 0x4b, 0xbe,
        0x30, 0x82, 0xdd, 0x1d, 0x3a, 0x24, 0x3c, 0xb2, 0x67, 0x0c, 0x36, 0x03, 0x51, 0x60, 0x3f, 0x67,
        0xf1, 0xb2, 0x77, 0xdc, 0x12, 0x9d, 0x7b, 0xce, 0x65, 0xf8, 0x75, 0xea, 0x23, 0x63, 0x99, 0x54,
        0x37, 0xc0, 0x3c, 0x42, 0x77, 0x12, 0xb7, 0xca, 0x54, 0xf1, 0x26, 0x1d, 0x1e, 0xd1, 0xab, 0x2c,
        0xaf, 0xb6, 0x91, 0x2e, 0xbd, 0x84, 0x0b, 0xf2, 0x1a, 0x1e, 0x26, 0x1e, 0x00, 0x12, 0xb7, 0x77,
        0xd6, 0x61, 0x1c, 0xce, 0xa9, 0x10, 0x19, 0xaa, 0x88, 0xe6, 0x35, 0x29, 0x32, 0x5f, 0x57, 0xa7,
        0x94, 0x93, 0xa1, 0x2b, 0xeb, 0x9b, 0x17, 0x2a, 0xaa, 0x60, 0xd5, 0x19, 0xb2, 0x4e, 0x5a, 0xe2,
        0xc9, 0x4a, 0x00, 0x68, 0x6e, 0x59, 0x36, 0xa6, 0xa0, 0xf9, 0x19, 0xa2, 0xc7, 0xc9, 0xd4, 0x29,
        0x5c, 0x99, 0x3c, 0x5c, 0xe2, 0xcb, 0x94, 0x40, 0x8b, 0xf4, 0x3b, 0xd2, 0x38, 0x7d, 0xbf, 0xd0,
    ],
    [
        0xcc, 0x6d, 0x5d, 0x0b, 0x70, 0x25, 0x5d, 0x68, 0xfe, 0xbe, 0x6c, 0x3f, 0xa4, 0xd9, 0x95, 0x5f,
        0x30, 0xae, 0x34, 0x39, 0x00, 0x89, 0xdc, 0x5a, 0xc8, 0x82, 0x24, 0x3a, 0xfc, 0xda, 0x3c, 0x1f,
        0x73, 0x3f, 0x63, 0xaa, 0x53, 0xbd, 0x4e, 0xb5, 0x33, 0x48, 0x59, 0xc1, 0xb7, 0xe0, 0x0c, 0x99,
        0xec, 0x3b, 0x32, 0x26, 0xb3, 0xb1, 0xe2, 0x8e, 0x54, 0x41, 0x55, 0xdb, 0x1d, 0x90, 0x0b, 0x48,
        0xf3, 0x3f, 0xca, 0x1f, 0x19, 0xeb, 0x7f, 0x56, 0x52, 0xd7, 0x20, 0x67, 0x59, 0x4f, 0x4e, 0xdc,
        0xbb, 0x6a, 0x8e, 0x45, 0x88, 0x0b, 0x93, 0xac, 0xcd, 0x0e, 0x29, 0x18, 0x7a, 0x16, 0x8d, 0x8d,
        0xc2, 0x88, 0x6a, 0x9d, 0x39, 0xf4, 0x93, 0x14, 0xcd, 0xe0, 0x6b, 0xc7, 0x28, 0x21, 0x5c, 0x97,
        0x70, 0x7c, 0xab, 0x53, 0x46, 0x33, 0x03, 0x18, 0xdf, 0x91, 0xfe, 0x06, 0xc0, 0xff, 0xa2, 0x58,
        0xf3, 0xb0, 0x6b, 0x9b, 0x71, 0x91, 0x23, 0xda, 0x92, 0x67, 0x14, 0x34, 0x9f, 0xa5, 0xaf, 0x65,
        0x62, 0xe8, 0x7f, 0x79, 0x35, 0x32, 0x29, 0x3e, 0x4f, 0xdc, 0xc7, 0x8e, 0xf1, 0x21, 0x9d, 0x3b,
        0x61, 0xfc, 0x0b, 0x02, 0xec, 0xe4, 0xa7, 0xea, 0x77, 0xe7, 0x21, 0x63, 0x97, 0x7f, 0x23, 0x8a,
        0x8b, 0xbe, 0x4e, 0x90, 0xc0, 0x89, 0x04, 0x44, 0x90, 0x57, 0x41, 0xb5, 0x74, 0xad, 0xb1, 0xe9,
        0xf3, 0x91, 0xc7, 0x27, 0x3e, 0x00, 0x81, 0x99, 0xee, 0x38, 0xf5, 0x32, 0x4f, 0x27, 0x4f, 0x64,
        0x39, 0x3d, 0xd3, 0x0b, 0x99, 0xd5, 0x99, 0xd6, 0x10, 0x4b, 0x43, 0x17, 0x38, 0x34, 0x54, 0x63,
        0x19, 0x36, 0xbd, 0x15, 0xb1, 0x06, 0x1e, 0xde, 0x1b, 0xaf, 0xeb, 0xfa, 0x56, 0xb8, 0x8d, 0x9d,
        0x14, 0x1a, 0xa6, 0x49, 0x56, 0x19, 0xca, 0xc1, 0x40, 0x6d, 0x71, 0xde, 0x68, 0xc1, 0xc3, 0x4a,
    ],
    [
        0x69, 0x31, 0x5c, 0xab, 0x7f, 0x5b, 0xe9, 0x81, 0x32, 0x58, 0x32, 0x0a, 0x97, 0xf3, 0xc7, 0xcf,
        0xbb, 0x1d, 0xcf, 0x0e, 0x83, 0x35, 0x4c, 0x58, 0xce, 0xf7, 0x8a, 0xe4, 0xb0, 0xe4, 0x83, 0x48,
        0x81, 0x77, 0x7c, 0x3f, 0xbc, 0x27, 0x3a, 0x1b, 0xa4, 0xe9, 0x06, 0xa4, 0x15, 0xab, 0x90, 0x10,
        0x7d, 0x74, 0xda, 0xfc, 0x36, 0x09, 0xcc, 0xf7, 0x12, 0xb6, 0xf4, 0x94, 0xe9, 0x8b, 0x6a, 0x3b,
        0x5e, 0x71, 0x46, 0x3e, 0x0b, 0x78, 0xad, 0x3b, 0x94, 0x5b, 0x89, 0x85, 0xa3, 0xe0, 0x01, 0xeb,
        0x84, 0x41, 0xaa, 0xd7, 0xb3, 0x17, 0x16, 0xc3, 0x6c, 0xb1, 0x81, 0x73, 0xec, 0xe4, 0x6e, 0x09,
        0x56, 0xee, 0x7a, 0xf6, 0x75, 0x6a, 0x73, 0x95, 0x8d, 0xda, 0x51, 0x63, 0x8b, 0xbb, 0xe0, 0x4d,
        0xf8, 0xa0, 0x27, 0xf2, 0x9f, 0xc8, 0x15, 0x5a, 0x23, 0x85, 0x58, 0x04, 0x4a, 0x57, 0x28, 0x20,
        0x6d, 0x9d, 0x85, 0x83, 0x3c, 0xbf, 0x02, 0xb0, 0x96, 0xe8, 0x73, 0x6f, 0x20, 0x6e, 0xb0, 0xe4,
        0xc6, 0xfa, 0x71, 0xa6, 0x5d, 0xc5, 0xa0, 0xa3, 0xf8, 0x5c, 0x99, 0xcb, 0x9c, 0x04, 0x3a, 0xb2,
        0x04, 0x8d, 0xa2, 0x9d, 0x32, 0xf0, 0xbd, 0xaa, 0xea, 0x81, 0x79, 0xe2, 0xa1, 0xba, 0x89, 0x12,
        0xd5, 0x9f, 0x81, 0xeb, 0x63, 0xe7, 0xe5, 0xd4, 0xe9, 0x0e, 0x30, 0xbc, 0xcb, 0x70, 0xdd, 0x51,
        0x77, 0xc0, 0x80, 0xb3, 0x49, 0x03, 0x9a, 0xb8, 0x8c, 0xa7, 0x63, 0x62, 0x8f, 0x72, 0x5c, 0xa6,
        0xa0, 0xcf, 0x4f, 0xb4, 0x86, 0xfd, 0x49, 0xfa, 0x4a, 0x85, 0xdb, 0xfe, 0x61, 0xb7, 0x3a, 0xd7,
        0x83, 0x70, 0x57, 0x49, 0x83, 0xa7, 0x10, 0x73, 0x74, 0x37, 0x87, 0xfd, 0x6b, 0x28, 0xb7, 0x31,
        0x1e, 0x54, 0x1c, 0xe9, 0xd0, 0xb1, 0xca, 0x76, 0x3b, 0x21, 0xf7, 0x67, 0xbb, 0x48, 0x69, 0x39,
    ],
    [
        0x8d, 0xd1, 0x8c, 0x7b, 0x83, 0x8c, 0xa8, 0x18, 0xa7, 0x4a, 0x14, 0x03, 0x88, 0xb3, 0xce, 0x74,
        0xbf, 0x5b, 0x87, 0x67, 0xa7, 0x85, 0x6b, 0x62, 0x96, 0x7c, 0xa9, 0xa6, 0xf6, 0x9e, 0xf4, 0x73,
        0xc5, 0xc4, 0xb0, 0x2b, 0x73, 0x2e, 0x36, 0x77, 0xdf, 0xba, 0x57, 0xff, 0x7f, 0xe9, 0x84, 0xe1,
        0x8d, 0x7b, 0xa2, 0xef, 0x4f, 0x10, 0xf3, 0xd3, 0xe8, 0xb4, 0xba, 0x20, 0x28, 0x79, 0x18, 0xd6,
        0x0f, 0x1c, 0xaa, 0xbd, 0x0e, 0x45, 0xf7, 0x6c, 0x68, 0xb9, 0x29, 0x40, 0x1a, 0xcf, 0xb6, 0x0a,
        0x13, 0xf8, 0xc0, 0x9c, 0x87, 0x10, 0x36, 0x14, 0x73, 0xa1, 0x75, 0x27, 0x14, 0x55, 0xaf, 0x78,
        0x9a, 0x08, 0xc9, 0x05, 0xf2, 0xec, 0x24, 0x1b, 0x07, 0x4a, 0xdc, 0xf6, 0x48, 0xc6, 0x25, 0xcd,
        0x12, 0x1d, 0xaf, 0x51, 0x8f, 0xe9, 0xca, 0x2c, 0x80, 0x57, 0x78, 0xb7, 0x96, 0x07, 0x19, 0x77,
        0x6e, 0x16, 0x45, 0x47, 0x8e, 0x9c, 0x18, 0x55, 0xf1, 0x72, 0xb3, 0x8a, 0xea, 0x4e, 0x8d, 0x90,
        0x2e, 0xbc, 0x08, 0xac, 0xf6, 0xa0, 0x5c, 0x16, 0xe3, 0x7a, 0xee, 0x67, 0xb8, 0x58, 0xdc, 0x16,
        0x40, 0xed, 0xf9, 0x18, 0xb3, 0x0e, 0xd8, 0xee, 0xe1, 0xfa, 0xc3, 0x9f, 0x82, 0x99, 0x32, 0x41,
        0x34, 0xbe, 0xc9, 0x50, 0x36, 0xe5, 0x66, 0xaa, 0x0d, 0x43, 0xf0, 0x3f, 0x26, 0x7c, 0xf3, 0x87,
        0x26, 0xa4, 0xf5, 0xf8, 0xa0, 0x32, 0x46, 0x74, 0x2e, 0x5a, 0xe2, 0xe7, 0x6b, 0x02, 0xa8, 0xd0,
        0xcf, 0xb8, 0x33, 0x15, 0x3b, 0x4f, 0xc7, 0x7a, 0xe8, 0x3d, 0x75, 0xd2, 0xfe, 0x42, 0x22, 0x22,
        0xa8, 0x21, 0x33, 0xfb, 0xb0, 0x87, 0x92, 0x99, 0xca, 0xd7, 0xd7, 0x88, 0xac, 0xe4, 0x75, 0x83,
        0x56, 0xbf, 0xce, 0xed, 0x4f, 0xf6, 0x22, 0x07, 0xca, 0xbc, 0xd2, 0xef, 0x1b, 0x75, 0xd6, 0x2d,
    ],
    [
        0xd2, 0x4f, 0x76, 0x51, 0xeb, 0xa1, 0xad, 0x84, 0xd6, 0x19, 0xe6, 0x97, 0xd9, 0xd3, 0x58, 0x6b,
        0xfb, 0xb8, 0x20, 0xfd, 0x49, 0x56, 0x1b, 0x50, 0x61, 0x10, 0x57, 0xb8, 0x78, 0x07, 0xc1, 0x4a,
        0xa2, 0xea, 0x47, 0x80, 0x00, 0x4a, 0xb3, 0x4e, 0x6f, 0x1a, 0xc1, 0xd5, 0x22, 0xf8, 0x54, 0x2f,
        0x33, 0xe5, 0x7f, 0xb4, 0x13, 0x02, 0xa3, 0xa1, 0x8b, 0x1c, 0x6f, 0x19, 0xd6, 0x42, 0xb3, 0x24,
        0x4b, 0x04, 0x30, 0x10, 0x02, 0x23, 0x6f, 0x10, 0x03, 0x4b, 0x0e, 0x33, 0x55, 0x22, 0xa4, 0x78,
        0xec, 0xd2, 0x4a, 0x11, 0x8b, 0xfc, 0xff, 0x14, 0x7a, 0xed, 0x06, 0x47, 0x86, 0xfc, 0xf0, 0x03,
        0x0f, 0x75, 0x07, 0xe4, 0x9a, 0xd3, 0xbb, 0x0d, 0x97, 0x1f, 0x6f, 0x80, 0x62, 0xa6, 0x9e, 0xc6,
        0xb1, 0x10, 0x81, 0xa1, 0x6d, 0x55, 0x0f, 0x9e, 0x1b, 0xb7, 0xf5, 0xdc, 0x62, 0xa8, 0x63, 0x58,
        0xcf, 0x2f, 0x6a, 0xad, 0x5e, 0xd3, 0x3f, 0xbd, 0x8d, 0x9b, 0x2a, 0x8b, 0xdf, 0x60, 0xb9, 0xaf,
        0xaa, 0x70, 0xb4, 0xa8, 0x17, 0x99, 0x72, 0xb9, 0x88, 0x9d, 0x3d, 0x2a, 0x11, 0x87, 0x1e, 0xf3,
        0x9d, 0x33, 0x8d, 0xed, 0x52, 0x60, 0x36, 0x71, 0xff, 0x7b, 0x37, 0x84, 0x3d, 0x27, 0x9e, 0xd9,
        0xdf, 0x58, 0xf7, 0xc2, 0x58, 0x0c, 0x9d, 0x5e, 0xee, 0x23, 0x83, 0x70, 0x3f, 0x95, 0xbc, 0xf5,
        0x42, 0x86, 0x91, 0x5b, 0x3f, 0x77, 0x31, 0xd2, 0xb7, 0x09, 0x59, 0x53, 0xf5, 0xf2, 0xe5, 0xf1,
        0xdc, 0x92, 0x83, 0x14, 0xc1, 0xa2, 0x25, 0x62, 0x13, 0xfd, 0xd4, 0xc5, 0x54, 0x9d, 0x9c, 0x27,
        0x6c, 0xc2, 0x75, 0x8b, 0xbc, 0xc7, 0x4e, 0x0a, 0xf6, 0x5c, 0x2f, 0x12, 0x8e, 0x25, 0xbb, 0xf2,
        0x5f, 0x89, 0xaa, 0xea, 0xd9, 0xcd, 0x05, 0x74, 0x20, 0xd6, 0x17, 0xed, 0xf0, 0x66, 0x6c, 0x7b,
    ],
];

// Default RC4 key (20 bytes)
const RC4_KEY: [u32; 5] = [0xd0dba9d7, 0x13a0a96c, 0x80410df0, 0x2ccdbe1f, 0xe570a86b];

// RSA parameters
const RSA_MODULUS: u64 = 18446744073709551605; // 0xfffffffffffffff5
const RSA_DEC_KEY: u64 = 11;
const RSA_ENC_KEY: u64 = 2682110966135737091;

// Used to generate/change the encryption key and seeds.
// "Beefcode" is the new V7+ seed code:
// BEEFC0DE VVVVVVVV, where VVVVVVVV = val.
pub fn beefcode(init: i32, val: u32) {
    // Easy access to all bytes of val
    let v: Vec<usize> = val.to_le_bytes().iter().map(|&i| i as usize).collect();

    unsafe {
        // Set up key and seeds
        if init != 0 {
            beefcodf = 0;
            key.copy_from_slice(&RC4_KEY);

            if val != 0 {
                seeds.copy_from_slice(&SEEDS);
                key[0] = u32::from(seeds[3][v[3]]) << 24
                    | u32::from(seeds[2][v[2]]) << 16
                    | u32::from(seeds[1][v[1]]) << 8
                    | u32::from(seeds[0][v[0]]);
                key[1] = u32::from(seeds[0][v[3]]) << 24
                    | u32::from(seeds[3][v[2]]) << 16
                    | u32::from(seeds[2][v[1]]) << 8
                    | u32::from(seeds[1][v[0]]);
                key[2] = u32::from(seeds[1][v[3]]) << 24
                    | u32::from(seeds[0][v[2]]) << 16
                    | u32::from(seeds[3][v[1]]) << 8
                    | u32::from(seeds[2][v[0]]);
                key[3] = u32::from(seeds[2][v[3]]) << 24
                    | u32::from(seeds[1][v[2]]) << 16
                    | u32::from(seeds[0][v[1]]) << 8
                    | u32::from(seeds[3][v[0]]);
            } else {
                seeds.copy_from_slice(&[[0; 256]; 5]);
            }
        } else {
            if val != 0 {
                key[0] = u32::from(seeds[3][v[3]]) << 24
                    | u32::from(seeds[2][v[2]]) << 16
                    | u32::from(seeds[1][v[1]]) << 8
                    | u32::from(seeds[0][v[0]]);
                key[1] = u32::from(seeds[0][v[3]]) << 24
                    | u32::from(seeds[3][v[2]]) << 16
                    | u32::from(seeds[2][v[1]]) << 8
                    | u32::from(seeds[1][v[0]]);
                key[2] = u32::from(seeds[1][v[3]]) << 24
                    | u32::from(seeds[0][v[2]]) << 16
                    | u32::from(seeds[3][v[1]]) << 8
                    | u32::from(seeds[2][v[0]]);
                key[3] = u32::from(seeds[2][v[3]]) << 24
                    | u32::from(seeds[1][v[2]]) << 16
                    | u32::from(seeds[0][v[1]]) << 8
                    | u32::from(seeds[3][v[0]]);
            } else {
                seeds.copy_from_slice(&[[0; 256]; 5]);
                key[0] = 0;
                key[1] = 0;
                key[2] = 0;
                key[3] = 0;
            }
        }

        // Use key to encrypt seeds with RC4
        let k = slice_to_u8_mut(&mut key);
        for i in 0..5 {
            let mut rc4 = Rc4::new(k);
            // Encrypt seeds
            rc4.crypt(&mut seeds[i]);
            // Encrypt original key for next round
            rc4.crypt(k);
        }

        // Back up key
        oldkey.copy_from_slice(&key);
    }
}

// Encrypts a V7+ code.
pub fn encrypt_code(addr: u32, val: u32) -> (u32, u32) {
    let mut code = (addr, val);
    encrypt_code_mut(&mut code.0, &mut code.1);
    code
}

pub fn encrypt_code_mut(addr: &mut u32, val: &mut u32) {
    let oldaddr = *addr;
    let oldval = *val;

    unsafe {
        // Step 1: Multiplication, modulo (2^32)
        *addr = mul_encrypt(*addr, oldkey[0].wrapping_sub(oldkey[1]));
        *val = mul_encrypt(*val, oldkey[2].wrapping_add(oldkey[3]));

        // Step 2: RC4
        let mut code = [*addr, *val];
        let mut rc4 = Rc4::new(slice_to_u8(&key));
        rc4.crypt(slice_to_u8_mut(&mut code));
        *addr = code[0];
        *val = code[1];

        // Step 3: RSA
        rsa::crypt(addr, val, RSA_ENC_KEY);

        // Step 4: Encryption loop of 64 cycles, using the generated seeds
        let s = slice::from_raw_parts(seeds.as_ptr() as *const u32, 5 * 64);
        for i in 0..64 {
            *addr = (addr.wrapping_add(s[2 * 64 + i]) ^ s[0 * 64 + i]).wrapping_sub(*val ^ s[4 * 64 + i]);
            *val = (val.wrapping_sub(s[3 * 64 + i]) ^ s[1 * 64 + i]).wrapping_add(*addr ^ s[4 * 64 + i]);
        }

        // BEEFC0DE
        if (oldaddr & 0xfffffffe) == 0xbeefc0de {
            beefcode(0, oldval);
            //beefcodf = 1;
            return;
        }

        // BEEFC0DF
        if beefcodf != 0 {
            let mut rc4 = Rc4::new(slice_to_u8(&[oldaddr, oldval]));
            rc4.crypt(slice_to_u8_mut(&mut seeds));
            beefcodf = 0;
            return;
        }
    }
}

// Decrypts a V7+ code.
pub fn decrypt_code(addr: u32, val: u32) -> (u32, u32) {
    let mut code = (addr, val);
    decrypt_code_mut(&mut code.0, &mut code.1);
    code
}

pub fn decrypt_code_mut(addr: &mut u32, val: &mut u32) {
    unsafe {
        // Step 1: Decryption loop of 64 cycles, using the generated seeds
        let s = slice::from_raw_parts(seeds.as_ptr() as *const u32, 5 * 64);
        for i in (0..64).rev() {
            *val = (val.wrapping_sub(*addr ^ s[4 * 64 + i]) ^ s[1 * 64 + i]).wrapping_add(s[3 * 64 + i]);
            *addr = (addr.wrapping_add(*val ^ s[4 * 64 + i]) ^ s[0 * 64 + i]).wrapping_sub(s[2 * 64 + i]);
        }

        // Step 2: RSA
        rsa::crypt(addr, val, RSA_DEC_KEY);

        // Step 3: RC4
        let mut code = [*addr, *val];
        let mut rc4 = Rc4::new(slice_to_u8(&key));
        rc4.crypt(slice_to_u8_mut(&mut code));
        *addr = code[0];
        *val = code[1];

        // Step 4: Multiplication with multiplicative inverse, modulo (2^32)
        *addr = mul_decrypt(*addr, oldkey[0].wrapping_sub(oldkey[1]));
        *val = mul_decrypt(*val, oldkey[2].wrapping_add(oldkey[3]));

        // BEEFC0DF
        if beefcodf != 0 {
            let mut rc4 = Rc4::new(slice_to_u8(&[*addr, *val]));
            rc4.crypt(slice_to_u8_mut(&mut seeds));
            beefcodf = 0;
            return;
        }

        // BEEFC0DE
        if (*addr & 0xfffffffe) == 0xbeefc0de {
            beefcode(0, *val);
            //beefcodf = 1;
            return;
        }
    }
}

// Multiplication, modulo (2^32)
fn mul_encrypt(a: u32, b: u32) -> u32 {
    a.wrapping_mul(b | 1)
}

// Multiplication with multiplicative inverse, modulo (2^32)
fn mul_decrypt(a: u32, b: u32) -> u32 {
    a.wrapping_mul(mul_inverse(b | 1))
}

// Computes the multiplicative inverse of @word, modulo (2^32).
// Original MIPS R5900 coding converted to C, and now to Rust.
fn mul_inverse(word: u32) -> u32 {
    if word == 1 {
        return 1;
    }
    let mut a2 = 0u32.wrapping_sub(word) % word;
    if a2 == 0 {
        return 1;
    }
    let mut t1 = 1u32;
    let mut a3 = word;
    let mut a0 = 0u32.wrapping_sub(0xffffffff / word);
    while a2 != 0 {
        let mut v0 = a3 / a2;
        let v1 = a3 % a2;
        let a1 = a2;
        a3 = a1;
        let a1 = a0;
        a2 = v1;
        v0 = v0.wrapping_mul(a1);
        a0 = t1.wrapping_sub(v0);
        t1 = a1;
    }
    t1
}

#[cfg(test)]
mod tests {
    use super::*;

    fn mul_tests() -> Vec<(u32, u32, u32)> {
        vec![
            (0x00000000, 0xa686d3b6, 0x00000000),
            (0x000e0000, 0xa686d3b6, 0xac620000),
            (0x0067bd20, 0x4fd931ff, 0x200802e0),
            (0x2ba0a76e, 0xa686d3b6, 0x24050002),
            (0x4adfd954, 0x4fd931ff, 0x9029beac),
            (0x7c016806, 0x2912dedd, 0x000000be),
            (0xa9422f21, 0xa686d3b6, 0x03d203e7),
            (0xfff576e0, 0xa686d3b6, 0x27bd0020),
        ]
    }

    #[test]
    fn test_mul_encrypt() {
        for t in mul_tests().iter() {
            assert_eq!(t.0, mul_encrypt(t.2, t.1));
        }
    }

    #[test]
    fn test_mul_decrypt() {
        for t in mul_tests().iter() {
            assert_eq!(t.2, mul_decrypt(t.0, t.1));
        }
    }

    #[test]
    fn test_mul_inverse() {
        let tests = vec![
            (0x0d313243, 0x6c7b2a6b),
            (0x0efd8231, 0xd4c096d1),
            (0x2912dedd, 0xe09de975),
            (0x4fd931ff, 0x9a62cdff),
            (0x5a53abb5, 0x58f42a9d),
            (0x9ab2af6d, 0x1043b265),
            (0xa686d3b7, 0x57ed7a07),
            (0xec35a92f, 0xd2743dcf),
        ];
        for t in tests.iter() {
            assert_eq!(t.1, mul_inverse(t.0));
        }
    }

    struct Test {
        decrypted: (u32, u32),
        encrypted: (u32, u32),
    }

    #[cfg_attr(rustfmt, rustfmt_skip)]
    fn tests() -> Vec<Test> {
        vec![
            // Test {
            //     decrypted: (0, 0),
            //     encrypted: (0, 0),
            // },
        ]
    }

    #[test]
    fn test_encrypt_code() {
        for t in tests().iter() {
            let result = encrypt_code(t.decrypted.0, t.decrypted.1);
            assert_eq!(t.encrypted, result);
        }
    }

    #[test]
    fn test_decrypt_code() {
        for t in tests().iter() {
            let result = decrypt_code(t.encrypted.0, t.encrypted.1);
            assert_eq!(t.decrypted, result);
        }
    }
}
