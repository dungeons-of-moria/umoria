// Copyright (c) 1981-86 Robert A. Koeneke
// Copyright (c) 1987-94 James E. Wilson
//
// SPDX-License-Identifier: GPL-3.0-or-later

// Generic helper functions used throughout the code

// Eventually we want these helpers to have no external dependencies
// other than the standard library functions.

#pragma once

int getAndClearFirstBit(uint32_t &flag);
void insertNumberIntoString(char *to_string, const char *from_string, int32_t number, bool show_sign);
void insertStringIntoString(char *to_string, const char *from_string, const char *str_to_insert);
bool isVowel(char ch);
bool stringToNumber(const char *str, int &number);
uint32_t getCurrentUnixTime();
void humanDateString(char *day);
