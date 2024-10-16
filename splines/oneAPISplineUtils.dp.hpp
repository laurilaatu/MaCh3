#pragma once
// Forward declare the kernel and pipe names
// (This prevents unwanted name mangling in the optimization report.)
class FeederA;
class FeederB;
class Matmul;
class Drain;
class APipe;
class BPipe;
class CPipe;
class DonePipe;