// Copyright (c) 1981-86 Robert A. Koeneke
// Copyright (c) 1987-94 James E. Wilson
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

// rng.cpp
uint32_t getRandomSeed();
void setRandomSeed(uint32_t seed);
int32_t rnd();
