// Gobreaker
//
// Copyright (C) 2013 Mathias Lafeldt <mathias.lafeldt@gmail.com>
//
// This file is part of Gobreaker.
//
// Gobreaker is licensed under the terms of the MIT License. See LICENSE file.

package gobreaker

var seedtable = [3][16]uint32{
	{
		0x0a0b8d9b, 0x0a0133f8, 0x0af733ec, 0x0a15c574,
		0x0a50ac20, 0x0a920fb9, 0x0a599f0b, 0x0a4aa0e3,
		0x0a21c012, 0x0a906254, 0x0a31fd54, 0x0a091c0e,
		0x0a372b38, 0x0a6f266c, 0x0a61dd4a, 0x0a0dbf92,
	},
	{
		0x00288596, 0x0037dd28, 0x003beef1, 0x000bc822,
		0x00bc935d, 0x00a139f2, 0x00e9bbf8, 0x00f57f7b,
		0x0090d704, 0x001814d4, 0x00c5848e, 0x005b83e7,
		0x00108cf7, 0x0046ce5a, 0x003a5bf4, 0x006faffc,
	},
	{
		0x1dd9a10a, 0xb95ab9b0, 0x5cf5d328, 0x95fe7f10,
		0x8e2d6303, 0x16bb6286, 0xe389324c, 0x07ac6ea8,
		0xaa4811d8, 0x76ce4e18, 0xfe447516, 0xf9cd94d0,
		0x4c24dedb, 0x68275c4e, 0x72494382, 0xc8aa88e8,
	},
}

type Code struct {
	addr, val uint32
}

func CB1EncryptCode(code *Code) {
	cmd := byte(code.addr >> 28)

	tmp := code.addr & 0xff000000
	code.addr = ((code.addr & 0xff) << 16) | ((code.addr >> 8) & 0xffff)
	code.addr = (tmp | ((code.addr + seedtable[1][cmd]) & 0x00ffffff)) ^ seedtable[0][cmd]

	if cmd > 2 {
		code.val = code.addr ^ (code.val + seedtable[2][cmd])
	}
}

func CB1DecryptCode(code *Code) {
	cmd := byte(code.addr >> 28)

	if cmd > 2 {
		code.val = (code.addr ^ code.val) - seedtable[2][cmd]
	}

	tmp := code.addr ^ seedtable[0][cmd]
	code.addr = tmp - seedtable[1][cmd]
	code.addr = (tmp & 0xff000000) | ((code.addr & 0xffff) << 8) | ((code.addr >> 16) & 0xff)
}
