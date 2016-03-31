// Tests for Gobreaker
//
// Copyright (C) 2013 Mathias Lafeldt <mathias.lafeldt@gmail.com>
//
// This file is part of Gobreaker.
//
// Gobreaker is licensed under the terms of the MIT License. See LICENSE file.

package gobreaker

import "testing"

var cryptCodeTests = []struct {
	decrypted, encrypted Code
}{
	{Code{0x0031789a, 0x00000063}, Code{0x0ac93a95, 0x00000063}},
	{Code{0x1031a028, 0x0000ffff}, Code{0x1a613d30, 0x0000ffff}},
	{Code{0x201f6024, 0x00000000}, Code{0x2a973dbd, 0x00000000}},
	{Code{0x902db32c, 0x0c0baff1}, Code{0x9ad420d3, 0x180ddeda}},
	{Code{0xa008060c, 0x08028007}, Code{0xaae071c0, 0xaca684dd}},
}

func TestCB1EncryptCode(t *testing.T) {
	for _, test := range cryptCodeTests {
		code := test.decrypted
		want := test.encrypted

		CB1EncryptCode(&code)

		if code != want {
			t.Errorf("expected code %08x, got %08x", want, code)
		}
	}
}

func TestCB1DecryptCode(t *testing.T) {
	for _, test := range cryptCodeTests {
		code := test.encrypted
		want := test.decrypted

		CB1DecryptCode(&code)

		if code != want {
			t.Errorf("expected code %08x, got %08x", want, code)
		}
	}
}
